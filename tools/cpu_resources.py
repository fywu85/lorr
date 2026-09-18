"""Inspect Linux CPU affinity and enforced cgroup quotas without changing them."""
import os
import resource
from pathlib import Path


def cpu_resources():
    affinity = sorted(os.sched_getaffinity(0))
    representatives = []
    cores = {}
    for cpu in affinity:
        topology = Path('/sys/devices/system/cpu/cpu{}/topology'.format(cpu))
        key = tuple((topology / name).read_text().strip() for name in ['physical_package_id', 'core_id'])
        if key not in cores:
            cores[key] = []
            representatives.append(cpu)
        cores[key].append(cpu)
    memberships = []
    for line in Path('/proc/self/cgroup').read_text().splitlines():
        _, controllers, group = line.split(':', 2)
        if not controllers or 'cpu' in controllers.split(','):
            memberships.append((bool(controllers), group))
    limits = []
    quotas = []
    for line in Path('/proc/self/mountinfo').read_text().splitlines():
        left, right = line.split(' - ', 1)
        mount = left.split()
        filesystem = right.split()
        v1 = filesystem[0] == 'cgroup' and 'cpu' in filesystem[2].split(',')
        v2 = filesystem[0] == 'cgroup2'
        if not (v1 or v2):
            continue
        root, point = Path(mount[3]), Path(mount[4])
        for is_v1, group in memberships:
            if is_v1 != v1:
                continue
            try:
                relative = Path(group).relative_to(root)
            except ValueError:
                continue
            node = point / relative
            while node.is_dir():
                if v1 and (node / 'cpu.cfs_quota_us').exists():
                    quota = int((node / 'cpu.cfs_quota_us').read_text())
                    period = int((node / 'cpu.cfs_period_us').read_text())
                    limits.append({'path': str(node), 'quota_us': quota, 'period_us': period})
                    if quota >= 0:
                        quotas.append(quota / period)
                elif v2 and (node / 'cpu.max').exists():
                    quota, period = (node / 'cpu.max').read_text().split()
                    limits.append({'path': str(node), 'quota_us': quota, 'period_us': int(period)})
                    if quota != 'max':
                        quotas.append(int(quota) / int(period))
                if node == point:
                    break
                node = node.parent
    model = next((line.split(':', 1)[1].strip() for line in Path('/proc/cpuinfo').read_text().splitlines()
                  if line.startswith('model name')), 'unknown')
    memory_limit = resource.getrlimit(resource.RLIMIT_AS)
    return {'hostname': os.uname().nodename, 'cpu_model': model, 'logical_cpu_affinity': affinity,
            'logical_cpus_by_physical_core': list(cores.values()),
            'address_space_limit_bytes': [None if value == resource.RLIM_INFINITY else value for value in memory_limit],
            'physical_cores_visible': len(representatives), 'representative_cpus': representatives,
            'cgroup_cpu_limits': limits, 'effective_cpu_quota': min(quotas) if quotas else None}


if __name__ == '__main__':
    import json
    print(json.dumps(cpu_resources(), indent=2))

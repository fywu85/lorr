#!/usr/bin/env python3
"""Add two declared prices to unchanged, independently verified CITY/GAME masks."""
import datetime
import hashlib
import json
from pathlib import Path
import re
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE=Path(__file__).resolve().parent
MAPS={'city':'mr24/city.domain/maps/Paris_1_256.map','game':'mr24/game.domain/maps/brc202d.map'}
def digest(data):return hashlib.sha256(data).hexdigest()
def main():
 records=[]
 for name,map_name in MAPS.items():
  header=ROOT/'cgar/tricks'/(name+'_native.hpp');original=header.read_text();updated=original
  get=lambda symbol:re.search(symbol+r'\[\] = "([0-9a-f]{64})"',original).group(1)
  rows,cols=map(int,re.search(name+r'_rows = (\d+), '+name+r'_cols = (\d+)',original).groups())
  masks=''.join(re.findall(r'"([0-9a-fx]+)"',original.split(name+'_masks[] =',1)[1].split(';',1)[0]))
  map_path=ROOT/map_name;assert digest(map_path.read_bytes())==get(name+'_map_sha256')
  cells=''.join(map_path.read_text().splitlines()[4:]);assert len(masks)==len(cells)==rows*cols
  occupancy=bytes(int(ch in '@T') for ch in cells);assert digest(occupancy)==get(name+'_occupancy_sha256')
  independent=[]
  for cell,wall in enumerate(occupancy):
   row,col=divmod(cell,cols);mask=(1<<(0 if row%2 else 2))|(1<<(1 if col%2 else 3))
   assert masks[cell]==('x' if wall else format(mask,'x'))
   independent.extend(int(not wall and (d==(0 if row%2 else 2) or d==(1 if col%2 else 3))) for d in range(4))
  fields={}
  for price in (4,8,12,16,20,24,28,32,48,64):
   field=bytes(price if against else 4 for against in independent);sha=digest(field);fields[str(price)]=sha
   symbol=name+('_field_sha256' if price==16 else '_lane'+str(price)+'_field_sha256')
   if price not in (20,28) or symbol in original:assert get(symbol)==sha,(name,price)
   else:
    anchor='inline constexpr char '+name+'_lane'+str(24 if price==20 else 32)+'_field_sha256'
    assert anchor in updated
    updated=updated.replace(anchor,'inline constexpr char '+symbol+'[] = "'+sha+'";\n'+anchor,1)
  # Apart from the two new identities the archived mask, metadata and old fields
  # must be byte-for-byte unchanged.
  stripped=updated
  for price in (20,28):stripped=re.sub(r'inline constexpr char '+name+'_lane'+str(price)+r'_field_sha256\[\] = "[0-9a-f]{64}";\n','',stripped)
  old_stripped=original
  for price in (20,28):old_stripped=re.sub(r'inline constexpr char '+name+'_lane'+str(price)+r'_field_sha256\[\] = "[0-9a-f]{64}";\n','',old_stripped)
  assert stripped==old_stripped
  header.write_text(updated)
  records.append(dict(name=name,rows=rows,cols=cols,map_sha256=digest(map_path.read_bytes()),previous_header_sha256=digest(original.encode()),header_sha256=digest(updated.encode()),fields_sha256=fields,old_fields_exact=True,independent_parity_and_occupancy=True,mask_unchanged=True,checked_values_per_field=len(independent)))
 (HERE/'asset-extension.json').write_text(json.dumps(dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),method='Add only20/28 hash identities to unchanged archived NMS masks; independent coordinate parity and occupancy; all prior field hashes checked.',records=records),indent=2)+'\n')
 print('CITY_GAME_NEARBY_FIELDS_VERIFIED',','.join(r['name'] for r in records))
if __name__=='__main__':main()

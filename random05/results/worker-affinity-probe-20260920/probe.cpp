#include <omp.h>
#include <sched.h>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <cstdio>
int main(int argc,char** argv) {
    if(argc!=2)return 2;
    std::set<int> allowed;std::istringstream stream(argv[1]);std::string word;
    while(std::getline(stream,word,','))allowed.insert(std::stoi(word));
    int bad=0;std::vector<int> observed(allowed.size());
    #pragma omp parallel num_threads(allowed.size()) reduction(+:bad)
    {
        cpu_set_t mask;CPU_ZERO(&mask);sched_getaffinity(0,sizeof(mask),&mask);
        int count=0;for(int c=0;c<CPU_SETSIZE;++c)if(CPU_ISSET(c,&mask)){++count;if(!allowed.count(c))++bad;}
        observed[omp_get_thread_num()]=sched_getcpu();
        #pragma omp critical
        std::printf("thread=%d cpu=%d mask_count=%d place=%d places=%d\n",omp_get_thread_num(),sched_getcpu(),count,omp_get_place_num(),omp_get_num_places());
        if(omp_get_proc_bind()!=omp_proc_bind_false && count!=1)++bad;
    }
    if(omp_get_proc_bind()!=omp_proc_bind_false && std::set<int>(observed.begin(),observed.end())!=allowed)++bad;
    return bad?1:0;
}

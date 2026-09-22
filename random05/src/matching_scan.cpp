// Four independent double-precision Hungarian relaxations at a time. Runtime
// dispatch keeps the solver portable; there is no fast-math or FMA contraction.
#include "engine.hpp"
#include <cstring>
#include <stdexcept>
#if (defined(__x86_64__) || defined(__i386__)) && defined(__GNUC__)
#include <immintrin.h>
#endif

namespace r05 {
bool matching_scan_simd_supported() {
#if (defined(__x86_64__) || defined(__i386__)) && defined(__GNUC__)
    return __builtin_cpu_supports("avx2");
#else
    return false;
#endif
}

#if (defined(__x86_64__) || defined(__i386__)) && defined(__GNUC__)
__attribute__((target("avx2")))
#endif
int matching_scan_simd(const float* costs,const double* dual,double active_dual,
                       const uint8_t* visited,double* distance,int* previous,
                       int columns,int predecessor,double& delta) {
#if (defined(__x86_64__) || defined(__i386__)) && defined(__GNUC__)
    const __m256d active=_mm256_set1_pd(active_dual);
    __m256d best=_mm256_set1_pd(1e30),indices=_mm256_setzero_pd();
    int j=1;
    for(;j+3<=columns;j+=4) {
        uint32_t flags;std::memcpy(&flags,visited+j,sizeof(flags));
        const __m128i available32=_mm_cmpeq_epi32(
            _mm_cvtepu8_epi32(_mm_cvtsi32_si128(int(flags))),_mm_setzero_si128());
        const __m256d available=_mm256_castsi256_pd(_mm256_cvtepi32_epi64(available32));
        const __m256d reduced=_mm256_sub_pd(
            _mm256_sub_pd(_mm256_cvtps_pd(_mm_loadu_ps(costs+j-1)),active),_mm256_loadu_pd(dual+j));
        const __m256d old=_mm256_loadu_pd(distance+j);
        const __m256d improve=_mm256_and_pd(available,_mm256_cmp_pd(reduced,old,_CMP_LT_OQ));
        const __m256d current=_mm256_blendv_pd(old,reduced,improve);
        _mm256_storeu_pd(distance+j,current);
        const int mask=_mm256_movemask_pd(improve);
        if(mask&1)previous[j]=predecessor;
        if(mask&2)previous[j+1]=predecessor;
        if(mask&4)previous[j+2]=predecessor;
        if(mask&8)previous[j+3]=predecessor;
        const __m256d better=_mm256_and_pd(available,_mm256_cmp_pd(current,best,_CMP_LT_OQ));
        best=_mm256_blendv_pd(best,current,better);
        indices=_mm256_blendv_pd(indices,_mm256_setr_pd(j,j+1,j+2,j+3),better);
    }
    double values[4],positions[4];_mm256_storeu_pd(values,best);_mm256_storeu_pd(positions,indices);
    int next=0;delta=1e30;
    // A lane keeps its first equal minimum. The reduction chooses the smallest
    // original column across lanes, exactly matching the ordered scalar scan.
    for(int k=0;k<4;++k) {
        const int index=int(positions[k]);
        if(values[k]<delta || (index && values[k]==delta && index<next)) {
            delta=values[k];next=index;
        }
    }
    for(;j<=columns;++j)if(!visited[j]) {
        const double reduced=double(costs[j-1])-active_dual-dual[j];
        if(reduced<distance[j]){distance[j]=reduced;previous[j]=predecessor;}
        if(distance[j]<delta){delta=distance[j];next=j;}
    }
    return next;
#else
    (void)costs;(void)dual;(void)active_dual;(void)visited;(void)distance;
    (void)previous;(void)columns;(void)predecessor;(void)delta;
    throw std::logic_error("SIMD matching called without runtime support");
#endif
}
}

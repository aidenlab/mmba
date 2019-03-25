#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;

void cpuMul(int *i,int *j,double *x,int m,double *v,int k,double *res, int max_ij, int thread_count) {
	int p;
	#pragma omp parallel for private(p) firstprivate(res)
	for (p=0;p<k;p++) 
		res[p] = 0;
	// memset(res, 0, k*sizeof(double));

	// max_ij+=10;
	int upper = (max_ij+16-1)/16;
	max_ij = upper*16;

	#pragma omp parallel firstprivate(res,x,v,i,j) private(p) num_threads(thread_count) 
	{
		int id = omp_get_thread_num();
		int total = omp_get_num_threads();
		int delta = max_ij/total;
		int begin = id * delta;
		int end = begin + delta;
		if(id == total-1) {
			end = max_ij-1;
			delta = end - begin+1;
		}
		double *ires;
		// #pragma omp critical
		ires = (double*) malloc(sizeof(double) * delta);

		// memset(ires, 0, delta*sizeof(double));
		for(p=0;p<delta;p++)
			ires[p] = 0;
		for (p=0;p<m;p++) {
			int ip = i[p];
			int jp = j[p];

			if(ip >= begin && ip < end) 
				ires[ip-begin] += x[p]*v[jp];
			
			if(jp >= begin && jp < end) 
				ires[jp-begin] += x[p]*v[ip];

		}
		for(p=0;p<delta;p++) 
			res[p+begin] = ires[p];
		// memcpy(res+p+begin, ires, sizeof(double)*delta);
		free(ires);
	}

}


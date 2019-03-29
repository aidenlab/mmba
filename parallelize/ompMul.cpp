#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;



void ompMul(int *i,int *j,double *x,int m,double *v,int k,double *res, int max_ij, int thread_count) {
	int p;
	// #pragma omp parallel for private(p) firstprivate(res)
	// for (p=0;p<k;p++) 
	// 	res[p] = 0;
	memset(res, 0, k*sizeof(double));



	#pragma omp parallel for firstprivate(x,v,i,j) private(p) num_threads(thread_count) reduction(+:res[0:k])
	for (p=0;p<m;p++) {
		// const int ip = i[p];
		// const int jp = j[p];
		// const double xx = x[p];

		// res[jp] += xx*v[ip];

		// res[ip] += xx*v[jp];


		res[j[p]] += x[p]*v[i[p]];
		res[i[p]] += x[p]*v[j[p]];
		
	}


/*
	#pragma omp parallel for firstprivate(res,x,v,i,j) private(p) num_threads(thread_count) 
	for (p=0;p<m/2;p++) {
		const int p0 = 2*p;
		const int p1 = 2*p+1;

		#pragma omp atomic update
		res[j[p0]] += x[p0]*v[i[p0]];

		#pragma omp atomic update
		res[j[p1]] += x[p1]*v[i[p1]];



		#pragma omp atomic update
		res[i[p0]] += x[p0]*v[j[p0]];
		
		#pragma omp atomic update
		res[i[p1]] += x[p1]*v[j[p1]];
	}
	if(m%2 == 1) {
		p = m/2*2+1;
		res[j[p]] += x[p]*v[i[p]];
		res[i[p]] += x[p]*v[j[p]];
	}

*/



/*
	max_ij+=10;
	const int upper = (max_ij+16-1)/16;
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

		// #pragma omp for schedule(static)
		for (p=0;p<m;p++) {
			int ip = i[p];
			int jp = j[p];

			if(jp >= begin && jp < end) 
				ires[jp-begin] += x[p]*v[ip];

			if(ip >= begin && ip < end) 
				ires[ip-begin] += x[p]*v[jp];
			
		}

		for(p=0;p<delta;p++) 
			res[p+begin] = ires[p];
		// memcpy(res+p+begin, ires, sizeof(double)*delta);
		free(ires);
	}
	*/
}


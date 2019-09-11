
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <real.h>


void utmvMulOmp(int *i,int *j,real *x,long m,real *v,int k,real *res) {
// void utmvMulOmp(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res) {
	long p;

	// #pragma omp parallel for private(p) firstprivate(res)
	// for (p=0;p<k;p++) 
	// 	res[p] = 0;
	memset(res, 0, k*sizeof(real));


/*
 * Normal approach
 */
// #if 0
	#pragma omp parallel for firstprivate(x,v,i,j) private(p) reduction(+:res[0:k]) 
	for (p=0;p<m;p++) {
		res[i[p]] += x[p]*v[j[p]];
		res[j[p]] += x[p]*v[i[p]];
	}
// #endif


/* 
 * Using extra arrays to minimize the cache thrashing effect
 */
#if 0
	#pragma omp parallel firstprivate(x,v,i,j) private(p) reduction(+:res[0:k]) 
	{
		real *res2;
		// #pragma omp critical
		res2 = (real*) malloc(sizeof(real) * k);
		memset(res2, 0, k * sizeof(real));

		#pragma omp for
		for (p=0;p<m;p++) {
			res2[i[p]] += x[p]*v[j[p]];
			res2[j[p]] += x[p]*v[i[p]];
		}

		for(p=0;p<k;p++)
			res[p] += res2[p];

		// #pragma omp critical
		free(res2);
	}
#endif

}


#ifndef BUILD_PYTHON_LIB
void utmvMul(int *i, int *j, real *x, long m, real *v, int k, real *res) {
	utmvMulOmp(i, j, x, m, v, k, res);
}
#endif

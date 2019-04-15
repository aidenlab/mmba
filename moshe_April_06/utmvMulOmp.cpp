
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void utmvMul(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res) {
	int ic,p;

	// #pragma omp parallel for private(p) firstprivate(res)
	// for (p=0;p<k;p++) 
	// 	res[p] = 0;
	memset(res, 0, k*sizeof(double));


/*
 * Normal approach
 */
// #if 0
	#pragma omp parallel for firstprivate(x,v,i,j) private(ic,p) reduction(+:res[0:k]) 
    for (ic=0;ic<c;ic++) {
    	for (p=0;p<m[ic];p++) {
            res[i[ic][p]] += x[ic][p]*v[j[ic][p]];
            res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
//                if (i[ic][p] < j[ic][p]) res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
    	}
	}
// #endif


/* 
 * Using extra arrays to minimize the cache thrashing effect
 */
#if 0
	#pragma omp parallel firstprivate(x,v,i,j) private(ic,p) reduction(+:res[0:k]) 
	{
		double *res2;
		// #pragma omp critical
		res2 = (double*) malloc(sizeof(double) * k);
		memset(res2, 0, k * sizeof(double));

		#pragma omp for
	    for (ic=0;ic<c;ic++) {
	    	for (p=0;p<m[ic];p++) {
	            res2[i[ic][p]] += x[ic][p]*v[j[ic][p]];
	            res2[j[ic][p]] += x[ic][p]*v[i[ic][p]];
	//                if (i[ic][p] < j[ic][p]) res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
	    	}
		}

		for(ic=0;ic<k;ic++)
			res[ic] += res2[ic];

		// #pragma omp critical
		free(res2);
	}
#endif


/*
 * Factoring out the common arrays
 */
#if 0
	#pragma omp parallel for firstprivate(x,v,i,j) private(ic,p) reduction(+:res[0:k]) 
    for (ic=0;ic<c;ic++) {
		int *pic = i[ic];
		int *pjc = j[ic];
		double *px = x[ic];
    	for (p=0;p<m[ic];p++) {
            res[pic[p]] += px[p]*v[pjc[p]];
            res[pjc[p]] += px[p]*v[pic[p]];
//                if (i[ic][p] < j[ic][p]) res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
    	}
	}
#endif
}


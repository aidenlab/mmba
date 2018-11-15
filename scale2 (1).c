#include <stdlib.h>
#include <math.h>

//	this function is used to sort rows sums
int cmpfunc (const void * a, const void * b) {
   return ( *(double*)a - *(double*)b );
}

/********************************************************************************************************************
*	m is the number of nonzero bins

*	i and j are the row and column indices of the nonzero bins and x is their counts; only upper triangular 
	part is needed; i and j are 0-based

*	z is the "target" vector, i.e. we want rows (and columns) sums to be equal to z

*	on exit b will hold the scaling vector, i.e. by multiplying the rows and columns of the original matrix 
	by be we get the scaled matrix; probably need to output 1/b to be compatible with bias vector

*	tol is the desired relative error

*	perc is the percentage of low rows sums to be excluded (i.e. 0.01, 0.02, etc.)

*	maxiter is the maximum number of iterations allowed

*	report contains the relative error after each iteration
***********************************************************************************************************************/

int scale1(int m,int *i,int *j,double *x, double *z,double *b, double tol,double perc,int maxiter, double *report) {  
	int k,p,n;
	double high, low, err;
//	will be allocated so need to be freed
	int *bad;
	double *r, *r0, *s, *one;	

//	find the matrix dimensions
	k = 0;
	for (p=0; p<m;p++) if (j[p] > k) k=j[p];
	k++;

	r = (double *) malloc(k*sizeof(double));
	r0 = (double *) malloc(k*sizeof(double));
	bad = (int *) malloc(k*sizeof(int));
	one = (double *) malloc(k*sizeof(double));
	s = (double *) malloc(k*sizeof(double));

//	find rows sums
	for (p=0;p<k;p++) r[p] = 0;
	for (p=0;p<m;p++) {
		r[i[p]] += x[p];
		if (i[p] < j[p]) r[j[p]] += x[p];
	}

//	find relevant percentiles
	for (p=0; p<k;p++) r0[p] = r[p];
	qsort(r0,k,sizeof(double),cmpfunc);
	n = 0;
	for (p=0;p<k;p++) if (r0[p] == 0) n++;
	int lind = n-1 + (int)(((double)(k-n))*perc+0.5);
	int hind = n-1 + (int)(((double)(k-n))*(1.0-0.1*perc)+0.5);
	if (lind < 0) lind = 0;
	if (hind >= k) hind = k-1;
	low = r0[lind];
	high = r0[hind];
	free(r0);

//	find the "bad" rows and exclude them
	for (p=0;p<k;p++) {
		if (r[p] < low || r[p] > high || isnan(z[p])) {
			bad[p] = 1;
			z[p] = 1.0;
		}
		else bad[p] = 0;
	}
	for (p=0;p<k;p++) b[p] = 1.0 - bad[p];
	for (p=0;p<k;p++) one[p] = 1.0 - bad[p];
	
//	start iterations
//	r is the current rows sum; s is the correction vector to be applied ti riws and columns
	err = 10.0*(1.0+tol);
	int iter=0;
	while(err > tol && iter++ < maxiter) {
		for (p=0;p<k;p++) if (bad[p] == 1) r[p] = 1.0;
		for (p=0;p<k;p++) s[p] = sqrt(z[p]/r[p]);
		for (p=0;p<k;p++) b[p] *= s[p];
	
		for (p=0;p<k;p++) r[p] = 0;
		for (p=0;p<m;p++) {
			r[i[p]] += x[p]*b[j[p]];
			if (i[p] < j[p]) r[j[p]] += x[p]*b[i[p]];
		}
		for (p=0;p<k;p++) r[p] *= b[p];
	
//	calculate the current relative error
		err = 0;
		for (p=0;p<k;p++) {
			if (bad[p] == 1) continue;
			if (fabs((r[p]-z[p])/z[p]) > err) err = fabs((r[p]-z[p])/z[p]);
		}
		report[iter-1]=err;
	}
	for (p=0;p<k;p++) if (bad[p] == 1) b[p] = NAN;

	free(bad);
	free(one);
	free(r);
	free(s);
	
	return(iter);	
}


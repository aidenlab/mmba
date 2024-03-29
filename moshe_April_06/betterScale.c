#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void utmvMul(int c, int **i,int **j,double **x,int *m,double *v,int k,double *es);

//	this function is used to sort rows sums
int cmpfunc (const void * a, const void * b) {
   if ( *(double*)a < *(double*)b ) return -1;
   if ( *(double*)a > *(double*)b ) return 1;
   return(0);
}

/********************************************************************************************************************
*
*	This function allows more that 2^31 - 1 nonzero entries. It acceptc a list of c arrays where array i contains m[i] elements
*

*	c is the number of arrays

*	m is array containing the number of elements of the c arrays

*	i and j are lists of c 0-based arrays each containing the row and column indices of the nonzero bins

*	x is a list of c arrays containing the nonzero matrix entries

*
*	i, j, and x define the upper triangle of the (squarre symmetric) matrix
*

*	z is the "target" vector, i.e. we want rows (and columns) sums to be equal to z

*	on exit b will hold the scaling vector, i.e. by multiplying the rows and columns of the original matrix 
	by b we get the scaled matrix; 

*	on exit report contains the relative error after each iteration

*
*	bekow are arguments having default values
*

*	verb indicates whether the function needs to output yhe progress; 1 means report, 0 means run silent

*	tol is the desired relative error

*	perc is the percentage of low rows sums to be excluded (i.e. 0.01, 0.02, etc.)

*	perc1 is the percentage of low and high values of z to ignore

*	maxiter is the maximum number of iterations allowed

*	del and trial are for determining that the convergence is too slow and early termination (before maxiter iteration): if 
	the relative error decreased by less than del for trials consecuitive iterations the call is terminated and -iter is 
	returned (where iter is the number of iterations); calling function can check the return value and know whether convergence 
	was reached
*
*	Note that making any optional argument negative causes the default value to be used
*
***********************************************************************************************************************/

int scale(int c, int *m,int **i,int **j,double **x, double *z,double *b, double report[][2],
int verb=0, double tol=1.0e-3,double perc=1.0e-2,double perc1=0.25e-2, int maxiter=200, double del=1.0e-2,int trials=5) 
{  
	if (tol < 0) tol=1.0e-3;
	if (perc < 0) perc=1.0e-2;
	if (perc1 < 0) perc=0.25e-2;
	if (maxiter < 0) maxiter=200;
	if (verb < 0) verb=0;
	if (del < 0) del=1.0e-2;
	if (trials < 0) trials=5;

	int k,p,ic,n;
	double high, low, err;
//	will be allocated so need to be freed
	int *bad, lind, hind;
	double *r, *r0, *s, *one, *zz;	

//	find the matrix dimensions
	k = 0;
	for (ic=0;ic<c;ic++) for (p=0; p<m[ic];p++) if (j[ic][p] > k) k=j[ic][p];
	k++;
	double *current = (double *) malloc(k*sizeof(double));
	r = (double *) malloc(k*sizeof(double));
	r0 = (double *) malloc(k*sizeof(double));
	bad = (int *) malloc(k*sizeof(int));
	int *bad1 = (int *) malloc(k*sizeof(int));
	one = (double *) malloc(k*sizeof(double));
	s = (double *) malloc(k*sizeof(double));
	zz = (double *) malloc(k*sizeof(double));
	int l = 0;
	for (p=0;p<k;p++) {
		if (isnan(z[p])) continue; 
		if (z[p] > 0) zz[l++] = z[p];
	}
	qsort(zz,l,sizeof(double),cmpfunc);
	lind = (int)(((double) l)*perc1+0.5);
	hind = (int)(((double) l)*(1.0-perc1)+0.5);
	if (lind < 0) lind = 0;
	if (hind >= l) hind = l-1;
	low = zz[lind];
	high = zz[hind];
	free(zz);
	for (p=0;p<k;p++) if (z[p] > 0 && (z[p] < low || z[p] > high)) z[p] = NAN;

	for (p=0;p<k;p++) one[p] = 1.0;
	for (p=0;p<k;p++) if (z[p] == 0) one[p] = 0;

//	find rows sums
	utmvMul(c,i,j,x,m,one,k,r);

//	find relevant percentiles
	for (p=0; p<k;p++) r0[p] = r[p];
	qsort(r0,k,sizeof(double),cmpfunc);
	n = 0;
	for (p=0;p<k;p++) if (r0[p] == 0) n++;
	lind = n-1 + (int)(((double)(k-n))*perc+0.5);
        hind = n-1 + (int)(((double)(k-n))*(1.0-0.1*perc)+0.5);

	if (lind < 0) lind = 0;
        if (hind >= k) hind = k-1;
	low = r0[lind];
	high = r0[hind];
	free(r0);

//	find the "bad" rows and exclude them
	for (p=0;p<k;p++) {
		if (((r[p] < low || r[p] > high) && z[p] > 0) || isnan(z[p])) {
//		if ((r[p] < low  && z[p] > 0) || isnan(z[p])) {
			bad[p] = 1;
			z[p] = 1.0;
		}
		else bad[p] = 0;
	}

	for (p=0;p<k;p++) b[p] = 1.0 - bad[p];
	for (p=0;p<k;p++) one[p] = 1.0 - bad[p];
	for (p=0;p<k;p++) if (z[p] == 0) one[p] = 0;
	for (p=0;p<k;p++) bad1[p] = 1.0 - one[p];
	
//	start iterations
//	r is the current rows sum; s is the correction vector to be applied to rows and columns
	err = 10.0*(1.0+tol);
	int iter=0;
	int stuck=0;
	double ber;
	for (p=0;p<k;p++) current[p] = b[p];
	while(err > tol && iter++ < maxiter) {
		for (p=0;p<k;p++) if (bad1[p] == 1) r[p] = 1.0;
		for (p=0;p<k;p++) s[p] = sqrt(z[p]/r[p]);
		for (p=0;p<k;p++) b[p] *= s[p];
	
		utmvMul(c,i,j,x,m,b,k,r);
		for (p=0;p<k;p++) r[p] *= b[p];
	
//	calculate the current relative error
		err = 0;
		ber=0;
		for (p=0;p<k;p++) {
			if (bad1[p] == 1) continue;
			if (fabs((r[p]-z[p])/z[p]) > err) err = fabs((r[p]-z[p])/z[p]);
			if (fabs(b[p]-current[p]) > ber) ber = fabs(b[p]-current[p]);
		}
		report[iter-1][0]=err;
		report[iter-1][1]=ber;
		if (verb) printf("%d: %30.15lf %30.15lf\n",iter,err,ber);
		for (p=0;p<k;p++) current[p] = b[p];
		if (iter < trials+2) continue;
		if (err > (1.0-del)*report[iter-2][0]) stuck++;
		else stuck = 0;
		if (stuck >= trials) break;
	}
	for (p=0;p<k;p++) if (bad[p] == 1) b[p] = NAN;
	free(bad);
	free(bad1);
	free(one);
	free(current);
	free(r);
	free(s);
	
	if (err > tol) return(-iter);	
	else return(iter);	
}


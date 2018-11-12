#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>

int scale1(int m,int *i,int *j,double *x, double *z,double *b, double tol,double perc,int maxiter, double *report);

int m1 = 140000123;
int maxiter = 200;
//double perc = 0.02;
double tol = 1.0e-3;
//char infile[] = "GM12878/bal_big_chr1_1K.h5";
//char infile[] = "GM12878/bal_big_chr1_5K.h5";
char outfile[] = "junk.bfile";

int main(int argc, char *argv[]) {
	int m,k,p,n,iter;
	int *i, *j, *y;
	double *x, *z;
	struct timeb t0,t1,start,end;
	ftime(&start);
	FILE *fin = fopen(argv[1],"r");
	FILE *finV = fopen(argv[2],"r");
	double perc = atof(argv[3]);

	double *report = (double *) malloc(maxiter*sizeof(double));
	
	i = (int *) malloc(m1*sizeof(int));
	j = (int *) malloc(m1*sizeof(int));
	x = (double *) malloc(m1*sizeof(double));
	z = (double *) malloc(m1*sizeof(double));
	ftime(&t0);
//	read the target vector
	n = 0;
	while(fscanf(finV,"%lf",&z[n]) == 1) n++;
//	allocate the scaling vector and initialise it to NaN
	double *b = (double *) malloc((n+1)*sizeof(double));
	fclose(finV);
	for (p=0;p<n;p++) b[p] = NAN;
//	read the matrix
	m = 0;
	while(fscanf(fin,"%d %d %lf",&i[m],&j[m],&x[m]) == 3) m++;
//	make rows and column indices 0-based
	for (p=0;p<m;p++) {
		i[p]--;
		j[p]--;
	}
	fclose(fin);
	ftime(&t1);
	printf("took %ld seconds to read %d records\n",(long) (t1.time - t0.time),m);

//	perform matrix scaling; keep in mind that target vector and bins counts will be altered so you may want to 
//	keep a copy of them
	iter = scale1(m,i,j,x, z,b, tol,perc,maxiter, report);
	ftime(&end);
	printf("took %ld seconds\n",(long) (end.time - start.time));
	printf("iterations took %15.10lf seconds\n",((double) (end.time - t1.time)) + 0.001*(end.millitm - t1.millitm) );
//	print the convergence report
	for (p=0;p<iter;p++) printf("%d: %30.15lg\n",p+1,report[p]);
//	print the scaling vector (you may want to print 1/b to be compatible with "normal" bias vector
	FILE *fout = fopen(outfile,"w");
	for (p=0;p<n;p++) fprintf(fout,"%30.15lf\n",b[p]);
	fclose(fout);

	return(n);		
}


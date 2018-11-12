#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(double*)a - *(double*)b );
}

// hard-coded values; should become parameters

int m1 = 2100000000;
int maxC = 100;
int maxiter = 200;
double perc = 0.03;
double tol = 1.0e-3;
char infile[] = "GM12878/big_srt.h5";
char outfile[] = "big_srt.bfile";

int main(void) {
	int *m,k,p,q,c,n,ibad;
	int **i, **j;
	double **x;
	struct timeb t0,t1,start,end;
	ftime(&start);
	FILE *fin = fopen(infile,"r");
	
	m = (int *) malloc(maxC*sizeof(int));
	int *y = (int *) malloc(m1*sizeof(int));

	c = 0;
	i = (int **) malloc(maxC*sizeof(int *));
	j = (int **) malloc(maxC*sizeof(int *));
	x = (double **) malloc(maxC*sizeof(double *));
	while(c < maxC) {
		ftime(&t0);
		i[c] = (int *) malloc(m1*sizeof(int));
		j[c] = (int *) malloc(m1*sizeof(int));
		k = 0;
		while(fscanf(fin,"%d %d %d",&i[c][k],&j[c][k],&y[k]) == 3 && k < m1) k++;
		for (p=0;p<k;p++) {
			i[c][p]--;
			j[c][p]--;
		}
		x[c] = (double *) malloc(k*sizeof(double));
		for (p=0; p<k; p++) x[c][p] = (double) y[p];
		m[c++] = k;
		ftime(&t1);
		printf("took %ld seconds to read %d records\n",(long) (t1.time - t0.time),k);
		if (k < m1) break;
	}
	fclose(fin);
	free(y);

	k = 0;
	for (q=0;q<c;q++) for (p=0; p<m[q];p++) if (j[q][p] > k) k=j[q][p];
	k++;
	double *r = (double *) malloc(k*sizeof(double));
	double *r0 = (double *) malloc(k*sizeof(double));
	for (p=0;p<k;p++) r[p] = 0;
	for (q=0; q<c; q++) for (p=0;p<m[q];p++) {
		r[i[q][p]] += x[q][p];
		if (i[q][p] < j[q][p]) r[j[q][p]] += x[q][p];
	}

	for (p=0; p<k;p++) r0[p] = r[p];
	qsort(r0,k,sizeof(double),cmpfunc);
	n = 0;
	while(r0[n] == 0) n++;
	double low = r0[n-1+(int)((k-n)*perc)];
	double high = r0[n-1+(int)((k-n)*(1.0-0.1*perc))];
	int *bad = (int *) malloc(k*sizeof(int));
	for (p=0;p<k;p++) {
		if (r[p] < low || r[p] > high) bad[p] = 1;
		else bad[p] = 0;
	}
	double *b = (double *) malloc(k*sizeof(double));
	for (p=0;p<k;p++) b[p] = 1.0;
	double *one = (double *) malloc(k*sizeof(double));
	for (p=0;p<k;p++) {
		if (bad[p] == 1) one[p] = 0;
		else one[p] = 1.0;
	}
	
	double *s = (double *) malloc(k*sizeof(double));
	double mm, MM, err = 10.0*(1.0+tol);
	int iter=0;
	while(err > 0.001 && iter++ < maxiter) {
		for (p=0;p<k;p++) if (bad[p] == 1) r[p] = 1.0;
		for (p=0;p<k;p++) s[p] = 1.0/sqrt(r[p]);
		for (p=0;p<k;p++) if (bad[p] == 1) s[p] = 0;
		for (p=0;p<k;p++) b[p] *= sqrt(r[p]);
	
		for (q=0; q<c; q++) for (p=0;p<m[q];p++) x[q][p] = x[q][p]*s[i[q][p]]*s[j[q][p]];

		for (p=0;p<k;p++) r[p] = 0;
		for (q=0; q<c; q++) for (p=0;p<m[q];p++) {
			r[i[q][p]] += x[q][p]*one[j[q][p]];
			if (i[q][p] < j[q][p]) r[j[q][p]] += x[q][p]*one[i[q][p]];
		}
	
		mm = 1.0e9;
		MM = 0;
		for (p=0;p<k;p++) {
			if (bad[p] == 1) continue;
			if (r[p] < mm) mm = r[p];
			if (r[p] > MM) MM = r[p];
		}
		err = (MM-mm)/mm;	
		printf("%d: %30.15lg\n",iter,err);
		fflush(stdout);
	}
	ftime(&end);
	printf("took %ld seconds\n",(long) (end.time - start.time));
	FILE *fout = fopen(outfile,"w");
	for (p=0;p<k;p++) {
		if (bad[p] == 0) fprintf(fout,"%30.15lf\n",b[p]);
		else fprintf(fout,"NaN\n");
	}
	fclose(fout);

	return(n);		
}


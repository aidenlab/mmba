#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>

int scale(int c, int *m,int **i,int **j,double **x, double *z,double *b, double *report, int verb=0,double tol=1.0e-3,double perc=1e-2,double perc1=0.25e-2, int maxiter=200,double del=1.0e-2,int trials=5);

// hard-coded values; should become parameters
int maxC = 100;
double tol=5.0e-4;

int main(int argc, char *argv[]) {
	int *m,k,p,q,c,n,ibad,iter;
	int **i, **j;
	double **x;
	double *z, *z0;
	struct timeb t0,t1,start,end;
	if (argc < 4) {
	  printf("Usage: %s <infile> <vector_file> <outfile>\n", argv[0]);
	  printf("  <infile>: file containing tuples delineating sparse matrix in upper triangular format\n");
	  printf("  <vector_file>: vector to scale to (use all 1s for balancing)\n");
	  printf("  <outfile>: output file, will contain scaled matrix in sparse upper triangular format\n");
	  exit(1);
	}

        FILE *fin = fopen(argv[1],"r");
        FILE *finV = fopen(argv[2],"r");
        FILE *fout = fopen(argv[3],"w");

	if (fin == NULL) {
	  fprintf(stderr, "Error! File %s cannot be opened for reading\n", argv[1]);
	  exit(1);
	}
	if (finV == NULL) {
	  fprintf(stderr, "Error! File %s cannot be opened for reading\n", argv[2]);
	  exit(1);
	}
	if (fout == NULL) {
	  fprintf(stderr, "Error! File %s cannot be opened for writing\n", argv[3]);
	  exit(1);
	}

	ftime(&start);
        int m1 = (int) 7e8; 
	if (argc > 4) m1 = (int) atof(argv[4]);
        double perc = 1.0e-2;
        if (argc > 5) perc = atof(argv[5]);
	int verb=1;
	if (argc > 6) verb = atoi(argv[6]);
	int maxiter=300;

	
	m = (int *) malloc(maxC*sizeof(int));
	z = (double *) malloc(m1*sizeof(double));
	z0 = (double *) malloc(m1*sizeof(double));

	c = 0;
	i = (int **) malloc(maxC*sizeof(int *));
	j = (int **) malloc(maxC*sizeof(int *));
	x = (double **) malloc(maxC*sizeof(double *));
	while(c < maxC) {
		ftime(&t0);
		i[c] = (int *) malloc(m1*sizeof(int));
		j[c] = (int *) malloc(m1*sizeof(int));
		x[c] = (double *) malloc(m1*sizeof(double));
		k = 0;
		while(fscanf(fin,"%d %d %lf",&i[c][k],&j[c][k],&x[c][k]) == 3) {
			k++;
			if (k == m1) break;
		}
		if (k == 0) break;	
		for (p=0;p<k;p++) {
			i[c][p]--;
			j[c][p]--;
		}
		m[c++] = k;
		ftime(&t1);
		printf("took %ld seconds to read %d records\n",(long) (t1.time - t0.time),k);
		if (k < m1) break;
	}
	fclose(fin);
	printf("finished reading\n");
        n = 0;
        while(fscanf(finV,"%lf",&z0[n]) == 1) n++;
	for (p=0;p<n;p++) z[p] = z0[p];
        double *b = (double *) malloc((n+1)*sizeof(double));
	for (p=0;p<n;p++) b[p] = NAN;
        fclose(finV);
	
	double *report = (double *) malloc(maxiter*sizeof(double));

	ftime(&t1);
//	iter = scale(c, m,i,j,x, z,b, tol,perc,perc1,maxiter, report,verb);
	iter = scale(c, m,i,j,x, z,b,report,verb);

        ftime(&end);
        printf("took %ld seconds\n",(long) (end.time - start.time));
        printf("iterations took %15.10lf seconds\n",((double) (end.time - t1.time)) + 0.001*(end.millitm - t1.millitm) );
        if (!verb) for (p=0;p<abs(iter);p++) printf("%d: %30.15lg\n",p+1,report[p]);
	int count=0;
	while (iter < 0 && count++ <3) {
		printf("Did not converge!!!\n");
		perc=1.5*perc;
		printf("new perc = %lg\n",perc);
		for (p=0;p<n;p++) z[p] = z0[p];
		ftime(&t1);
//		iter = scale(c, m,i,j,x, z,b, tol,perc,perc1,maxiter, report,verb);
		iter = scale(c, m,i,j,x, z,b,report,verb,-1,perc,0.005);
        	ftime(&end);
        	printf("iterations took %15.10lf seconds\n",((double) (end.time - t1.time)) + 0.001*(end.millitm - t1.millitm) );
        	if (!verb) for (p=0;p<abs(iter);p++) printf("%d: %30.15lg\n",p+1,report[p]);
	}
        for (p=0;p<n;p++) fprintf(fout,"%30.15lf\n",b[p]);
        fclose(fout);
	if (count >= 3) printf("still did not converge!!!\n");
        return(iter);
}


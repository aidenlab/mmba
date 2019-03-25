#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#include "timing.h"

extern void simpleMul(int *i,int *j,double *x,int m,double *v,int k,double *res);
extern void cpuMul(int *i,int *j,double *x,int m,double *v,int k,double *res,int max_ij, int thread_count);
extern void cudaMul(int *i,int *j,double *x,int m,double *v,int k,double *res, int max_ij, int thread_count, int Q, double simpleTime);


int main(int argc, char *argv[]) {
  int m,m1,k,p;
  int *i, *j;
  double *x;
  // struct timeb t0,t1;
  // struct timeb tm0,tm1;
  
  FILE *fin = fopen(argv[1],"r");
  m1 = (int) atof(argv[2]);
  int thread_count = atoi(argv[3]);
  printf("m1: %d\n", m1);
  
    i = (int *) malloc(m1*sizeof(int));
    j = (int *) malloc(m1*sizeof(int));
    x = (double *) malloc(m1*sizeof(double));


  m = 0;
  int max_ij = 0;
  k = 0;
  while(m < m1 && fscanf(fin,"%d %d %lf",&i[m],&j[m],&x[m]) == 3) { 
    i[m]--;
    j[m]--;
    if(i[m] > max_ij)
      max_ij = i[m];
    if(j[m] > max_ij)
      max_ij = j[m];
    if (j[m] > k) 
      k=j[m];
    if (j[m] == i[m]) 
      x[m] *= 0.5;
    m++;
  }
  fclose(fin);
  printf("finished reading %d records\n",m);
  // for (p=0;p<m;p++) {
  //     i[p]--;
  //     j[p]--;
  //   }

  // k = 0;
  // for (p=0; p<m;p++) if (j[p] > k) k=j[p];
  k++;

  // for (p=0; p<m;p++) if (j[p] == i[p]) x[p] *= 0.5;

  printf("k = %d\n", k);

  double *b = (double *) malloc(k*sizeof(double));
  double *res = (double *) malloc(k*sizeof(double));
  double *newres = (double *) malloc(k*sizeof(double));

  srand(1234567);
  for (p=0;p<k;p++) b[p] = drand48();

  int Q = 20;
  // ftime(&t0);
  timingBegin();
  for(int q=0;q<Q;q++)
    simpleMul(i,j,x,m,b,k,res);
  // ftime(&t1);
  timingEnd();

  // double simpleTime = (t1.time - t0.time)*1000.0 + (t1.millitm - t0.millitm);
  double simpleTime = timingCount();
  printf("Single-threaded multiplication took %lg milliseconds\n",simpleTime/Q);

  printf("----------------------------------\n");

/*  now your function
*/
  // ftime(&tm0);
  timingBegin();
  for(int q=0;q<Q;q++)
    cpuMul(i,j,x,m,b,k,newres,max_ij,thread_count);
  // ftime(&tm1);
  timingEnd();

  // double timing = (tm1.time - tm0.time)*1000.0 + (tm1.millitm - tm0.millitm);
  double timing = timingCount();
  printf("Multi-threaded (OpenMP) multiplication took %lg milliseconds\n", timing/Q);

  double err = 0;
  for (p=0;p<k; p++) if (fabs(res[p] - newres[p]) > err) err = fabs(res[p] - newres[p]);
  printf("the error is %lg\n",err);
  printf("speedup is %.2f\n", simpleTime/timing);

  printf("----------------------------------\n");

  /*
    GPU section
  */
  cudaMul(i,j,x,m,b,k,newres,max_ij,thread_count,Q,simpleTime);

  err = 0;
  for (p=0;p<k; p++) 
    if (fabs(res[p] - newres[p]) > err) 
      err = fabs(res[p] - newres[p]);
  printf("the error is %lg\n",err);

  printf("----------------------------------\n");


  // ftime(&tm0);
  timingBegin();
  for(int q=0;q<Q;q++)
    cudaMul(i,j,x,m,b,k,newres,max_ij,thread_count,1,-1);
  // ftime(&tm1);
  timingEnd();

  // timing = (tm1.time - tm0.time)*1000.0 + (tm1.millitm - tm0.millitm);
  timing = timingCount();
  printf("CUDA w/ data transfer: multiplication took %lg milliseconds\n", timing/Q);
  printf("speedup is %.2f\n", simpleTime/timing);

  err = 0;
  for (p=0;p<k; p++) if (fabs(res[p] - newres[p]) > err) err = fabs(res[p] - newres[p]);
  printf("the error is %lg\n",err);


  return(0);
}

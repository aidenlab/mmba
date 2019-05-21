
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cuda.h>
#include <cuda_runtime.h>


#define UTMV_DEFAULT_VECTOR_SIZE 1024


template<class T>
__global__
void gpu_kernel_res_init(int k, T *res) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= k)
		return;

	res[index] = 0;
}


__device__ 
double atomicAddDouble(double* address, double val)
{
    unsigned long long int* address_as_ull =
                              (unsigned long long int*)address;
    unsigned long long int old = *address_as_ull, assumed;

    do {
        assumed = old;
        old = atomicCAS(address_as_ull, assumed,
                        __double_as_longlong(val +
                               __longlong_as_double(assumed)));

    // Note: uses integer comparison to avoid hang in case of NaN (since NaN != NaN)
    } while (assumed != old);

    return __longlong_as_double(old);
}

__global__
void gpu_kernel_utmv(int *m, int **i, int **j, double **x, double *v, double *res) {
	int ic = blockIdx.x;
	int *ii = i[ic];
	int *jj = j[ic];
	double *xx = x[ic];
	int mic = m[ic];

	int iter_count = (mic + blockDim.x - 1) / blockDim.x;

	// int index2 = blockIdx.x * blockDim.x + threadIdx.x;
	// if(index2 == 0)
	// 	printf("blockIdx.x = %d - iter_count = %d\n", blockDim.x, iter_count);


	int index = threadIdx.x;
	for(int p=0;p < iter_count;p++) {

		// int index = threadIdx.x + p*blockDim.x;
		index += blockDim.x;

		if(index >= mic)
			break;

		int ix = ii[index];
		int jx = jj[index];
		double xv = xx[index];

		// res[jx] += xx*v[ix];
		atomicAdd(&res[jx], xv*v[ix]);

		// res[ix] += xx*v[jx];
		atomicAdd(&res[ix], xv*v[jx]);
	}
}


__global__
void gpu_kernel_utmv_one_kernel_per_ic(int *m, int **i, int **j, double **x, double *v, double *res, int ic) {
	int *ii = i[ic];
	int *jj = j[ic];
	double *xx = x[ic];
	int mic = m[ic];

	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= mic)
		return;

	int ix = ii[index];
	int jx = jj[index];
	double xv = xx[index];

	// res[jx] += xx*v[ix];
	atomicAddDouble(&res[jx], xv*v[ix]);

	// res[ix] += xx*v[jx];
	atomicAddDouble(&res[ix], xv*v[jx]);
}


static int first_time_visit = 0;
static int **d_i = NULL;
static int **d_j = NULL;
static double **d_x = NULL;
static int *d_m = NULL;
static double *d_v = NULL;
static double *d_res = NULL;


template<class T>
inline
void mmbaCudaAllocMatrix(T **dev, int c, int *m) {
	T **temp = (T**) malloc(sizeof(T*) * c);
	for(int p=0;p<c;p++) {
		T *q;
		if(cudaMalloc((void**) &q, sizeof(T) * m[p]) != cudaSuccess) {
			printf("Unable to allocate memory!\n");
			exit(1);
		}
		temp[p] = q;
	}

	cudaMemcpy(dev, temp, sizeof(T*) * c, cudaMemcpyDefault);
	free(temp);
}


template<class T>
inline
void mmbaCudaMemcpy(T **to, T **from, int c, int *m) {
	T **temp = (T**) malloc(sizeof(T*) * c);
	cudaMemcpy(temp, to, sizeof(T*) * c, cudaMemcpyDefault);
	for(int i=0;i<c;i++) {
		cudaMemcpy(temp[i], from[i], sizeof(T) * m[i], cudaMemcpyDefault);
	}
	free(temp);
}


inline
void utmvCudaAlloc(int c, int **i, int **j, double **x, int *m, double *v, int k, double *res) {
	if(first_time_visit)
		return;

	first_time_visit = 1;

	if(cudaMalloc((void***) &d_i, sizeof(int*) * c) != cudaSuccess) {
		printf("Unable to allocate memory on line %d!\n", __LINE__);
		exit(1);
	}
	mmbaCudaAllocMatrix<int>(d_i, c, m);
	mmbaCudaMemcpy<int>(d_i, i, c, m);

	cudaMalloc((void***) &d_j, sizeof(int*) * c);
	mmbaCudaAllocMatrix<int>(d_j, c, m);
	mmbaCudaMemcpy<int>(d_j, j, c, m);

	cudaMalloc((void***) &d_x, sizeof(double*) * c);
	mmbaCudaAllocMatrix<double>(d_x, c, m);
	mmbaCudaMemcpy<double>(d_x, x, c, m);



	cudaMalloc((void**) &d_m, sizeof(int) * c);
	cudaMemcpy(d_m, m, sizeof(int) * c, cudaMemcpyDefault);

	cudaMalloc((void**) &d_v, sizeof(double) * k);

	cudaMalloc((void**) &d_res, sizeof(double) * k);

}


// #define ST_COUNT 8

void utmvMulCuda(int c, int **i, int **j, double **x, int *m, double *v, int k, double *res) {
	int vector, gang;

	utmvCudaAlloc(c, i, j, x, m, v, k, res);

	cudaMemcpy(d_v, v, sizeof(double) * k, cudaMemcpyDefault);

	vector = UTMV_DEFAULT_VECTOR_SIZE;
	gang = (k+vector-1) / vector;
	gpu_kernel_res_init<double><<<gang, vector>>>(k, d_res);
	// cudaDeviceSynchronize();


/*
 * Considering all ic'es at once
 */
	vector = UTMV_DEFAULT_VECTOR_SIZE;
	gang = c;
	gpu_kernel_utmv<<<gang, vector>>>(d_m, d_i, d_j, d_x, d_v, d_res);


/*
 * Running a kernel per ic
 */
#if 0
	vector = 1024;
	gang = (m[0] + vector - 1) / vector;
	cudaStream_t st[ST_COUNT];
	for(int st_i=0;st_i < ST_COUNT;st_i++)
		cudaStreamCreate(&st[st_i]);
	for(int ic = 0;ic < c;ic+=ST_COUNT) {
		for(int qc=0;qc < ST_COUNT;qc++)
			gpu_kernel_utmv_one_kernel_per_ic<<<gang, vector, 0, st[qc]>>>(d_m, d_i, d_j, d_x, d_v, d_res, ST_COUNT*ic + qc);
	}
	// cudaDeviceSynchronize();
#endif

	cudaMemcpy(res, d_res, sizeof(double) * k, cudaMemcpyDefault);
}

#ifndef BUILD_PYTHON_LIB
void utmvMul(int c, int **i, int **j, double **x, int *m, double *v, int k, double *res) {
	utmvMulCuda(c, i, j, x, m, v, k, res);
}
#endif

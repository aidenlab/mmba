#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#include "timing.h"

// Includes CUDA
#include <cuda_runtime.h>

// Utilities and timing functions
// #include <helper_functions.h>    // includes cuda.h and cuda_runtime_api.h

// CUDA helper functions
// #include <helper_cuda.h>         // helper functions for CUDA error check

// Includes, kernels
// #include "simpleAtomicIntrinsics_kernel.cuh"


using namespace std;

__global__
void kernel_init_zero(double *res, int k) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= k)
		return;

	res[index] = 0;
}

__global__
void kernel_init_zero_2(double *res, double *res2, int k) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= k)
		return;

	res[index] = 0;
	res2[index] = 0;
}

__global__
void kernel_calc(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	//int end = index+for_loop_size;
	//for(int p=index;p<end;p++) {
	int p = index;
	int ix = i[p];
	int jx = j[p];
	double xx = x[p];

	// res[jx] += xx*v[ix];
	atomicAdd(&res[jx], xx*v[ix]);


	// res[ix] += xx*v[jx];
    atomicAdd(&res[ix], xx*v[jx]);		
	//}
}



__global__
void kernel_calc_2(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
        int index = blockIdx.x * blockDim.x + threadIdx.x;
        if(index >= m)
                return;

	int ix,jx;
	double xx;
	int p = index;
    int i1 = ix = i[p];
    int j1 = jx = j[p];
    double x1 = xx = x[p];
    int i2 = ix = i[p+1];
    int j2 = jx = j[p+1];
    double x2 = xx = x[p+1];
	
    ix = i1;
    jx = j1;
    xx = x1;
    atomicAdd(&res[ix], xx*v[jx]);
    ix = i2;
    jx = j2;
    xx = x2;
	atomicAdd(&res[ix], xx*v[jx]);


	ix = i1;
	jx = j1;
	xx = x1;
    atomicAdd(&res[jx], xx*v[ix]);
    ix = i2;
    jx = j2;
    xx = x2;
    atomicAdd(&res[jx], xx*v[ix]);
}



__global__
void kernel_calc_2for(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if(index >= m)
        return;

    int end = index+for_loop_size;
    for(int p=index;p<end;p++) {
        int ix = i[p];
        int jx = j[p];
        double xx = x[p];

        // res[ix] += xx*v[jx];
        atomicAdd(&res[ix], xx*v[jx]);
    }
    for(int p=index;p<end;p++) {
        int ix = i[p];
        int jx = j[p];
        double xx = x[p];

        // res[jx] += xx*v[ix];
        atomicAdd(&res[jx], xx*v[ix]);
	}	
}


__global__
void kernel_calc_res_ix(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int end = index+for_loop_size;
	for(int p=index;p<end;p++) {
		int ix = i[p];
		int jx = j[p];
		double xx = x[p];

		// res[ix] += xx*v[jx];
		atomicAdd(&res[ix], xx*v[jx]);

	}
}


__global__
void kernel_calc_res_jx(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int end = index+for_loop_size;
	for(int p=index;p<end;p++) {
		int ix = i[p];
		int jx = j[p];
		double xx = x[p];

		// res[jx] += xx*v[ix];
		atomicAdd(&res[jx], xx*v[ix]);
	}
}







__global__
void kernel_calc_with_shared(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int end = index+for_loop_size;
	__shared__ int shared_i[512];
	__shared__ int shared_j[512];
	for(int p=index;p<end;p++) {
		shared_i[threadIdx.x+p-index] = i[index];
		shared_j[threadIdx.x+p-index] = j[index];
	}

	__syncthreads();

	for(int p=index;p<end;p++) {
		int ix = shared_i[threadIdx.x+p-index];
		int jx = shared_j[threadIdx.x+p-index];
		double xx = x[p];

		// res[ix] += xx*v[jx];
		atomicAdd(&res[ix], xx*v[jx]);

		// res[jx] += xx*v[ix];
		atomicAdd(&res[jx], xx*v[ix]);
	}
}



__global__
void kernel_calc_with_shared_v(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int end = index+for_loop_size;

	__shared__ double shared_v_i[1024];
	__shared__ double shared_v_j[1024];
	// __shared__ double shared_res[512];
	for(int p=index;p<end;p++) {
		int index_internal = threadIdx.x+p-index;
		shared_v_i[index_internal] = v[i[p]];
		shared_v_j[index_internal] = v[j[p]];
		// shared_res[index_internal] = 0;
	}

	__syncthreads();

	for(int p=index;p<end;p++) {
		int index_internal = threadIdx.x+p-index;
		double xx = x[p];

		// res[ix] += xx*v[jx];
		atomicAdd(&res[i[p]], xx*shared_v_j[index_internal]);

		// res[jx] += xx*v[ix];
		atomicAdd(&res[j[p]], xx*shared_v_i[index_internal]);
	}
}




__global__
void kernel_calc_output_shared(int k, int m, int *i, int *j, double *x, double *v, double *res, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int index2 = index * for_loop_size;

	for(int p=0;p<m;p++) {
		int ix = i[p];
		int jx = j[p];

		if(ix >= index2 && ix < index2+for_loop_size) {
			double xx = x[p];
			res[ix] += xx*v[jx];
			// atomicAdd(&res[ix], xx*v[jx]);
		}

		if(jx >= index2 && jx < index2+for_loop_size) {
			double xx = x[p];
			res[jx] += xx*v[ix];
			// atomicAdd(&res[jx], xx*v[ix]);
		}
	}
}





__global__
void kernel_calc_sep_mem(int k, int m, int *i, int *j, double *x, double *v, double *res, double *res2, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= m)
		return;

	int end = index+for_loop_size;
	for(int p=index;p<end;p++) {
		int ix = i[p];
		int jx = j[p];
		double xx = x[p];

		// res[ix] += xx*v[jx];
		atomicAdd(&res[ix], xx*v[jx]);

		// res2[jx] += xx*v[ix];
		// atomicAdd(&res2[jx], xx*v[ix]);

	}
}

__global__
void kernel_calc_summation(int k, int m, double *res, double *res2, int for_loop_size) {
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	if(index >= k)
		return;

	int end = index+for_loop_size;
	for(int p=index;p<end;p++) {
		res[p] += res2[p];
	}
}







void cudaMul(int *i,int *j,double *x,int m,double *v,int k,double *res, int max_ij, int thread_count, int Q, double simpleTime) {
	int *d_i,*d_j;
	double *d_x,*d_v,*d_res,*d_res2;

	cudaMalloc((void**) &d_i, sizeof(int) * m);
	cudaMemcpy(d_i, i, sizeof(int)*m, cudaMemcpyDefault);
	cudaMalloc((void**) &d_j, sizeof(int) * m);
	cudaMemcpy(d_j, j, sizeof(int)*m, cudaMemcpyDefault);
	cudaMalloc((void**) &d_x, sizeof(double) * m);
	cudaMemcpy(d_x, x, sizeof(double)*m, cudaMemcpyDefault);
	cudaMalloc((void**) &d_v, sizeof(double) * k);
	cudaMemcpy(d_v, v, sizeof(double)*k, cudaMemcpyDefault);

	cudaMalloc((void**) &d_res, sizeof(double) * k);
	cudaMalloc((void**) &d_res2, sizeof(double) * k);

	int for_loop_size, delta;

	cudaStream_t st1, st2;
	cudaStreamCreate(&st1);
	cudaStreamCreate(&st2);

	// struct timeb tm0,tm1;
	// ftime(&tm0);
	timingBegin();
	for(int q=0;q<Q;q++) {
		// int p;
		// #pragma omp parallel for private(p)
		// for (p=0;p<k;p++) 
		// 	res[p] = 0;
		int vector = 1024;
		int gang = (k+vector-1) / vector;
		kernel_init_zero<<<gang, vector>>>(d_res, k);
		// kernel_init_zero<<<gang, vector>>>(d_res2, k);
		// kernel_init_zero_2<<<gang, vector>>>(d_res, d_res2, k);


		for_loop_size = 1;
		vector = 64;
		delta = for_loop_size * vector;
		gang = (m+delta-1) / delta;

		kernel_calc<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);
                
		// kernel_calc_2<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);

		//kernel_calc_2for<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);

		// delta should be less than 512
		// kernel_calc_with_shared<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);

		// kernel_calc_output_shared<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);


//		kernel_calc_res_ix<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);
//		kernel_calc_res_jx<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);

		// kernel_calc_with_shared_v<<<gang, vector>>>(k, m, d_i, d_j, d_x, d_v, d_res, for_loop_size);


		// kernel_calc_sep_mem<<<gang, vector, 0, st1>>>(k, m, d_i, d_j, d_x, d_v, d_res, d_res2, for_loop_size);
		// kernel_calc_sep_mem<<<gang, vector, 0, st2>>>(k, m, d_j, d_i, d_x, d_v, d_res2, d_res2, for_loop_size);
		// for_loop_size = 1;
		// vector = 1024;
		// delta = for_loop_size * vector;
		// gang = (k+delta-1) / delta;
		// cudaStreamSynchronize(st1);
		// cudaStreamSynchronize(st2);
		// kernel_calc_summation<<<gang, vector, 0, st1>>>(k, m, d_res, d_res2, for_loop_size);
		// cudaStreamSynchronize(st1);

		cudaDeviceSynchronize();
	}
	// ftime(&tm1);
	timingEnd();

	// cudaStreamSynchronize(st1);
	// cudaStreamSynchronize(st2);
	cudaStreamDestroy(st1);
	cudaStreamDestroy(st2);

	if(simpleTime > 0) { 
		// double timing = (tm1.time - tm0.time)*1000.0 + (tm1.millitm - tm0.millitm);
		double timing = timingCount();
		printf("CUDA multiplication took %lg milliseconds\n", timing/Q);
		printf("speedup is %.2f\n", simpleTime/timing);
	}

	cudaMemcpy(res, d_res, sizeof(double)*k, cudaMemcpyDefault);

	cudaFree(d_i);
	cudaFree(d_j);
	cudaFree(d_x);
	cudaFree(d_v);
	cudaFree(d_res);
	if(d_res2)
		cudaFree(d_res2);

}


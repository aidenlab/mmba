# utmv libraries

## Build

To build the libraries and executables, run `make`. 

After successfully building the whole package, you will have following libraries in this folder:

 1. `libutmvserial.so`: the serial implementation of the `utmv` function for a single core. **Source code: utmvMul.c** 
 2. `libutmvomp.so`: the multi-threaded implementation of the `uvmv` function for conventional processors (like Intel Xeon, AMD Athlon, and IBM Power9). **Source code: utmvMulOmp.cpp**
 3. `libutmvcuda.so`: the CUDA implementation of the `utmv` function on NVidia GPUs. **Source code: utmvMulCuda.cu**

And, you will also have following executables in the folder:

 1. `serialrun`: the serial execution of the main application with `libutmvserial.so` library. 
 2. `omprun`: the OpenMP execution of the main application with `libutmvserial.so` library. 
 3. `cudarun`: the CUDA execution of the main application with `libutmvserial.so` library. 

By running each one of above-mentioned executable files, you will run what their names imply. However, you can also override the behavior by overloading the libraries. For instance, the following line shows how you can run `serialrun` while targeting GPUs.

```
LD_PRELOAD=libutmvcuda.so ./serialrun <options>
```

This is particularly helpful when you don't want to compile the whole application for each architecture and approach. You would have one single executable file. In this case, we can only create the `serialrun` file and the three libraries. 

## OpenMP threads

To control the number of the OpenMP threads for the OpenMP version of the code, you can use the environmental variable supported by the OpenMP library. For instance, if you want to limit the execution to 4 threads, run following command:

```
export OMP_NUM_THREADS=4
```

**Note:** Please be aware to not to set this variable to more than number of available hardware cores. To find the total available number of hardware cores on a system, use `lscpu` command in Linux. One of the few last lines that starts with `NUMA node0 CPU(s):` specifies the available HW cores. 

## Dependencies

For the OpenMP version, we are using `array reduction` capability of OpenMP, which is supported in OpenMP 4.5+. 

OpenMP 4.5 is fully supported for C and C++ from GCC 6.1.

For CUDA compilation, the latest version of CUDA is recommended especially when dealing with latest devices from NVidia.






CFLAGS = -O3 -fopenmp
# CFLAGS = -O0 -g -fopenmp

CLIBS = -lm


CUDA_CFLAGS = -std c++11 -O3 -Xcompiler -fopenmp,-O3 -arch=compute_70
# CUDA_CFLAGS = -O0 -g -Xcompiler -fopenmp
CUDA_FLAGS += --use_fast_math


TARGETS=serialrun omprun cudarun

all: $(TARGETS)


serialrun: mainScale.o betterScale.o libutmvserial.so
	g++ $(CFLAGS) -o $@ mainScale.o betterScale.o $(CLIBS) -L. -lutmvserial

omprun: mainScale.o betterScale.o libutmvomp.so
	g++ $(CFLAGS) -o $@ mainScale.o betterScale.o $(CLIBS) -L. -lutmvomp


cudarun: mainScale.o betterScale.o libutmvcuda.so 
	g++ $(CFLAGS) -o $@ mainScale.o betterScale.o $(CLIBS) -L. -lutmvcuda



mainScale.o: mainScale.c
	g++ $(CFLAGS) -c $<

betterScale.o: betterScale.c
	g++ $(CFLAGS) -c $<



libutmvserial.so: utmvMul.c
	g++ $(CFLAGS) -fPIC -c $<
	g++ $(CFLAGS) -shared -o $@ utmvMul.o


libutmvomp.so: utmvMulOmp.cpp
	g++ $(CFLAGS) -fPIC -c $<
	g++ $(CFLAGS) -shared -o $@ utmvMulOmp.o


libutmvcuda.so: utmvMulCuda.cu
	nvcc $(CUDA_CFLAGS) -Xcompiler -fPIC -c $<
	nvcc $(CUDA_CFLAGS) -shared -o $@ utmvMulCuda.o






clean: clean-libs
	rm -rf *.o $(TARGETS)


clean-libs:
	rm -rf *.a *.so 



# g++ -O3 -fopenmp -o tmul.exe testMult.c simpleMul.c cpuMul.c -lm 

CFLAGS = -std=c++11 -O3 -fopenmp 
LIBS = -lm 
# LIBS += /usr/lib/x86_64-linux-gnu/libtcmalloc.so.4

CUDA_FLAGS = -std c++11 -O3 -Xcompiler -fopenmp -arch=compute_70
# CUDA_FLAGS += --prec-div=false --prec-sqrt=false 
CUDA_FLAGS += --use_fast_math


LINK_TARGET = tmul.exe
OBJS = testMult.o simpleMul.o cpuMul.o cudaMul.o


all: $(LINK_TARGET)
	@echo all done.

$(LINK_TARGET): $(OBJS)
	nvcc $(CUDA_FLAGS) -o $@ $^ $(LIBS)


%.o: %.c 
	g++ -O3 -fopenmp -o $@ -c $<

%.o: %.cu
	nvcc $(CUDA_FLAGS) -o $@ -c $<



clean:
	rm -rf *.o tmul.exe
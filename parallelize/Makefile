
# CFLAGS = -std=c++11 -O3 -fopenmp 
CFLAGS = -std=c++11 -O3 -fopenmp 
LIBS = -lm 
# LIBS += /usr/lib/x86_64-linux-gnu/libtcmalloc.so.4

CUDA_FLAGS = -std c++11 -O3 -ccbin /usr/bin/g++ -Xcompiler -O3,-fopenmp -arch=compute_70
# CUDA_FLAGS += --prec-div=false --prec-sqrt=false 
CUDA_FLAGS += --use_fast_math


LINK_TARGET = tmul.exe
OBJS = testMult.o simpleMul.o ompMul.o cudaMul.o


all: $(LINK_TARGET)
	@echo all done.

$(LINK_TARGET): $(OBJS)
	nvcc $(CUDA_FLAGS) -o $@ $^ $(LIBS)


%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $<

%.o: %.cu
	nvcc $(CUDA_FLAGS) -o $@ -c $<



clean:
	rm -rf *.o tmul.exe
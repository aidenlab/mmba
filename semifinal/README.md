Balancing functions and main programs for per chromosome and GW balancing of HiC files. For GW balancing it is now possible to choose between INTER only or full GW matrix.

To build executable which balances a single chromosome based on our SCALE:

**g++ -O2 --std=c++14 -o singleSK.exe d_balanceSingle.cpp getSingleMatrix.cpp d_SKBalance.c d_thMul.c d_ppNormVector.c ~/BCM/straw/C++/straw.cpp -I/home/moshe/BCM/straw/C++ -lz -lcurl -lpthread**

Run

**./singleSK.exe**

to see the usage.

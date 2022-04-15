Balancing functions and main programs for per chromosome and GW balancing of HiC files. For GW balancing it is now possible to choose between INTER only or full GW matrix.

To build executable which balances a single chromosome based on our SCALE:

**g++ -O2 --std=c++14 -o singleSK.exe d_balanceSingle.cpp getSingleMatrix.cpp d_SKBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**

where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./singleSK.exe**

to see the usage.


To build executable which balances a single chromosome based on Ruiz and Uçar symmetry preserving algorithm:

**g++ -O2 --std=c++14 -o singleRU.exe d_balanceSingle.cpp getSingleMatrix.cpp d_RUBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**


where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./singleRU.exe**

to see the usage.




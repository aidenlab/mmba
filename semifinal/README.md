Balancing functions and main programs for per chromosome and GW balancing of HiC files. For GW balancing it is now possible to choose between INTER only or full GW matrix.

To build an executable which balances a single chromosome based on our SCALE:

**g++ -O2 --std=c++0x -o singleSK.exe d_balanceSingle.cpp getSingleMatrix.cpp d_SKBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**

where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./singleSK.exe**

to see the usage.


To build an executable which balances a single chromosome based on Ruiz and Uçar symmetry preserving algorithm:

**g++ -O2 --std=c++0x -o singleRU.exe d_balanceSingle.cpp getSingleMatrix.cpp d_RUBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**


where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./singleRU.exe**

to see the usage.


To build an executable which balances a GW matrix (either INTER only or full) based on our SCALE:

**g++ -O2 --std=c++0x -o GW_SK.exe d_balGW.cpp getGWMatrix.cpp d_SKBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**

where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./GW_SK.exe**

to see the usage.



To build an executable which balances a GW matrix (either INTER only or full) based on our SCALE:

**g++ -O2 --std=c++0x -o GW_SK.exe d_balGW.cpp getGWMatrix.cpp d_SKBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**

where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./GW_SK.exe**

to see the usage.


To build an executable which balances a GW matrix (either INTER only or full) based on Ruiz and Uçar symmetry preserving algorithm:

**g++ -O2 --std=c++14 -o GW_RU.exe d_balGW.cpp getGWMatrix.cpp d_RUBalance.c d_thMul.c d_ppNormVector.c STRAW/straw.cpp -ISTRAW -lz -lcurl -lpthread**

where **STRAW** is the path to straw (straw.cpp and straw.h must be there).

Run

**./GW_RU.exe**

to see the usage.


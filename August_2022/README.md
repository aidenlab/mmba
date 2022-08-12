**Latest version of balancing**

To compile, make sure that you are using version 7 of gcc and g++. Then do:

**g++ -O2 -o d_singleRU1.exe d_balanceSingle.cpp d_thMul.c d_ppNormVector.c d_RUBalance1.c getSingleMatrix.cpp May_28_2022_straw/C++/straw.cpp -I May_28_2022/straw/C++ -lz -lcurl -lpthread**

where May_28_2022 is the folder with the May 28, 2022 version of straw.

Then use as normally. Run
**./d_singleRU1.exe**

to see the usage.

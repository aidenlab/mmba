**SK Balancing with Adaptive Percentage of Removed Rows**


To complie, make sure that you are using version 7 of g++ and then do:

**g++ -O2 -o balanceAdaptive.exe balanceSingleAdaptive.cpp adaptiveScale.c getSingleMatrix.cpp d_ppNormVector.c d_thMul.c location_of_May_2022_straw/C++/straw.cpp -I location_of_May_2022_straw/C++ -lz -lcurl -lpthread**

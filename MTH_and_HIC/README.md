Multi-thread functions for GW balancing or scaling of HiC contacts matrix.  

It comes in two flavours: either reads the GW contacts matrix from a file in sparse matrix format (bin_i, bin_j, count) or extracts it (with some additional information) from a hic file.  

To build an executable which reads the contacts matrix in triplets format do the following:  
g++ -O --std=c99 -o thScale.exe thMain.c thScale.c thMul.c -lpthread  
then run ./thScale.exe  
for short help.  
Pay attention to -z flag. Do not use it or specify -z 0 to be consistent with KR; put -z 1 to remove rows with 0 on the diagonal;  
-T specifies the number of threads (default is 1). For large files this can speed up the computation significantly.

The other (and more recommended) set of functions reads the contacts matrix, chromosome sizes, etc. directly from the hic file. To dio so it uses straw.cpp (and straw.h) from the straw project (which need to be downloaded as well).  
To create an executable do:  
g++ -O --std=c++0x -o createNormVector.exe createNormVector.cpp getMatrix.cpp straw.cpp hicScale.c thMul.c -I. -lz -lcurl -lpthread  
Then run  
./createNormVector.exe  
to see the help message.  
Note that now the target vector is optional. If present, scaling to this vector is performed, if absent, balancing is performed.  
The output file comes in a format which can be added to the hic file by juicer_tools addNorm. If balancing was performed, the name of the new norm is GW_BAL; if scaling was performed it is GW_SCAL.





Multi-thread functions for GW balancing or scaling of HiC contacts matrix.  

It comes in two flavours: either reads the GW contacts matrix from a file in sparse matrix format (bin_i, bin_j, count) or extracts it (with some additional information) from a hic file.  

To build an executable which reads the contacts matrix in triplets format do the following:  
**g++ -O --std=c99 -o thScale.exe thMain.c bestHicScale.c thMul.c -lpthread**  
then run ./thScale.exe  
for short help.  
Pay attention to -z flag. Do not use it or specify -z 0 to be consistent with KR; put -z 1 to remove rows with 0 on the diagonal;  
-T specifies the number of threads (default is 1). For large files this can speed up the computation significantly.

The other (**and more recommended**) set of functions reads the contacts matrix, chromosome sizes, etc. directly from the hic file. To dio so it uses straw.cpp (and straw.h) from the straw project (which need to be downloaded as well).  
To create an executable do:  
**g++ -O --std=c++0x -o createNormVector.exe createNormVector.cpp getMatrix.cpp straw.cpp bestHicScale.c ppNormVector.c thMul.c -I. -lz -lcurl -lpthread**  
Then run  
./createNormVector.exe  
to see the help message.  
Note that now the target vector is optional. If present, scaling to this vector is performed, if absent, balancing is performed.  
The output file comes in a format which can be added to the hic file by juicer_tools addNorm. If balancing was performed, the name of the new norm is GW_BAL; if scaling was performed it is GW_SCAL.  

There are also two functions for intrachromosme balancing and scaling:  
To balance every chromosome of HiC matrix, do:  
**g++ -O --std=c++0x -o createNormVectorIntra.exe createNormVectorIntra.cpp bestHicScale.c ppNormVector.c straw.cpp thMul.c getHiCInfo.cpp -I. -lz -lcurl -lpthread**  
Then run  
**./createNormVectorIntra.exe**  
to see help message.  
To balance or scale one particular chromosome of HiC matrix do:  
**g++ -O --std=c++0x -o createNormVectorIntraSingle.exe createNormVectorIntraSingle.cpp bestHicScale.c ppNormVector.c straw.cpp thMul.c getMatrix.cpp -I. -lz -lcurl -lpthread**  
Then run  
**./createNormVectorIntraSingle.exe**  
to see help message.  

The flags are:  
**-p**: fraction of nonzerp rows to remove, e.g. -p 0.005 to remove 0.5% of the rows; default: 0.01 but it is better to use a smaller number, say 1.0e-3 or even 1.4e-4 (will be updated automatically if there is no convergence).  
**-P**: delta_p - by how much to increase **p** if there is no convergence; default is 0.5e-2. Better to make it 0.5*p.  
**-q** and **-Q**: like **p** and **P** but for the percentage of lowest and highest values in the target vector to remove (for scaling only - ignored for balancing); default: 0.25e-2 and 1.0e-3.  
**-I**: maximum iterations (before updating **p** and **q**); default: 100; set it to a higher number if needed.  
**-A**: maximum total number of iterations; default: 200; set to a higher value if needed.  
**-T**: number of threads to use; default: 1; a very significant speed up is achieved with higher values (12 or 16 for really large data sets).  
**-z**: whether to remove rows with 0 at the diagonal (1 for yes, 0 for no); default: 0.  
**-t**: tolerance (stopping criterion); default: 5.0e-4; don't make it less than 1.0e-5 since we are using single precision (float).  
**-v**: how much output to print; 0 - no output, 1 (default) - summary; 2 - each iteration. 






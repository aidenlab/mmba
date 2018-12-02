# MMBA - Moshe's Matrix Balancing and Scaling Algorithm 

- `finalscale.c`: The function which performs matrix scaling; if the target vector is all 1 it performs matrix balancing.  
It allows matrices with arbitrarily many nonzero elements. The only limit is the RAM size.  
It has several mandatory arguments and additional optional arguments with default values. Setting any optional argument to a negative number results in default value being used.  
The mandatory arguments are:  

__c__ - the number of arrays holding row indexes, column indexes and values of the sparse matrix in upper-triangular form  
__m__ - an integer array of length __c__ hoding the number of elements in the c arrays  
__i__ - a list of __c__ arrays containing 0-based row indexes  
__j__ - a list of __c__ arrays containing 0-based column indexes  
__x__ - a list of __c__ arrays containing the matrix nonzero values  
__z__ - the target vector; if row i is a "good" row it will be scaled to have the sum of z[i]  
__b__ - on exit will contain the scaling vector for all "good" rows and NaN for "bad" rows; must be pre-allocated and contain at least as many elements as __z__.  
__report__ - on exit will contain the maximum (relative) errors after each iteration; must be pre-allocated and have at least __maxiter__ elements  

Optional arguments with default values:  

__verb__ - whether to output error after each iteration; 0 (default) for silent running and 1 for verbose mode  
__tol__ - the requested maximal relative error; default: 1.0e-3  
__perc__ - what percentage of lowest sum nonzerow rows to ignore; default: 0.01, i.e. 1%  
__perc1__ - what percentage of lowest and highest target vector values to ignore (has no effect for balancing); default: 0.25e-2  
__maxiter__ - maximum number of iterations; default: 200  
__del__ - the least error decrease thought at each iteration; default: 0.01  
__trials__ - how many consecuitive iterations may have less that __del__ decrease in error the function will exit prematurely if this number is surpassed); default: 5  

Return value:  
on successful call returns __iter__ - the number of iterations needed for convergence  
on unsuccessful call (premature termination) returns __-iter__, so if the return value is negative the function can be called again with higher __perc__ and/or __perc1__ and/or __maxiter__

- `mainScale.c`: An example of main function calling __scale__. It has several mandatory and several optional command line arguments. It also has one hard coded argument __maxC__ - the maximum number of arrays that can be allocated; currently it is 100; note that __maxC__*__m1__*16 should be less than your RAM size in bytes; only as many arrays as needed to store the matrix will be allocated  

Mandatory command line arguments:  

__fin__ - the file to read the matrix from; each row should contain tripletsL i, j, value where i and j are integers and represent 1-based row and column numbers (will be converted to 0-based values before the call to __scale__); number of rows and columns in the matrix will be the highest value of j
__finV__ - the file to read the target vector from (one value per line)
__fout__ - the file to output the scaling vector

Optional command line arguments:  

__m1__ - how big arrays to allocate; should be less than 2^31 - 1 = 2,147,483,647; default: 7e8 = 700,000,000  
__perc__ - the value of __perc__ to be passed to __scale__; default: 1.0e-2  
__verb__ - the value of __verb__ to pass to __scale__

A few comments:
* I do not scale the bias vector to have mean/median of 1 but this is trivial to do.
* Since all the values are integers I read them as such and then convert to double. This way reading in the matrix takes less time. Can be easily changed to read doubles.

# Compiling
`g++ -O3 -lm -o scale.a mainScale.c finalScale.c`

alternatively make a shared library by

`g++ -O3 -shared -c -lm -fPIC -o scale.so finalScale.c `

and then 

`g++ -O3 -o scale.a mainScale.c scale.so`

# Example of running 
./scale.a hg19_chr1_1K.h5 chr1_1K.scal chr1_1K.bvec  
or  
./scale.a hg19_chr1_1K.h5 chr1_1K.scal chr1_1K.bvec 2e8  - making __m1__ 200,000,000
etc.

# Utilities  
- `sbuild_big.R`: an R script to create genome-wige contacts matrix (in sparse upper triangular form) from a .hic file. The user needs to edit the file and make the below changes before running the scriot:  
__line 3__: change the path o where your straw-R.cpp file is  
__line 8__: choose the path to where your .hic file is  
__line 9__: change binsize to the desired one  
__line 10__: change path to where your chromosome lengths file is: the order of the chromosomes determines their order in the resulting contacts matrix; make sure that same order (same chromosome lengths file) is used when creating target vector.  
__line 20__: change the path to where you want the contacts matrix to be output to  

- `norm_vec.sh`: a bash script to build genome-wide target vector from .wig file  
__Running__  
source norm_vec.sh wigFile res genomeFile  
i.e.  
source norm_vec.sh GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.wig 1000 hg19.chrom.sizes  
__Notes:__  
genomeFile should be the same one you use to create contacts matrix  
res (second argument) should be a multiple of 1000  
__insert_norm_vector.awk__ must be in current directory  
__bedtools__ must be available  
it produces GM12878_
__res__
_kb_CTCF_normvector_1over.txt where res is the resolution in kb  
use __build_target_vector.R__ to build scaling vector  

- `norm_vec1.sh`: a stand-alone bash script to create genome-wide scale vector  
__Running example__  
source norm_vec1.sh GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.wig 1000 hg19.chrom.sizes  
produces hg19_'__res__'_kb.scal file where res is the resolution in kylobytes  
note that __bedtools__ must be installed


# Obsolete
- `scale1.c`  
- `scale2.c`
- `utesti.c`
- `myScale.c`

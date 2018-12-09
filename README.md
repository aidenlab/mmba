# MMBA - Moshe's Matrix Balancing and Scaling Algorithm 

Balance an upper triangular sparse matrix of any size (as long as you have enough RAM). Can do the entire genome at 1 kb resolution if sufficiently large percentage of "low" rows are excluded.

There are two C functions in two files:

- `zeroScale.c`: The function which performs matrix scaling; if the target vector is all 1 it performs matrix balancing.  
It allows matrices with arbitrarily many nonzero elements. The only limit is the RAM size.  
- `mainScale.c`: Driver function that calls `zeroScale`.

The main function takes in several optional arguments:
* **-m** the size of arrays that will be allocated; should be less than 2^31
* **-p** the percentage of low rows to be filtered out
* **-q** the percentage of highest and lowest nonzero values in target vector to be filtered out
* **-v** whether the main program should report anything (1 for yes, 0 for no)
* **-s** whether scale should be run in a silent mode (1 for yes and 0 for no). If scale is run in silent mode (which may be desirable) it produces no output and the convergence report is printed by the main function
* **-C** maximum number of arrays that may be allocated; note that `maxC`&#42;`m`&#42;16 should be less than your RAM size in bytes; only as many arrays as needed to store the matrix will be allocated
* **-t** how small the relative error in row sums needs to be
* **-d** the minimal percentage of decrease in row sums error at each iteration
* **-f** for how many consecutive iterations the decrease may be below what is specified by -d
* **-a** if there was no convergence, how many times to rerun scale each time increasing perc and perc1 by 50%; the 50% increase is still hard coded.
  

Mandatory command line arguments:  

* **fin** - the file to read the matrix from; each row should contain tripletsL i, j, value where i and j are integers and represent 1-based row and column numbers (will be converted to 0-based values before the call to `scale`); number of rows and columns in the matrix will be the highest value of j
* **finV** - the file to read the target vector from (one value per line)
* **fout** - the file to output the scaling vector

A comment:
* I do not scale the bias vector to have mean/median of 1 but this is trivial to do.

# Compiling
`g++ -O3 -lm -o scale.a mainScale.c zeroScale.c`

alternatively make a shared library by

`g++ -O3 -shared -c -lm -fPIC -o scale.so zeroScale.c `

and then 

`g++ -O3 -o scale.a mainScale.c scale.so`

# Example of running 
```
juicer_tools dump observed NONE https://hicfiles.s3.amazonaws.com/hiseq/gm12878/in-situ/combined.hic 1 1 BP 5000 chr1_5Kb.txt
awk '{$1=($1/5000) + 1; $2=($2/5000) + 1; print}' chr1_5Kb.txt  > tmp; mv tmp chr1_5Kb.txt
./bigWigToWig GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.bigWig -chrom=chr1 chr1.wig
awk 'BEGIN{m=0}$0!~/^#/{v=int($2/5000); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){print a[i]*25/5000}else{print 0}}}' chr1.wig > chr1.out
./scale.a chr1_5Kb.txt chr1.out myfile.out
```

`./scale.a hg19_chr1_1K.h5 chr1_1K.scal chr1_1K.bvec`  

or  

`./scale.a -m 2e8 hg19_chr1_1K.h5 chr1_1K.scal chr1_1K.bvec `

# Utilities  
- `sbuild_big.R`: an R script to create genome-wige contacts matrix (in sparse upper triangular form) from a .hic file. This version takes care of the case where __straw__ swaps thr order of input chromosomes. The user needs to edit the file and make the below changes before running the scriot:  
__line 3__: change the path o where your straw-R.cpp file is  
__line 11__: choose the path to where your .hic file is  
__line 12__: change path to where your chromosome lengths file is: the order of the chromosomes determines their order in the resulting contacts matrix; make sure that same order (same chromosome lengths file) is used when creating target vector.  
__line 22__: change the path to where you want the contacts matrix to be output to. If not it will be output to new_big_
__res__
K.h5 where res is binsize/1000.  
__Running__  
Rscript --vanilla sbuild_big.R binsize (i.e. Rscript --vanilla sbuild_big.R 10000 ).

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

- `build_target_vector.R`: to be used after running __norm_vec.sh__ 
- `insert_norm_vector.awk`: needed for __norm_vec.sh__

- `norm_vec1.sh`: a stand-alone bash script to create genome-wide scale vector  
__Running__  
source norm_vec1.sh wigFile res genomeFile outFile  
i.e.  
source norm_vec1.sh GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.wig 1000 hg19.chrom.sizes hg19_1kb.scal  
note that __bedtools__ must be installed

# Draft on Overleaf:
https://www.overleaf.com/2341499184ppwhnyfbyxqs


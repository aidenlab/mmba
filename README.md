# MMBA - Moshe's Matrix Balancing Algorithm 

* BETA Prototype *

Balance an upper triangular sparse matrix of any size (as long as you have enough RAM).
Can do the entire genome at 1 kb resolution if sufficiently large percentage of "low" rows are excluded.

There are two C functions in two files: 
* `myScale.c`: driver code which uses `scale2.c` 
* `scale2.c`: replaces scale1.c. Same functionality but better performance, some bugs fixed and the initial matrix is not altered.  

Obsolete:
* `scale1.c`: old version of scaling
* `utesti.c`: old version of file that calls scaling

Usage example:
```
juicer_tools dump observed NONE https://hicfiles.s3.amazonaws.com/hiseq/gm12878/in-situ/combined.hic 1 1 BP 5000 chr1_5Kb.txt
awk '{$1=$1/5000; $2=$2/5000; print}' chr1_5Kb.txt  > tmp; mv tmp chr1_5Kb.txt
./bigWigToWig GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.bigWig -chrom=chr1 chr1.wig
awk 'BEGIN{m=0}$0!~/^#/{v=int($2/5000); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){print a[i]*25/5000}else{print 0}}}' chr1.wig > chr1.out
./scale.a chr1_5Kb.txt chr1.out 0.02


The hard-coded values are: 
* `m1` - the maximum size of an array, i.e. number of nonzero elements (currently 140,000,123)
* `maxC` - how many such arrays toy allow to create (currently 100); consider your RAM size
* `maxiter` - maximum number of iterations (currently 200)
* `tol` - relative error allowed in sums of rows (currently 0.001)
* `outfile` - where to output the bias vector

It also expects 3 command line arguments:
the first one is the name of the file where the matrix is; it should contain triplets of i,j,x where i and j are the row and column indices and x is the counts number; please note that i and j are (base 1) indices and not genomic coordinates
the second argument is the file to read the target vector from; should have one value per row and contain at least as many entries as the matrix number of rows/columns (if it has more these values are ignored and the output vector will have them as NaNs)
the third argument is the percentage of nonzero rows/columns to be filtered out (put 0.02 for 2%, etc.)

A few comments:
* I do not scale the bias vector to have mean/median of 1 but this is trivial to do.
* Since all the values are integers I read them as such and then convert to double. This way reading in the matrix takes less time. Can be easily changed to read doubles.

# Compiling
`g++ -O3 -lm -o scale.a myScale.c scale2.c`

alternatively I make a shared library by

`g++ -O3 -shared -c -lm -o scale2.so scale2.c `

and then 

`g++ -O3 -o scale.a myScale.c scale2.so`

# Obsolete
* `scale1.c`
* `utesti.c`: at the moment a main function with hard-coded options; will be chenged to a proper function and driver routine. It allows to balance upper triangular sparse matrix of any size (as long as you have enough RAM), i.e. it lifts the limit of 2^31-1 on the number of non-zero elements. Will be adapted to do matrix scaling as well. Can do the entire genome at 1 kb resolution if sufficiently large percentage of "low" rows is excluded.
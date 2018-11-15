# MMBA - Moshe's Matrix Balancing Algorithm 

- `utesti.c`: at the moment a main function with hard-coded options; will be chenged to a proper function and driver routine. It allows to balance upper triangular sparse matrix of any size (as long as you have enough RAM), i.e. it lifts the limit of 2^31-1 on the number of non-zero elements. Will be adapted to do matrix scaling as well. Can do the entire genome at 1 kb resolution if sufficiently large percentage of "low" rows is excluded.

It is not a production quality one but rather a prototype. However I introduced one improvement. We are no longer limited to up to two matrices each with 2^31-1 non-zero entries. We may have as many of them as the RAM permits. Their number is determined automatically and all of them are treated automatically as well.
If you (or someone else) look at the code, at the top (before the program itself) there are several hard-coded definitions so each time you want to run the program with different values the program should be change and recompiled (on Linux I use: g++ -O3 -lm -o utesti.a utesti.c ). But it is very straightforward to convert this to a function so that all these hard-coded values become parameters.
The hard-coded values are: 
m1 - the maximum size of an array (currently 2.1e9)
maxC - how many such arrays toy allow to create (currently 100); consider your RAM size
maxiter - maximum number of iterations (currently 200)
perc - percentage of lowest row sum nonzero rows (and columns) to be excluded (currently 0.03)
tol - relative error allowed in sums of rows (currently 0.01)
infile - set to the file where you matrix in sparse format (row,column,value) is
outfile - where to output the bias vector

Few comments:
I do not scale the bias vector to have mean/median of 1 but this is trivial to do;
Since all the values are integers I read them as such and then convert to double. This way reading in the matrix takes less time. Can be easily changed to read doubles.

I ran this new program on the entire genome at 1 kb resolution. The entire process took 3675 seconds, 508 of which took reading in the matrix. It required 80 iterations.  When run on same data at 2 kb resolution (still 2.06 B non-zero entries) it took 1014 seconds, 434 of which were spent reading the matrix. It required only 22 iterations (removing only 2% of rows) - this is the reason for such a speed-up.
```


\b# Scaling\b

There are two C functions in two files: 
- `myScale.c`: driver code which uses `scale1.c` 
- `scale1.c`: the function which performs matrix scaling while myScale.c is an example of main program calling scale1.c.  
- `scale2.c`: replaces scale1.c. Same functionality but better performance, some bugs fixed and the initial matrix is not altered.  

At the moment all is hard-coded:
limit for the number of nonzero elements (`m1` - currently 140,000,123)
maximum number of iterations (`maxiter` - currently 200)
desired relative error (`tol` - currently 0.001)
file to output the scaling vector (`outfile` - currently `junk.bfile`)

It also expects 3 command line arguments:
the first one is the name of the file where the matrix is; it should contain triplets of i,j,x where i and j are the row and column indices and x is the counts number; please note that i and j are (base 1) indices and not genomic coordinates
the second argument is the file to read the target vector from; should have one value per row and contain at least as many entries as the matrix number of rows/columns (if it has more these values are ignored and the output vector will have them as NaNs)
the third argument is the percentage of nonzero rows/columns to be filtered out (put 0.02 for 2%, etc.)

# Compiling
`g++ -O3 -lm -o scale.a myScale.c scale1.c`

alternatively I make a shared library by

`g++ -O3 -shared -c -lm -o scale1.so scale1.c `

and then 

`g++ -O3 -o scale.a myScale.c scale1.so`

# Obsolete
- `scale1.c`

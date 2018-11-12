# MMBA - Moshe's Matrix Balancing Algorithm 

```
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

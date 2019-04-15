#!/bin/bash


./cudarun -m 1547859 -C 500  ../parallelize/chr1_1K.h5 hg19_2K.scal junk.junk > cuda.output

sleep 3

./omprun -m 1547859 -C 500  ../parallelize/chr1_1K.h5 hg19_2K.scal junk.junk > omp.output

sleep 3

./serialrun -m 1547859 -C 500  ../parallelize/chr1_1K.h5 hg19_2K.scal junk.junk > serial.output

#!/bin/bash
source /broad/software/scripts/useuse
use Java-1.7

for chr in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X
do
    echo $chr
    java -jar ./juicer_tools.jar dump observed $2 $1 $chr $chr BP 5000 | awk -v res=5000 -v chr=$chr 'BEGIN{print "fixedStep","chrom=chr"chr,"start=1","step=5000"}{$1=($1/res)+1; $2=($2/res)+1; a[$1]+=$3; if ($1!=$2){a[$2]+=$3} if ($1>m){m=$1} if($2>m){m=$2}}END{for (i=1;i<=m; i++){if (i in a){print a[i]}else {print 0}}}' >> $3
done

x=`awk -v res=$2 'BEGIN {print res/1000}'`
bedtools makewindows -g $3 -w $2 > hg19_${x}kb_windows.bed
bedtools map -a hg19_${x}kb_windows.bed -b $1 -c 4 -o sum | awk -v res=$2 'BEGIN{FS="\t";OFS="\t"}($4=="."){print "NaN"}($4!="."){print 1/($4/res*25)}' > $4


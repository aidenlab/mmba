x=`awk -v res=$2 'BEGIN {print res/1000}'`
bedtools makewindows -g $3 -w $2 > hg19_${x}kb_windows.bed
bedtools map -a hg19_${x}kb_windows.bed -b $1 -c 4 -o sum | awk -v res=$2 'BEGIN{FS="\t";OFS="\t"}($4=="."){print$1,$2,$3,$4}($4!="."){print $1,$2,$3,1/($4/res*25)}' >GM12878_${x}kb_CTCF_pre_normvector_1over.bed
awk -v res=$2 -f ./insert_norm_vector.awk GM12878_${x}kb_CTCF_pre_normvector_1over.bed | awk '$0~/^vector/{print $0; next}{print $4}' > GM12878_${x}kb_CTCF_normvector_1over.txt


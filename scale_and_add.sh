#!/bin/bash

chrs="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X"
resolutions="5000 10000 25000 50000 100000 250000 500000 1000000 2500000"
hicfile_target="GM_HiChIP_CTCF_hg19.hic"
hicfile="/HIC0000/HiSeq/MboI_magic_biorep/inter_30.hic"
juicer_tools="java -jar /aidenlab/juicer_tools.1.7.6_jcuda.0.8.jar"
fname1=$(basename -- "$hicfile")
fname2=$(basename -- "$hicfile_target")
filename="$(echo -e "${fname1%.*}_scaled_${fname2%.*}" | tr -d '[:space:]')"
rm -f ${filename}.txt

for chr in chrs
do
    # make this a command line options
    #${bigWigToWig} ${bigwigfile} -chrom=chr${chr} chr${chr}.wig
    for res in resolutions
    do
	# create source matrix for scaling: sparse, upper triangular
	${juicer_tools} dump observed NONE ${hicfile} $chr $chr BP $res | awk -v res=$res '{$1=($1/res)+1; $2=($2/res)+1; print}' > chr${chr}_${res}.txt

	# create target vector: row sums of target hic file
	${juicer_tools} dump observed NONE ${hicfile_target} $chr $chr BP $res | awk -v res=$res '{$1=($1/res)+1; $2=($2/res)+1; a[$1]+=$3; if ($1!=$2){a[$2]+=$3} if ($1>m){m=$1} if($2>m){m=$2}}END{for (i=1;i<=m; i++){if (i in a){print a[i]}else {print 0}}}' > chr${chr}_${res}_summed.txt 

 # raw ChIP-Seq vector binned at resolution                                                                       
#        awk -v res=$res 'BEGIN{m=0}$0!~/^#/{v=int($2/res); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){val=a[i]*25/res; print val;}else{print "NaN"}}}' chr${chr}.wig > chr${chr}.out

	# call scale; output is normalization vector
	./scale.a -q 0 chr${chr}_${res}.txt chr${chr}_${res}_summed.txt chr${chr}_scaled.out

	# calculate scaling constant factor (so sums of matrices remain same)                                            
        factor=$(awk 'BEGIN{count=1}(FNR==NR){if ($1==0 || $1=="nan"){array[count]="nan"}else{array[count]=1/$1;} count=count+1;}(FNR!=NR){if (array[$1]!="nan" && array[$2]!="nan" && array[$1]>0 && array[$2]>0){ value=$3/(array[$1]*array[$2]); norm_sum = norm_sum+value; sum=sum+$3; if ($1!=$2){norm_sum=norm_sum+value; sum=sum+$3;}}}END{print sqrt(norm_sum/sum)}' chr${chr}_scaled.out chr${chr}_${res}.txt)

        echo $factor

	# concatenate scaled normalization vector to file (needs to be 1/v)
        awk -v res=$res -v chr=$chr -v bw=$filename -v factor=$factor  'BEGIN{OFS="\t"; print "vector", bw,chr,res,"BP"}{if ($1=="nan"){$1="NaN"}else if ($1==0){$1="NaN"}else{$1=factor/$1;} print}' chr${chr}_scaled.out >> ${filename}.txt
    done 
done

${juicer_tools} addNorm $hicfile ${filename}.txt



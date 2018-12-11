#!/bin/bash
# Script for taking a wig file of transcription factor, scaling it, and adding it to a hic file
# Currently hardcoded bigwig and hic files to addNorm and juicer_tools command

hicfile=/Users/nchernia/Downloads/tmp.hic
bigwigfile=/Users/nchernia/Downloads/GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.bigWig
juicer_tools="java -jar /Users/nchernia/gits/Juicebox/out/artifacts/juicer_tools_jar/juicer_tools.jar " 
bigWigToWig=/Users/nchernia/Downloads/bigWigToWig
resolutions="5000 10000 25000 50000 100000 250000 500000 1000000 2500000"

fname1=$(basename -- "$hicfile")
fname2=$(basename -- "$bigwigfile")
filename="$(echo -e "${fname1%.*}_scaled_${fname2%.*}" | tr -d '[:space:]')"
rm -f ${filename}.txt
resolutions=1000000
#2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X Y
for chr in 1 
do
    ${bigWigToWig} ${bigwigfile} -chrom=chr${chr} chr${chr}.wig
    for res in $resolutions
    do
	x=`awk -v res=$res 'BEGIN {print res/1000}'`
	${juicer_tools} dump observed NONE $hicfile $chr $chr BP $res chr${chr}_${x}Kb.txt
	awk -v res=$res '{$1=($1/res) + 1; $2=($2/res) + 1; print}' chr${chr}_${x}Kb.txt  > tmp; mv tmp chr${chr}_${x}Kb.txt

	sum=$(awk '{sum+=$3; if ($1!=$2){sum+=$3}END{print sum}')

	# raw ChIP-Seq vector
	awk -v res=$res 'BEGIN{m=0}$0!~/^#/{v=int($2/res); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){val=a[i]*25/res; print val;}else{print "NaN"}}}' chr${chr}.wig > chr${chr}.out
	
	./scale.a chr${chr}_${x}Kb.txt chr${chr}.out chr${chr}_scaled.out

	# calculate scaling constant factor (so sums of matrices remain same)
	factor=$(awk 'BEGIN{count=1}(FNR==NR){array[count]=$1; count=count+1;}(FNR!=NR){if (array[$1]!="nan" && array[$2]!="nan"&& array[$1]>0 && array[$2]>0){ value=$3/(array[$1]*array[$2]); norm_sum = norm_sum+value; sum=sum+$3; if ($1!=$2){norm_sum=norm_sum+value; sum=sum+$3;}}}END{print sqrt(norm_sum/sum)}' chr${chr}_scaled.out chr${chr}_${x}Kb.txt)

	echo $factor
	awk -v res=$res -v chr=$chr -v bw=$filename -v factor=$factor  'BEGIN{OFS="\t"; print "vector", bw,chr,res,"BP"}{if ($1=="nan"){$1="NaN"}else{$1=$1*factor;} print}' chr${chr}_scaled.out >> ${filename}.txt
    done
done

${juicer_tools} addNorm $hicfile ${filename}.txt
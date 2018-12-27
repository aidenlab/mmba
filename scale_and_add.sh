#!/bin/bash

juicer_tools="java -Xmx16000m -jar /broad/aidenlab/neva/scaling/mmba/juicer_tools.jar "
bigWigToWig="/broad/aidenlab/neva/scaling/mmba/bigWigToWig"

if [[ $# -ne 2 ]]
then
    echo "Usage: $0 <target> <hic file>"
    echo " <target> should be a hic file or a bigWig file"
    echo " <hic file> will be altered to have a LOADED norm matching target"
    exit 1
fi

target=$1
hicfile=$2
fname1=$(basename -- "$hicfile")
fname2=$(basename -- "$target")

if [ "${fname1: -4}" != ".hic" ]
then
    echo "Usage: $0 <target> <hic file>"
    echo " <target> should be a hic file or a bigWig file"
    echo " <hic file> will be altered to have a LOADED norm matching target"
    exit 1
fi

chrs="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X"
resolutions="5000 10000 25000 50000 100000 250000 500000 1000000 2500000"

if [ "${fname2: -4}" == ".hic" ]
then
    isbigwig=0
elif [ "${fname2: -3}" == ".bw" ]
then
    isbigwig=1
elif [ "${fname2: -7}" == ".bigWig" ]
then
    isbigwig=1
else
    echo "Target $target does not appear to be a bigWig or hic file"
    echo "Usage: $0 <target> <hic file>"
    exit 1
fi 

filename="$(echo -e "${fname1%.*}_scaledby_${fname2%.*}" | tr -d '[:space:]')"
rm -f ${filename}.txt

for chr in $chrs
do
    # check if bigWig 
    # make this a command line options
    if [[ $isbigwig -eq 1 ]]
    then
	      ${bigWigToWig} ${target} -chrom=chr${chr} chr${chr}.wig
    fi

    for res in $resolutions
    do
        # create source matrix for scaling: sparse, upper triangular
	      ${juicer_tools} dump observed NONE ${hicfile} $chr $chr BP $res | awk -v res=$res '{$1=($1/res)+1; $2=($2/res)+1; print}' > chr${chr}_${res}.txt

	      if [[ $isbigwig -eq 1 ]]
	      then
	      # raw ChIP-Seq vector binned at resolution
            awk -v res=$res 'BEGIN{m=0}$0!~/^#/{v=int($2/res); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){val=a[i]*25/res; print val;}else{print "NaN"}}}' chr${chr}.wig > chr${chr}.out
	          # call scale; output is normalization vector
	          ./scale.a chr${chr}_${res}.txt chr${chr}.out scaled.out
	          rm chr${chr}.out
	      else
	          # create target vector: row sums of target hic file 
	          ${juicer_tools} dump observed NONE ${target} $chr $chr BP $res | awk -v res=$res '{$1=($1/res)+1; $2=($2/res)+1; a[$1]+=$3; if ($1!=$2){a[$2]+=$3} if ($1>m){m=$1} if($2>m){m=$2}}END{for (i=1;i<=m; i++){if (i in a){print a[i]}else {print 0}}}' > chr${chr}_${res}_summed.txt 
	          # call scale; output is normalization vector
	          ./scale.a chr${chr}_${res}.txt chr${chr}_${res}_summed.txt scaled.out
	          rm chr${chr}_${res}_summed.txt
	      fi

	# calculate scaling constant factor (so sums of matrices remain same)                                            
        factor=$(awk 'BEGIN{count=1}(FNR==NR){if ($1==0 || $1=="nan"){array[count]="nan"}else{array[count]=1/$1;} count=count+1;}(FNR!=NR){if (array[$1]!="nan" && array[$2]!="nan" && array[$1]>0 && array[$2]>0){ value=$3/(array[$1]*array[$2]); norm_sum = norm_sum+value; sum=sum+$3; if ($1!=$2){norm_sum=norm_sum+value; sum=sum+$3;}}}END{print sqrt(norm_sum/sum)}' scaled.out chr${chr}_${res}.txt)
        
        echo "Constant scale factor $factor"

	# concatenate scaled normalization vector to file (needs to be 1/v)
        awk -v res=$res -v chr=$chr -v bw=$filename -v factor=$factor  'BEGIN{OFS="\t"; print "vector", bw,chr,res,"BP"}{if ($1=="nan"){$1="NaN"}else if ($1==0){$1="NaN"}else{$1=factor/$1;} print}' scaled.out >> ${filename}.txt
	      rm scaled.out
    done 
done

${juicer_tools} addNorm $hicfile ${filename}.txt



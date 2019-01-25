#!/bin/bash

juicer_tools="java -Xmx16000m -jar /broad/aidenlab/neva/scaling/mmba/juicer_tools.jar "

if [[ $# -ne 1 ]]
then
    echo "Usage: $0 <hic file> <chrom sizes>"
    echo " <hic file> will be altered to have a LOADED norm with this balanced"
    exit 1
fi

hicfile=$1
fname1=$(basename -- "$hicfile")

chrs="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X"
resolutions="5000 10000 25000 50000 100000 250000 500000 1000000 2500000"
sizes=(249250621 243199373 198022430 191154276 180915260 171115067 159138663 146364022 141213431 135534747 135006516 133851895 115169878 107349540 102531392 90354753 81195210 78077248 59128983 63025520 48129895 51304566 155270560)

filename="$(echo -e "${fname1%.*}_scaledby_mmba" | tr -d '[:space:]')"
rm -f ${filename}.txt

for chr in $chrs
do
    echo "Chromosome $chr"

    for res in $resolutions
    do
        # create source matrix for scaling: sparse, upper triangular
	${juicer_tools} dump observed NONE ${hicfile} $chr $chr BP $res | awk -v res=$res '{$1=($1/res)+1; $2=($2/res)+1; print}' > chr${chr}_${res}.txt

	if [[ $chr == "X" ]]
	then
	    echo "${sizes[22]}" | awk -v res=$res '{len=int($1/res); for (i=0; i<=len; i++){print "1"}}' > chr${chr}.out 
	else
	    echo "${sizes[${chr}-1]}" | awk -v res=$res '{len=int($1/res); for (i=0; i<=len; i++){print "1"}}' > chr${chr}.out 
	fi

        # call scale; output is normalization vector
        # if the regular call fails, loosen parameters
        if ! ./scale.a -q 0 chr${chr}_${res}.txt chr${chr}.out scaled.out
        then
	    ./scale.a -p 0.04 -q 0.01 chr${chr}_${res}.txt chr${chr}.out scaled.out
        fi
	rm chr${chr}.out

	# calculate scaling constant factor (so sums of matrices remain same)                                            
        factor=$(awk 'BEGIN{count=1}(FNR==NR){if ($1==0 || $1=="nan"){array[count]="nan"}else{array[count]=1/$1;} count=count+1;}(FNR!=NR){if (array[$1]!="nan" && array[$2]!="nan" && array[$1]>0 && array[$2]>0){ value=$3/(array[$1]*array[$2]); norm_sum = norm_sum+value; sum=sum+$3; if ($1!=$2){norm_sum=norm_sum+value; sum=sum+$3;}}}END{print sqrt(norm_sum/sum)}' scaled.out chr${chr}_${res}.txt)
        
        echo "Constant scale factor $factor"

	# concatenate scaled normalization vector to file (needs to be 1/v)
        awk -v res=$res -v chr=$chr -v bw=$filename -v factor=$factor  'BEGIN{OFS="\t"; print "vector", bw,chr,res,"BP"}{if ($1=="nan"){$1="NaN"}else if ($1==0){$1="NaN"}else{$1=factor/$1;} print}' scaled.out >> ${filename}.txt
	      rm scaled.out
    done 
done

${juicer_tools} addNorm $hicfile ${filename}.txt



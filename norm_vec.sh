if [[ $# -ne 2 && $# -ne 1 ]]
then
    echo "Usage: $0 wigFile [res]"
    echo "  e.g. $0 GSM733752_hg19_wgEncodeBroadHistoneGm12878CtcfStdSig.wig 1000"
    echo "  all resolutions will be used if res not given"
    exit
fi

if [[ $# -eq 1 ]]
then
    resolutions="5000 10000 25000 50000 100000 250000 500000 1000000 2500000"
else
    resolutions=$2
fi


rm -f ${1}.out
for res in $resolutions
do
    awk -v res=$res -v bw=$1 'BEGIN{OFS="\t"; m=-1}$0!~/^#/{if ($1!=chr){for (i=0;i<=m;i++){if (i in a){val=a[i]*25/res; print 1/val;}else{print "NaN"}} delete a; m=0; chr=$1; print "vector", bw, chr, res, "BP";} v=int($2/res); a[v]+=$4; if (m<v){m=v}}END{for (i=0;i<=m;i++){if (i in a){val=a[i]*25/res; print 1/val;}else{print "NaN"}}}' $1 >> $1.out
done

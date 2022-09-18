BEGIN{
    temp="chr1"
    print "vector CTCF_ChIP_Seq chr1  "res" BP"
}

temp!=$1{
    temp=$1
    print "vector CTCF_ChIP_Seq "temp"  "res" BP"
}
$0

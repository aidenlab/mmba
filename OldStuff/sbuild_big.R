Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
library(Rcpp)
sourceCpp("~/HiC/straw/R/straw-R.cpp")

options(stringsAsFactors=FALSE)
options(scipen=10)

args <- commandArgs(trailingOnly = TRUE)
binsize <- as.numeric(args[1])

hicfile <-"/home/mols0001/HiC/GM12878/GM12878_insitu_combined_30.hic"
x <- read.table("~/HiC/hg19.chrom.sizes")
bad <- which(x[,1] == "chrM")
if (length(bad) > 0) x[bad,1] <- "chrMT"
chrs <- gsub("chr","",x[,1])
len <- as.numeric(x[,2])
n <- nrow(x)
off <- rep(0,n)
off[1] <- 1
for (i in 1:(n-1)) off[i+1] <- off[i] + ceiling(len[i]/binsize)

con <- file(paste0("new_big_",binsize/1000,"K.h5"),"w")
for (i in 1:n) for (j in i:n) {
	x <- straw_R(paste("NONE",hicfile,chrs[i],chrs[j],"BP",binsize))
	a <- as.numeric(max(x$x))
	b <- as.numeric(max(x$y))
	if ((a-b)*(len[i]-len[j]) < 0) {
		temp <- x$x
		x$x <- x$y
		x$y <- temp
	}
	x$x <- off[i] + floor(x$x/binsize)
	x$y <- off[j] + floor(x$y/binsize)
	write.table(x,con,append=TRUE,quote=FALSE,col.names=FALSE,row.names=FALSE)
	print(paste(chrs[i],chrs[j]))
}
close(con)

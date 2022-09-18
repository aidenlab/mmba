build_target_vector <- function(norm_vector_file,genome_file,target_vector_file) {
	options(stringsAsFactors=FALSE)
	x <- readLines(norm_vector_file)
	ind <- grep("chr",x)
	ind <- c(ind,length(x)+1)
	y <- read.table(genome_file)
	chrs <- y[,1]
	z <- character()
	for (chr in chrs) {j <- grep(paste0("\\b",chr,"\\b"),x[ind]); z <- c(z,x[(ind[j]+1):(ind[j+1]-1)])}
	z[z=="."] <- "NaN"
	writeLines(z,target_vector_file)
}

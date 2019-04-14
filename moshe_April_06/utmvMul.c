void utmvMul(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res) {
	int ic,p;
	for (p=0;p<k;p++) 
		res[p] = 0;
    for (ic=0;ic<c;ic++) 
    	for (p=0;p<m[ic];p++) {
            res[i[ic][p]] += x[ic][p]*v[j[ic][p]];
            res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
//                if (i[ic][p] < j[ic][p]) res[j[ic][p]] += x[ic][p]*v[i[ic][p]];
		}
}


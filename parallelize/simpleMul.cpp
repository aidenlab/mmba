void simpleMul(int *i,int *j,double *x,int m,double *v,int k,double *res) {
	int p;
	for (p=0;p<k;p++) res[p] = 0;
	for (p=0;p<m;p++) {
                res[i[p]] += x[p]*v[j[p]];
                res[j[p]] += x[p]*v[i[p]];
	}
}


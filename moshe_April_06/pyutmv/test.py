
import pyutmv
import numpy as np

i = np.arange(200).reshape(10,20)
x = np.arange(200).reshape(10,20)
m = np.arange(200)
v = np.arange(200)
res = np.arange(200)

# void utmvMulSerial(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res) {
pyutmv.pyutmvSerial(3,i,i,x,m,v,2,res)
pyutmv.pyutmvOmp(3,i,i,x,m,v,2,res)
pyutmv.pyutmvCuda(3,i,i,x,m,v,2,res)

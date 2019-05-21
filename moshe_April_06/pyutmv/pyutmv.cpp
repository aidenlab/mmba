
#include <Python.h>
//#include <iostream>
#include "numpy/arrayobject.h"


extern "C" void utmvMulSerial(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res);
void utmvMulOmp(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res);
extern void utmvMulCuda(int c, int **i,int **j,double **x,int *m,double *v,int k,double *res);

template <class T>
T **parseArrayObjectDoublePtr(PyArrayObject *vec_i) {

	int nn = vec_i->dimensions[1];
	T *tmp = (T*) vec_i->data;
	T **i = (T**) malloc(sizeof(T*) * (nn-1));
	for(int q=0;q<nn-1;q++,tmp+=nn)
		i[q] = tmp;

	return i;
}

// Function 1: A simple 'hello world' function
static PyObject* pyutmvSerial(PyObject* self, PyObject* args) {

	int c,k;
	PyArrayObject *vec_i, *vec_j, *vec_x, *vec_m, *vec_v, *vec_res;

    /* Parse tuples separately since args will differ between C fcns */
    if (!PyArg_ParseTuple(args, "iO!O!O!O!O!iO!",
    		&c,
    		&PyArray_Type, &vec_i,
    		&PyArray_Type, &vec_j,
			&PyArray_Type, &vec_x,
			&PyArray_Type, &vec_m,
			&PyArray_Type, &vec_v,
			&k,
			&PyArray_Type, &vec_res)) {

		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
        return Py_None;
    }

    if(vec_i == NULL || vec_j == NULL || vec_x == NULL || vec_m == NULL || vec_v == NULL || vec_res == NULL) {
		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
		return Py_None;
    }

	int **i = parseArrayObjectDoublePtr<int>(vec_i);
	int **j = parseArrayObjectDoublePtr<int>(vec_j);
	double **x = parseArrayObjectDoublePtr<double>(vec_x);
	int *m = (int*) vec_m->data;
	double *v = (double*) vec_v->data;
	double *res = (double*) vec_res->data;

	utmvMulSerial(c, i, j, x, m, v, k, res);

	free(i);
	free(j);
	free(x);
    return Py_None;
}


static PyObject* pyutmvOmp(PyObject* self, PyObject* args) {

	int c,k;
	PyArrayObject *vec_i, *vec_j, *vec_x, *vec_m, *vec_v, *vec_res;

	/* Parse tuples separately since args will differ between C fcns */
	if (!PyArg_ParseTuple(args, "iO!O!O!O!O!iO!",
						  &c,
						  &PyArray_Type, &vec_i,
						  &PyArray_Type, &vec_j,
						  &PyArray_Type, &vec_x,
						  &PyArray_Type, &vec_m,
						  &PyArray_Type, &vec_v,
						  &k,
						  &PyArray_Type, &vec_res)) {

		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
		return Py_None;
	}

	if(vec_i == NULL || vec_j == NULL || vec_x == NULL || vec_m == NULL || vec_v == NULL || vec_res == NULL) {
		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
		return Py_None;
	}

	int **i = parseArrayObjectDoublePtr<int>(vec_i);
	int **j = parseArrayObjectDoublePtr<int>(vec_j);
	double **x = parseArrayObjectDoublePtr<double>(vec_x);
	int *m = (int*) vec_m->data;
	double *v = (double*) vec_v->data;
	double *res = (double*) vec_res->data;

	utmvMulOmp(c, i, j, x, m, v, k, res);

	free(i);
	free(j);
	free(x);
	return Py_None;
}


static PyObject* pyutmvCuda(PyObject* self, PyObject* args) {

	int c,k;
	PyArrayObject *vec_i, *vec_j, *vec_x, *vec_m, *vec_v, *vec_res;

	/* Parse tuples separately since args will differ between C fcns */
	if (!PyArg_ParseTuple(args, "iO!O!O!O!O!iO!",
						  &c,
						  &PyArray_Type, &vec_i,
						  &PyArray_Type, &vec_j,
						  &PyArray_Type, &vec_x,
						  &PyArray_Type, &vec_m,
						  &PyArray_Type, &vec_v,
						  &k,
						  &PyArray_Type, &vec_res)) {

		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
		return Py_None;
	}

	if(vec_i == NULL || vec_j == NULL || vec_x == NULL || vec_m == NULL || vec_v == NULL || vec_res == NULL) {
		PyErr_SetString(PyExc_ValueError, "Unable to parse tuple!");
		return Py_None;
	}

	int **i = parseArrayObjectDoublePtr<int>(vec_i);
	int **j = parseArrayObjectDoublePtr<int>(vec_j);
	double **x = parseArrayObjectDoublePtr<double>(vec_x);
	int *m = (int*) vec_m->data;
	double *v = (double*) vec_v->data;
	double *res = (double*) vec_res->data;

	utmvMulCuda(c, i, j, x, m, v, k, res);

	free(i);
	free(j);
	free(x);
	return Py_None;
}


// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef pyutmvMethods[] = {
	{ "pyutmvSerial", pyutmvSerial, METH_VARARGS, "Python utmv Serial Wrapper" },
	{ "pyutmvOmp", pyutmvOmp, METH_VARARGS, "Python utmv OpenMP Wrapper" },
	{ "pyutmvCuda", pyutmvCuda, METH_VARARGS, "Python utmv CUDA Wrapper" },
	{ NULL, NULL, 0, NULL }
};


// Initializes our module using our above struct
PyMODINIT_FUNC initpyutmv() {
    Py_InitModule("pyutmv", pyutmvMethods);
	import_array();
}


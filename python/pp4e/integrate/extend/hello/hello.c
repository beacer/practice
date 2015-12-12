#include <Python.h>
#include <string.h>

static PyObject *
message(PyObject *self, PyObject *args)
{
    char *fromPython;
    char result[1024];

    if (!PyArg_Parse(args, "(s)", &fromPython))
        return NULL;
    else {
        strcpy(result, "Hello, ");
        strcat(result, fromPython);
        return Py_BuildValue("s", result);
    }
}

static PyMethodDef hello_methods[] = {
    {"message", message, METH_VARARGS, "func doc"},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDev hellomodule = {
    PyMethodDef_HEAD_INIT,
    "hello", "mod doc", -1, hello_methods,
};

PyMODINIT_FUNC
PyInit_hello()
{
    return PyModule_Create(&hellomodule);
}

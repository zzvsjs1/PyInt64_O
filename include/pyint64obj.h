#ifndef PY_INT64_H
#define PY_INT64_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyTypeObject PyInt64_Type;

typedef struct
{
    PyObject_HEAD

    int64_t ob_int64val;
} PyInt64Object;

// Public functions.
int64_t PyInt64_AsInt64(PyObject*);

PyObject* PyInt64_FromString(PyObject*);

PyObject* PyInt64_FromInt64(int64_t);

PyObject* PyInt64_FromPyInt64(PyObject*);

// Public Macros
#define PyInt64_Check(ob) (PyObject_TypeCheck(ob, &PyInt64_Type))
#define PyInt64_CheckExact(ob) (Py_IS_TYPE(ob, &PyInt64_Type))
#define PyInt64_GetValue(ob) (((PyInt64Object*)ob)->ob_int64val)

#ifdef __cplusplus
}
#endif
#endif // !PY_INT64_H
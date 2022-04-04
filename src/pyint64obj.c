#include <stdbool.h>

#include "pyint64obj.h"
#include "string_unitily.h"

/* 
 * Macro and helper that convert PyObject obj to a C int64t and store
 * the value in dbl.  If conversion to int64t raises an exception, obj is
 * set to NULL, and the function invoking this macro returns NULL.  If
 * obj is not of float or int type, Py_NotImplemented is incref'ed,
 *  stored in obj, and returned from the function invoking this macro.
 */
#define CONVERT_TO_INT64(obj, int64_val)             \
    if (PyInt64_Check(obj))                          \
        int64_val = PyInt64_AsInt64(obj);            \
    else if (convert_to_int64(&obj, &int64_val) < 0) \
        return obj;

#define CHECK_BINOP(v,w)                                \
    do {                                                \
        if (!PyInt64_Check(v) || !PyInt64_Check(w))     \
            Py_RETURN_NOTIMPLEMENTED;                   \
    } while(0)


// Method.

static int
convert_to_int64(PyObject **v, int64_t *val)
{
    PyObject* obj = *v;

    if (PyLong_Check(obj))
    {
        *val = PyLong_AsLongLong(obj);
        if (*val == -1 && PyErr_Occurred())
        {
            *v = NULL;
            return -1;
        }
    }
    else
    {
        Py_INCREF(Py_NotImplemented);
        *v = Py_NotImplemented;
        return -1;
    }

    return 0;
}

static PyObject *
pyint64_subtype_new(PyTypeObject *type, PyObject *x);

static PyObject*
pyint64_new_impl(PyTypeObject *type, PyObject *x);

static PyObject *
pyint64_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs);

static PyObject *
pyint64_repr(PyInt64Object *v);

static void
pyint64_dealloc(PyInt64Object *obj);

PyObject*
pyint64_richcompare(PyObject *self, PyObject *other, int op);

static Py_hash_t
pyint64_hash(PyInt64Object *v);


// START Number operations.
static PyObject*
pyint64_add(PyObject *left, PyObject *right);

static PyObject*
pyint64_sub(PyObject *left, PyObject *right);

static PyObject*
pyint64_mul(PyObject *left, PyObject *right);

static PyObject*
pyint64_div(PyObject *left, PyObject *right);

static PyObject*
pyint64_remainder(PyObject *left, PyObject *right);

static PyObject*
pyint64_divmod(PyObject *left, PyObject *right);

static PyObject*
pyint64_pow(PyObject *v, PyObject *w, PyObject *x);

static PyObject*
pyint64_negative(PyObject *v);

static PyObject*
pyint64_positive(PyObject *v);

static PyObject*
pyint64_absolute(PyObject *v);

static int
pyint64_bool(PyObject *v);

static PyObject*
pyint64_invert(PyObject *v);

static PyObject*
pyint64_lshift(PyObject *left, PyObject *right);

static PyObject*
pyint64_rshift(PyObject *left, PyObject *right);

static PyObject*
pyint64_and(PyObject *left, PyObject *right);

static PyObject*
pyint64_xor(PyObject *left, PyObject *right);

static PyObject*
pyint64_or(PyObject *left, PyObject *right);

static PyObject*
pyint64_int(PyObject *v);

static PyObject*
pyint64_float(PyObject *v);

// END Number operations

static PyModuleDef pyint64_module = 
{
    PyModuleDef_HEAD_INIT,
    .m_name = "pyint64",
    .m_doc = "A int64 object.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_pyint64()
{
    PyObject *this_module;
    if (PyType_Ready(&PyInt64_Type) < 0)
    {
        return NULL;
    }

    this_module = PyModule_Create(&pyint64_module);
    if (this_module == NULL)
    {
        return NULL;
    }

    Py_INCREF(&pyint64_module);

    if (PyModule_AddObject(this_module, "Pyint64", (PyObject*)&PyInt64_Type) < 0) 
    {
        Py_DECREF(&PyInt64_Type);
        Py_DECREF(this_module);
        return NULL;
    }

    return this_module;
}

// Number methods.
static 
PyNumberMethods pyint64_as_number = {
    .nb_add = pyint64_add,
    .nb_subtract = pyint64_sub,
    .nb_multiply = pyint64_mul,
    .nb_remainder = pyint64_remainder,
    .nb_divmod = pyint64_divmod,
    .nb_power = pyint64_pow,
    .nb_negative = (unaryfunc)pyint64_negative,
    .nb_positive = pyint64_positive,
    .nb_absolute = pyint64_absolute,
    .nb_bool = (inquiry)pyint64_bool,
    .nb_invert = pyint64_invert,
    .nb_lshift = pyint64_lshift,
    .nb_rshift = pyint64_rshift,
    .nb_and = pyint64_and,
    .nb_xor = pyint64_xor,
    .nb_or = pyint64_or,
    .nb_int = pyint64_int,
    .nb_float = pyint64_float,
};

static 
PyMethodDef pyint64_methods[] = 
{

    {NULL} /* sentinel */
};

static
PyGetSetDef pyint64_getset[] = 
{
    {NULL}  /* Sentinel */
};

// Type object.
PyTypeObject PyInt64_Type = 
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "pyint64.Pyint64",
    .tp_basicsize = sizeof(PyInt64Object),
    .tp_doc = "Python Int64 object",
    .tp_dealloc = (destructor)pyint64_dealloc,
    .tp_repr = (reprfunc)pyint64_repr,
    .tp_as_number = &pyint64_as_number,
    .tp_hash = (hashfunc)pyint64_hash,
    .tp_getattro = PyObject_GenericGetAttr,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_richcompare = pyint64_richcompare,
    .tp_methods = pyint64_methods,
    .tp_getset = pyint64_getset,
    .tp_new = pyint64_new,
};

int64_t PyInt64_AsInt64(PyObject* object)
{
    if (object == NULL) 
    {
        PyErr_BadInternalCall();
        return -1;
    }

    if (PyInt64_Check(object))
    {
        return ((PyInt64Object*)object)->ob_int64val;
    }

    PyLongObject* value = (PyLongObject *)PyNumber_Index(object);
    if (value == NULL)
    {
        return -1;
    }

    int64_t ret = PyLong_AsLongLong((PyObject*)value);
    if (ret == -1 && PyErr_Occurred())
    {
        return -1;
    }

    Py_DECREF(value);
    return ret;
}

PyObject*
PyInt64_FromInt64(int64_t value)
{
    PyInt64Object* obj = PyObject_Malloc(sizeof(PyInt64Object));
    if (!obj)
    {
        return PyErr_NoMemory();
    }

    PyObject_Init((PyObject*)obj, &PyInt64_Type);
    obj->ob_int64val = value;
    return (PyObject*)obj;
}

PyObject* 
PyInt64_FromPyInt64(PyObject* pyint64)
{
    if (PyInt64_CheckExact(pyint64))
    {
        Py_INCREF(pyint64);
    }
    else
    {

    }
        
    return pyint64;
}

PyObject* PyInt64_FromString(PyObject* string)
{
    if (!PyUnicode_Check(string))
    {
        PyErr_Format(PyExc_TypeError, 
            "The type must be str, not '%.200s'",
            Py_TYPE(string)->tp_name);
        return NULL;
    }

    // Call is digit
    if (!Py_IsTrue(PyObject_CallMethod(string, "isdigit", NULL)))
    {
        PyErr_Format(PyExc_ValueError, 
            "The str value must be digit, not '%.200S'",
            string);
        return NULL;
    }

    PyObject* py_int = PyLong_FromUnicodeObject(string, 10);
    if (!py_int)
    {
        return PyErr_NoMemory();
    }

    int64_t value = PyLong_AsLongLong(py_int);
    if (value == -1 && PyErr_Occurred())
    {
        Py_DECREF(py_int);
        return NULL;
    }

    Py_DECREF(py_int);
    return PyInt64_FromInt64(value);
}

static PyObject *
pyint64_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
    if (PyTuple_GET_SIZE(args) > 1)
    {
        PyErr_SetString(&PyErr_BadArgument, "Arguments more than 1.");
        return NULL;
    }

    PyObject* ret = NULL;
    PyObject* tuple_item = NULL;
    
    if (PyTuple_GET_SIZE(args) == 1)
    {
        // Get first item.
        tuple_item = PyTuple_GET_ITEM(args, 0);
    }    

    return pyint64_new_impl(subtype, tuple_item);
}

static PyObject *
pyint64_subtype_new(PyTypeObject *type, PyObject *x)
{
    PyObject* newObj = type->tp_alloc(type, 0);
    PyObject* temp = pyint64_new_impl(&PyInt64_Type, x);
    
    if (!newObj)
    {
        Py_DECREF(temp);
        return NULL;
    }

    ((PyInt64Object*) newObj)->ob_int64val = ((PyInt64Object*) temp)->ob_int64val;
    Py_DECREF(temp);
    return newObj;
}

static PyObject*
pyint64_new_impl(PyTypeObject *type, PyObject *input)
{
    if (type != &PyInt64_Type)
    {
        if (!input)
        {
            input = PyLong_FromLongLong(0);
        }

        return pyint64_subtype_new(type, input);
    }

    if (!input)
    {
        return PyInt64_FromInt64(0);
    }

    if (PyUnicode_Check(input))
    {
        return PyInt64_FromString(input);
    }

    return PyInt64_FromPyInt64(input);
}

static void
pyint64_dealloc(PyInt64Object *obj)
{
    Py_TYPE(obj)->tp_free((PyObject*)obj);
}

static PyObject *
pyint64_repr(PyInt64Object *v)
{
    char buffer[21];
    
    int64_t value = PyInt64_GetValue(v);
    char* bufferEnd = buffer + 21;
    char* next = bufferEnd;

    const uint64_t uvalue = (uint64_t)(value);

    PyObject* result = PyUnicode_FromStringAndSize(next, bufferEnd - next);
    if (!result)
    {
        return PyErr_NoMemory();
    }

    return result;
}


/* Pyint64 Number Methods */

static PyObject*
pyint64_add(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);
    return PyInt64_FromInt64(a + b);
}

static PyObject*
pyint64_sub(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);
    return PyInt64_FromInt64(a - b);
}

static PyObject*
pyint64_mul(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);
    return PyInt64_FromInt64(a * b);
}

static PyObject*
pyint64_div(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b == 0) 
    {
        PyErr_SetString(PyExc_ZeroDivisionError, "int64 division by zero");
        return NULL;
    }

    return PyInt64_FromInt64(a * b);
}

static PyObject*
pyint64_remainder(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b == 0) 
    {
        PyErr_SetString(PyExc_ZeroDivisionError, "int64 division by zero");
        return NULL;
    }

    return PyInt64_FromInt64(a % b);
}

static PyObject*
pyint64_divmod(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b == 0) 
    {
        PyErr_SetString(PyExc_ZeroDivisionError, "int64 division by zero");
        return NULL;
    }

    PyObject* div = PyInt64_FromInt64(a / b);
    PyObject* mod = PyInt64_FromInt64(a % b);
    PyObject* ret = PyTuple_New(2);

    if (!ret) 
    {
        Py_DECREF(div);
        Py_DECREF(mod);
        return NULL;
    }

    PyTuple_SET_ITEM(ret, 0, (PyObject *)div);
    PyTuple_SET_ITEM(ret, 1, (PyObject *)mod);
    return ret;
}

static int64_t
pyint64_pow_impl(const int64_t src, const int64_t n)
{
    if (n == 0)
    {
        return 1;
    }

    if (n == 1)
    {
        return src;
    }

    if (n & 1)
    {
        const int64_t temp = pyint64_pow_impl(src, (n - 1) / 2);
        return temp * temp * src;
    }

    const int64_t temp = pyint64_pow_impl(src, n / 2);
    return temp * temp;
}

static PyObject*
pyint64_pow(PyObject *v, PyObject *w, PyObject *x)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(v, a);
    CONVERT_TO_INT64(w, b);

    // Two arguments.
    if (!x)
    {
        return PyInt64_FromInt64(pyint64_pow_impl(a, b));
    }

    PyObject* long_a = PyLong_FromLongLong(a);
    if (!long_a)
    {
        return PyErr_NoMemory();
    }

    PyObject* long_b = PyLong_FromLongLong(b);
    if (!long_b)
    {
        Py_DECREF(long_a);
        return PyErr_NoMemory();
    }

    int64_t c;
    CONVERT_TO_INT64(x, c);

    if (c == 0)
    {
        PyErr_SetString(PyExc_ValueError, "pow() 3rd argument cannot be 0");
        return NULL;
    }

    PyObject* long_c = PyLong_FromLongLong(c);
    if (!long_c)
    {
        Py_DECREF(long_a);
        Py_DECREF(long_b);
        return PyErr_NoMemory();
    }

    PyObject* result = PyNumber_Power(long_a, long_b, long_c);
    Py_DECREF(long_a);
    Py_DECREF(long_b);
    Py_DECREF(long_c);

    if (!result)
    {
        return PyErr_NoMemory();
    }

    return result;
}

static PyObject*
pyint64_negative(PyObject *v)
{
    int64_t a;
    CONVERT_TO_INT64(v, a);
    return PyInt64_FromInt64(-a);
}

static PyObject*
pyint64_positive(PyObject *v)
{
    return PyInt64_FromPyInt64(v);
}

static PyObject*
pyint64_absolute(PyObject *v)
{
    int64_t a;
    CONVERT_TO_INT64(v, a);
    return PyInt64_FromInt64(a >= 0 ? a : -a);
}

static int
pyint64_bool(PyObject *v)
{
    int64_t a;
    return ((PyInt64Object*)v)->ob_int64val != 0;
}

static PyObject*
pyint64_invert(PyObject *v)
{
    int64_t a;
    CONVERT_TO_INT64(v, a)
    return PyInt64_FromInt64(~a);
}

static PyObject*
pyint64_lshift(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b < 0) 
    {
        PyErr_SetString(PyExc_ValueError, "negative shift count");
        return NULL;
    }

    if (b == 0) 
    {
        return PyInt64_FromInt64(0);
    }

    return PyInt64_FromInt64(a << b);
}

static PyObject*
pyint64_rshift(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b < 0) 
    {
        PyErr_SetString(PyExc_ValueError, "negative shift count");
        return NULL;
    }

    if (b == 0) 
    {
        return PyInt64_FromInt64(0);
    }

    return PyInt64_FromInt64(a >> b);
}

static PyObject*
pyint64_and(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    return PyInt64_FromInt64(a & b);
}

static PyObject*
pyint64_xor(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    return PyInt64_FromInt64(a ^ b);
}

static PyObject*
pyint64_or(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    return PyInt64_FromInt64(a | b);
}

static PyObject*
pyint64_int(PyObject *v)
{
    return PyLong_FromLongLong(((PyInt64Object*)v)->ob_int64val);
}

static PyObject*
pyint64_float(PyObject *v)
{
    return PyFloat_FromDouble((double)((PyInt64Object*)v)->ob_int64val);
}

/* Pyint64 Number Methods End */

PyObject*
pyint64_richcompare(PyObject *self, PyObject *other, int op)
{
    CHECK_BINOP(self, other);

    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(self, a);
    CONVERT_TO_INT64(other, b);
   
    Py_RETURN_RICHCOMPARE(a, b, op);
}

static Py_hash_t
pyint64_hash(PyInt64Object *v)
{
    int64_t value = PyInt64_GetValue(v);
    unsigned char* first = (unsigned char*) &value;
    uint64_t offsetBit = 14695981039346656037ULL;

    for (size_t index = 0; index < sizeof(int64_t); ++index) {
        offsetBit ^= (size_t)(first[index]);
        offsetBit *= (uint64_t)1099511628211;
    }

    return (Py_hash_t)offsetBit;
}
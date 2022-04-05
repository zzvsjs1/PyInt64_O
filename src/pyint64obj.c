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
    else if (PyFloat_Check(obj))
    {
        const double d_value = PyFloat_AsDouble(obj);
        if (d_value == -1.0 && PyErr_Occurred())
        {
            *v = NULL;
            return -1;
        }

        *val = (int64_t)d_value;
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

// static PyObject*
// pyint64_new_impl(PyTypeObject *type, PyObject *x);

// static PyObject *
// pyint64_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs);

// Constuctor
static int
pyint64__init__(PyInt64Object *self, PyObject *args, PyObject *kwds);

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
pyint64_power(PyObject *v, PyObject *w, PyObject *x);

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
pyint64_floor_divide(PyObject *left, PyObject *right);

static PyObject*
pyint64_true_divide(PyObject *left, PyObject *right);

static PyObject*
pyint64_int(PyObject *v);

static PyObject*
pyint64_float(PyObject *v);

static PyObject*
pyint64__str__(PyObject *self);

// END Number operations

static PyModuleDef pyint64_module = 
{
    PyModuleDef_HEAD_INIT,
    .m_name = "pyint64",
    .m_doc = "A int64 object module.",
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
    .nb_power = (ternaryfunc)pyint64_power,
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
    .nb_floor_divide = pyint64_floor_divide,
    .nb_true_divide = pyint64_true_divide,
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
    .tp_str = pyint64__str__,
    .tp_repr = (reprfunc)pyint64_repr,
    .tp_as_number = &pyint64_as_number,
    .tp_hash = (hashfunc)pyint64_hash,
    .tp_getattro = PyObject_GenericGetAttr,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_richcompare = pyint64_richcompare,
    .tp_methods = pyint64_methods,
    .tp_getset = pyint64_getset,
    .tp_init = (initproc)pyint64__init__,
    .tp_new = PyType_GenericNew,
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
        return PyInt64_GetValue(object);
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

    // Call is str.digit
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

static int
pyint64_parseUnicode(PyInt64Object *self, PyObject *arg)
{
    return 0;
}

static int
pyint64__init__impl__(PyInt64Object *self, PyObject *arg)
{
    if (!arg)
    {
        self->ob_int64val = 0;
        return 0;
    }

    if (PyUnicode_Check(arg))
    {
         if (!Py_IsTrue(PyObject_CallMethod(arg, "isdigit", NULL)))
        {
            PyErr_Format(PyExc_ValueError, 
                "The str value must be digit, not '%.200S'",
                    arg);
            return -1;
        }

        return pyint64_parseUnicode(self, arg);
    }

    if (PyLong_Check(arg))
    {
        const int64_t value = PyLong_AsLongLong(arg);
        if (value == -1 && PyErr_Occurred())
        {
            return -1;
        }

        self->ob_int64val = value;
        return 0;
    }

    if (PyFloat_Check(arg))
    {
        const double value = PyFloat_AsDouble(arg);
        if (value == -1.0 && PyErr_Occurred())
        {
            return -1;
        }

        self->ob_int64val = (int64_t)value;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "The type %s is not support by PyInt64.", Py_TYPE(arg)->tp_name);
    return -1;
}

static int
pyint64__init__(PyInt64Object *self, PyObject *args, PyObject *kwds)
{
    const Py_ssize_t size = PyTuple_Size(args);
    PyObject* arg1 = NULL;

    if (size > 1)
    {
        PyErr_Format(PyExc_TypeError, 
            "Pyint64() takes at most 1 arguments (%zd given)", 
            size
        );

        return -1;
    }
    else if (size == 1)
    {
        arg1 = PyTuple_GET_ITEM(args, 0);
    }
   
    return pyint64__init__impl__(self, arg1);
}

static void
pyint64_dealloc(PyInt64Object *obj)
{
    Py_TYPE(obj)->tp_free((PyObject*)obj);
}

static PyObject*
pyint64_repr(PyInt64Object *self)
{
   return pyint64__str__((PyObject*)self);
}

static PyObject*
pyint64__str__(PyObject* self)
{
    char buffer[21];
    const int64_t value = PyInt64_GetValue(self);
    char* bufferEnd = buffer + 21;
    char* next = signedToString(value, buffer);

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

    return PyInt64_FromInt64(a / b);
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
pyint64_power_pylong(PyObject *v, PyObject *w, PyObject *x)
{
    PyObject* a;
    PyObject* b;
    PyObject* c;

    if (PyLong_Check(v))
    {

    }
    else
    {
        
    }

    if (PyLong_Check(w))
    {

    }
    else
    {
        
    }

    if (PyLong_Check(x))
    {

    }
    else
    {
        
    }



    Py_XDECREF(a);
    Py_XDECREF(b);
    Py_XDECREF(c);
    return PyErr_NoMemory();
}

static PyObject*
pyint64_power(PyObject *v, PyObject *w, PyObject *x)
{
    int64_t a;
    int64_t b;
    int64_t c;

    const bool has_x = Py_IsNone(x);

    if (PyLong_Check(v))
    {
        a = PyLong_AsLongLong(v);
        if (a == -1 && PyErr_Occurred())
        {
            return NULL;
        }
    }
    else
    {
        CONVERT_TO_INT64(v, a);
    }

    if (PyLong_Check(w))
    {
        b = PyLong_AsLongLong(w);
        if (b == -1 && PyErr_Occurred())
        {
            return NULL;
        }
    }
    else
    {
        CONVERT_TO_INT64(w, b);
    }

    if (!has_x)
    {
        return PyInt64_FromInt64(pyint64_pow_impl(a, b));
    }
    else
    {
        if (PyLong_Check(v))
        {
            c = PyLong_AsLongLong(v);
            if (c == -1 && PyErr_Occurred())
            {
                return NULL;
            }
        }

        return PyInt64_FromInt64(pyint64_pow_impl(a, b) % c);
    }

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
        PyErr_SetString(PyExc_ValueError, "Negative shift count");
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
        PyErr_SetString(PyExc_ValueError, "Negative shift count");
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
    return PyLong_FromLongLong(PyInt64_GetValue(v));
}

static PyObject*
pyint64_float(PyObject *v)
{
    return PyFloat_FromDouble((double)PyInt64_GetValue(v));
}

static PyObject*
pyint64_floor_divide(PyObject *left, PyObject *right)
{
    int64_t a;
    int64_t b;
    CONVERT_TO_INT64(left, a);
    CONVERT_TO_INT64(right, b);

    if (b == 0)
    {
        PyErr_SetString(PyExc_ZeroDivisionError, "Divide by zero.");
        return NULL;
    }

    return PyInt64_FromInt64(a / b);
}

static PyObject*
pyint64_true_divide(PyObject *left, PyObject *right)
{
    if (PyInt64_Check(left) && PyInt64_Check(right))
    {
        return pyint64_floor_divide(left, right);
    }

    if (PyInt64_Check(left))
    {
        int64_t a;
        CONVERT_TO_INT64(left, a);
        if (PyLong_Check(right))
        {
            PyObject* a_l = PyLong_FromLongLong(a);
            if (!a_l)
            {
                return PyErr_NoMemory();
            }

            PyObject* result = PyNumber_TrueDivide(a_l, right);
            if (!result)
            {
                Py_DecRef(a_l);
                return PyErr_NoMemory();
            }

            Py_DecRef(a_l);
            return result;
        }
        else if (PyFloat_Check(right))
        {
            PyObject* a_l = PyFloat_FromDouble((double)a);
            if (!a_l)
            {
                return PyErr_NoMemory();
            }

            PyObject* result = PyNumber_TrueDivide(a_l, right);
            if (!result)
            {
                Py_DecRef(a_l);
                return PyErr_NoMemory();
            }

            Py_DecRef(a_l);
            return result;
        }
        
        Py_RETURN_NOTIMPLEMENTED;
    }

    int64_t b;
    CONVERT_TO_INT64(left, b);
    if (PyLong_Check(right))
    {
        PyObject* b_l = PyLong_FromLongLong(a);
        if (!b_l)
       {
            return PyErr_NoMemory();
       }
       PyObject* result = PyNumber_TrueDivide(b_l, right);
        if (!result)
        {
            Py_DecRef(b_l);
            return PyErr_NoMemory();
        }

        Py_DecRef(b_l);
        return result;
    }
    else if (PyFloat_Check(right))
    {
        PyObject* b_l = PyFloat_FromDouble((double)b);
        if (!b_l)
        {
            return PyErr_NoMemory();
        }

        PyObject* result = PyNumber_TrueDivide(b_l, right);
        if (!result)
        {
            Py_DecRef(b_l);
            return PyErr_NoMemory();
        }

        Py_DecRef(b_l);
        return result;
    }

    Py_RETURN_NOTIMPLEMENTED;
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

    for (size_t index = 0; index < sizeof(int64_t); ++index) 
    {
        offsetBit ^= (size_t)(first[index]);
        offsetBit *= (uint64_t)1099511628211;
    }

    return (Py_hash_t)offsetBit;
}
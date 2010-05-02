#include <Python.h>
#include "structmember.h"
#include <ei.h>
#include <erl_interface.h>
#include "eterm.h"

static void
Eterm_dealloc(EtermObject *self)
{
	if(self->term){
		if(ERL_TYPE(self->term) & ERL_COMPOUND){
			erl_free_compound(self->term);
		}else{
			erl_free_term(self->term);
		}
	}
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Eterm_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	EtermObject *self;
	self = (EtermObject *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->term = NULL;
	}
	return (PyObject *)self;
}

static int
Eterm_init(EtermObject *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"atom", "binary", "string", NULL};
	const char *atom = NULL;
	const char *binary = NULL;
	int binary_size = 0;
	const char *string = NULL;

	if (!PyArg_ParseTupleAndKeywords(
			args, kwds, "|ss#s", kwlist,
			&atom, &binary, &binary_size,
			&string)){
		return -1;
	}
	if(atom){
		self->term = erl_mk_atom(atom);
	}else if(binary){
		self->term = erl_mk_binary(binary, binary_size);
	}else if(string){
		self->term = erl_mk_string(string);
	}

	return 0;
}

static PyMemberDef Eterm_members[] = {
	{NULL}  /* Sentinel */
};

static PyObject *
Eterm_gettype(EtermObject *self, void *closure)
{
	return Py_BuildValue("i", ERL_TYPE(self->term));
}

static PyGetSetDef Eterm_getseters[] = {
	{"type", (getter)Eterm_gettype, NULL, NULL, NULL},
	{NULL}  /* Sentinel */
};

static PyObject *
Eterm_print_term(EtermObject *self, PyObject *args)
{
	int ret = 0;
	PyObject *stream = NULL;
	FILE *fp = stdout;

	if (!PyArg_ParseTuple(args, "|O", &stream)){
		return NULL;
	}
	if(stream && PyFile_Check(stream)){
		fp = PyFile_AsFile(stream);
	}
	if(self->term){
		ret = erl_print_term(fp, self->term);
		fprintf(fp, "\n");
	}
	return Py_BuildValue("i", ret);
}

static PyObject *
Eterm_is_integer(EtermObject *self)
{
	if(ERL_IS_INTEGER(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_unsigned_integer(EtermObject *self)
{
	if(ERL_IS_UNSIGNED_INTEGER(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_float(EtermObject *self)
{
	if(ERL_IS_FLOAT(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_atom(EtermObject *self)
{
	if(ERL_IS_ATOM(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_pid(EtermObject *self)
{
	if(ERL_IS_PID(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_port(EtermObject *self)
{
	if(ERL_IS_PORT(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_ref(EtermObject *self)
{
	if(ERL_IS_REF(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_tuple(EtermObject *self)
{
	if(ERL_IS_TUPLE(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_binary(EtermObject *self)
{
	if(ERL_IS_BINARY(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_nil(EtermObject *self)
{
	if(ERL_IS_NIL(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_empty_list(EtermObject *self)
{
	if(ERL_IS_LIST(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_cons(EtermObject *self)
{
	if(ERL_IS_CONS(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

static PyObject *
Eterm_is_list(EtermObject *self)
{
	if(ERL_IS_LIST(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}

#ifdef ERL_IS_LONGLONG
static PyObject *
Eterm_is_longlong(EtermObject *self)
{
	if(ERL_IS_LONGLONG(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}
#endif

#ifdef ERL_IS_UNSIGNED_LONGLONG
static PyObject *
Eterm_is_unsigned_longlong(EtermObject *self)
{
	if(ERL_IS_UNSIGNED_LONGLONG(self->term)){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}
#endif

static PyMethodDef Eterm_methods[] = {
	{"print_term", (PyCFunction)Eterm_print_term, METH_VARARGS,
     "print EtermObject"
    },
	{"is_integer", (PyCFunction)Eterm_is_integer, METH_NOARGS, NULL},
	{"is_unsigned_integer", (PyCFunction)Eterm_is_unsigned_integer,
	 METH_NOARGS, NULL},
	{"is_float", (PyCFunction)Eterm_is_float, METH_NOARGS, NULL},
	{"is_atom", (PyCFunction)Eterm_is_atom, METH_NOARGS, NULL},
	{"is_pid", (PyCFunction)Eterm_is_pid, METH_NOARGS, NULL},
	{"is_port", (PyCFunction)Eterm_is_port, METH_NOARGS, NULL},
	{"is_ref", (PyCFunction)Eterm_is_ref, METH_NOARGS, NULL},
	{"is_tuple", (PyCFunction)Eterm_is_tuple, METH_NOARGS, NULL},
	{"is_binary", (PyCFunction)Eterm_is_binary, METH_NOARGS, NULL},
	{"is_nil", (PyCFunction)Eterm_is_nil, METH_NOARGS, NULL},
	{"is_empty_list", (PyCFunction)Eterm_is_empty_list, METH_NOARGS, NULL},
	{"is_cons", (PyCFunction)Eterm_is_cons, METH_NOARGS, NULL},
	{"is_list", (PyCFunction)Eterm_is_list, METH_NOARGS, NULL},
#ifdef ERL_IS_LONGLONG
	{"is_longlong", (PyCFunction)Eterm_is_longlong, METH_NOARGS, NULL},
#endif
#ifdef ERL_IS_UNSIGNED_LONGLONG
	{"is_unsigned_longlong", (PyCFunction)Eterm_is_unsigned_longlong,
	 METH_NOARGS, NULL},
#endif
	{NULL}  /* Sentinel */
};

static PyObject *
Eterm_str(PyObject *self)
{
	PyObject *ret;
	EtermObject *eterm = (EtermObject *)self;
	ei_x_buff buf;
	char *s = NULL;
	int i = 0;

	ei_x_new(&buf);
	ei_x_encode_term(&buf, eterm->term);
	ei_s_print_term(&s, buf.buff, &i);
	ret = PyString_FromString(s);
	free(s);
	return ret;
}

PyTypeObject EtermType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"pyerl.Eterm",             /*tp_name*/
	sizeof(EtermObject),       /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Eterm_dealloc, /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	Eterm_str,                 /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,        /*tp_flags*/
	"Eterm objects",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	Eterm_methods,             /* tp_methods */
	Eterm_members,             /* tp_members */
	Eterm_getseters,           /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)Eterm_init,      /* tp_init */
	0,                         /* tp_alloc */
	Eterm_new,                 /* tp_new */
};

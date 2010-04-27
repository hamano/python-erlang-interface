#include <Python.h>
#include "structmember.h"
#include <ei.h>
#include <erl_interface.h>
#include "eterm.h"

static void
Eterm_dealloc(EtermObject *self)
{
	if(self->term){
		erl_free_compound(self->term);
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

static PyMethodDef Eterm_methods[] = {
/*
	{"print_term", (PyCFunction)Eterm_print_term, METH_NOARGS,
     "print EtermObject for debug"
    },
*/
	{"print_term", (PyCFunction)Eterm_print_term, METH_VARARGS,
     "print EtermObject"
    },
    {NULL}  /* Sentinel */
};

static PyObject *
Eterm_str(PyObject *self)
{
	PyObject *ret;
	EtermObject *eterm = (EtermObject *)self;

	switch(ERL_TYPE(eterm->term)){
	case ERL_UNDEF:
		ret = PyString_FromString("UNDEF");
		break;
	case ERL_INTEGER:
		ret = PyString_FromFormat("%d", ERL_INT_VALUE(eterm->term));
		break;
	case ERL_U_INTEGER:
		ret = PyString_FromFormat("%u", ERL_INT_UVALUE(eterm->term));
		break;
	case ERL_ATOM:
		ret = PyString_FromFormat("%s", ERL_ATOM_PTR(eterm->term));
		break;
	case ERL_PID:
		ret = PyString_FromFormat(
			"<%s.%d.%d>",
			ERL_PID_NODE(eterm->term),
			ERL_PID_NUMBER(eterm->term),
			ERL_PID_SERIAL(eterm->term));
		break;
	case ERL_PORT:
		ret = PyString_FromString("#Port");
		break;
	default:
		ret = PyString_FromString("ERROR");
	}
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

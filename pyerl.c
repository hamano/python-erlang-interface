#include <Python.h>
#include "structmember.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ei.h>
#include <erl_interface.h>
#include "eterm.h"

static char pyerl_doc[] = "Erlang Interface for Python\n";

static PyObject *
pyerl_init(PyObject *self, PyObject *args)
{
	int x = 0;
	int y = 0;
	if(!PyArg_ParseTuple(args, "|ii", &x, &y)){
		return NULL;
	}
	erl_init((void *)x, y);
	Py_RETURN_NONE;
}

static PyObject *
pyerl_set_compat_rel(PyObject *self, PyObject *args)
{
	unsigned int rel;
	if(!PyArg_ParseTuple(args, "I", &rel)){
		return NULL;
	}
	erl_set_compat_rel(rel);
	Py_RETURN_NONE;
}

static PyObject *
pyerl_connect_init(PyObject *self, PyObject *args)
{
	int ret;
	int number;
	char *cookie;
	short creation;

	if(!PyArg_ParseTuple(args, "ish", &number, &cookie, &creation)){
		return NULL;
	}
	ret = erl_connect_init(number, cookie, creation);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_connect_xinit(PyObject *self, PyObject *args)
{
	int ret;
	char *host, *alive, *node, *addr;
	char *cookie;
	short creation;
	struct in_addr ia;

	if(!PyArg_ParseTuple(args, "sssssh",
						  &host, &alive, &node, &addr, &cookie, &creation)){
		return NULL;
	}
	ia.s_addr = inet_addr(addr);
	ret = erl_connect_xinit(host ,alive, node, &ia, cookie, creation);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_connect(PyObject *self, PyObject *args)
{
	int ret;
	char *node;

	if(!PyArg_ParseTuple(args, "s", &node))
		return NULL;
	ret = erl_connect(node);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_xconnect(PyObject *self, PyObject *args)
{
	int ret;
	char *addr, *alive;
	struct in_addr ia;

	if(!PyArg_ParseTuple(args, "ss", &addr, &alive))
		return NULL;
	ia.s_addr = inet_addr(addr);
	ret = erl_xconnect(&ia, alive);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_close_connection(PyObject *self, PyObject *args)
{
	int ret;
	int fd;
	if(!PyArg_ParseTuple(args, "i", &fd)){
		return NULL;
	}
	ret = erl_close_connection(fd);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_xreceive_msg(PyObject *self, PyObject *args)
{
	int ret;
	int fd;
	int size = 1024;
	unsigned char *buf = malloc(size);
	ErlMessage emsg;
	EtermObject *msg;

	if(!PyArg_ParseTuple(args, "i", &fd)){
		return NULL;
	}
	if(fd < 0){
		return NULL;
	}
	if(!(msg = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	ret = erl_xreceive_msg(fd, &buf, &size, &emsg);
	msg->term = erl_copy_term(emsg.msg);
	/* TODO: bad return */
	return Py_BuildValue("iO", ret, msg);
}

static PyObject *
pyerl_send(PyObject *self, PyObject *args)
{
	int ret;
	int fd;
	PyObject *to;
	PyObject *msg;
	EtermObject *eto;
	EtermObject *emsg;

	if(!PyArg_ParseTuple(args, "iOO", &fd, &to, &msg)){
		return NULL;
	}
	if(fd < 0){
		return NULL;
	}
	if(!PyObject_TypeCheck(to, &EtermType) ||
	   !PyObject_TypeCheck(msg, &EtermType)){
		return NULL;
	}
	eto = (EtermObject *)to;
	emsg = (EtermObject *)msg;
	ret = erl_send(fd, eto->term, emsg->term);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_reg_send(PyObject *self, PyObject *args)
{
	int ret;
	int fd;
	char *to;
	PyObject *msg;
	EtermObject *emsg;

	if(!PyArg_ParseTuple(args, "isO", &fd, &to, &msg)){
		return NULL;
	}
	if(fd < 0){
		return NULL;
	}
	if(!PyObject_TypeCheck(msg, &EtermType)){
		return NULL;
	}
	emsg = (EtermObject *)msg;
	ret = erl_reg_send(fd, to, emsg->term);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_rpc(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	int fd;
	char *mod, *fun;
	EtermObject *arg;

	if(!PyArg_ParseTuple(args, "issO", &fd, &mod, &fun, &arg)){
		return NULL;
	}
	if(!PyObject_TypeCheck(args, &EtermType)){
		return NULL;
	}
	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	ret->term = erl_rpc(fd, mod, fun, arg->term);
	return (PyObject *)ret;
}

static PyObject *
pyerl_publish(PyObject *self, PyObject *args)
{
	int ret;
	int port;
	if(!PyArg_ParseTuple(args, "i", &port)){
		return NULL;
	}
	ret = erl_publish(port);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_unpublish(PyObject *self, PyObject *args)
{
	int ret;
	char *alive;

	if(!PyArg_ParseTuple(args, "s", &alive))
		return NULL;
	ret = erl_unpublish(alive);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_thiscookie(PyObject *self, PyObject *args)
{
	const char *ret;
	ret = erl_thiscookie();
	return Py_BuildValue("s", ret);
}

static PyObject *
pyerl_thisnodename(PyObject *self, PyObject *args)
{
	const char *ret;
	ret = erl_thisnodename();
	return Py_BuildValue("s", ret);
}

static PyObject *
pyerl_thishostname(PyObject *self, PyObject *args)
{
	const char *ret;
	ret = erl_thishostname();
	return Py_BuildValue("s", ret);
}

static PyObject *
pyerl_thisalivename(PyObject *self, PyObject *args)
{
	const char *ret;
	ret = erl_thishostname();
	return Py_BuildValue("s", ret);
}

static PyObject *
pyerl_thiscreation(PyObject *self, PyObject *args)
{
	int ret;
	ret = erl_thiscreation();
	return Py_BuildValue("i", ret);
}


static PyObject *
pyerl_cons(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	PyObject *head;
	PyObject *tail;
	EtermObject *ehead;
	EtermObject *etail;

	if(!PyArg_ParseTuple(args, "OO", &head, &tail)){
		return NULL;
	}
	if(!PyObject_TypeCheck(head, &EtermType)||
	   !PyObject_TypeCheck(tail, &EtermType)){
		return NULL;
	}
	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	ehead = (EtermObject *)head;
	etail = (EtermObject *)tail;
	if(!ehead->term || !etail->term){
		return NULL;
	}
	if(!(ret->term = erl_cons(ehead->term, etail->term))){
		EtermType.tp_dealloc((PyObject *)ret);
		return NULL;
	}
	return (PyObject *)ret;
}

static PyObject *
pyerl_copy_term(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	PyObject *term;
	EtermObject *eterm;

	if(!PyArg_ParseTuple(args, "O", &term)){
		return NULL;
	}
	if(!PyObject_TypeCheck(term, &EtermType)){
		return NULL;
	}

	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	eterm = (EtermObject *)term;
	if(!eterm->term){
		return NULL;
	}
	if(!(ret->term = erl_copy_term(eterm->term))){
		EtermType.tp_dealloc((PyObject *)ret);
		return NULL;
	}
	return (PyObject *)ret;
}

static PyObject *
pyerl_element(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	int position;
	PyObject *tuple;
	EtermObject *etuple;
	int size;

	if(!PyArg_ParseTuple(args, "iO", &position, &tuple)){
		return NULL;
	}
	if(!PyObject_TypeCheck(tuple, &EtermType)){
		return NULL;
	}
	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	etuple = (EtermObject *)tuple;
	if(!etuple->term){
		return NULL;
	}
	size = erl_size(etuple->term);
	if(position < 1 || position > size){
		return NULL;
	}
	ret->term = erl_element(position, etuple->term);
	return Py_BuildValue("O", ret);
}

static PyObject *
pyerl_hd(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	PyObject *term;
	EtermObject *eterm;

	if(!PyArg_ParseTuple(args, "O", &term)){
		return NULL;
	}
	if(!PyObject_TypeCheck(term, &EtermType)){
		return NULL;
	}
	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	eterm = (EtermObject *)term;
	if(!eterm->term){
		return NULL;
	}
	ret->term = erl_hd(eterm->term);
	if(!ret->term){
		EtermType.tp_dealloc((PyObject *)ret);
		Py_RETURN_NONE;
	}
	return Py_BuildValue("O", ret);
}

static PyObject *
pyerl_tl(PyObject *self, PyObject *args)
{
	EtermObject *ret;
	PyObject *term;
	EtermObject *eterm;

	if(!PyArg_ParseTuple(args, "O", &term)){
		return NULL;
	}
	if(!PyObject_TypeCheck(term, &EtermType)){
		return NULL;
	}
	if(!(ret = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	eterm = (EtermObject *)term;
	if(!eterm->term){
		return NULL;
	}
	ret->term = erl_tl(eterm->term);
	if(!ret->term){
		EtermType.tp_dealloc((PyObject *)ret);
		Py_RETURN_NONE;
	}
	return Py_BuildValue("O", ret);
}

static PyObject *
pyerl_mk_atom(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *string;

	if(!PyArg_ParseTuple(args, "s", &string)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_atom(string))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_binary(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *bptr;
	int size;

	if(!PyArg_ParseTuple(args, "s#", &bptr, &size)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_binary(bptr, size))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_empty_list(PyObject *self, PyObject *args)
{
	EtermObject *eterm;

	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_empty_list())){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_estring(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *string;
	int len;

	if(!PyArg_ParseTuple(args, "s#", &string, &len)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_estring(string, len))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_float(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	double f;

	if(!PyArg_ParseTuple(args, "d", &f)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_float(f))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_int(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	int n;

	if(!PyArg_ParseTuple(args, "i", &n)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_int(n))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_longlong(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	long long ll;

	if(!PyArg_ParseTuple(args, "L", &ll)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_longlong(ll))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_list(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	PyObject *array;
	Py_ssize_t size;
	int i;
	PyObject *obj;
	EtermObject *eobj;
	ETERM **eterm_array;

	if(!PyArg_ParseTuple(args, "O", &array)){
		return NULL;
	}
	if(!PyList_Check(array)){
		return NULL;
	}
	size = PyList_Size(array);
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	eterm_array = (ETERM **)malloc(sizeof(ETERM *) * size);
	for(i=0; i<size; i++){
		obj = PyList_GetItem(array, i);
		if(!PyObject_TypeCheck(obj, &EtermType)){
			PyErr_SetString(PyExc_TypeError, "Expected pyerl_mk_list");
			return NULL;
		}
		eobj = (EtermObject *)obj;
		eterm_array[i] = erl_copy_term(eobj->term);
	}

	if(!(eterm->term = erl_mk_list(eterm_array, size))){
		free(eterm_array);
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	free(eterm_array);
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_pid(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *node;
	unsigned int number;
	unsigned int serial;
	unsigned int creation;

	if(!PyArg_ParseTuple(args, "sIII", &node, &number, &serial, &creation)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_pid(node, number, serial, creation))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_port(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *node;
	unsigned int number;
	unsigned int creation;

	if(!PyArg_ParseTuple(args, "sII", &node, &number, &creation)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_port(node, number, creation))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_ref(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *node;
	unsigned int number;
	unsigned int creation;

	if(!PyArg_ParseTuple(args, "sII", &node, &number, &creation)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_ref(node, number, creation))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_long_ref(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *node;
	unsigned int n1, n2, n3;
	unsigned int creation;

	if(!PyArg_ParseTuple(args, "sIIII", &node, &n1, &n2, &n3, &creation)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_long_ref(node, n1, n2, n3, creation))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_string(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *string;

	if(!PyArg_ParseTuple(args, "s", &string)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_string(string))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_tuple(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	PyObject *array;
	Py_ssize_t size;
	int i;
	PyObject *obj;
	EtermObject *eobj;
	ETERM **eterm_array;

	if(!PyArg_ParseTuple(args, "O", &array)){
		return NULL;
	}
	if(!PyTuple_Check(array)){
		return NULL;
	}
	size = PyTuple_Size(array);
	eterm_array = (ETERM **)malloc(sizeof(ETERM *) * size);
	for(i=0; i<size; i++){
		obj = PyTuple_GetItem(array, i);
		if(!PyObject_TypeCheck(obj, &EtermType)){
			PyErr_SetString(PyExc_TypeError, "Expected pyerl_mk_tuple");
			return NULL;
		}
		eobj = (EtermObject *)obj;
		eterm_array[i] = erl_copy_term(eobj->term);
	}

	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		free(eterm_array);
		return NULL;
	}

	if(!(eterm->term = erl_mk_tuple(eterm_array, size))){
		free(eterm_array);
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	free(eterm_array);
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_uint(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	unsigned int n;

	if(!PyArg_ParseTuple(args, "I", &n)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_uint(n))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_ulonglong(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	unsigned long long ll;

	if(!PyArg_ParseTuple(args, "K", &ll)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_ulonglong(ll))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_mk_var(PyObject *self, PyObject *args)
{
	EtermObject *eterm;
	const char *name;

	if(!PyArg_ParseTuple(args, "s", &name)){
		return NULL;
	}
	if(!(eterm = (EtermObject *)EtermType.tp_new(&EtermType, NULL, NULL))){
		return NULL;
	}
	if(!(eterm->term = erl_mk_var(name))){
		EtermType.tp_dealloc((PyObject *)eterm);
		return NULL;
	}
	return (PyObject *)eterm;
}

static PyObject *
pyerl_print_term(PyObject *self, PyObject *args)
{
	int ret = 0;
	PyObject *stream;
	PyObject *term;
	EtermObject *eterm;
	FILE *fp = stdout;

	if(!PyArg_ParseTuple(args, "OO", &stream, &term)){
		return NULL;
	}
	if(!PyFile_Check(stream)){
		return NULL;
	}
	fp = PyFile_AsFile(stream);
	if(!PyObject_TypeCheck(term, &EtermType)){
		return NULL;
	}
	eterm = (EtermObject *)term;
	if(!eterm->term){
		return NULL;
	}
	ret = erl_print_term(fp, eterm->term);
	fprintf(fp, "\n");

	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_size(PyObject *self, PyObject *args)
{
	int ret = 0;
	PyObject *term = NULL;
	EtermObject *eterm;

	if(!PyArg_ParseTuple(args, "O", &term)){
		return NULL;
	}

	if(!PyObject_TypeCheck(term, &EtermType)){
		return NULL;
	}

	eterm = (EtermObject *)term;
	if(!eterm->term){
		return NULL;
	}
	ret = erl_size(eterm->term);
	return Py_BuildValue("i", ret);
}

static PyObject *
pyerl_eterm_release(PyObject *self, PyObject *args)
{
	erl_eterm_release();
	Py_RETURN_NONE;
}

static PyObject *
pyerl_eterm_statistics(PyObject *self, PyObject *args)
{
	unsigned long allocated;
	unsigned long freed;
	erl_eterm_statistics(&allocated, &freed);
	return Py_BuildValue("(kk)", allocated, freed);
}

static PyMethodDef methods[] = {
	{"init", pyerl_init, METH_VARARGS, "\
This function must be called before any of the others in the\n\
erl_interface library in order to initialize the library functions.\n\
But It doesn't need to call this function in pyerl due to already\n\
called at import."},
	{"set_compat_rel", pyerl_set_compat_rel, METH_VARARGS, "\
A call to set_compat_rel(release_number) sets the erl_interface\n\
library in compatibility mode of release release_number."},
	{"connect_init", pyerl_connect_init, METH_VARARGS, "\
These functions initialize the erl_connect module.\n\
connect_init() provides an alternative interface which does not\n\
require as much information from the caller. Instead, connect_init()\n\
uses gethostbyname() to obtain default values."},
	{"connect_xinit", pyerl_connect_xinit, METH_VARARGS, "\
These functions initialize the erl_connect module.\n\
connect_xinit() stores for later use information about the node's host\n\
name host, alive name alive, node name node, IP address addr, cookie\n\
cookie, and creation number creation."},
	{"connect", pyerl_connect, METH_VARARGS, "\
These functions set up a connection to an Erlang node."},
	{"xconnect", pyerl_xconnect, METH_VARARGS, "\
These functions set up a connection to an Erlang node."},
	{"close_connection", pyerl_close_connection, METH_VARARGS, "\
This function closes an open connection to an Erlang node."},
	{"xreceive_msg", pyerl_xreceive_msg, METH_VARARGS, NULL},
	{"send", pyerl_send, METH_VARARGS, "\
This function sends an Erlang term to a process."},
	{"reg_send", pyerl_reg_send, METH_VARARGS, "\
This function sends an Erlang term to a registered process."},
	{"rpc", pyerl_rpc, METH_VARARGS, "\
These functions support calling Erlang functions on remote nodes. "},

	{"publish", pyerl_publish, METH_VARARGS, NULL},
	{"unpublish", pyerl_unpublish, METH_VARARGS, NULL},
	/* TODO */
	/* {"accept", pyerl_accept, METH_VARARGS, NULL}, */
	{"thiscookie", pyerl_thiscookie, METH_VARARGS, NULL},
	{"thisnodename", pyerl_thisnodename, METH_VARARGS, NULL},
	{"thishostname", pyerl_thishostname, METH_VARARGS, NULL},
	{"thisalivename", pyerl_thisalivename, METH_VARARGS, NULL},
	{"thiscreation", pyerl_thiscreation, METH_VARARGS, NULL},

	{"cons", pyerl_cons, METH_VARARGS, NULL},
	{"copy_term", pyerl_copy_term, METH_VARARGS, NULL},
	{"element", pyerl_element, METH_VARARGS, NULL},
	{"hd", pyerl_hd, METH_VARARGS, NULL},
	{"tl", pyerl_tl, METH_VARARGS, NULL},

	{"mk_atom", pyerl_mk_atom, METH_VARARGS, NULL},
	{"mk_binary", pyerl_mk_binary, METH_VARARGS, NULL},
	{"mk_empty_list", pyerl_mk_empty_list, METH_VARARGS, NULL},
	{"mk_estring", pyerl_mk_estring, METH_VARARGS, NULL},
	{"mk_float", pyerl_mk_float, METH_VARARGS, NULL},
	{"mk_int", pyerl_mk_int, METH_VARARGS, NULL},
	{"mk_longlong", pyerl_mk_longlong, METH_VARARGS, NULL},
	{"mk_list", pyerl_mk_list, METH_VARARGS, NULL},
	{"mk_pid", pyerl_mk_pid, METH_VARARGS, NULL},
	{"mk_port", pyerl_mk_port, METH_VARARGS, NULL},
	{"mk_ref", pyerl_mk_ref, METH_VARARGS, NULL},
	{"mk_long_ref", pyerl_mk_long_ref, METH_VARARGS, NULL},
	{"mk_string", pyerl_mk_string, METH_VARARGS, NULL},
	{"mk_tuple", pyerl_mk_tuple, METH_VARARGS, NULL},
	{"mk_uint", pyerl_mk_uint, METH_VARARGS, NULL},
	{"mk_ulonglong", pyerl_mk_ulonglong, METH_VARARGS, NULL},
	{"mk_var", pyerl_mk_var, METH_VARARGS, NULL},

	{"print_term", pyerl_print_term, METH_VARARGS, NULL},
	{"size", pyerl_size, METH_VARARGS, NULL},

	{"eterm_release", pyerl_eterm_release, METH_NOARGS, NULL},
	{"eterm_statistics", pyerl_eterm_statistics, METH_NOARGS, NULL},
	{NULL, NULL}
};

void initpyerl(void)
{
	PyObject *m;

	erl_init(NULL, 0);
	if (PyType_Ready(&EtermType) < 0)
		return;

	m = Py_InitModule3("pyerl", methods, pyerl_doc);
	if (m == NULL)
		return;
	Py_INCREF(&EtermType);
	PyModule_AddObject(m, "Eterm", (PyObject *)&EtermType);

	PyModule_AddIntConstant(m, "COMPOUND", ERL_COMPOUND);
	PyModule_AddIntConstant(m, "UNDEF", ERL_UNDEF);
	PyModule_AddIntConstant(m, "INTEGER", ERL_INTEGER);
	PyModule_AddIntConstant(m, "U_INTEGER", ERL_U_INTEGER);
	PyModule_AddIntConstant(m, "ATOM", ERL_ATOM);
	PyModule_AddIntConstant(m, "PID", ERL_PID);
	PyModule_AddIntConstant(m, "PORT", ERL_PORT);
	PyModule_AddIntConstant(m, "REF", ERL_REF);
	PyModule_AddIntConstant(m, "CONS", ERL_CONS);
	PyModule_AddIntConstant(m, "LIST", ERL_LIST);
	PyModule_AddIntConstant(m, "NIL", ERL_NIL);
	PyModule_AddIntConstant(m, "EMPTY_LIST", ERL_EMPTY_LIST);
	PyModule_AddIntConstant(m, "TUPLE", ERL_TUPLE);
	PyModule_AddIntConstant(m, "BINARY", ERL_BINARY);
	PyModule_AddIntConstant(m, "FLOAT", ERL_FLOAT);
	PyModule_AddIntConstant(m, "VARIABLE", ERL_VARIABLE);
	PyModule_AddIntConstant(m, "SMALL_BIG", ERL_SMALL_BIG);
	PyModule_AddIntConstant(m, "U_SMALL_BIG", ERL_U_SMALL_BIG);
	PyModule_AddIntConstant(m, "FUNCTION", ERL_FUNCTION);
	PyModule_AddIntConstant(m, "BIG", ERL_BIG);
	PyModule_AddIntConstant(m, "LONGLONG", ERL_LONGLONG);
	PyModule_AddIntConstant(m, "U_LONGLONG", ERL_U_LONGLONG);
}

#ifndef HAVE_PYERL_ETERM_H
#define HAVE_PYERL_ETERM_H

#include <Python.h>

extern PyTypeObject EtermType;

typedef struct {
	PyObject_HEAD
	ETERM *term;
    /* Type-specific fields go here. */
} EtermObject;

#endif /* HAVE_PYERL_ETERM_H */

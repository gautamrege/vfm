/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_pyapi_functions
#define INCLUDED_pyapi_functions

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vfm_error.h>
#include <vfm_common.h>
#include <pyapi_bridge.h>
#include <pyapi_vadapter.h>
#include <pyapi_vfabric.h>
/*
 * Validate the input data, convert it into the python object and then validate
 * if it is a dictionary object or not.
 */
PyObject *
validate_dictionary(PyObject * , PyObject *);

/* Add a key and its value in the result dictionary*/
int
add_key_values(PyObject* result, const char* key, PyObject * value);


/* Parse the guid and send the actual values in the uint8 array*/        
int
parse_string_to_unicode(char *buff,
                int length,
                uint8_t *out_buff);

PyObject *
parse_unicode_to_string(uint64_t val, int length);

#endif /* INCLUDED_pyapi_functions*/

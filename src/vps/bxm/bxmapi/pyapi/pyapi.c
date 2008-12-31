/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <pyapi.h>
#include <bxm_vadapter.h>
/* 
 * Parse the vadapter dictionary and fill up the structures.
 * Params :  
 * [IN] *i_vadapter_attr : the dictionary object which contains the data to
 *                         fill up the bxm_vadapter_attr_t structure.
 * [OUT] *attr           : Contains the structure of the vadapter.
 * [OUT] *bitmask        : Contains the bitmask set for the modifying 
 *                         attributes.
 * Return : 
 *         NULL and on failure it will throw an exception.
 */

PyObject*
parse_vadapter_structure(PyObject* i_dict, bxm_vadapter_attr_t *attr,
                                        bxm_vadapter_attr_bitmask_t *bitmask)
{
       PyObject* dict_object = NULL;
       PyObject* p_dict_object = NULL;
       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "name"))) {
                /* copy the valus in the structure */
                memcpy(attr->name, PyString_AsString(dict_object),
                                sizeof(attr->name));
                bitmask->name = 1;
       }

       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "desc"))) {
                memcpy(attr->desc, PyString_AsString(dict_object),
                                sizeof(attr->desc));
                bitmask->desc = 1;
       }
       
       if (NULL != (dict_object = 
                            PyDict_GetItemString(i_dict, "protocol"))) {
                
               /* check if the protocol is integer or not */
                if(PyInt_Check(dict_object))
                        attr->protocol = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->protocol = 1;
       }
       
       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "running_mode"))) {
                if(PyInt_Check(dict_object))
                        attr->running_mode = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->running_mode = 1;
       }

       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "io_module_id"))) {
                if(PyInt_Check(dict_object))
                         attr->io_module_id = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->io_module_id = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "assignment_type"))) {
                if(PyInt_Check(dict_object))
                         attr->assignment_type = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->assignment_type = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "vfabric_id"))) {
                if(PyInt_Check(dict_object))
                         attr->vfabric_id = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->vfabric_id = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "v_en_attr"))) {
                /* Check whether it is an dictionary of an en attr or fc attr*/
                if (PyDict_Check(dict_object)) {
                        /* Parse the en structure */
                        if (NULL != (p_dict_object = 
                              PyDict_GetItemString(dict_object, "mac"))) {
                                    memcpy(attr->en_attr.mac,
                                        PyString_AsString(p_dict_object), 6);
                        }
                        if (NULL != (p_dict_object = 
                               PyDict_GetItemString(dict_object, "vlan"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->en_attr.vlan =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                        if (NULL != (p_dict_object = 
                               PyDict_GetItemString(dict_object, "mtu"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->en_attr.mtu =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                        if (NULL != (p_dict_object = PyDict_GetItemString(
                                           dict_object, "promiscuous_mode"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->en_attr.promiscuous_mode =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                }
                bitmask->protocol_attr = 1;
       }
       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "v_fc_attr"))) {
                /* Check whether it is an dictionary of an en attr or fc attr*/
                if (PyDict_Check(dict_object)) {
                        /* Parse the en structure */
                        if (NULL != (p_dict_object = PyDict_GetItemString(
                                                      dict_object, "wwnn"))) {
                                if (PyLong_Check(p_dict_object))
                                     attr->fc_attr.wwnn = 
                                             PyLong_AsLong(p_dict_object);
                        }
                        if (NULL != (p_dict_object = 
                               PyDict_GetItemString(dict_object, "wwpn"))) {
                                if (PyLong_Check(p_dict_object))
                                        attr->fc_attr.wwpn =
                                                PyLong_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                        if (NULL != (p_dict_object = 
                               PyDict_GetItemString(dict_object, "fcid"))) {
                                        memcpy((attr->fc_attr.fcid),
                                        PyString_AsString(p_dict_object), 3);
                        }
                        if (NULL != (p_dict_object = PyDict_GetItemString(
                                                     dict_object, "mtu"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->fc_attr.mtu =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                        if (NULL != (p_dict_object = PyDict_GetItemString(
                                                     dict_object, "spma"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->fc_attr.spma =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                        if (NULL != (p_dict_object = PyDict_GetItemString(
                                                     dict_object, "fpma"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->fc_attr.fpma =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                }
                bitmask->protocol_attr = 1;
       }
       return NULL;
out:
       return NULL;
}

int
add_key_values(PyObject* dict, const char* key, PyObject *value)
{
        /* 
         * If the dict is not a dictionary object then create is as a new 
         * dictionary object and fill up the values.
         */
        if(PyDict_SetItemString(dict, key, value)== -1) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in creating a result dictionary.");
                return -1;
        }
        return 0;
}

PyObject*
validate_dictionary(PyObject* args, PyObject* dict)
{
        /* Parse the touple and get the dictionary object */
        if (!PyArg_ParseTuple(args, "O", &dict)) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in parsing the object");
                goto out;
        }
        /* check if it is a dictionary object */
        if (!PyDict_Check(dict)) {
                PyErr_SetString(PyExc_StandardError,
                                "Please send the input as a dictionary object");
        }
out:
        return dict;
}


/* 
 * Parse the vfabric dictionary and fill up the structures.
 * Params :  
 * [IN] *i_vfabric_attr : the dictionary object which contains the data to
 *                         fill up the bxm_vfabric_attr_t structure.
 * [OUT] *attr            : Contains the structure of the vfabric.
 * [OUT] *bitmask         : Contains the bitmask of the vfabric structure.
 * 
 * Return : 
 *         NULL and on failure it will throw an exception.
 */
PyObject*
parse_vfabric_structure(PyObject* i_dict, bxm_vfabric_attr_t *attr,
                                bxm_vfabric_attr_bitmask_t *bitmask)
{
       PyObject* dict_object = NULL;
       PyObject* p_dict_object = NULL;

       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "name"))) {
                /* copy the valus in the structure */
                memcpy(attr->name, PyString_AsString(dict_object),
                                sizeof(attr->name));
                bitmask->name = 1;
       }

       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "desc"))) {
                memcpy(attr->desc, PyString_AsString(dict_object),
                                sizeof(attr->desc));
                bitmask->desc = 1;
       }
       
       if (NULL != (dict_object = 
                            PyDict_GetItemString(i_dict, "protocol"))) {
                
               /* check if the protocol is integer or not */
                if(PyInt_Check(dict_object))
                        attr->protocol = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->protocol_attr = 1;
       }
       
       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "running_mode"))) {
                if(PyInt_Check(dict_object))
                        attr->running_mode = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->running_mode = 1;
       }

       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "primary_gateway"))) {
                if(PyInt_Check(dict_object))
                         attr->primary_gateway = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->primary_gateway = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "backup_gateway"))) {
                if(PyInt_Check(dict_object))
                         attr->backup_gateway = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->backup_gateway = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "ha_state"))) {
                if(PyInt_Check(dict_object))
                         attr->ha_state = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->ha_state = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "v_en_attr"))) {
                /* Check whether it is an dictionary of an en attr or fc attr*/
                if (PyDict_Check(dict_object)) {
                        /* Parse the en structure */
                        if (NULL != (p_dict_object = 
                               PyDict_GetItemString(dict_object, "vlan"))) {
                                if (PyInt_Check(p_dict_object))
                                        attr->en_attr.vlan =
                                                PyInt_AsLong(p_dict_object);
                                else
                                        goto out;
                        }
                }
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "auto_failover"))) {
                if(PyInt_Check(dict_object))
                         attr->auto_failover = PyInt_AsLong(dict_object);
                else
                         goto out;
                /* 
                 * TODO : Currently in the bitmask structure this filed is 
                 * not defined.
                 * bitmask->auto_failover = 1;
                 */
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict, "auto_failback"))) {
                if(PyInt_Check(dict_object))
                         attr->auto_failback = PyInt_AsLong(dict_object);
                else
                         goto out;
                bitmask->auto_failback = 1;
       }
       return NULL;
out:

       PyErr_SetString(PyExc_StandardError,
                       "Error in parsing the dictionary");
       return NULL;
}



/*
 * @brief The python wrapper for calling bxm_vadapter_create.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for vadapter creation:
 *            {'name' : <name>, 'desc' : * <desc>, 'protocol' : <protocol> }
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vadapter id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject*
py_bxm_vadapter_create(PyObject* self, PyObject *args)
{
        PyObject* bxm_dict = NULL, *result = NULL;
        bxm_error_t err;
        bxm_vadapter_attr_t attr;
        bxm_vadapter_attr_bitmask_t bitmask;
        bxm_vadapter_id_t vadapter_id = 0;
        /*validate the input object and get the dictionary object*/
        bxm_dict = validate_dictionary(args, bxm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vadapter_structure(bxm_dict, &attr, &bitmask);
        err = BXM_SUCCESS;
        err = bxm_vadapter_create(attr.name, attr.desc, attr.protocol,
                                                        &vadapter_id);
        if (err != BXM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in creating the vadapter.");
                goto out;
        }
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }
        printf("\nvadapter id : %d", vadapter_id); 
        getchar();
        if (-1 != add_key_values(result, "id", Py_BuildValue("i",vadapter_id)))
                return result;
out:
        return NULL;
}
/*
 * @brief The python wrapper for calling bxm_vadapter_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vadapter properties :
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vadapter id> }. 
 * On failure, it will return NULL and throw the relevant Python exception.
 *
 */
PyObject*
py_bxm_vadapter_edit_general_attr(PyObject* self, PyObject *args)
{
        PyObject* bxm_dict = NULL, *result = NULL;
        bxm_error_t err = BXM_SUCCESS; 
        bxm_vadapter_attr_t attr;
        bxm_vadapter_attr_bitmask_t bitmask;
        /*validate the input object and get the dictionary object*/
        bxm_dict =  validate_dictionary(args, bxm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vadapter_structure(bxm_dict, &attr, &bitmask);

        err = bxm_vadapter_edit_general_attr(attr._vadapter_id, &bitmask,
                                                          &attr);
        if (err != BXM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in upadating the vadapter properties.");
                goto out;
        }
        if(-1 != add_key_values(result, "id",
                                Py_BuildValue("i", attr._vadapter_id)))
                return result;
out:
        return NULL;
}

/*
 * @brief The python wrapper for calling bxm_vfabric_create.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for vfabric creation:
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject*
py_bxm_vfabric_create(PyObject* self, PyObject *args)
{
        PyObject* bxm_dict =  NULL, *result = NULL;
        bxm_error_t err = BXM_SUCCESS;
        bxm_vfabric_attr_t attr;
        bxm_vfabric_attr_bitmask_t bitmask;
        bxm_vfabric_id_t vfabric_id = 0;
        /*validate the input object and get the dictionary object*/
        bxm_dict =  validate_dictionary(args, bxm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vfabric_structure(bxm_dict, &attr, &bitmask);
        err = bxm_vfabric_create(attr.name, attr.desc, attr.protocol,
                                                        &vfabric_id);
        if (err != BXM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in creating the vfabric.");
                goto out;
        }
        if(-1 != add_key_values(result, "id", Py_BuildValue("i", vfabric_id)))
                return result;
out:
        return NULL;
}       

/*
 * @brief The python wrapper for calling bxm_vfabric_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vfabric properties:
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject*
py_bxm_vfabric_edit_general_attr(PyObject* self, PyObject *args)
{
        PyObject* bxm_dict = NULL, *result = NULL;
        bxm_error_t err = BXM_SUCCESS;
        bxm_vfabric_attr_t attr;
        bxm_vfabric_attr_bitmask_t bitmask;
        /*validate the input object and get the dictionary object*/
        bxm_dict =  validate_dictionary(args, bxm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vfabric_structure(bxm_dict, &attr, &bitmask);

        err = bxm_vfabric_edit_general_attr(attr._vfabric_id, &bitmask, &attr);
        
        if (err != BXM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in updating the vfabric properties.");
                goto out;
        }
        if(-1 != add_key_values(result, "id",
                                Py_BuildValue("i", attr._vfabric_id)))
                return result;
out:
        return NULL;
}

/*
 * @brief The python wrapper for calling bxm_vfabric_online.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to change the running mode of 
 *                 the vfabric to online:
 *
 * @return PyObject which contains the following dictionary on success:
 *                 {'id' : <vfabric id>}
 *                  On failure, it will return NULL and throw the
 *                  relevant Python exception.
 *
 */
PyObject*
py_bxm_vfabric_online(PyObject* self, PyObject *args)
{
        PyObject* bxm_dict = NULL , *dict_object = NULL;
        /* To store the end result in dictionary */
        PyObject* result = NULL;
        bxm_error_t err = BXM_SUCCESS;
        bxm_vfabric_id_t vfabric_id = 0;
        /*validate the input object and get the dictionary object*/
        bxm_dict =  validate_dictionary(args, bxm_dict);
        /* Pass this dictionary object to get the vfabric_id*/

        if (NULL != (dict_object = 
                         PyDict_GetItemString(bxm_dict, "id"))) {
                if(PyInt_Check(dict_object))
                        vfabric_id = PyInt_AsLong(dict_object);
                else {
                        PyErr_SetString(PyExc_StandardError,
                                 "Error in parsing the vfabric id.");
                        goto out;
                }
        }    
        err = bxm_vfabric_online(vfabric_id);
        if (err != BXM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                            "Error in changing the running mode of vfabric");
                goto out;
        }
        if(-1 != add_key_values(result, "id", Py_BuildValue("i", vfabric_id)))
                return result;
out:
        return NULL;
}

/* Define the methods inside the bxm object */
static PyMethodDef BxmMethods[] = {
        {"py_bxm_vadapter_create", py_bxm_vadapter_create, METH_VARARGS,
                        "Create a vadapter and return the vadapter id."},
        {"py_bxm_vadapter_edit_general_attr",
                py_bxm_vadapter_edit_general_attr, METH_VARARGS,
          "Edit a vadapter properties and return the vadapter id."},
        {"py_bxm_vfabric_create", py_bxm_vfabric_create,
            METH_VARARGS, "Create a vfabric and return the vfabric id."},
        {"py_bxm_vfabric_edit_general_attr", py_bxm_vfabric_edit_general_attr,
                                   METH_VARARGS,
                   "Edit the vfabric properties and return the vfabric id."},
        {"py_bxm_vfabric_online", py_bxm_vfabric_online, METH_VARARGS,
                "Change the running mode of vfabric to ONLINE."},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initbxm(void)
{
        (void) Py_InitModule("bxm", BxmMethods);
}

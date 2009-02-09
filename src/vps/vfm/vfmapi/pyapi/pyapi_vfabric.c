#include <pyapi.h>


/* 
 * Parse the vfabric dictionary and fill up the structures.
 * Params :  
 * [IN] *i_vfabric_attr : the dictionary object which contains the data to
 *                         fill up the vfm_vfabric_attr_t structure.
 * [OUT] *attr            : Contains the structure of the vfabric.
 * [OUT] *bitmask         : Contains the bitmask of the vfabric structure.
 * 
 * Return : 
 *         NULL and on failure it will throw an exception.
 */

PyObject*
parse_vfabric_structure(PyObject* i_dict, vfm_vfabric_attr_t *attr,
                                vfm_vfabric_attr_bitmask_t *bitmask)
{
       PyObject* dict_object = NULL;
       PyObject* p_dict_object = NULL;

       if (NULL != (dict_object = 
                            PyDict_GetItemString(i_dict, "id"))) {
                
               /* check if the protocol is integer or not */
                if(PyInt_Check(dict_object))
                        attr->_vfabric_id = PyInt_AsLong(dict_object);
                else
                        goto out;
       }

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

int
create_vfabric_dictionary(PyObject *result, int num_result,
                                vfm_vfabric_attr_t *results)
{
        int error = 0;
        int i = 0, j =0;
        PyObject *innerDict, *temp, *list, *tempDict;
        uint64_t var = 0;

        for (i = 0; i < num_result; i++) {
                if ((innerDict = PyDict_New())== NULL) {
                        PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                        error = -1;
                        goto out;
                }
                
                temp = Py_BuildValue("s", (results+i)->name);
                if(-1 == PyDict_SetItemString(innerDict, "name", temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("s", (results+i)->desc);
                if(-1 == PyDict_SetItemString(innerDict, "desc", temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->_state);
                if(PyDict_SetItemString(innerDict, "ha_state", temp) == -1) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->primary_gateway);
                if(-1 == PyDict_SetItemString(innerDict, "primary_gateway",temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->backup_gateway);
                if(-1 == PyDict_SetItemString(innerDict, "backup_gateway",temp)) {
                        error = -1;
                        goto out;
                }

                temp = Py_BuildValue("i", (results+i)->running_mode);
                if(-1 == PyDict_SetItemString(innerDict, "running_mode",temp)) {
                        error = -1;
                        goto out;
                }
                
                temp = Py_BuildValue("i", (results+i)->auto_failover);
                if(-1 == PyDict_SetItemString(innerDict, "auto_failover",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->auto_failback);
                if(-1 == PyDict_SetItemString(innerDict, "auto_failback",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->_ctx_table_id);
                if(-1 == PyDict_SetItemString(innerDict, "ctx_table_id",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->_num_vadapter);
                if(PyDict_SetItemString(innerDict, "num_vadapter_id", temp)
                                                                   == -1) {
                        error = -1;
                        goto out;
                }
                if(NULL == (list = PyList_New((results+i)->_num_vadapter))) {
                        error = -1;
                        goto out;
                }

                for (j = 0; j < (results+i)->_num_vadapter; j++) {
                        temp = Py_BuildValue("i", 
                                        ((results+i)->_vadapter_id[j]));
                        if (-1 == PyList_SetItem(list, j, temp)) {
                                error = -1;
                                goto out;
                        }
                }
                if (PyDict_SetItemString(innerDict, "vadapter_id", list)
                                                                   == -1) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->protocol);
                if (PyDict_SetItemString(innerDict, "protocol", temp)
                                                                   == -1) {
                        error = -1;
                        goto out;
                }

                if ((results+i)->protocol == VFM_PROTOCOL_EN) {
                        if ((tempDict = PyDict_New())== NULL) {
                                PyErr_SetString(PyExc_StandardError,
                                 "Error in creating a EN protocol Dictionary");
                                error = -1;
                                goto out;
                        }
                        memcpy(&var, (results+i)->en_attr._mac, 6); 
                        temp = parse_unicode_to_string(var, 6);
                        if (-1 == PyDict_SetItemString(tempDict, "mac",temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i", (results+i)->en_attr.vlan);
                        if(-1 == PyDict_SetItemString(tempDict, "vlan",
                                                                     temp)) {
                                error = -1;
                                goto out;
                        }
                        if(-1 == PyDict_SetItemString(innerDict, "en_attr",
                                                                 tempDict)) {
                                error = -1;
                                goto out;
                        }
                }
                if ((results+i)->protocol == VFM_PROTOCOL_FC) {
                        if ((tempDict = PyDict_New())== NULL) {
                                PyErr_SetString(PyExc_StandardError,
                                  "Error in creating a FC protocol Dictionary");
                                error = -1;
                                goto out;
                        }
                        temp = parse_unicode_to_string(
                                        (results+i)->fc_attr._wwnn, 8);
                        if (-1 == PyDict_SetItemString(tempDict, "wwnn",temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = parse_unicode_to_string(
                                        (results+i)->fc_attr._wwpn, 8);
                        if (-1 == PyDict_SetItemString(tempDict, "wwpn",temp)) {
                                error = -1;
                                goto out;
                        }
                        memcpy(&var, (results+i)->fc_attr._fcid, 3);
                        temp = parse_unicode_to_string(var, 3);
                        if (-1 == PyDict_SetItemString(tempDict, "fcid",temp)) {
                                error = -1;
                                goto out;
                        }
                        if(-1 == PyDict_SetItemString(innerDict, "fc_attr",
                                                                 tempDict)) {
                                error = -1;
                                goto out;
                        }

                }
                temp = Py_BuildValue("i", (results+i)->_vfabric_id);
                if(PyDict_SetItem(result, temp, innerDict)== -1) {
                        error = -1;
                        goto out;
                }
        }
out:
        return error;
        
}


/*
 * @brief The python wrapper for calling vfm_vfabric_create.
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
py_vfm_vfabric_create(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict =  NULL, *result = NULL;
        vfm_error_t err = VFM_SUCCESS;
        vfm_vfabric_attr_t attr;
        vfm_vfabric_attr_bitmask_t bitmask;
        vfm_vfabric_id_t vfabric_id = 0;
        /*validate the input object and get the dictionary object*/
        vfm_dict =  validate_dictionary(args, vfm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vfabric_structure(vfm_dict, &attr, &bitmask);
        err = vfm_vfabric_create(attr.name, attr.desc, attr.protocol,
                                                        &vfabric_id);
        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in creating the vfabric.");
                goto out;
        }
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }

        if(-1 != add_key_values(result, "id", Py_BuildValue("i", vfabric_id)))
                return result;
out:
        return NULL;
}       

/*
 * @brief The python wrapper for calling vfm_vfabric_edit_general_attr.
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
py_vfm_vfabric_edit_general_attr(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err = VFM_SUCCESS;
        vfm_vfabric_attr_t attr;
        vfm_vfabric_attr_bitmask_t bitmask;
        /*validate the input object and get the dictionary object*/
        vfm_dict =  validate_dictionary(args, vfm_dict);
        memset(&bitmask, 0, sizeof(vfm_vfabric_attr_bitmask_t));
        /* Pass this dictionary object to parse the key and its values*/
        parse_vfabric_structure(vfm_dict, &attr, &bitmask);

        err = vfm_vfabric_edit_general_attr(attr._vfabric_id, &bitmask, &attr);
        
        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in updating the vfabric properties.");
                goto out;
        }
        
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }

        if(-1 != add_key_values(result, "id",
                Py_BuildValue("i", attr._vfabric_id)))
                return result;
out:
        return NULL;
}

/*
 * @brief The python wrapper for calling vfm_vfabric_online.
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
py_vfm_vfabric_online(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict = NULL , *dict_object = NULL;
        /* To store the end result in dictionary */
        PyObject* result = NULL;
        vfm_error_t err = VFM_SUCCESS;
        vfm_vfabric_id_t vfabric_id = 0;
        /*validate the input object and get the dictionary object*/
        vfm_dict =  validate_dictionary(args, vfm_dict);
        /* Pass this dictionary object to get the vfabric_id*/
        if (NULL != (dict_object = 
                         PyDict_GetItemString(vfm_dict, "id"))) {
                if(PyInt_Check(dict_object))
                        vfabric_id = PyInt_AsLong(dict_object);
                else {
                        PyErr_SetString(PyExc_StandardError,
                                 "Error in parsing the vfabric id.");
                        goto out;
                }
        }    
        err = vfm_vfabric_online(vfabric_id);
        if (err != VFM_SUCCESS) {
                if (err == VFM_ERROR_VFABRIC)
                        PyErr_SetString(PyExc_StandardError,"The vfabric is not associated with any vadapter");
                else if (err == VFM_ERROR_IOMODULE)
                        PyErr_SetString(PyExc_StandardError,"The io module is not associated with vadapter");
                else
                        PyErr_SetString(PyExc_StandardError,
                            "Error in changing the running mode of vfabric");
                goto out;
        }
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }
        if(-1 != add_key_values(result, "id", Py_BuildValue("i", vfabric_id)))
                return result;
out:
        return NULL;
}

PyObject *
get_vfabric_data(PyObject* self, PyObject* args)
{
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err;
        vfm_vfabric_attr_t attr;
        vfm_vfabric_attr_bitmask_t bitmask;
        uint32_t num_result = 0 ,i = 0;
        vfm_vfabric_attr_t *results = NULL;

        memset(&bitmask, 0, sizeof(vfm_vfabric_attr_bitmask_t));
        /*validate the input object and get the dictionary object*/
        if(args != NULL)
                vfm_dict = validate_dictionary(args, vfm_dict);

        /* Pass this dictionary object to parse the key and its values*/
        if(vfm_dict != NULL)
                 parse_vfabric_structure(vfm_dict, &attr, &bitmask);

        err = VFM_SUCCESS;
        
        err = vfm_vfabric_select_inventory(&attr, bitmask, 
                                                &num_result, &results);

        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in querying the Vfabric inventory");
                goto out;
        }
        if (NULL == (result = PyDict_New())) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                goto out;
        }
        if (num_result == 0) {
                PyErr_SetString(PyExc_StandardError,
                        "No Vfabric records found.");
                result = NULL;
                goto out;
        }
        if (-1 != create_vfabric_dictionary(result, num_result, results)) {
                printf("result dictionary created.. \n");
        }
        else
                PyErr_SetString(PyExc_StandardError,
                        "Error in Filling up the result dictionary");
out:
        if (results) {
                for(i = 0; i < num_result; i++) {
                        free((results+i)->_vadapter_id); 
                }
                free(results);
        }
        return result;
}


PyObject*
py_vfm_vfabric_select_inventory(PyObject *self, PyObject* args)
{
        return get_vfabric_data(self, args);
}


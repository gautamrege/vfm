#include <pyapi.h>

/* 
 * Parse the vadapter dictionary and fill up the structures.
 * Params :  
 * [IN] *i_vadapter_attr : the dictionary object which contains the data to
 *                         fill up the vfm_vadapter_attr_t structure.
 * [OUT] *attr           : Contains the structure of the vadapter.
 * [OUT] *bitmask        : Contains the bitmask set for the modifying 
 *                         attributes.
 * Return : 
 *         NULL and on failure it will throw an exception.
 */

PyObject*
parse_vadapter_structure(PyObject* i_dict, vfm_vadapter_attr_t *attr,
                                        vfm_vadapter_attr_bitmask_t *bitmask)
{
       PyObject* dict_object = NULL;
       PyObject* p_dict_object = NULL;


       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "id"))) {
                if(PyInt_Check(dict_object))
                        attr->_vadapter_id = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->id = 1;
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


/*
 * @brief The python wrapper for calling vfm_vadapter_online.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to change the running mode of 
 *                 the vadapter to online:
 *
 * @return PyObject which contains the following dictionary on success:
 *                 {'id' : <vadapter id>}
 *                  On failure, it will return NULL and throw the
 *                  relevant Python exception.
 *
 */
PyObject*
py_vfm_vadapter_online(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict = NULL , *dict_object = NULL;
        /* To store the end result in dictionary */
        PyObject* result = NULL;
        vfm_error_t err = VFM_SUCCESS;
        vfm_vadapter_id_t vadapter_id = 0;

        /*validate the input object and get the dictionary object*/
        vfm_dict = validate_dictionary(args, vfm_dict);

        /* Pass this dictionary object to get the vadapter_id*/
        if (NULL != (dict_object = 
                         PyDict_GetItemString(vfm_dict, "id"))) {
                if(PyInt_Check(dict_object))
                        vadapter_id = PyInt_AsLong(dict_object);
                else {
                        PyErr_SetString(PyExc_StandardError,
                                 "Error in parsing the vadapter id.");
                        goto out;
                }
        }    
        err = vfm_vadapter_online(vadapter_id);
        if (err != VFM_SUCCESS) {
                if (err == VFM_ERROR_VFABRIC)
                        PyErr_SetString(PyExc_StandardError,"The vadapter is not associated with any vfabric");
                else if (err == VFM_ERROR_IOMODULE)
                        PyErr_SetString(PyExc_StandardError,"The io module is not associated with vadapter");
                else
                        PyErr_SetString(PyExc_StandardError,
                            "Error in changing the running mode of vadapter");
                goto out;
        }
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }
        if(-1 != add_key_values(result, "id", Py_BuildValue("i", vadapter_id)))
                return result;
out:
        return NULL;
}

/*
 * @brief This function create the vadapter dictionary and accordingly sets the
 * the keys and values in the dictionary. It processes the result returned from
 * the API and then creates the dictionary appropriately.
 *
 * @param[in] num_result : Number of objects found in database.
 *
 * @param[in] *results : Pointer to the vadapter structure.
 *
 * @param[out] *result : Py object which will contain the dictionary of the
 *                        vadapter attrbutes.
 */

int
create_vadapter_dictionary(PyObject *result, uint32_t num_result,
                vfm_vadapter_attr_t *results)
{
        int error = 0;
        int i = 0;
        PyObject *innerDict, *temp, *tempDict;
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
                temp = Py_BuildValue("i", (results+i)->io_module_id);
                if(-1 == PyDict_SetItemString(innerDict, "io_module_id",temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->running_mode);
                if(-1 == PyDict_SetItemString(innerDict, "running_mode",temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->init_type);
                if(-1 == PyDict_SetItemString(innerDict, "init_type", temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->assignment_type);
                if(-1 == PyDict_SetItemString(innerDict, "assignment_type",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->vfabric_id);
                if(-1 == PyDict_SetItemString(innerDict, "vfabric_id",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                temp = Py_BuildValue("i", (results+i)->_gw_id);
                if(PyDict_SetItemString(innerDict, "gw_id", temp) == -1) {
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
                        memcpy(&var, (results+i)->en_attr.mac, 6); 
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
                        temp = Py_BuildValue("i", (results+i)->en_attr.mtu);
                        if(-1 == PyDict_SetItemString(tempDict, "mtu",
                                                                     temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i",
                                        (results+i)->en_attr.promiscuous_mode);
                        if(-1 == PyDict_SetItemString(tempDict,
                                             "promiscuous_mode", temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i",
                                      (results+i)->en_attr.silent_listener);
                        if(-1 == PyDict_SetItemString(tempDict,
                                           "silent_listener", temp)) {
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
                                        (results+i)->fc_attr.wwnn, 8);
                        if (-1 == PyDict_SetItemString(tempDict, "wwnn",temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = parse_unicode_to_string(
                                        (results+i)->fc_attr.wwpn, 8);
                        if (-1 == PyDict_SetItemString(tempDict, "wwpn",temp)) {
                                error = -1;
                                goto out;
                        }
                        memcpy(&var, (results+i)->fc_attr.fcid, 3);
                        temp = parse_unicode_to_string(var, 3);
                        if (-1 == PyDict_SetItemString(tempDict, "fcid",temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i", (results+i)->fc_attr.mtu);
                        if(-1 == PyDict_SetItemString(tempDict, "mtu",
                                                                     temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i", (results+i)->fc_attr.spma);
                        if(-1 == PyDict_SetItemString(tempDict, "spma", temp)) {
                                error = -1;
                                goto out;
                        }
                        temp = Py_BuildValue("i", (results+i)->fc_attr.fpma);
                        if(-1 == PyDict_SetItemString(tempDict, "fpma", temp)) {
                                error = -1;
                                goto out;
                        }
                        if(-1 == PyDict_SetItemString(innerDict, "fc_attr",
                                                                 tempDict)) {
                                error = -1;
                                goto out;
                        }

                }
                temp = Py_BuildValue("i", (results+i)->_vadapter_id);
                if(PyDict_SetItem(result, temp, innerDict)== -1) {
                        error = -1;
                        goto out;
                }
        }
out:
        return error;

        return 0;
}

/*
 * @brief
 * This function will call the API to get the vadapter information from the
 * server. The information obtained will be passed to the CLI for display.
 *
 * @param[in] args : It is tehe python object which contains the dictionary
 *                   of the keys and values.
 */
PyObject *
get_vadapter_data(PyObject* self, PyObject* args)
{
        /*validate the input object and get the dictionary object*/
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err;
        vfm_vadapter_attr_t attr;
        vfm_vadapter_attr_bitmask_t bitmask;
        uint32_t num_result = 0;
        vfm_vadapter_attr_t *results = NULL;

        memset(&bitmask, 0, sizeof(vfm_vadapter_attr_bitmask_t));
        /*validate the input object and get the dictionary object*/
        if(args != NULL)
                vfm_dict = validate_dictionary(args, vfm_dict);

        /* Pass this dictionary object to parse the key and its values*/
        if(vfm_dict != NULL)
                 parse_vadapter_structure(vfm_dict, &attr, &bitmask);

        err = VFM_SUCCESS;
        
        err = vfm_vadapter_select_inventory(&attr, &bitmask, 
                                                &num_result, &results);

        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in querying the Vadapter inventory");
                goto out;
        }
        if (NULL == (result = PyDict_New())) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                goto out;
        }
        if (num_result == 0) {
                PyErr_SetString(PyExc_StandardError,
                        "No Vadapter records found.");
                result = NULL;
                goto out;
        }
        if (-1 != create_vadapter_dictionary(result, num_result, results)) {
                printf("result dictionary created.. \n");
        }
        else
                PyErr_SetString(PyExc_StandardError,
                        "Error in Filling up the result dictionary");
out:
        if (results) 
             free(results);

        return result;

}

/*
 * @brief
 * This functions is the python wrapper for the C API implementation.
 * It converts the input Python dictionaries to C structures which are
 * then used by the API's.
 */
PyObject *
py_vfm_vadapter_select_inventory(PyObject* self, PyObject *args)
{
        return get_vadapter_data(self, args);
}

PyObject*
py_vfm_vadapter_query_general_attr(PyObject* self, PyObject* args)
{
        return get_vadapter_data(self, args);
}
/*
 * @brief The python wrapper for calling vfm_vadapter_create.
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
py_vfm_vadapter_create(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err;
        vfm_vadapter_attr_t attr;
        vfm_vadapter_attr_bitmask_t bitmask;
        vfm_vadapter_id_t vadapter_id = 0;
        /*validate the input object and get the dictionary object*/
        vfm_dict = validate_dictionary(args, vfm_dict);
        /* Pass this dictionary object to parse the key and its values*/
        parse_vadapter_structure(vfm_dict, &attr, &bitmask);
        err = VFM_SUCCESS;
        err = vfm_vadapter_create(attr.name, attr.desc, attr.protocol,
                                                        &vadapter_id);
        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in creating the vadapter.");
                goto out;
        }
        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }
        if (-1 != add_key_values(result, "id", Py_BuildValue("i",vadapter_id)))
                return result;
out:
        return NULL;
}
/*
 * @brief The python wrapper for calling vfm_vadapter_edit_general_attr.
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
py_vfm_vadapter_edit_general_attr(PyObject* self, PyObject *args)
{
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err = VFM_SUCCESS; 
        vfm_vadapter_attr_t attr;
        vfm_vadapter_attr_bitmask_t bitmask;
        /*validate the input object and get the dictionary object*/
        vfm_dict =  validate_dictionary(args, vfm_dict);
        memset(&bitmask, 0, sizeof(vfm_vadapter_attr_bitmask_t));
        /* Pass this dictionary object to parse the key and its values*/
        parse_vadapter_structure(vfm_dict, &attr, &bitmask);
        
        err = vfm_vadapter_edit_general_attr(attr._vadapter_id, &bitmask,
                                                          &attr);
        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in upadating the vadapter properties.");
                goto out;
        }
        

        if ((result = PyDict_New())== NULL) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                return NULL;
        }
        if(-1 != add_key_values(result, "id",
                                Py_BuildValue("i", attr._vadapter_id)))
                return result;
out:
        return NULL;
}

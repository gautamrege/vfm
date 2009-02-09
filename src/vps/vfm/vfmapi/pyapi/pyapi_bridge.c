/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <pyapi.h>
//#define TEST1



int 
parse_bridge_structure(PyObject* i_dict, vfm_bd_attr_t *attr,
                                        vfm_bd_attr_bitmask_t *bitmask)
{
       PyObject* dict_object = NULL;
       int error = 0;
       char *addr;
       uint8_t guid[8];

       if (NULL != (dict_object = 
                         PyDict_GetItemString(i_dict, "bd_guid"))) {

                        addr = PyString_AsString(dict_object);
                        if(-1 == parse_string_to_unicode(addr, 8, guid)) {
                                error = -1;
                                goto out;
                        }
                        memcpy(&attr->_bd_guid, guid, sizeof(guid));
                        bitmask->guid = 1;
       }

       if (NULL != (dict_object = PyDict_GetItemString(i_dict, "desc"))) {
                memcpy(attr->desc, PyString_AsString(dict_object),
                                sizeof(attr->desc));
                bitmask->desc = 1;
       }
       
       if (NULL != (dict_object = 
                            PyDict_GetItemString(i_dict, "state"))) {
                
               /* check if the protocol is integer or not */
                if(PyInt_Check(dict_object))
                        attr->_state = PyInt_AsLong(dict_object);
                else
                        goto out;
                bitmask->state = 1;
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
                         PyDict_GetItemString(i_dict, "firmware_version"))) {
                memcpy(attr->_firmware_version, PyString_AsString(dict_object),
                                        sizeof(attr->_firmware_version));
                bitmask->firmware_version = 1;
       }
       if (NULL != (dict_object = 
                       PyDict_GetItemString(i_dict,"vfm_guid"))) {
                        addr = PyString_AsString(dict_object);
                        if(-1 == parse_string_to_unicode(addr, 8, guid)) {
                                error = -1;
                                goto out;
                        }
                        memcpy(&attr->_vfm_guid, guid, sizeof(guid));
                bitmask->vfm_guid = 1;
       }
out:
       return error;
}

int
create_bd_dictionary(PyObject *result, uint32_t num_result,
                                                vfm_bd_attr_t *results)
{
        /* 
         * If the dict is not a dictionary object then create is as a new 
         * dictionary object and fill up the values.
         */
        int error = 0;
        int i = 0, j =0;
        char tmp_buff[64];
        PyObject *innerDict, *temp, *list;
        
        for (i = 0; i < num_result; i++) {
                if ((innerDict = PyDict_New())== NULL) {
                        PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                        error = -1;
                        goto out;
                }

                temp = Py_BuildValue("i", (results+i)->_state);
                if(PyDict_SetItemString(innerDict, "state", temp) == -1) {
                        error = -1;
                        goto out;
                }
               
                temp = Py_BuildValue("s", (results+i)->desc);
                if(-1 == PyDict_SetItemString(innerDict, "desc", temp)) {
                        error = -1;
                        goto out;
                }


                temp = Py_BuildValue("i", (results+i)->_last_keep_alive);
                if(-1 == PyDict_SetItemString(innerDict, "last_keep_alive",temp)) {
                        error = -1;
                        goto out;
                }

                temp = Py_BuildValue("i", (results+i)->running_mode);
                if(-1 == PyDict_SetItemString(innerDict, "running_mode",temp)) {
                        error = -1;
                        goto out;
                }
                
                temp = Py_BuildValue("s", (results+i)->_firmware_version);
                if(-1 == PyDict_SetItemString(innerDict, "firmware_version",
                                                                  temp)) {
                        error = -1;
                        goto out;
                }
                
                temp = Py_BuildValue("i", (results+i)->_num_gw_module);
                if(PyDict_SetItemString(innerDict, "num_gw_modules", temp)
                                                                   == -1) {
                        error = -1;
                        goto out;
                }
                if(NULL == (list = PyList_New((results+i)->_num_gw_module))) {
                        error = -1;
                        goto out;
                }

                for (j = 0; j < (results+i)->_num_gw_module; j++) {
                        memset(tmp_buff, 0, sizeof(tmp_buff));
                        memcpy(tmp_buff, (results+i)->_gw_module_index[j],
                                        sizeof(vfm_gw_module_index_t));
                        temp = Py_BuildValue("s", tmp_buff);
                        if (-1 == PyList_SetItem(list, j, temp)) {
                                error = -1;
                                goto out;
                        }
                }
                if(PyDict_SetItemString(innerDict, "gw_module_index", list)
                                                                   == -1) {
                        error = -1;
                        goto out;
                }

                temp = parse_unicode_to_string((results+i)->_vfm_guid,8);
                if(-1 == PyDict_SetItemString(innerDict, "vfm_guid",temp)) {
                        error = -1;
                        goto out;
                }
                temp = parse_unicode_to_string((results+i)->_bd_guid,8);
                PyDict_SetItem(result, temp, innerDict);
        }

out:
        return error;
}

PyObject *
get_bridge_data(PyObject* self, PyObject* args)
{
        PyObject* vfm_dict = NULL, *result = NULL;
        vfm_error_t err;
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        uint32_t num_result = 0;
        vfm_bd_attr_t *results = NULL;
        int i = 0;

        memset(&bitmask, 0, sizeof(vfm_bd_attr_bitmask_t));
        /*validate the input object and get the dictionary object*/
        if(args != NULL)
                vfm_dict = validate_dictionary(args, vfm_dict);

        /* Pass this dictionary object to parse the key and its values*/
        if(vfm_dict != NULL)
                 parse_bridge_structure(vfm_dict, &attr, &bitmask);

        err = VFM_SUCCESS;
        
#ifdef TEST1
        results = malloc(3 * sizeof(vfm_bd_attr_t));
        memset(results, 0, 3 * sizeof(vfm_bd_attr_t));
        num_result = 3;
        results[0]._bd_guid = 0x0321234;
        results[1]._bd_guid = 0x2323232;
        results[2]._bd_guid = 0x1231234;
        
        strcpy(results[0].desc,"bd 1");
        strcpy(results[1].desc,"bd 2");
        strcpy(results[2].desc ,"bd 3");
        
        strcpy(results[0]._firmware_version, "v1.1");
        strcpy(results[1]._firmware_version, "v1.1");
        strcpy(results[2]._firmware_version, "v1.1");
#else
        err = vfm_bd_select_inventory(&attr, bitmask, &num_result, &results);
#endif        

        if (err != VFM_SUCCESS) {
                PyErr_SetString(PyExc_StandardError,
                                "Error in querying the Bridge inventory");
                goto out;
        }
        if (NULL == (result = PyDict_New())) {
                PyErr_SetString(PyExc_StandardError,
                        "Error in creating a dictionary to store the results");
                goto out;
        }
        if (num_result == 0) {
                PyErr_SetString(PyExc_StandardError,
                        "No Bridge records found.");
                result = NULL;
                goto out;
        }
        if (-1 != create_bd_dictionary(result, num_result, results)) {
                printf("result dictionary created.. \n");
        }
        else
                PyErr_SetString(PyExc_StandardError,
                        "Error in Filling up the result dictionary");
out:
        if (results) {
                for (i = 0; i < num_result; i++) {
                        free((results + i)->_gw_module_index);
                }
                free(results);
        }
        return result;
}

/*
 * @brief The python wrapper for calling vfm_bd_select_inventory.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the bridge inventory:
 *            {'bd_guid' : <bridge_guid>,
 *             'desc' : * <desc>, 
 *             'state' : <state>,
 *             'running_mode' : <running_mode>,
 *             'firmware_version' : <firmware_version>,
 *             'vfm_guid' : <vfm_guid>
 *             }
 *
 * @return PyObject which contains the following dictionary on success:
 *            {'bd_guid' : <bridge_guid>, 
 *             'desc' : * <desc>, 
 *             'state' : <state>,
 *             'running_mode' : <running_mode>,
 *             'firmware_version' : <firmware_version>,
 *             'vfm_guid' : <vfm_guid>,
 *             'num_gw_modules' : <Number of gateway modules>,
 *             'gateway_module_index' :<Dictionary containing the 
 *                                           gateway module index>
 *             'last_keep_alive' : <last keepalive from BridgeX device>
 *             }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_bd_select_inventory(PyObject* self, PyObject *args)
{
        return get_bridge_data(self, args);
}

PyObject*
py_vfm_bd_query_general_attr(PyObject* self, PyObject* args)
{
        return get_bridge_data(self, args);
}

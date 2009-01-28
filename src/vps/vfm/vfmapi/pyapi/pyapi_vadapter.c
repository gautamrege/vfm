#include <pyapi.h>
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
        vfm_dict =  validate_dictionary(args, vfm_dict);
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


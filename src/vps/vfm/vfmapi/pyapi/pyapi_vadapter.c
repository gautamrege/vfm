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
 * @brief This function parses the vadapter dictionary and accordingly sets the
 * bitmask of the keys given in the dictionary. And also sets their values in
 * the attibute structure respectively.
 *
 * @param[in] i_dict : Input dictionary with keys and values of vadapter.
 *
 * @param[out] *attr : Pointer to the vadapter structure.
 *
 * @param[out] *bitmask : Pointer to the bitmask.
 *
int
parse_vadapter_structure(PyObject* i_dict, vfm_vadapter_attr_t *attr,
                                        vfm_vadapter_attr_bitmask_t *bitmask)
{
      * Parse the dictionary.
      * Set bitmask
      * Set values in structures.
      *
}
*/

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
                vfm_bd_attr_t *results)
{
        /* Create new dictionary if required. If error then return the error */

        /* Set the Keys and the values for all the attributes */

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


        /*
         * Pass this dictionary object to parse the key and its values
         * Set the values and the bitmask.
         */

        /*
         * Call the parse_vadapter_structure() function.
         * Which internally calls the API function.
         */
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

/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <pyapi.h>

/* Parse the guid and send the actual values in the uint8 array*/        
int
parse_string_to_unicode(char *buff,
                int length,
                uint8_t *out_buff)
{
        int i =0, j=0;
        for (i =0; i < length*3-1; i++) {
                if (buff[i] == ':')
                        continue;
                else if (buff[i] >= 'A' && buff[i] <= 'F')
                        buff[i] -= 55;
                else if (buff[i] >= 'a' && buff[i] <= 'f')
                        buff[i] -= 87;
                else if (buff[i] >= '0' && buff[i] <= '9')
                        buff[i] -= 48;
                else
                        return -1;
        }
        for (i =0; i < length; i++, j+=3)
                out_buff[i] = buff[j]<<4 | buff[j+1];
        return 0;
}

/* Convert the uint64 value in uint8 and generate a string in the guid format
 * e.g : FF:65:00:D3:56:E5:1A:52
 */
PyObject *
parse_unicode_to_string(uint64_t val, int length)
{
        uint8_t guid[8];
        uint8_t buff[25];
        int i = 0; 
        int j = 0;

        memset(buff, 0, sizeof(buff));
        memset(guid, 0, sizeof(guid));
        memcpy(guid, &val, sizeof(guid));

        for(i =0 ; i < length; i++)
        {
                buff[j]   = ((guid[i] & 0xF0) >> 4);
                if (buff[j] <=0x09)
                        buff[j] += 48;
                else if (buff[j] >= 0x0A && buff[j] <= 0x0F)
                        buff[j] += 55;

                j++;
                buff[j] = (guid[i] & 0x0F );
                if (buff[j] <= 0x09)
                        buff[j] += 48;
                else if (buff[j] >= 0x0A && buff[j] <= 0x0F)
                        buff[j] += 55;
                j++;
                if ( i < (length-1))
                        buff[j] = ':';
                j++;
        }
        return Py_BuildValue("s", buff);
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


/* Define the methods inside the vfm object */
static PyMethodDef VfmMethods[] = {
        {"py_vfm_vadapter_create", py_vfm_vadapter_create, METH_VARARGS,
                        "Create a vadapter and return the vadapter id."},
        {"py_vfm_vadapter_edit_general_attr",
                py_vfm_vadapter_edit_general_attr, METH_VARARGS,
          "Edit a vadapter properties and return the vadapter id."},
        {"py_vfm_vfabric_create", py_vfm_vfabric_create,
            METH_VARARGS, "Create a vfabric and return the vfabric id."},
        {"py_vfm_vfabric_edit_general_attr", py_vfm_vfabric_edit_general_attr,
                                   METH_VARARGS,
                   "Edit the vfabric properties and return the vfabric id."},
        {"py_vfm_vfabric_online", py_vfm_vfabric_online, METH_VARARGS,
                "Change the running mode of vfabric to ONLINE."},
        {"py_vfm_vadapter_online", py_vfm_vadapter_online, METH_VARARGS,
                "Change the running mode of vadapter to ONLINE."},
        {"py_vfm_bd_select_inventory", py_vfm_bd_select_inventory,
                 METH_VARARGS, "Display the Bridge device inventory."},
        {"py_vfm_bd_query_general_attr", py_vfm_bd_query_general_attr,
                 METH_VARARGS, "Query the Bridge device inventory."},
        {"py_vfm_vfabric_select_inventory", py_vfm_vfabric_select_inventory,
                 METH_VARARGS, "Query the Vfabric inventory."},
        {"py_vfm_vadapter_select_inventory", py_vfm_vadapter_select_inventory,
                 METH_VARARGS, "Query the Vadapter inventory."},

        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initvfm(void)
{
        (void) Py_InitModule("vfm", VfmMethods);
}

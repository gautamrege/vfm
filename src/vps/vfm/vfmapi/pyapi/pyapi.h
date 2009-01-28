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
#include <vfm_vfabric.h>
#include <vfm_vadapter.h>

/*
 * Validate the input data, convert it into the python object and then validate
 * if it is a dictionary object or not.
 */
PyObject *
validate_dictionary(PyObject * , PyObject *);

/* Add a key and its value in the result dictionary*/
int
add_key_values(PyObject* result, const char* key, PyObject * value);



/*
 * @brief The python wrapper for calling vfm_vadapter_create.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for vadapter creation:
 *            {'name' : <string[64]:name>,
 *             'desc' : <string[64]:desc>,
 *             'protocol' : <1|2|3: 1=EN, 2=IB, 3=FC>
 *            }
 * Example: {'name':'vadapter1', 'desc':'vadapter1 desc', 'protocol':1}
 *
 * @note Mandatory params: 'name', 'protocol'.
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <int:vadapter id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_create(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vadapter_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vadapter properties :
 *            {
 *              'id' : <int:vadapter_id>, 
 *              'name' : <string[64]:name>,
 *              'desc' : <string[64]:desc>,
 *              'protocol' : <1|2|3: 1=EN, 2=IB, 3=FC>
 *              'running_mode' : <0|1: 0=OFFLINE, 1=ONLINE>
 *              'io_module_id' : <int:io_module_id>,
 *              'init_type' : <1|2: 1=HOST_INITIATED, 2=NETWORK_INITITATED>
 *              'assignment_type' : <1|2|3: 1=AUTO, 2=ONCE, 3=MANUAL>,
 *              'vfabric_id' : <int:vfabric_id>
 *              'en_attr' : <Dictionary to edit the en attributes>
 *                           {
 *                              'mac' : <string:'xx:xx:xx:xx:xx:xx'>,
 *                              'vlan' : <int_array:vlan ids>,
 *                              'mtu' : <int:mtu>,
 *                              'promiscuous_mode' : <0|1: 0=no, 1=yes>
 *                              'silent_listener' : <0|1: 0=no, 1=yes>
 *                           }
 *              'fc_attr' : <Dictionary to edit the fc attributes>
 *                           {
 *                              'wwnn' : <string:'xx:xx:xx:xx:xx:xx:xx:xx'>,
 *                              'wwpn' : <string:'xx:xx:xx:xx:xx:xx:xx:xx'>,
 *                              'fcid' : <string:'xx:xx:xx'>,
 *                              'mtu' : <int:mtu>
 *                              'spma' : <0|1: 0=DISABLED, 1=ENABLED>,
 *                              'fpma' : <0|1: 0=DISABLED, 1=ENABLED>,
 *                           }
 *           }
 *
 * Example: { 'id' : 1, 'name' : 'vadapter1', 'protocol' : 1,
 *            'en_attr' : { 'mac' : 'AA:BB:CC:DD:EE:FF', 
 *                          'vlan' : [ 1, 2, 3 ],
 *                          'mtu' : 1500 }
 *          }
 *
 * @note Mandatory fields: 'id'
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vadapter id> }. 
 * On failure, it will return NULL and throw the relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_edit_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_create.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for vfabric creation:
 *            {'name' : <string[64]:name>,
 *             'desc' : <string[64]:desc>,
 *             'protocol' : <1|2|3: 1=EN, 2=IB, 3=FC>
 *            }
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <int:vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vfabric_create(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vfabric properties:
 *            {
 *              'id' : <int:vfabric_id>, 
 *              'name' : <string[64]:name>,
 *              'desc' : <string[64]:desc>,
 *              'protocol' : <1|2|3: 1=EN, 2=IB, 3=FC>
 *              'running_mode' : <0|1: 0=OFFLINE, 1=ONLINE>
 *              'primary_gateway' : <int:gateway id>,
 *              'backup_gateway' : <int:backup_gateway>,
 *              'ha_state' : <0|1: 0=NORMAL, 1=FAILOVER>
 *              'auto_failover' : <0|1: 0=DISABLED, 1=ENABLEd>
 *              'auto_failback' : <0|1: 0=DISABLED, 1=ENABLEd>
 *              'en_attr' : <Dictionary to edit the en attributes>
 *                           {
 *                              'vlan' : <int_array:vlan ids>,
 *                           }
 *              'v_fc_attr' : <Dictionary to edit the fc attributes>
 *                           {
 *                              TODO:Empty dictionary as we can not edit any
 *                              attribute of the vfabric_fc_attr
 *                           }
 *           }
 *
 * Example: { 'id' : 1, 'name' : 'vadapter1', 'protocol' : 1,
 *            'en_attr' : { 'vlan' : [ 1, 2, 3 ]}
 *          }
 *
 * @note Mandatory fields: id
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <int:vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vfabric_edit_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_online.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to change the running mode of 
 *                 the vfabric to online:
 *                 {'id' : <int:vfabric id>}
 *
 * @return PyObject which contains the following dictionary on success:
 *                 {'id' : <vfabric id>}
 *                  On failure, it will return NULL and throw the
 *                  relevant Python exception.
 *
 */
PyObject *
py_vfm_vfabric_online(PyObject* self, PyObject *args);


/*
 * @brief The python wrapper for calling vfm_vadapter_online.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to change the running mode of 
 *                 the vadapter to online:
 *                 {'id' : <int:vadapter id>}
 *
 * @return PyObject which contains the following dictionary on success:
 *                 {'id' : <vadapter id>}
 *                  On failure, it will return NULL and throw the
 *                  relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_online(PyObject* self, PyObject *args);

#endif /* INCLUDED_pyapi_functions */

/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_pyapi_vfabric_functions
#define INCLUDED_pyapi_vfabric_functions

#include <vfm_vfabric.h>
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
 * { 'id' : <int:vfabric id> }. On failure, it will return NULL and throw he
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
 * @brief The python wrapper for calling vfm_vfabric_select_inventory. 
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to show the vfabric inventory:
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
 *              'en_attr' : <Dictionary for the en attributes>
 *                           {
 *                              'mac' : <string : xx:xx:xx:xx:xx:xx>
 *                              'vlan' : <int vlan id>,
 *                           }
 *              'v_fc_attr' : <Dictionary for the fc attributes>
 *                           {
 *                              'wwnn' : <long>
 *                              'wwpn' : <long>
 *                              'fcid' : <string : xx:xx:xx>
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
 */
PyObject *
py_vfm_vfabric_select_inventory(PyObject *self, PyObject *args);
#endif /* INCLUDED _pyapi_vfabric_functions*/

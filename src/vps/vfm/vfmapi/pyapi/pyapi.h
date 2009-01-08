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
PyObject *py_vfm_vadapter_create(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vadapter_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vadapter properties :
 *            {
                'id' : <vadapter_id>, 
 *              'name' : <name>, 'desc' : * <desc>,
 *              'protocol' : <protocol>, 'running_mode' : <running_mode>
 *              'io_module_id' : <io_module_id>, 'init_type' : <init_type>
 *              'assignment_type' : <assignment_type>,
 *              'vfabric_id' : <vfabric_id>
 *              'v_en_attr' : <Dictionary to edit the en attributes>
 *                           {
 *                              'mac' : <mac>, 'vlan' : <vlan>,
 *                              'mtu' : <mtu>,
 *                              'promiscuous_mode' : <promiscuous_mode>
 *                              'silent_listener' : <silent_listener>
 *                           }
 *              'v_fc_attr' : <Dictionary to edit the fc attributes>
 *                           {
 *                              'wwnn' : <wwnn>, 'wwpn' : <wwpn>
 *                              'fcid' : <fcid>, 'mtu' : <mtu>
 *                              'spma' : <spma>, 'fpma' : <fpma>
 *                           }
 *           }
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vadapter id> }. 
 * On failure, it will return NULL and throw the relevant Python exception.
 *
 */
PyObject *py_vfm_vadapter_edit_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_create.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for vfabric creation:
 *            {'name' : <name>, 'desc' : * <desc>, 'protocol' : <protocol> }
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *py_vfm_vfabric_create(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_edit_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to edit the vfabric properties:
 *            {
                'id' : <vfabric_id>, 
 *              'name' : <name>, 'desc' : * <desc>,
 *              'protocol' : <protocol>, 'running_mode' : <running_mode>
 *              'primary_gateway' : <primary_gateway>,
 *              'backup_gateway' : <backup_gateway>,
 *              'ha_state' : <ha_state>
 *              'auto_failover' : <auto_failover>
 *              'auto_failback' : <auto_failback>
 *              'v_en_attr' : <Dictionary to edit the en attributes>
 *                           {
 *                              'vlan' : <vlan>,
 *                           }
 *              'v_fc_attr' : <Dictionary to edit the fc attributes>
 *                           {
 *                              TODO:Empty dictionary as we can not edit any
 *                              attribute of the vfabric_fc_attr
 *                           }
 *           }
 *
 * @return PyObject which contains the following dictionary on success:
 * { 'id' : <vfabric id> }. On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *py_vfm_vfabric_edit_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vfabric_online.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to change the running mode of 
 *                 the vfabric to online:
 *                 {'id' : <vfabric id>}
 *
 * @return PyObject which contains the following dictionary on success:
 *                 {'id' : <vfabric id>}
 *                  On failure, it will return NULL and throw the
 *                  relevant Python exception.
 *
 */
PyObject *py_vfm_vfabric_online(PyObject* self, PyObject *args);

#endif /* INCLUDED_pyapi_functions */

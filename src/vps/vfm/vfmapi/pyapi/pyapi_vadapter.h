/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_pyapi_vadapter_functions
#define INCLUDED_pyapi_vadapter_functions

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vfm_error.h>
#include <vfm_common.h>
#include <vfm_vadapter.h>

/*
 * @brief The python wrapper for calling vfm_vadapter_select_inventory.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the vadapter inventory:
 *            { 
 *              'vadapter_id' : <vadapter_id>, 
 *              'desc' : * <desc>, 
 *              'state' : <state>,
 *              'running_mode' : <running_mode>,
 *
 * @return PyObject which contains the following dictionary on success:
 *            {'vadapter_id' : <Dictionary containing the attributes of 
 *                              the vadapter>
 *                          {
 *                              'name' : <name>,
 *                              'desc' : <desc>,
 *                              'protocol' : <protocol>,
 *                              'state' : <state>,
 *                              'running_mode' : <running_mode>,
 *                              'io_module_id' : <io_module_id>,
 *                              'init_type'    : <init_type>
 *                              'assignment_type : <assignment_type>
 *                              'en_attr : <Dictionary of en attributes>
 *                              'fc_attr : <Dictionary of fc attributes>
 *                              'vfabric_id : <vfabric_id>,
 *                              'gw_id' : <gw_id>
 *                          }
 *           }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_select_inventory(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vadapter_query_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the properties of the specific 
 *            vadapter :
 *            {'vadapter_id' : <vadapter_id>}
 *                          {
 *             TODO: These fields may not be needed as we are querying only
 *                   specific vadapter.
 *                              'name' : <name>,
 *                              'desc' : <desc>,
 *                              'protocol' : <protocol>,
 *                              'state' : <state>,
 *                              'running_mode' : <running_mode>,
 *                              'io_module_id' : <io_module_id>,
 *                              'init_type'    : <init_type>
 *                              'assignment_type : <assignment_type>
 *                              'en_attr : <Dictionary of en attributes>
 *                              'fc_attr : <Dictionary of fc attributes>
 *                              'vfabric_id : <vfabric_id>,
 *                              'gw_id' : <gw_id>
 *                          }
 *
 *           }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_query_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_vadapter_query_port_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to query the properties of the port in the 
 * vadapter:
 *            {
 *             'vadapter_id' : <vadapter_id>,
 *            }
 *
 * @return PyObject which contains the following vfm_gw_module_attr_t dictionary * on success :
 *            {
 *             'port_id' : <port id>, 
 *             'physical_index' : <physical_index>, 
 *             'type' : <type>,
 *             'protocol' : <protocol>,
 *             'state' : <port state>,
 *             'mtu' : <Dictinary of either en or fc or ib attributes>
 *                       {
 *                        'en_attr' : 
 *                                       {
 *                                        'mac' : <mac address>
 *                                        'vlan': <vlan numbers>
 *                                       }
 *                        'fc_attr' :   
 *                                       {
 *                                        'wwnn' : <wwnn>,
 *                                        'wwpn' : <wwpn>,
 *                                        'fcid' : <fcid>
 *                                       }
 *                        'ib_attr' : 
 *                                       {
 *                                        'ib_guid' : <guid>,
 *                                        'lid' : <lid>,
 *                                        'qpn' : <qpn>,
 *                                        'sl' : <sl>
 *                                       }
 *                       }
 *            }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_vadapter_query_port_attr(PyObject* self, PyObject *args);

#endif /* INCLUDED_pyapi_vadapter_functions */

/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_pyapi_bridge_functions
#define INCLUDED_pyapi_bridge_functions

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vfm_error.h>
#include <vfm_common.h>
#include <vfm_bridge_device.h>

/*
 * @brief The python wrapper for calling vfm_bd_select_inventory.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the bridge inventory:
 *            { 
 *              'bd_guid' : <bridge_guid>, 
 *              'desc' : * <desc>, 
 *              'state' : <state>,
 *              'running_mode' : <running_mode>,
 *              'firmware_version' : <firmware_version>,
 *              'vfm_guid' : <vfm_guid>
 *            }
 *
 * @return PyObject which contains the following dictionary on success:
 *            {'bd_guid' : <Dictionary containing the attributes of 
 *                              the bridge>
 *                          {
 *                              'desc' : * <desc>, 
 *                              'state' : <state>,
 *                              'running_mode' : <running_mode>,
 *                              'firmware_version' : <firmware_version>,
 *                              'vfm_guid' : <vfm_guid>,
 *                              'num_gw_modules' : <Number of gateway modules>,
 *                              'gateway_module_index' : <List
 *                                      containing the gateway module index>,
 *                              'state' : <state>
 *                              'last_keep_alive' : <last keepalive from 
 *                                                              BridgeX device>
 *                          }
 *           }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_bd_select_inventory(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_bd_query_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the properties of the specific 
 *            bridge device :
 *            {'bd_guid' : <bridge_guid>}
 *
 * @return PyObject which contains the following dictionary on success:
 *            {'bd_guid' : <Dictionary containing the attributes of
 *                              the bridge>
 *                          {
 *                              'desc' : * <desc>, 
 *                              'state' : <state>,
 *                              'running_mode' : <running_mode>,
 *                              'firmware_version' : <firmware_version>,
 *                              'vfm_guid' : <vfm_guid>,
 *                              'num_gw_modules' : <Number of gateway modules>,
 *                              'gateway_module_index' : <List
 *                                      containing the gateway module index>,
 *                              'state' : <state>
 *                              'last_keep_alive' : <last keepalive from 
 *                                                              BridgeX device>
 *                          }
 *           }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_bd_query_general_attr(PyObject* self, PyObject *args);

/*
 * @brief The python wrapper for calling vfm_bd_query_general_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary for displaying the gateway module inventory:
 *            {
 *             'bd_guid' : <bridge_guid>,
 *             'num_gw_modules' : <number of gateway module>,
 *             'gateway_module_ids' : <Dictionary of the gateway module ids>
 *            }
 *
 * @return PyObject which contains the following vfm_gw_module_attr_t 
 * dictionary on success :
 *            {'gw_module_id' : <gateway module id>, 
 *             'gw_module_index' : <gateway_module_index>, 
 *             'int_protocol' : <internal protocol>,
 *             'ext_protocol' : <external protocol>,
 *             'num_int_ports' : <number of internal ports>,
 *             'num_ext_ports' : <number of external ports>,
 *             'int_ports' : <Dictionary of the internal port index>,
 *             'ext_ports' : <Dictionary of the external port index>,
 *             'num_gw' : <number of gateways per gateway module>,
 *             'gw_ids' : <Dictionary of the gateway ids>
 *             }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_gw_module_attr_t(PyObject* self, PyObject *args);


/*
 * @brief The python wrapper for calling vfm_bd_query_port_attr.
 *
 * @param[in] self The python object
 *
 * @param[in] args Dictionary to query the properties of the port in the 
 * BridgeX device:
 *            {
 *             'bd_guid' : <bridge_guid>,
 *             'num_ports' : <number of ports>,
 *             'port_ids' : <dictionary of the port ids>
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
 *             'supported_speed' : <Supported speed of the port>,
 *             'actual_speed' : <Actual speed of the port>,
 *             'rx_stat' : <Incomming traffic stats on the port>,
 *             'tx_stat' : <Incomming traffic stats on the port>
 *            }
 * On failure, it will return NULL and throw the
 * relevant Python exception.
 *
 */
PyObject *
py_vfm_bd_query_port_attr(PyObject* self, PyObject *args);
#endif /* INCLUDED_pyapi_bridge_functions */

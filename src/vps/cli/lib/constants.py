#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#

GEN_UNSPECIFIED       =   0    # any unspecified error; default.
GEN_SYNTAX_ERROR      =   1
GEN_UNKNOWN_OBJECT    =   2
GEN_NOT_ACTIVE        =   3
GEN_NOT_PRIMARY       =   4

TERMINAL_COUNTS = 1

SOFTWARE_VERSION = "0.0.1"

VPS_PRODUCT = "BridgeX Manager"

INTERNAL_ERROR_MESSAGE = \
"""An Error has occurred. Please contact VirtualPlane Systems Customer Support."""

_VFM_STAN     = 'BXM>'
_VFM_EXEC     = 'BXM#'
_CLI_PROMPT_  = 'BXM'

_VIEW_VADAPTER = "v_bxm_vadapter_attr"
_VIEW_BRIDGE = "v_bxm_bridge_attr"
_VIEW_GATEWAY = "v_bxm_gateway_attr"
_VIEW_IOMODULE = "bxm_io_module_attr"

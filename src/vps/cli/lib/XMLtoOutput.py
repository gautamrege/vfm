#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#
import xml.dom.minidom

import escape

def _literal_value(str, try_unescape):
    """If the string is all whitespace, then we return ''. Otherwise we
	return the str itself. If try_unescape is True, we try to use
        escape's unescape() method on the string. Errors are ignored.
    """
    if str == None or str.strip() == '':
	return ''
    if try_unescape:
        try:
            return escape.unescape(str)
        except:
            return str
    return str

def _get_leaf_value(node, try_unescape):
    """Return a string if it's a leaf node storing only text.
	Return None otherwise.
	The definition of leaf is that all its children are TEXT_NODE.
	And we normalize those TEXT_NODEs so we only have one child.
    """
    node.normalize()
    if not node.hasChildNodes():
	return _literal_value(node.nodeValue, try_unescape)

    if len(node.childNodes) > 1:
	return None

    child = node.childNodes[0]
    if child.nodeType == xml.dom.Node.TEXT_NODE:
	return _literal_value(child.nodeValue, try_unescape)
    return None
	

def _recurse_make_dict(node, list, try_unescape):
    """Add all children of this node to the dict. If this node is a leaf,
	then add the literal value. Otherwise, add a dictionary.
    """
    data = _get_leaf_value(node, try_unescape)
    if data is None:
	dict = {}
	list.append(dict)
	for child in node.childNodes:
	    if child.nodeType == xml.dom.Node.TEXT_NODE:
		continue
	    if dict.has_key(child.nodeName):
		child_list = dict[child.nodeName]
	    else:
		child_list = []
		dict[child.nodeName] = child_list
	    _recurse_make_dict(child, child_list, try_unescape)
    else:
	list.append(data)


def make_dict(file, parentElementName, try_unescape=False):
    """Pull out all the child elements of 'parentElementName' and
       return them in a dictionary where the keys are the element names.
       file - The source of the XML: either a file name or a file stream.
    """
    dom = xml.dom.minidom.parse(file)
    roots = dom.getElementsByTagName(parentElementName)
    if len(roots) == 0:
	raise KeyError(parentElementName)

    root = roots[0]

    list = []
    _recurse_make_dict(root, list, try_unescape)
    # The only element in the list is the dict of the root
    return list[0]


                      

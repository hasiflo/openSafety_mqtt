#ifndef _XML_HELPER_H_
#define _XML_HELPER_H_

#include <libxml/tree.h>
#include <stdint.h>

/* Descend when finding/walking */
#define XML_DESCEND (uint8_t)1
/* Don't descend when finding/walking */
#define XML_NO_DESCEND (uint8_t)0
/* Descend for first find */
#define XML_DESCEND_FIRST (uint8_t)-1

/**
 * xmlWalkNext Walk to the next logical node in the tree
 * @param node current node
 * @param top constrains the search to a particular node's children
 * @param descend Search mode, toggles if search descendes into tree
 * @return next logical node in the tree
 */
xmlNodePtr xmlWalkNext(xmlNodePtr node, xmlNodePtr top, int descend);

/**
 * xmlFindElement
 * @param node Current node
 * @param top constrains the search to a particular node's children
 * @param name Name of a node, or NULL for all
 * @param attr Name of an attribute, or NULL for all
 * @param value Value of an attribute, or NULL for all
 * @param descend Search mode, toggles if search descendes into tree
 * @return node that matches search parameter
 */
xmlNodePtr xmlFindElement (xmlNodePtr node, xmlNodePtr top, 
	const xmlChar * name, const xmlChar * attr, const xmlChar * value, 
	int descend );

#endif /* _XML_HELPER_H_ */

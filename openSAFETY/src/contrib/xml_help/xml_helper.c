/**
 * \file xml_helper.c
 *
 * Copyright (c) 2011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 * \}
 * \author Matthias Hartl, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \version 1.0
 */
#include <xml_help/xml_helper.h>
#include <string.h>

xmlNodePtr xmlFindElement (xmlNodePtr node, xmlNodePtr top, //Nodes
    const xmlChar * name, const xmlChar * attr, const xmlChar * value, //Search parameter
    int descend ) // Mode
{
    xmlNodePtr result = NULL;
    xmlChar *temp = NULL; /* Current attribute value */

   /* check input */
    if ( node && top && (!(!attr && value)) )
    {
        /* Start with the next node */
        node = xmlWalkNext(node, top, descend);

        /* Loop until a matching element is found */
        while (node != NULL && result == NULL)
        {
           /* If this node matches */
            if (node->type == XML_ELEMENT_NODE &&
                node->name && ( !name || !xmlStrcmp(node->name, name) ) )
            {
                /* If there is an attribute to check */
                if (!attr)
                    result = node;
                else
                {
                    /* Check for an attribute */
                    if ((temp = xmlGetProp(node, attr)) != NULL)
                    {
                        /* Check the attribute */
                        if (!value || !xmlStrcmp(value, temp))
                        {
                            result = node;
                        }

                        /* Free the copied value */
                        xmlFree(temp);
                    }
                }
            }
            if(result != NULL)
                break;
            /* No match, move on to the next node */
            if (descend == XML_DESCEND )
              node = xmlWalkNext(node, top, XML_DESCEND);
            else
              node = node->next;
        }
    }
    return (result);
}


xmlNodePtr xmlWalkNext(xmlNodePtr node, xmlNodePtr top, int descend)
{
    xmlNodePtr result = NULL;

    /* Input check*/
    if( node )
    {
        /* If descending*/
        if(node->children && descend)
            result = node->children;
        /* If we are not at the top*/
        else if(!(node == top))
        {
            /* When there is a next node*/
            if(node->next)
                result = node->next;
            /* when not, get up until top to get a next node*/
            else if(node->parent && node->parent != top)
            {
                node = node->parent;
                while (!node->next)
                  if (node->parent != top && node->parent)
                    node = node->parent;
                  else
                    break;

                result = node->next;
            }
        }
    }
    return(result);
}



/* double x-y-z triplet with int index */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include "node.h"

Node node_create(double x, double y, double z, int indx)
{
  Node node;
  
  node = (Node)malloc( sizeof(NodeStruct) );
  if (NULL == node) {
    printf("%s: %d: malloc failed in node_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  node->xyz[0] = x;
  node->xyz[1] = y;
  node->xyz[2] = z;
  node->indx=indx;
  
  return node;
}

KNIFE_STATUS node_initialize( Node node, double *xyz, int indx )
{  
  node->xyz[0] = xyz[0];
  node->xyz[1] = xyz[1];
  node->xyz[2] = xyz[2];
  node->indx=indx;

  return KNIFE_SUCCESS;
}

void node_free( Node node )
{
  if ( NULL == node ) return;
  free( node );
}

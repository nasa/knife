
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

  node->x = x;
  node->y = y;
  node->z = z;
  node->indx=indx;
  
  return node;
}

void node_initialize( Node node, double *xyz, int indx )
{  
  node->x = xyz[0];
  node->y = xyz[1];
  node->z = xyz[2];
  node->indx=indx;
}

void node_free( Node node )
{
  if ( NULL == node ) return;
  free( node );
}

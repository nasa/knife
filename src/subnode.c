
/* ddouble u-v-w triplet with Node parent */

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
#include "subnode.h"

Subnode subnode_create( double u, double v, double w, 
			Node node, Intersection intersection)
{
  Subnode subnode;
  
  subnode = (Subnode)malloc( sizeof(SubnodeStruct) );
  if (NULL == subnode) {
    printf("%s: %d: malloc failed in subnode_create\n",
	   __FILE__,__LINE__);
    return NULL;
  }

  subnode->uvw[0] = u;
  subnode->uvw[1] = v;
  subnode->uvw[2] = w;
  subnode->node = node;
  subnode->intersection = intersection;
  
  return subnode;
}

void subnode_free( Subnode subnode )
{
  if ( NULL == subnode ) return;
  free( subnode );
}

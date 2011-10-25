
/* ddouble u-v-w triplet with Node parent */



/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef __APPLE__       /* Not needed on Mac OS X */
#include <float.h>
#else
#include <malloc.h>
#include <values.h>
#endif

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

KNIFE_STATUS subnode_uvw( Subnode subnode, double *uvw )
{
  if ( NULL == subnode ) return KNIFE_NULL;
  
  uvw[0] = subnode->uvw[0];
  uvw[1] = subnode->uvw[1];
  uvw[2] = subnode->uvw[2];
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS subnode_xyz( Subnode subnode, double *xyz )
{
  if ( NULL == subnode ) return KNIFE_NULL;

  if (NULL != subnode->node)
    {
      xyz[0] = subnode->node->xyz[0];
      xyz[1] = subnode->node->xyz[1];
      xyz[2] = subnode->node->xyz[2];
      return KNIFE_SUCCESS;
    }

  if (NULL != subnode->intersection) 
    {
      xyz[0] = intersection_x(subnode->intersection);
      xyz[1] = intersection_y(subnode->intersection);
      xyz[2] = intersection_z(subnode->intersection);
      return KNIFE_SUCCESS;
    }

  return KNIFE_NOT_FOUND;
}

double subnode_x( Subnode subnode )
{
  if (NULL != subnode->node) return subnode->node->xyz[0];
  if (NULL != subnode->intersection) 
    return intersection_x(subnode->intersection);
  return DBL_MAX;
}


double subnode_y( Subnode subnode )
{
  if (NULL != subnode->node) return subnode->node->xyz[1];
  if (NULL != subnode->intersection) 
    return intersection_y(subnode->intersection);
  return DBL_MAX;
}

double subnode_z( Subnode subnode )
{
  if (NULL != subnode->node) return subnode->node->xyz[2];
  if (NULL != subnode->intersection) 
    return intersection_z(subnode->intersection);
  return DBL_MAX;
}

double subnode_area( Subnode n0, Subnode n1, Subnode n2 )
{
  double a,b,c,d;
  a = n0->uvw[1]-n2->uvw[1];
  b = n0->uvw[2]-n2->uvw[2];
  c = n1->uvw[1]-n2->uvw[1];
  d = n1->uvw[2]-n2->uvw[2];
  return (0.5*(a*d-b*c));
}


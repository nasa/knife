
/* three subnodes and three sides */

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
#include "subtri.h"

Subtri subtri_create( Subnode n0, Subnode n1, Subnode n2 )
{
  Subtri subtri;
  
  subtri = (Subtri)malloc( sizeof(SubtriStruct) );
  if (NULL == subtri) {
    printf("%s: %d: malloc failed in subtri_create\n",
	   __FILE__,__LINE__);
    return NULL;
  }

  subtri->n0 = n0;
  subtri->n1 = n1;
  subtri->n2 = n2;

  return subtri;
}

Subtri subtri_shallow_copy( Subtri existing )
{
  Subtri subtri;
  
  subtri = (Subtri)malloc( sizeof(SubtriStruct) );
  if (NULL == subtri) {
    printf("%s: %d: malloc failed in subtri_shallow_copy\n",
	   __FILE__,__LINE__);
    return NULL;
  }

  subtri->n0 = existing->n0;
  subtri->n1 = existing->n1;
  subtri->n2 = existing->n2;

  return subtri;
}

void subtri_free( Subtri subtri )
{
  if ( NULL == subtri ) return;
  free( subtri );
}

KNIFE_STATUS subtri_replace_node( Subtri subtri, 
				  Subnode old_node, Subnode new_node )
{
  if (NULL == subtri) return KNIFE_NULL;

  if ( old_node == subtri->n0 )
    {
      subtri->n0 = new_node;
      return KNIFE_SUCCESS;
    }
  if ( old_node == subtri->n1 )
    {
      subtri->n1 = new_node;
      return KNIFE_SUCCESS;
    }
  if ( old_node == subtri->n2 )
    {
      subtri->n2 = new_node;
      return KNIFE_SUCCESS;
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS subtri_orient( Subtri subtri, Subnode subnode, 
			    Subnode *n0, Subnode *n1, Subnode *n2 )
{
  if (NULL == subtri) return KNIFE_NULL;

  if ( subnode == subtri->n0 )
    {
      *n0 = subtri->n0;
      *n1 = subtri->n1;
      *n2 = subtri->n2;
      return KNIFE_SUCCESS;
    }
  if ( subnode == subtri->n1 )
    {
      *n0 = subtri->n1;
      *n1 = subtri->n2;
      *n2 = subtri->n0;
      return KNIFE_SUCCESS;
    }
  if ( subnode == subtri->n2 )
    {
      *n0 = subtri->n2;
      *n1 = subtri->n0;
      *n2 = subtri->n1;
      return KNIFE_SUCCESS;
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS subtri_bary( Subtri subtri, Subnode node, double *bary )
{
  double area0,area1,area2,total;

  if (NULL == subtri) return KNIFE_NULL;

  area0 = subnode_area(node,       subtri->n1, subtri->n2);
  area1 = subnode_area(subtri->n0, node,       subtri->n2);
  area2 = subnode_area(subtri->n0, subtri->n1, node);
  total = area0+area1+area2;
  bary[0] = area0/total;
  bary[1] = area1/total;
  bary[2] = area2/total;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS subtri_area( Subtri subtri )
{
  return subnode_area(subtri_n0(subtri), subtri_n1(subtri), subtri_n2(subtri));
}

KNIFE_STATUS subtri_dump_geom( Subtri subtri, FILE *f )
{
  if (NULL == subtri) return KNIFE_NULL;
  fprintf(f,"%30.20e%30.20e%30.20e\n",
	  subnode_x(subtri_n0(subtri)),
	  subnode_y(subtri_n0(subtri)),
	  subnode_z(subtri_n0(subtri)));
  fprintf(f,"%30.20e%30.20e%30.20e\n",
	  subnode_x(subtri_n1(subtri)),
	  subnode_y(subtri_n1(subtri)),
	  subnode_z(subtri_n1(subtri)));
  fprintf(f,"%30.20e%30.20e%30.20e\n",
	  subnode_x(subtri_n2(subtri)),
	  subnode_y(subtri_n2(subtri)),
	  subnode_z(subtri_n2(subtri)));
  return KNIFE_SUCCESS;
}


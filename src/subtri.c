
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

void subtri_free( Subtri subtri )
{
  if ( NULL == subtri ) return;
  free( subtri );
}

KNIFE_STATUS subtri_bary( Subtri subtri, Subnode node, double *bary )
{
  double area0,area1,area2,total;

  area0 = subnode_area(node,       subtri->n1, subtri->n2);
  area1 = subnode_area(subtri->n0, node,       subtri->n2);
  area2 = subnode_area(subtri->n0, subtri->n1, node);
  total = area0+area1+area2;
  bary[0] = area0/total;
  bary[1] = area1/total;
  bary[2] = area2/total;

  return KNIFE_SUCCESS;
}


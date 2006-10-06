
/* volume defined by a watertight collection of triangles */

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
#include "poly.h"

Poly poly_create( void )
{
  Poly poly;
  
  poly = (Poly) malloc( sizeof(PolyStruct) );
  if (NULL == poly) {
    printf("%s: %d: malloc failed in poly_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  poly->triangles = array_create(4,40);

  return poly;
}

void poly_free( Poly poly )
{
  if ( NULL == poly ) return;
  array_free( poly->triangles );
  free( poly );
}

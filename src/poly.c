
/* volume defined by a watertight collection of triangles */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

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

  poly_initialize( poly );

  return poly;
}

KNIFE_STATUS poly_initialize( Poly poly )
{

  poly->triangle = array_create(4,40);

  return KNIFE_SUCCESS;
}

void poly_free( Poly poly )
{
  if ( NULL == poly ) return;
  array_free( poly->triangle );
  free( poly );
}

KNIFE_STATUS poly_tecplot_zone( Poly poly, FILE *f )
{
  Triangle triangle;
  int triangle_index;

  fprintf(f, "zone t='poly', i=%d, j=%d, f=fepoint, et=triangle\n",
	  3*poly_ntriangle(poly), poly_ntriangle(poly) );

  for ( triangle_index = 0;
	triangle_index < poly_ntriangle(poly); 
	triangle_index++)
    {
      triangle = poly_triangle(poly, triangle_index);
      triangle_dump_geom(triangle,f);
    } 
  for ( triangle_index = 0;
	triangle_index < poly_ntriangle(poly); 
	triangle_index++)
  fprintf(f, "%6d %6d %6d\n",
	  1+3*triangle_index,
	  2+3*triangle_index,
	  3+3*triangle_index );

 return KNIFE_SUCCESS;
}


/* volume defined by a watertight collection of masks */

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

  poly->mask = array_create(4,40);

  return KNIFE_SUCCESS;
}

void poly_free( Poly poly )
{
  if ( NULL == poly ) return;
  array_free( poly->mask );
  free( poly );
}

KNIFE_STATUS poly_add_triangle( Poly poly, Triangle triangle, 
				KnifeBool inward_pointing_normal )
{
  return poly_add_mask( poly, mask_create( triangle, inward_pointing_normal ) );
}


KNIFE_STATUS poly_tecplot_zone( Poly poly, FILE *f )
{
  Mask mask;
  int mask_index;
  int nsubtri;
  int subtri_index;

  nsubtri = 0;
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly, mask_index);
      nsubtri += triangle_nsubtri(mask_triangle(mask) );
    }

  fprintf(f, "zone t=poly, i=%d, j=%d, f=fepoint, et=triangle\n",
	  3*nsubtri, nsubtri );

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly, mask_index);
      triangle_dump_geom(mask_triangle(mask), f );
    }

  for ( subtri_index = 0;
	subtri_index < nsubtri; 
	subtri_index++)
    fprintf(f, "%6d %6d %6d\n",
	    1+3*subtri_index,
	    2+3*subtri_index,
	    3+3*subtri_index );

 return KNIFE_SUCCESS;
}


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
#include "cut.h"

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      triangle_eps(triangle);				      \
      return code;					      \
    }							      \
  }

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
  poly->surf = array_create(4,40);

  return KNIFE_SUCCESS;
}

void poly_free( Poly poly )
{
  if ( NULL == poly ) return;
  array_free( poly->mask );
  array_free( poly->surf );
  free( poly );
}

KNIFE_STATUS poly_add_triangle( Poly poly, Triangle triangle, 
				KnifeBool inward_pointing_normal )
{
  return poly_add_mask( poly, mask_create( triangle, inward_pointing_normal ) );
}

KnifeBool poly_has_surf_triangle( Poly poly, Triangle triangle )
{
  int surf_index;
  for ( surf_index = 0;
	surf_index < poly_nsurf(poly); 
	surf_index++)
    {
      if ( triangle == mask_triangle( poly_surf(poly, surf_index) ) )
	   return TRUE;
    }
  
  return FALSE;
}


KNIFE_STATUS poly_gather_surf( Poly poly )
{
  int mask_index;
  Triangle triangle, other;
  int cut_index;
  Cut cut;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      triangle = mask_triangle(poly_mask(poly, mask_index));
      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++)
	{
	  cut = triangle_cut(triangle,cut_index);
	  other = cut_other_triangle(cut,triangle);
	  if ( !poly_has_surf_triangle(poly,other) )
	    poly_add_surf(poly,mask_create( other, TRUE ));
	}
    }
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_determine_active_subtri( Poly poly )
{
  int mask_index;
  int cut_index;
  Triangle triangle, cutter;
  Cut cut;
  Subtri cutter_subtri01, cutter_subtri10;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    mask_deactivate_all_subtri( poly_mask(poly, mask_index) );

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    mask_deactivate_all_subtri( poly_surf(poly, mask_index) );
   
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      triangle = mask_triangle(poly_mask(poly, mask_index));
      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++)
	{
	  cut = triangle_cut(triangle,cut_index);
	  cutter = cut_other_triangle(cut,triangle);
	  
	  TRY( triangle_subtri_with_intersections( cutter, 
						   cut_intersection0(cut), 
						   cut_intersection1(cut),
						   &cutter_subtri01 ), 
	       "cutter_subtri01");
	  TRY( triangle_subtri_with_intersections( cutter, 
						   cut_intersection1(cut), 
						   cut_intersection0(cut),
						   &cutter_subtri10 ), 
	       "cutter_subtri10");

	}
    }


  return KNIFE_SUCCESS;
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
      nsubtri += mask_nsubtri( mask );
    }

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    {
      mask = poly_surf(poly, mask_index);
      nsubtri += mask_nsubtri( mask );
    }

  if (0==nsubtri) return KNIFE_SUCCESS;

  fprintf(f, "zone t=poly, i=%d, j=%d, f=fepoint, et=triangle\n",
	  3*nsubtri, nsubtri );

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly, mask_index);
      mask_dump_geom(mask, f );
    }

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    {
      mask = poly_surf(poly, mask_index);
      mask_dump_geom(mask, f );
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


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

  poly->primal_node = NULL;
  poly->topo = POLY_INTERIOR;
  poly->mask = array_create(4,40);
  poly->surf = array_create(4,40);

  return KNIFE_SUCCESS;
}

void poly_free( Poly poly )
{
  if ( NULL == poly ) return;
  node_free( poly->primal_node );
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

KNIFE_STATUS poly_mask_with_triangle( Poly poly, Triangle triangle, Mask *mask )
{
  int mask_index;

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    {
      if ( triangle == mask_triangle( poly_surf(poly, mask_index) ) )
	{
	  *mask = poly_surf(poly, mask_index);
	  return KNIFE_SUCCESS;
	}
    }
  
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      if ( triangle == mask_triangle( poly_mask(poly, mask_index) ) )
	{
	  *mask = poly_mask(poly, mask_index);
	  return KNIFE_SUCCESS;
	}
    }
  
  return KNIFE_NOT_FOUND;
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

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    TRY(mask_deactivate_all_subtri( poly_mask(poly, mask_index) ),"deact mask");

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    TRY(mask_deactivate_all_subtri( poly_surf(poly, mask_index) ),"deact surf");

   TRY( poly_activate_subtri_at_cuts( poly ), "activate at cuts");
   TRY( poly_paint( poly ), "paint");

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_activate_subtri_at_cuts( Poly poly )
{
  int mask_index;
  int cut_index;
  Triangle triangle, cutter;
  Cut cut;
  Subtri cutter_subtri01, cutter_subtri10;
  Subtri triang_subtri01, triang_subtri10;
  Mask mask, surf;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly, mask_index);
      triangle = mask_triangle(mask);
      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++)
	{
	  cut = triangle_cut(triangle,cut_index);
	  cutter = cut_other_triangle(cut,triangle);
	  TRY( poly_mask_with_triangle(poly, cutter, &surf), "cutter mask" );

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

	  TRY( triangle_subtri_with_intersections( triangle, 
						   cut_intersection0(cut), 
						   cut_intersection1(cut),
						   &triang_subtri01 ), 
	       "triang_subtri01");
	  TRY( triangle_subtri_with_intersections( triangle, 
						   cut_intersection1(cut), 
						   cut_intersection0(cut),
						   &triang_subtri10 ), 
	       "triang_subtri10");

	  if (subtri_above(triang_subtri01,cutter_subtri01))
	    TRY( mask_activate_subtri(mask, triang_subtri01), "active t01");

	  if (subtri_above(triang_subtri10,cutter_subtri01))
	    TRY( mask_activate_subtri(mask, triang_subtri10), "active t10");

	  if ( mask->inward_pointing_normal )
	    {
	      if (  subtri_above(cutter_subtri01,triang_subtri01))
		TRY( mask_activate_subtri(surf, cutter_subtri01), "active c01");

	      if (  subtri_above(cutter_subtri10,triang_subtri01))
		TRY( mask_activate_subtri(surf, cutter_subtri10), "active c10");
	    }
	  else
	    {
	      if ( !subtri_above(cutter_subtri01,triang_subtri01))
		TRY( mask_activate_subtri(surf, cutter_subtri01), "active!c01");

	      if ( !subtri_above(cutter_subtri10,triang_subtri01))
		TRY( mask_activate_subtri(surf, cutter_subtri10), "active!c10");
	    }

	}
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_paint( Poly poly )
{
  int mask_index;
  Mask mask;
  Triangle triangle;
  KnifeBool another_coat_of_paint;
  int segment_index;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    TRY( mask_paint( poly_mask(poly, mask_index) ), "mask paint");

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    TRY( mask_paint( poly_surf(poly, mask_index) ), "surf paint");

  another_coat_of_paint = TRUE;
  while (another_coat_of_paint)
    {
      another_coat_of_paint = FALSE;
      for ( mask_index = 0;
	    mask_index < poly_nmask(poly); 
	    mask_index++)
	{
	  mask = poly_mask(poly,mask_index);
	  triangle = mask_triangle(mask);
	  if ( (1 == triangle_nsubtri(triangle)) && 
	       !mask_subtri_active(mask,0) )
	    {
	      if ( poly_active_mask_with_nodes( poly, 
						triangle->node0,
						triangle->node1,
						triangle->node2)  )
		{
		  mask_activate_subtri_index( mask, 0 );
		  another_coat_of_paint = TRUE;
		}	       
	    }
	}
    }
  
  another_coat_of_paint = TRUE;
  while (another_coat_of_paint)
    {
      another_coat_of_paint = FALSE;
      for ( mask_index = 0;
	    mask_index < poly_nsurf(poly); 
	    mask_index++)
	{
	  mask = poly_surf(poly,mask_index);
	  triangle = mask_triangle(mask);
	  for ( segment_index = 0; segment_index < 3; segment_index++ )
	    TRY ( poly_paint_surf( poly, mask,
				   triangle->segment[segment_index],
				   &another_coat_of_paint ), "paint surf ");
	}
    }
  
  return KNIFE_SUCCESS;
}

KnifeBool poly_active_mask_with_nodes( Poly poly, 
				       Node n0, Node n1, Node n2  )
{
  int mask_index, subtri_index;
  Mask mask;
  Triangle triangle;
  
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( triangle_has2(triangle,n0,n1) )
	{
	  if ( KNIFE_SUCCESS != 
	       triangle_subtri_index_with_nodes( triangle,n0,n1,
						 &subtri_index ) )
	    {
	      printf("%s: %d: REALLY WRONG!! cannot find subtri\n",
		     __FILE__,__LINE__);
	      return FALSE;
	    }
	  if ( mask_subtri_active(mask,subtri_index ) )
	    return TRUE;
	}
      if ( triangle_has2(triangle,n1,n2) )
	{
	  if ( KNIFE_SUCCESS != 
	       triangle_subtri_index_with_nodes( triangle,n1,n2,
						 &subtri_index ) )
	    {
	      printf("%s: %d: REALLY WRONG!! cannot find subtri\n",
		     __FILE__,__LINE__);
	      return FALSE;
	    }
	  if ( mask_subtri_active(mask,subtri_index ) )
	    return TRUE;
	}
      if ( triangle_has2(triangle,n2,n0) )
	{
	  if ( KNIFE_SUCCESS != 
	       triangle_subtri_index_with_nodes( triangle,n2,n0,
						 &subtri_index ) )
	    {
	      printf("%s: %d: REALLY WRONG!! cannot find subtri\n",
		     __FILE__,__LINE__);
	      return FALSE;
	    }
	  if ( mask_subtri_active(mask,subtri_index ) )
	    return TRUE;
	}
    }

  return FALSE;
}

KNIFE_STATUS poly_mask_surrounding_node_activity( Poly poly, Node node,
						  KnifeBool *active )
{
  int mask_index;
  Mask mask;
  Triangle triangle;
  KnifeBool found;
  KnifeBool found_active;

  found = FALSE;
  found_active = FALSE;
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( triangle_has1(triangle,node) &&
	   !triangle_on_boundary(triangle) )
	{
	  found = TRUE;
	  if ( 1 != triangle_nsubtri(triangle) ) return KNIFE_FAILURE;
	  if ( mask_subtri_active(mask,0) ) found_active = TRUE;
	}
    }

  if (found) *active = found_active;

  return (found?KNIFE_SUCCESS:KNIFE_NOT_FOUND);
}

KNIFE_STATUS poly_centroid_volume( Poly poly, double *origin, 
				   double *centroid, double *volume )
{
  int mask_index;

  if (NULL == poly) return KNIFE_NULL;

  centroid[0] = 0.0;
  centroid[1] = 0.0;
  centroid[2] = 0.0;
  *volume   = 0.0;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    TRY(mask_centroid_volume_contribution( poly_mask(poly, mask_index),
					   origin, centroid, volume),
	"cent vol mask");

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    TRY(mask_centroid_volume_contribution( poly_surf(poly, mask_index),
					   origin, centroid, volume),
	"cent vol surf");

  if ( ABS(*volume) < 1.0e-14 )
    return KNIFE_DIV_ZERO;

  centroid[0] /= (*volume) + origin[0];
  centroid[1] /= (*volume) + origin[1];
  centroid[2] /= (*volume) + origin[2];

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_tecplot_zone( Poly poly, FILE *f )
{
  Mask mask;
  int mask_index;
  int nsubtri;
  int subtri_index;

  if ( POLY_EXTERIOR == poly_topo(poly) ) return KNIFE_SUCCESS;

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

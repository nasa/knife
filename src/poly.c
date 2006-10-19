
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

static int poly_tecplot_frame = 0;


#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
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

  if ( KNIFE_SUCCESS != poly_initialize( poly ))
    {
      poly_free( poly );
      return NULL;
    };

  return poly;
}

KNIFE_STATUS poly_initialize( Poly poly )
{

  poly->primal_node = NULL;
  poly->topo = POLY_INTERIOR;

  poly->mask = array_create(4,40);
  NOT_NULL( poly->mask, "poly mask array null" );

  poly->surf = array_create(4,40);
  NOT_NULL( poly->surf, "poly surf array null" );

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
	    TRY( poly_add_surf(poly,mask_create( other, TRUE ) ), "add surf");
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
  double volume01, volume10;

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

	  TRY( subtri_contained_volume6( cutter_subtri01, triang_subtri01, 
					 &volume01), "vol");

	  TRY( subtri_contained_volume6( cutter_subtri01, triang_subtri10, 
					 &volume10), "vol");

	  if ( ( volume01 > 0.0 && volume10 > 0.0 ) ||
	       ( volume01 < 0.0 && volume10 < 0.0 ) ||
	       volume01 == 0.0 || volume01 == 0.0 )
	    {
	      printf("%s: %d: inside inconsistent %.16e %.16e\n",
		     __FILE__,__LINE__,volume01, volume10);
	      return KNIFE_INCONSISTENT;
	    }


	  if ( volume01 > volume10 && surf->inward_pointing_normal )
	    {
	      TRY( mask_activate_subtri(mask, triang_subtri01), "active m01");
	    }
	  else
	    {
	      TRY( mask_activate_subtri(mask, triang_subtri10), "active m10");
	    }



	  TRY( subtri_contained_volume6( triang_subtri01, cutter_subtri01, 
					 &volume01), "vol");

	  TRY( subtri_contained_volume6( triang_subtri01, cutter_subtri10, 
					 &volume10), "vol");

	  if ( ( volume01 > 0.0 && volume10 > 0.0 ) ||
	       ( volume01 < 0.0 && volume10 < 0.0 ) ||
	       volume01 == 0.0 || volume01 == 0.0 )
	    {
	      printf("%s: %d: inside inconsistent %.16e %.16e\n",
		     __FILE__,__LINE__,volume01, volume10);
	      return KNIFE_INCONSISTENT;
	    }

	  if ( volume01 > volume10 && mask->inward_pointing_normal )
	    {
	      TRY( mask_activate_subtri(surf, cutter_subtri01), "active s01");
	    }
	  else
	    {
	      TRY( mask_activate_subtri(surf, cutter_subtri10), "active s10");
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

  KNIFE_STATUS verify_code;

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
  
  verify_code = poly_verify_painting( poly );

  if ( verify_code) poly_tecplot( poly );

  return verify_code;
}

KNIFE_STATUS poly_verify_painting( Poly poly )
{
  int mask_index;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    TRY( mask_verify_paint( poly_mask(poly, mask_index) ), "mask verify");

  for ( mask_index = 0;
	mask_index < poly_nsurf(poly); 
	mask_index++)
    TRY( mask_verify_paint( poly_surf(poly, mask_index) ), "surf verify");

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

KNIFE_STATUS poly_paint_surf( Poly poly, Mask surf, 
			      Segment segment, KnifeBool *another_pass )
{
  KNIFE_STATUS status;
  Triangle triangle, other_triangle;
  int subtri_index;

  triangle = mask_triangle(surf);

  status = triangle_subtri_index_with_nodes( triangle,
					     segment_node0(segment),
					     segment_node1(segment),
					     &subtri_index );

  if ( KNIFE_NOT_FOUND == status ) return KNIFE_SUCCESS;

  TRY( status, "find subtri with nodes" );
  
  if ( mask_subtri_active(surf,subtri_index) )
    {
      TRY( triangle_neighbor( triangle, segment, &other_triangle), 
	   "other tri" );
      TRY( poly_gather_active_surf( poly, other_triangle, 
				    another_pass ), "gather act surf");
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_gather_active_surf( Poly poly, Triangle triangle,
				      KnifeBool *true_if_added )
{
  Mask surf;

  if ( poly_has_surf_triangle( poly, triangle ) ) return KNIFE_SUCCESS;

  surf = mask_create( triangle, TRUE );

  TRY( poly_add_surf(poly,surf), "add surf" );

  (*true_if_added) = TRUE;

  return KNIFE_SUCCESS;
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
  (*volume)   = 0.0;

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

  centroid[0] = centroid[0] / (*volume) + origin[0];
  centroid[1] = centroid[1] / (*volume) + origin[1];
  centroid[2] = centroid[2] / (*volume) + origin[2];

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_directed_area_about( Poly poly, Node node,
				       double *directed_area )
{
  int mask_index;
  Mask mask;
  Triangle triangle;

  directed_area[0] = 0.0;
  directed_area[1] = 0.0;
  directed_area[2] = 0.0;

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( triangle_has1(triangle,node) &&
	   !triangle_on_boundary(triangle) )
	{
	  TRY( mask_directed_area_contribution( mask, directed_area),
	       "mask_directed_area_contribution failed" );
	}
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_face_geometry_about( Poly poly, Node node, FILE *f )
{
  int mask_index;
  Mask mask;
  Triangle triangle;
  int nsubtri;

  nsubtri = 0;
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( triangle_has1(triangle,node) &&
	   !triangle_on_boundary(triangle) )
	{
	  nsubtri += mask_nsubtri( mask );
	}
    }

  fprintf(f,"%d\n",nsubtri);

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( triangle_has1(triangle,node) &&
	   !triangle_on_boundary(triangle) )
	{
	  TRY( mask_dump_geom( mask, f),
	       "mask_dump_geom" );
	}
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_boundary_face_geometry( Poly poly, int face_index, FILE *f )
{
  int mask_index;
  Mask mask;
  Triangle triangle;
  int nsubtri;

  nsubtri = 0;
  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( face_index == triangle_boundary_face_index(triangle) )
	{
	  nsubtri += mask_nsubtri( mask );
	}
    }

  fprintf(f,"%d\n",nsubtri);

  for ( mask_index = 0;
	mask_index < poly_nmask(poly); 
	mask_index++)
    {
      mask = poly_mask(poly,mask_index);
      triangle = mask_triangle(mask);
      if ( face_index == triangle_boundary_face_index(triangle) )
	{
	  TRY( mask_dump_geom( mask, f),
	       "mask_dump_geom" );
	}
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_surf_geometry( Poly poly, FILE *f )
{
  int surf_index;
  int nsubtri;

  nsubtri = 0;
  for ( surf_index = 0;
	surf_index < poly_nsurf(poly); 
	surf_index++)
    nsubtri += mask_nsubtri( poly_surf(poly,surf_index) );

  fprintf(f,"%d\n",nsubtri);

  for ( surf_index = 0;
	surf_index < poly_nsurf(poly); 
	surf_index++)
    {
      TRY( mask_dump_geom( poly_surf(poly,surf_index), f), "mask_dump_geom" );
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS poly_tecplot( Poly poly )
{
  char filename[1025];
  FILE *f;
  sprintf(filename, "poly%04d.t",poly_tecplot_frame );
  printf("producing %s\n",filename);

  poly_tecplot_frame++;

  f = fopen(filename, "w");
  
  fprintf(f,"title=poly_geometry\nvariables=x,y,z\n");

  poly_tecplot_zone( poly, f );

  fclose(f);

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

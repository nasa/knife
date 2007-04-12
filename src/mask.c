
/* a triangle wrapper that extends to include orientation and active subtri */

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
#include "mask.h"

static int mask_tecplot_frame = 0;

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      triangle_eps(mask_triangle(mask));		      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == pointer){					      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

Mask mask_create( Triangle traingle, KnifeBool inward_pointing_normal )
{
  Mask mask;
  
  mask = (Mask)malloc( sizeof(MaskStruct) );
  if (NULL == mask) {
    printf("%s: %d: malloc failed in mask_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  mask->triangle = traingle;
  mask->inward_pointing_normal = inward_pointing_normal;
  mask->region   = NULL;

  return mask;
}

void mask_free( Mask mask )
{
  if ( NULL == mask ) return;
  if ( NULL != mask->region ) free( mask->region );
  free( mask );
}

int mask_nsubtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  if ( NULL == mask->region ) return triangle_nsubtri(mask_triangle(mask));

  nsubtri = 0;
  for (subtri_index = 0;
       subtri_index < triangle_nsubtri(mask_triangle(mask));
       subtri_index++)
    if ( mask_subtri_active(mask,subtri_index) ) nsubtri++;

  return nsubtri;
}

KNIFE_STATUS mask_activate_all_subtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  TRY( mask_deactivate_all_subtri( mask ), 
       "mask_deactivate_all_subtri in mask_activate_all_subtri" );

  nsubtri = triangle_nsubtri( mask_triangle(mask) );

  for (subtri_index = 0; subtri_index < nsubtri; subtri_index++)
    TRY( mask_activate_subtri_index( mask, subtri_index, 1 ), "activate_all" ); 

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_deactivate_all_subtri( Mask mask )
{
  int nsubtri;
  int subtri_index;

  nsubtri = triangle_nsubtri( mask_triangle(mask) );

  mask->region = (int *) malloc( nsubtri * sizeof(int) );
  if (NULL == mask->region) {
    printf("%s: %d: malloc mask->region failed in mask_deactivate_all_subtri\n",
	   __FILE__,__LINE__);
    return KNIFE_MEMORY; 
  }
  
  for (subtri_index = 0; subtri_index < nsubtri; subtri_index++)
    mask->region[subtri_index] = 0;
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_activate_subtri( Mask mask, Subtri subtri, int region )
{
  int subtri_index;

  if ( NULL == mask ) return KNIFE_NULL;

  TRY( triangle_subtri_index( mask_triangle(mask), subtri, &subtri_index ),
       "no index for subtri" );

  return mask_activate_subtri_index( mask, subtri_index, region ); 
}

KNIFE_STATUS mask_activate_subtri_index( Mask mask, 
					 int subtri_index, int region )
{

  if ( NULL == mask ) return KNIFE_NULL;
  if ( NULL == mask->region ) return KNIFE_ARRAY_BOUND;

  mask->region[subtri_index] = region;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_intersection_region( Mask mask,
				       Intersection intersection0, 
				       Intersection intersection1,
				       int *region )
{
  Triangle triangle;
  int subtri_index;
  triangle = mask_triangle(mask);

  *region = 0;

  TRY ( triangle_subtri_index_with_intersections( triangle,
						  intersection0, 
						  intersection1,
						  &subtri_index ), "st01");
  *region = MAX( *region, mask_subtri_region(mask,subtri_index) );

  TRY ( triangle_subtri_index_with_intersections( triangle,
						  intersection1, 
						  intersection0,
						  &subtri_index ), "st10");
  *region = MAX( *region, mask_subtri_region(mask,subtri_index) );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_set_intersection_region( Mask mask,
					   Intersection intersection0, 
					   Intersection intersection1,
					   int region )
{
  Triangle triangle;
  int subtri_index;
  triangle = mask_triangle(mask);

  TRY ( triangle_subtri_index_with_intersections( triangle,
						  intersection0, 
						  intersection1,
						  &subtri_index ), "st01");
  if ( mask_subtri_active(mask,subtri_index) )
    mask->region[subtri_index] = region;

  TRY ( triangle_subtri_index_with_intersections( triangle,
						  intersection1, 
						  intersection0,
						  &subtri_index ), "st10");
  if ( mask_subtri_active(mask,subtri_index) )
    mask->region[subtri_index] = region;

  return KNIFE_SUCCESS;
}


KNIFE_STATUS mask_paint( Mask mask )
{
  Triangle triangle;
  int subtri_index, neighbor_index;
  int common;
  Subtri subtri, neighbor;
  Subnode subnode0, subnode1;
  Cut cut;

  KnifeBool more_paint;

  triangle = mask_triangle(mask);

  more_paint = FALSE;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle,subtri_index); 
      subnode0 = subtri_n0(subtri);
      subnode1 = subtri_n1(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->region[subtri_index] != mask->region[neighbor_index] )
		{
		  more_paint = TRUE;
		  common = MAX( mask->region[subtri_index],
				mask->region[neighbor_index] );
		  mask->region[subtri_index]   = common;
		  mask->region[neighbor_index] = common;
		}
	    }
	}
      subnode0 = subtri_n1(subtri);
      subnode1 = subtri_n2(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->region[subtri_index] != mask->region[neighbor_index] )
		{
		  more_paint = TRUE;
		  common = MAX( mask->region[subtri_index],
				mask->region[neighbor_index] );
		  mask->region[subtri_index]   = common;
		  mask->region[neighbor_index] = common;
		}
	    }
	}
      subnode0 = subtri_n2(subtri);
      subnode1 = subtri_n0(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  if (KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, 
							     subnode1, subnode0,
							     &cut ) )
	    {
	      TRY( triangle_subtri_index( triangle, neighbor,
					  &neighbor_index ), "neighbor_index");
	      if ( mask->region[subtri_index] != mask->region[neighbor_index] )
		{
		  more_paint = TRUE;
		  common = MAX( mask->region[subtri_index],
				mask->region[neighbor_index] );
		  mask->region[subtri_index]   = common;
		  mask->region[neighbor_index] = common;
		}
	    }
	}
    } 
 
  if (more_paint) return mask_paint( mask );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_verify_paint( Mask mask )
{
  Triangle triangle;
  int subtri_index, neighbor_index;
  Subtri subtri, neighbor;
  Subnode subnode0, subnode1;
  Cut cut;

  KNIFE_STATUS cut_status;

  triangle = mask_triangle(mask);

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle,subtri_index); 
      subnode0 = subtri_n0(subtri);
      subnode1 = subtri_n1(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  TRY( triangle_subtri_index( triangle, neighbor,
				      &neighbor_index ), "neighbor");
	  cut_status = triangle_cut_with_subnodes( triangle, 
						   subnode1, subnode0,
						   &cut );
	    if ( KNIFE_NOT_FOUND == cut_status )
	      {
		if (mask->region[subtri_index] != mask->region[neighbor_index])
		  {
		    printf("%s: %d: inconsistent 01\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    return KNIFE_INCONSISTENT;
		  }
	      }
	    else
	      {
		TRY( cut_status, "cut stat");
		if ( mask_subtri_active(mask,subtri_index) &&
		     mask_subtri_active(mask,neighbor_index) )
		  {
		    printf("%s: %d: consistent 01\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    return KNIFE_INCONSISTENT;
		  }
		
	      }
		
	}
      subnode0 = subtri_n1(subtri);
      subnode1 = subtri_n2(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  TRY( triangle_subtri_index( triangle, neighbor,
				      &neighbor_index ), "neighbor");
	  cut_status = triangle_cut_with_subnodes( triangle, 
						   subnode1, subnode0,
						   &cut );
	    if ( KNIFE_NOT_FOUND == cut_status )
	      {
		if (mask->region[subtri_index] != mask->region[neighbor_index])
		  {
		    printf("%s: %d: inconsistent 12\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    return KNIFE_INCONSISTENT;
		  }
	      }
	    else
	      {
		TRY( cut_status, "cut stat");
		if ( mask_subtri_active(mask,subtri_index) &&
		     mask_subtri_active(mask,neighbor_index) )
		  {
		    printf("%s: %d: consistent 12\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    return KNIFE_INCONSISTENT;
		  }
		
	      }
	}
      subnode0 = subtri_n2(subtri);
      subnode1 = subtri_n0(subtri);
      if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, 
							   subnode1, subnode0,
							   &neighbor ) )
	{
	  TRY( triangle_subtri_index( triangle, neighbor,
				      &neighbor_index ), "neighbor");
	  cut_status = triangle_cut_with_subnodes( triangle, 
						   subnode1, subnode0,
						   &cut );
	    if ( KNIFE_NOT_FOUND == cut_status )
	      {
		if (mask->region[subtri_index] != mask->region[neighbor_index])
		  {
		    printf("%s: %d: inconsistent 20\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    return KNIFE_INCONSISTENT;
		  }
	      }
	    else
	      {
		TRY( cut_status, "cut stat");
		if ( mask_subtri_active(mask,subtri_index) &&
		     mask_subtri_active(mask,neighbor_index) )
		  {
		    printf("%s: %d: consistent 20\n",__FILE__,__LINE__);
		    triangle_tecplot(triangle);
		    triangle_examine_subnodes(triangle);
		    return KNIFE_INCONSISTENT;
		  }
		
	      }
	}
    } 
 
  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_dump_geom( Mask mask, FILE *f )
{
  Triangle triangle;
  int subtri_index;

  triangle = mask_triangle(mask);
  
  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    if ( mask_subtri_active(mask,subtri_index) ) 
      subtri_dump_geom( triangle_subtri(triangle, subtri_index), 
			mask_inward_pointing_normal( mask ), 
			mask_subtri_region(mask,subtri_index),
			f );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_centroid_volume_contribution( Mask mask, int region,
						double *origin,
						double *centroid, 
						double *volume )
{
  Triangle triangle;
  Subtri subtri;
  int subtri_index;

  double xyz0[3], xyz1[3], xyz2[3];
  double normal[3], triangle_area, area;
  double bary[3];
  double xyz[3];
  int i;

  int iquad;
  int nquad = 3;
  double bary0[]  = {4.0/6.0, 1.0/6.0, 1.0/6.0};
  double bary1[]  = {1.0/6.0, 4.0/6.0, 1.0/6.0};
  double bary2[]  = {1.0/6.0, 1.0/6.0, 4.0/6.0};
  double weight[] = {1.0/3.0, 1.0/3.0, 1.0/3.0};

  if ( NULL == mask ) return KNIFE_NULL;

  triangle = mask_triangle(mask);
  
  TRY( triangle_area_normal( triangle, &triangle_area, normal ), 
       "triangle area normal" );
  if ( mask_inward_pointing_normal( mask ) )
    {
      normal[0] = -normal[0];
      normal[1] = -normal[1];
      normal[2] = -normal[2];
    }

  for(i=0;i<3;i++) xyz0[i] = triangle_xyz0(triangle)[i] - origin[i];
  for(i=0;i<3;i++) xyz1[i] = triangle_xyz1(triangle)[i] - origin[i];
  for(i=0;i<3;i++) xyz2[i] = triangle_xyz2(triangle)[i] - origin[i];

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    if ( region == mask_subtri_region(mask,subtri_index) )
      {
	subtri = triangle_subtri(triangle,subtri_index);
	area = triangle_area * subtri_reference_area( subtri );
	for (iquad = 0; iquad<nquad; iquad++)
	  {
	    bary[0] = 
	      bary0[iquad]*subnode_u( subtri_n0(subtri) ) +
	      bary1[iquad]*subnode_u( subtri_n1(subtri) ) +
	      bary2[iquad]*subnode_u( subtri_n2(subtri) );
	    bary[1] = 
	      bary0[iquad]*subnode_v( subtri_n0(subtri) ) +
	      bary1[iquad]*subnode_v( subtri_n1(subtri) ) +
	      bary2[iquad]*subnode_v( subtri_n2(subtri) );
	    bary[2] = 
	      bary0[iquad]*subnode_w( subtri_n0(subtri) ) +
	      bary1[iquad]*subnode_w( subtri_n1(subtri) ) +
	      bary2[iquad]*subnode_w( subtri_n2(subtri) );

	    for(i=0;i<3;i++) xyz[i] = 
			       bary[0]*xyz0[i] + 
			       bary[1]*xyz1[i] + 
			       bary[2]*xyz2[i];

	    (*volume) += weight[iquad]*area*( xyz[0]*normal[0] + 
					      xyz[1]*normal[1] + 
					      xyz[2]*normal[2] ) / 3.0;
	    centroid[0] += weight[iquad]*area*(xyz[0]*xyz[0]*normal[0]) / 2.0;
	    centroid[1] += weight[iquad]*area*(xyz[1]*xyz[1]*normal[1]) / 2.0;
	    centroid[2] += weight[iquad]*area*(xyz[2]*xyz[2]*normal[2]) / 2.0;

	  }
      }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS mask_tecplot( Mask mask)
{
  Triangle triangle;
  int subtri_index;
  Subtri subtri;
  Subnode subnode;
  double uvw[3], xyz[3];

  char filename[1025];
  FILE *f;

  triangle = mask_triangle(mask);

  mask_tecplot_frame++;

  sprintf(filename, "mask%08d.t",mask_tecplot_frame );
  printf("producing %s\n",filename);

  f = fopen(filename, "w");

  fprintf(f,"title=mask_geometry\nvariables=v,w,x,y,z,a\n");
  fprintf(f, "zone t=mask, i=%d, j=%d, f=fepoint, et=triangle\n",
	  3*triangle_nsubtri(triangle), triangle_nsubtri(triangle) );

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      subnode = subtri_n0(subtri); NOT_NULL( subnode, "n0 NULL" );
      TRY( subnode_uvw( subnode, uvw ), "sn uvw" );
      TRY( subnode_xyz( subnode, xyz ), "sn xyz" );
      fprintf(f, " %.16e %.16e %.16e %.16e %.16e %d\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2], 
	      mask_subtri_active(mask,subtri_index) );
      subnode = subtri_n1(subtri); NOT_NULL( subnode, "n1 NULL" );
      TRY( subnode_uvw( subnode, uvw ), "sn uvw" );
      TRY( subnode_xyz( subnode, xyz ), "sn xyz" );
      fprintf(f, " %.16e %.16e %.16e %.16e %.16e %d\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2], 
	      mask_subtri_active(mask,subtri_index) );
      subnode = subtri_n2(subtri); NOT_NULL( subnode, "n2 NULL" );
      TRY( subnode_uvw( subnode, uvw ), "sn uvw" );
      TRY( subnode_xyz( subnode, xyz ), "sn xyz" );
      fprintf(f, " %.16e %.16e %.16e %.16e %.16e %d\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2], 
	      mask_subtri_active(mask,subtri_index) );
    }

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      fprintf(f, "%6d %6d %6d\n", 
	      1+3*subtri_index, 2+3*subtri_index, 3+3*subtri_index);
    }

  fclose(f);

  return KNIFE_SUCCESS;
}

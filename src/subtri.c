
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
#include <math.h>
#include "subtri.h"

#define TRY(fcn,msg)                                          \
  {                                                           \
    int code;                                                 \
    code = (fcn);                                             \
    if (KNIFE_SUCCESS != code){                               \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;                                            \
    }                                                         \
  }

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

double subtri_reference_area( Subtri subtri )
{
  return subnode_area(subtri_n0(subtri), subtri_n1(subtri), subtri_n2(subtri));
}

KNIFE_STATUS subtri_contained_volume6( Subtri subtri, Subtri other, 
				       double *volume6 )
{
  Subnode subnode;
  double xyz0[3], xyz1[3], xyz2[3], xyz3[3];

  subnode = subtri_n0(other);

  if ( subnode_same_parent( subnode, subtri_n0(subtri) ) ||
       subnode_same_parent( subnode, subtri_n1(subtri) ) ||
       subnode_same_parent( subnode, subtri_n2(subtri) ) )
    subnode = subtri_n1(other);

  if ( subnode_same_parent( subnode, subtri_n0(subtri) ) ||
       subnode_same_parent( subnode, subtri_n1(subtri) ) ||
       subnode_same_parent( subnode, subtri_n2(subtri) ) )
    subnode = subtri_n2(other);

  subnode_xyz(subtri_n0(subtri),xyz0);
  subnode_xyz(subtri_n1(subtri),xyz1);
  subnode_xyz(subtri_n2(subtri),xyz2);
  subnode_xyz(subnode,xyz3);

  *volume6 = intersection_volume6(xyz0,xyz1,xyz2,xyz3);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS subtri_dump_geom( Subtri subtri, KnifeBool reverse, FILE *f )
{
  char format[]=" %.16e %.16e %.16e";
  char formatl[]=" %.16e %.16e %.16e\n";
  if (NULL == subtri) return KNIFE_NULL;

  if (reverse)
    {
      fprintf(f,format,
	      subnode_x(subtri_n1(subtri)),
	      subnode_y(subtri_n1(subtri)),
	      subnode_z(subtri_n1(subtri)));
      fprintf(f,format,
	      subnode_x(subtri_n0(subtri)),
	      subnode_y(subtri_n0(subtri)),
	      subnode_z(subtri_n0(subtri)));
    }
  else
    {
      fprintf(f,format,
	      subnode_x(subtri_n0(subtri)),
	      subnode_y(subtri_n0(subtri)),
	      subnode_z(subtri_n0(subtri)));
      fprintf(f,format,
	      subnode_x(subtri_n1(subtri)),
	      subnode_y(subtri_n1(subtri)),
	      subnode_z(subtri_n1(subtri)));
    }
  fprintf(f,formatl,
	  subnode_x(subtri_n2(subtri)),
	  subnode_y(subtri_n2(subtri)),
	  subnode_z(subtri_n2(subtri)));
  return KNIFE_SUCCESS;
}

KNIFE_STATUS subtri_echo_uvw( Subtri subtri )
{
  char format[]=" %.16e %.16e %.16e\n";
  if (NULL == subtri) return KNIFE_NULL;


  printf( format,
	  subnode_u(subtri_n0(subtri)),
	  subnode_v(subtri_n0(subtri)),
	  subnode_w(subtri_n0(subtri)));
  printf( format,
	  subnode_u(subtri_n1(subtri)),
	  subnode_v(subtri_n1(subtri)),
	  subnode_w(subtri_n1(subtri)));
  printf( format,
	  subnode_u(subtri_n2(subtri)),
	  subnode_v(subtri_n2(subtri)),
	  subnode_w(subtri_n2(subtri)));

  return KNIFE_SUCCESS;
}

KNIFE_STATUS subtri_normal_area( Subtri subtri, 
				 double *normal,
				 double *area)
{
  double xyz0[3], xyz1[3], xyz2[3];
  double edge1[3], edge2[3];

  TRY( subnode_xyz( subtri_n0(subtri), xyz0), "norm area subnode0 xyz" );
  TRY( subnode_xyz( subtri_n1(subtri), xyz1), "norm area subnode1 xyz" );
  TRY( subnode_xyz( subtri_n2(subtri), xyz2), "norm area subnode2 xyz" );

  edge1[0] = xyz1[0]-xyz0[0];
  edge1[1] = xyz1[1]-xyz0[1];
  edge1[2] = xyz1[2]-xyz0[2];

  edge2[0] = xyz2[0]-xyz0[0];
  edge2[1] = xyz2[1]-xyz0[1];
  edge2[2] = xyz2[2]-xyz0[2];

  normal[0] = edge1[1]*edge2[2] - edge1[2]*edge2[1];
  normal[1] = edge1[2]*edge2[0] - edge1[0]*edge2[2];
  normal[2] = edge1[0]*edge2[1] - edge1[1]*edge2[0];

  (*area) = sqrt( normal[0]*normal[0] +
		  normal[1]*normal[1] +
		  normal[2]*normal[2] );

  if ( (*area) < 1.0e-14 ) {
    /* printf("%s: %d: subtri physical area is %e\n",
       __FILE__,__LINE__,(*area)); */
    return KNIFE_DIV_ZERO;
  }

  normal[0] /= (*area);
  normal[1] /= (*area);
  normal[2] /= (*area);
  
  (*area) *= 0.5;

  return KNIFE_SUCCESS;
}


/* need 3 point quadrature rule for quadratic function (centroid) */
KNIFE_STATUS subtri_centroid_volume_contribution( Subtri subtri, 
						  double *origin,
						  double *centroid,
						  double *volume,
			       KnifeBool outward_pointing_normal)
{
  double xyz0[3], xyz1[3], xyz2[3];
  double normal[3], area;
  double xyz[3];
  int i;
  
  int iquad;
  int nquad = 3;
  double bary0[]  = {4.0/6.0, 1.0/6.0, 1.0/6.0};
  double bary1[]  = {1.0/6.0, 4.0/6.0, 1.0/6.0};
  double bary2[]  = {1.0/6.0, 1.0/6.0, 4.0/6.0};
  double weight[] = {1.0/3.0, 1.0/3.0, 1.0/3.0};

  TRY( subnode_xyz( subtri_n0(subtri), xyz0), "cent vol subnode0 xyz" );
  TRY( subnode_xyz( subtri_n1(subtri), xyz1), "cent vol subnode1 xyz" );
  TRY( subnode_xyz( subtri_n2(subtri), xyz2), "cent vol subnode2 xyz" );

  for(i=0;i<3;i++) xyz0[i] -= origin[i];
  for(i=0;i<3;i++) xyz1[i] -= origin[i];
  for(i=0;i<3;i++) xyz2[i] -= origin[i];

  if ( KNIFE_SUCCESS != subtri_normal_area( subtri, normal, &area ) )
    {
      /*
	printf("%s: %d: warning subtri area %e (%e) omitted\n",
	__FILE__,__LINE__,area,subtri_reference_area( subtri ));
      */
      return KNIFE_SUCCESS;
    }

  for (iquad = 0; iquad<nquad; iquad++)
    {
      for(i=0;i<3;i++) xyz[i] = 
	bary0[iquad]*xyz0[i] + 
	bary1[iquad]*xyz1[i] + 
	bary2[iquad]*xyz2[i];

      if (outward_pointing_normal)
	{
	  (*volume) += weight[iquad]*area*( xyz[0]*normal[0] + 
					    xyz[1]*normal[1] + 
					    xyz[2]*normal[2] ) / 3.0;
	  centroid[0] += weight[iquad]*area*( xyz[0]*xyz[0]*normal[0] ) / 2.0;
	  centroid[1] += weight[iquad]*area*( xyz[1]*xyz[1]*normal[1] ) / 2.0;
	  centroid[2] += weight[iquad]*area*( xyz[2]*xyz[2]*normal[2] ) / 2.0;
	}
      else
	{
	  (*volume) -= weight[iquad]*area*( xyz[0]*normal[0] + 
					    xyz[1]*normal[1] + 
					    xyz[2]*normal[2] ) / 3.0;
	  centroid[0] -= weight[iquad]*area*( xyz[0]*xyz[0]*normal[0] ) / 2.0;
	  centroid[1] -= weight[iquad]*area*( xyz[1]*xyz[1]*normal[1] ) / 2.0;
	  centroid[2] -= weight[iquad]*area*( xyz[2]*xyz[2]*normal[2] ) / 2.0;
	}
    }

  return KNIFE_SUCCESS;
}


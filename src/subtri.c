
/* three subnodes and three sides */

/* Copyright 2007 United States Government as represented by the
 * Administrator of the National Aeronautics and Space
 * Administration. No copyright is claimed in the United States under
 * Title 17, U.S. Code.  All Other Rights Reserved.
 *
 * The knife platform is licensed under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
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

Subnode subtri_subnode( Subtri subtri, int index )
{
  if ( NULL == subtri ) return NULL;

  if ( 0 == index ) return subtri_n0(subtri);
  if ( 1 == index ) return subtri_n1(subtri);
  if ( 2 == index ) return subtri_n2(subtri);

  return NULL;
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
  return ( 2.0 * subnode_area( subtri_n0(subtri), 
			       subtri_n1(subtri), 
			       subtri_n2(subtri) ) );
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

KNIFE_STATUS subtri_dump_geom( Subtri subtri, 
			       KnifeBool reverse, int region, FILE *f )
{
  char format[]=" %.16e %.16e %.16e %4d";
  char formatl[]=" %.16e %.16e %.16e %4d\n";
  if (NULL == subtri) return KNIFE_NULL;

  if (reverse)
    {
      fprintf(f,format,
	      subnode_x(subtri_n1(subtri)),
	      subnode_y(subtri_n1(subtri)),
	      subnode_z(subtri_n1(subtri)),
	      region);
      fprintf(f,format,
	      subnode_x(subtri_n0(subtri)),
	      subnode_y(subtri_n0(subtri)),
	      subnode_z(subtri_n0(subtri)),
	      region);
    }
  else
    {
      fprintf(f,format,
	      subnode_x(subtri_n0(subtri)),
	      subnode_y(subtri_n0(subtri)),
	      subnode_z(subtri_n0(subtri)),
	      region);
      fprintf(f,format,
	      subnode_x(subtri_n1(subtri)),
	      subnode_y(subtri_n1(subtri)),
	      subnode_z(subtri_n1(subtri)),
	      region);
    }
  fprintf(f,formatl,
	  subnode_x(subtri_n2(subtri)),
	  subnode_y(subtri_n2(subtri)),
	  subnode_z(subtri_n2(subtri)),
	      region);
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

KNIFE_STATUS subtri_echo( Subtri subtri )
{
  char uvw_format[]="subnode %d uvw %20.17f %20.17f %20.17f\n";
  char xyz_format[]="subnode %d xyz %.16e %.16e %.16e\n";
  if (NULL == subtri) return KNIFE_NULL;

  printf( xyz_format,0,
	  subnode_x(subtri_n0(subtri)),
	  subnode_y(subtri_n0(subtri)),
	  subnode_z(subtri_n0(subtri)));
  printf( xyz_format,1,
	  subnode_x(subtri_n1(subtri)),
	  subnode_y(subtri_n1(subtri)),
	  subnode_z(subtri_n1(subtri)));
  printf( xyz_format,3,
	  subnode_x(subtri_n2(subtri)),
	  subnode_y(subtri_n2(subtri)),
	  subnode_z(subtri_n2(subtri)));

  printf( uvw_format,0,
	  subnode_u(subtri_n0(subtri)),
	  subnode_v(subtri_n0(subtri)),
	  subnode_w(subtri_n0(subtri)));
  printf( uvw_format,1,
	  subnode_u(subtri_n1(subtri)),
	  subnode_v(subtri_n1(subtri)),
	  subnode_w(subtri_n1(subtri)));
  printf( uvw_format,3,
	  subnode_u(subtri_n2(subtri)),
	  subnode_v(subtri_n2(subtri)),
	  subnode_w(subtri_n2(subtri)));

  return KNIFE_SUCCESS;
}

KNIFE_STATUS subtri_center( Subtri subtri, double *center )
{
  center[0] = ( subnode_x(subtri_n0(subtri)) +
		subnode_x(subtri_n1(subtri)) +
		subnode_x(subtri_n2(subtri)) ) / 3.0;
  center[1] = ( subnode_y(subtri_n0(subtri)) +
		subnode_y(subtri_n1(subtri)) +
		subnode_y(subtri_n2(subtri)) ) / 3.0;
  center[2] = ( subnode_z(subtri_n0(subtri)) +
		subnode_z(subtri_n1(subtri)) +
		subnode_z(subtri_n2(subtri)) ) / 3.0;
  return KNIFE_SUCCESS;
}


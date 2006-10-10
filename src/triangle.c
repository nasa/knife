
/* defined by three segments and their three shared nodes */

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
#include "triangle.h"

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2)
{
  Triangle triangle;
  
  triangle = (Triangle)malloc( sizeof(TriangleStruct) );
  if (NULL == triangle) {
    printf("%s: %d: malloc failed in triangle_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  triangle_initialize(triangle, segment0, segment1, segment2);

  return triangle;
}

KNIFE_STATUS triangle_initialize(Triangle triangle,
				 Segment segment0, 
				 Segment segment1, 
				 Segment segment2)
{

  triangle->segment[0] = segment0;
  triangle->segment[1] = segment1;
  triangle->segment[2] = segment2;

  segment_part_of( segment0, triangle );
  segment_part_of( segment1, triangle );
  segment_part_of( segment2, triangle );
  
  triangle->node0 = segment_common_node( segment1, segment2 );
  triangle->node1 = segment_common_node( segment0, segment2 );
  triangle->node2 = segment_common_node( segment0, segment1 );

  return KNIFE_SUCCESS;
}

void triangle_free( Triangle triangle )
{
  if ( NULL == triangle ) return;
  free( triangle );
}

KNIFE_STATUS triangle_extent( Triangle triangle, double *center, double *diameter )
{
  double dx, dy, dz;
  int i;

  for(i=0;i<3;i++)
    center[i] = ( triangle->node0->xyz[i] + 
		  triangle->node1->xyz[i] + 
		  triangle->node2->xyz[i] ) / 3.0;
  dx = triangle->node0->xyz[0] - center[0];
  dy = triangle->node0->xyz[1] - center[1];
  dz = triangle->node0->xyz[2] - center[2];
  *diameter = sqrt(dx*dx+dy*dy+dz*dz);
  dx = triangle->node1->xyz[0] - center[0];
  dy = triangle->node1->xyz[1] - center[1];
  dz = triangle->node1->xyz[2] - center[2];
  *diameter = MAX(*diameter,sqrt(dx*dx+dy*dy+dz*dz));
  dx = triangle->node2->xyz[0] - center[0];
  dy = triangle->node2->xyz[1] - center[1];
  dz = triangle->node2->xyz[2] - center[2];
  *diameter = MAX(*diameter,sqrt(dx*dx+dy*dy+dz*dz));
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_triangulate_cuts( Triangle triangle )
{
      printf("%s: %d: implement triangle_triangulate_cuts\n",
	     __FILE__,__LINE__);
  triangle = NULL;
  return KNIFE_IMPLEMENT;
}

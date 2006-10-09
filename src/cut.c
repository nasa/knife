
/* cut between two Triangles of two Intersections */

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
#include "cut.h"


#define cut_gather_intersection						\
  if ( NULL != intersection ) {						\
    if ( NULL == intersection0 ) { intersection0 = intersection; }	\
    else { if ( NULL == intersection1 ) { intersection1 = intersection; } \
      else { printf("%s: %d: cut_between improper intersection\n",	\
		    __FILE__,__LINE__); return NULL; } } }

Cut cut_between( Triangle triangle0, Triangle triangle1 )
{
  Cut cut;
  Intersection intersection;
  Intersection intersection0, intersection1;
  int segment_index;

  intersection0 = NULL;
  intersection1 = NULL;

  for (segment_index = 0 ; segment_index < 3; segment_index++ )
    {
      intersection = intersection_of( triangle1, 
				      triangle_segment( triangle0,
							segment_index ) );
      cut_gather_intersection;
    }
  
  for (segment_index = 0 ; segment_index < 3; segment_index++ )
    {
      intersection = intersection_of( triangle0, 
				      triangle_segment( triangle1,
							segment_index ) );
      cut_gather_intersection;
    }

  if ( NULL != intersection0 && NULL == intersection0 )
    { 
      printf("%s: %d: cut_between improper intersection\n",	\
	     __FILE__,__LINE__); 
      return NULL; 
    }

  if ( NULL != intersection0 && NULL != intersection0 )
    {
      cut = (Cut)malloc( sizeof(CutStruct) );
      if (NULL == cut) 
	{
	  printf("%s: %d: malloc failed in cut_between\n",
		 __FILE__,__LINE__);
	  return NULL; 
	}
      
      cut->triangle0 = triangle0;
      cut->triangle1 = triangle1;

      cut->intersection0 = intersection0;
      cut->intersection1 = intersection1;
  
      return cut;
    }
  else 
    {
      return NULL;
    }
}

void cut_free( Cut cut )
{
  if ( NULL == cut ) return;
  free( cut );
}



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

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

#define cut_gather_intersection						\
  if ( NULL != intersection ) {						\
    if ( NULL == intersection0 ) { intersection0 = intersection; }	\
    else { if ( NULL == intersection1 ) { intersection1 = intersection; } \
      else { printf("%s: %d: cut_between improper intersection >2\n",	\
		    __FILE__,__LINE__); return KNIFE_IMPROPER; } } }

KNIFE_STATUS cut_establish_between( Triangle triangle0, Triangle triangle1 )
{
  Cut cut;
  Intersection intersection;
  Intersection intersection0, intersection1;
  int segment_index;

  if ( NULL == triangle0 || NULL == triangle1 ) return KNIFE_NULL;

  intersection  = NULL;
  intersection0 = NULL;
  intersection1 = NULL;

  for (segment_index = 0 ; segment_index < 3; segment_index++ )
    {
      TRY( intersection_of( triangle1, 
			    triangle_segment( triangle0, segment_index ),
			    &intersection ),
	   "triangle1 segment intersection" );
      cut_gather_intersection;
    }
  
  for (segment_index = 0 ; segment_index < 3; segment_index++ )
    {
      TRY( intersection_of( triangle0, 
			    triangle_segment( triangle1, segment_index ),
			    &intersection ),
	   "triangle0 segment intersection" );
      cut_gather_intersection;
    }

  if ( NULL != intersection0 && NULL == intersection1 )
    { 
      printf("%s: %d: cut_between improper intersection = 1\n",
	     __FILE__,__LINE__); 
      triangle_tecplot(triangle0);
      triangle_tecplot(triangle1);
      return KNIFE_IMPROPER; 
    }

  if ( NULL != intersection0 && NULL != intersection0 )
    {
      cut = (Cut)malloc( sizeof(CutStruct) );
      if (NULL == cut) 
	{
	  printf("%s: %d: malloc failed in cut_between\n",
		 __FILE__,__LINE__);
	  return KNIFE_MEMORY; 
	}
      
      cut->triangle0 = triangle0;
      cut->triangle1 = triangle1;

      TRY( triangle_add_cut( triangle0, cut ), "add cut to tri0");
      TRY( triangle_add_cut( triangle1, cut ), "add cut to tri1");

      cut->intersection0 = intersection0;
      cut->intersection1 = intersection1;
    }

  return KNIFE_SUCCESS; 
}

void cut_free( Cut cut )
{
  if ( NULL == cut ) return;
  free( cut );
}


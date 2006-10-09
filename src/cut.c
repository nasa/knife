
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

Cut cut_between( Triangle triangle0, Triangle triangle1 )
{
  Cut cut;
  Intersection intersection0, intersection1;

  intersection0 = NULL;
  intersection1 = NULL;

  if ( two_intersections )
    {
      cut = (Cut)malloc( sizeof(CutStruct) );
      if (NULL == cut) 
	{
	  printf("%s: %d: malloc failed in cut_of\n",
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


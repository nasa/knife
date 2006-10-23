
/* connection between two Nodes */

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
#include "segment.h"

#define TRYN(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return NULL;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

#define NOT_NULLN(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return NULL;					      \
  }

Segment segment_create( Node node0, Node node1 )
{
  Segment segment;

  NOT_NULLN(node0,"node0 NULL in segment_create");
  NOT_NULLN(node1,"node1 NULL in segment_create");
  
  segment = (Segment)malloc( sizeof(SegmentStruct) );
  if (NULL == segment) {
    printf("%s: %d: malloc failed in segment_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  TRYN(segment_initialize( segment, node0, node1 ),"segment_initialize");
  
  return segment;
}

KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 )
{
  segment->node0 = node0;
  segment->node1 = node1;

  segment->intersection = array_create( 1, 10 );
  NOT_NULL( segment->intersection, "segment init intersection array NULL" );

  segment->triangle = array_create( 2, 10 );
  NOT_NULL( segment->triangle, "segment init triangle array NULL" );

  return(KNIFE_SUCCESS);
}

void segment_free( Segment segment )
{
  if ( NULL == segment ) return;
  array_free( segment->intersection );
  array_free( segment->triangle );
  free( segment );
}


Node segment_common_node( Segment segment0, Segment segment1 )
{
  Node node;
  node = NULL;
  if ( segment0->node0 == segment1->node0 ) node = segment0->node0;
  if ( segment0->node0 == segment1->node1 ) node = segment0->node0;
  if ( segment0->node1 == segment1->node0 ) node = segment0->node1;
  if ( segment0->node1 == segment1->node1 ) node = segment0->node1;
  return node;
}

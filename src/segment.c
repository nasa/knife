
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

KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 )
{
  segment->node0 = node0;
  segment->node1 = node1;
  segment->intersection = array_create( 5, 10 );
  segment->triangle = array_create( 2, 10 );
  return(KNIFE_SUCCESS);
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

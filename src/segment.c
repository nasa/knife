
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
#include "adj.h"


KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 )
{
  segment->node0 = node0;
  segment->node1 = node1;
  return KNIFE_SUCCESS;
}

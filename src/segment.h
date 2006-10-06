
/* connection between two Nodes */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SEGMENT_H
#define SEGMENT_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct SegmentStruct SegmentStruct;
struct SegmentStruct {
  Node node0, node1;
};
typedef SegmentStruct * Segment;

KNIFE_STATUS segment_initialize( Segment segment, Node node0, Node node1 );

END_C_DECLORATION

#endif /* SEGMENT_H */

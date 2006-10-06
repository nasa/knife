
/* defined by three segments and their three shared nodes */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "knife_definitions.h"
#include "segment.h"
#include "node.h"

BEGIN_C_DECLORATION

typedef struct TriangleStruct TriangleStruct;
struct TriangleStruct {
  Segment segment[3];
  Node node0, node1, node2;
};
typedef TriangleStruct * Triangle;

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2);
void triangle_free( Triangle );

END_C_DECLORATION

#endif /* TRIANGLE_H */

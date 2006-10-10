
/* double u-v-w triplet with Node parent */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef NODE_H
#define NODE_H

#include "knife_definitions.h"
#include "node.h"
#include "intersection.h"

BEGIN_C_DECLORATION

typedef struct SubnodeStruct SubnodeStruct;
struct SubnodeStruct {
  double uvw[3];
  Node node;
  Intersection intersection;
};
typedef SubnodeStruct * Subnode;

Subnode subnode_create( double *uvw, Node, Intersection );
void subnode_free( Subnode );

END_C_DECLORATION

#endif /* SUBNODE_H */

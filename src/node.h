
/* double x-y-z triplet with int index */

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

BEGIN_C_DECLORATION

typedef struct NodeStruct NodeStruct;
typedef NodeStruct * Node;

struct NodeStruct {
  double xyz[3];
};

Node node_create( double *xyz );
KNIFE_STATUS node_initialize( Node, double *xyz );

#define node_xyz(node) ((node)->xyz)
#define node_x(node) ((node)->xyz[0])
#define node_y(node) ((node)->xyz[1])
#define node_z(node) ((node)->xyz[2])

void node_free( Node );

END_C_DECLORATION

#endif /* NODE_H */

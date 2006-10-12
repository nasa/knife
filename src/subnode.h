
/* double u-v-w triplet with Node parent */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SUBNODE_H
#define SUBNODE_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct SubnodeStruct SubnodeStruct;
typedef SubnodeStruct * Subnode;
END_C_DECLORATION

#include "node.h"
#include "intersection.h"

BEGIN_C_DECLORATION

struct SubnodeStruct {
  double uvw[3];
  Node node;
  Intersection intersection;
};

Subnode subnode_create( double u, double v, double w,
			Node, Intersection );
void subnode_free( Subnode );

#define subnode_u( subnode ) ((subnode)->uvw[0]) 
#define subnode_v( subnode ) ((subnode)->uvw[1]) 
#define subnode_w( subnode ) ((subnode)->uvw[2]) 

#define subnode_intersection( subnode ) ((subnode)->intersection) 

double subnode_x( Subnode );
double subnode_y( Subnode );
double subnode_z( Subnode );

double subnode_area( Subnode node0, Subnode node1, Subnode node2 );



END_C_DECLORATION

#endif /* SUBNODE_H */

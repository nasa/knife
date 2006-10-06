
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
struct NodeStruct {
  double x,y,z;
  int indx;
};
typedef NodeStruct * Node;

END_C_DECLORATION

#endif /* NODE_H */

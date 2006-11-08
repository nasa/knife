
/* loop of subtri sides used for provable boundary recovery */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef LOOP_H
#define LOOP_H

#include <stdlib.h>
#include <stdio.h>
#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct LoopStruct LoopStruct;
typedef LoopStruct * Loop;
END_C_DECLORATION

#include "subnode.h"
#include "subtri.h"
#include "triangle.h"

BEGIN_C_DECLORATION

struct LoopStruct {
  int nside;
  int allocated;
  Subnode *side;
  Subnode node0, node1;
  Array subtri;
};

Loop loop_create( void );
void loop_free( Loop );

#define loop_nside(loop) ((loop)->nside)

KNIFE_STATUS loop_set_frame( int frame );

KNIFE_STATUS loop_add_side( Loop, Subnode, Subnode );
KNIFE_STATUS loop_remove_side( Loop, Subnode, Subnode );
KNIFE_STATUS loop_add_to_front( Loop loop, Subnode, Subnode );

KNIFE_STATUS loop_add_subtri( Loop, Subtri );

KNIFE_STATUS loop_subnode_after( Loop, Subnode node0, Subnode *node1);

KNIFE_STATUS loop_split( Loop, Subnode node0, Subnode node1, Loop *new_loop );

KNIFE_STATUS loop_triangulate( Loop, Triangle );

KNIFE_STATUS loop_tecplot( Loop );

END_C_DECLORATION

#endif /* LOOP_H */

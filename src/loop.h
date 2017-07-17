
/* loop of subtri sides used for provable boundary recovery */

/* Copyright 2007 United States Government as represented by the
 * Administrator of the National Aeronautics and Space
 * Administration. No copyright is claimed in the United States under
 * Title 17, U.S. Code.  All Other Rights Reserved.
 *
 * The knife platform is licensed under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
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

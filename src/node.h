
/* double x-y-z triplet with int index */

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
void node_free( Node );

#define node_xyz(node) ((node)->xyz)
#define node_x(node) ((node)->xyz[0])
#define node_y(node) ((node)->xyz[1])
#define node_z(node) ((node)->xyz[2])

END_C_DECLORATION

#endif /* NODE_H */


/* double u-v-w triplet with Node parent */

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

KNIFE_STATUS subnode_uvw( Subnode, double *uvw );

#define subnode_u( subnode ) ((subnode)->uvw[0]) 
#define subnode_v( subnode ) ((subnode)->uvw[1]) 
#define subnode_w( subnode ) ((subnode)->uvw[2]) 

#define subnode_node( subnode ) ((subnode)->node) 
#define subnode_intersection( subnode ) ((subnode)->intersection) 

KNIFE_STATUS subnode_xyz( Subnode, double *xyz );

double subnode_x( Subnode );
double subnode_y( Subnode );
double subnode_z( Subnode );

#define subnode_same_parent( s0, s1 ) \
  ( ( (s0)->node == (s1)->node ) && \
    ( (s0)->intersection == (s1)->intersection ) )

double subnode_area( Subnode node0, Subnode node1, Subnode node2 );

#define subnode_echo(sn,str) printf("%s u %f v %f w %f\n",(str),	\
				    (sn)->uvw[0],(sn)->uvw[1],(sn)->uvw[2]);


END_C_DECLORATION

#endif /* SUBNODE_H */

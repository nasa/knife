
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

#include <stdlib.h>
#include <stdio.h>
#include "node.h"

Node node_create( double *xyz )
{
  Node node;
  
  node = (Node)malloc( sizeof(NodeStruct) );
  if (NULL == node) {
    printf("%s: %d: malloc failed in node_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  node->xyz[0] = xyz[0];
  node->xyz[1] = xyz[1];
  node->xyz[2] = xyz[2];
  
  return node;
}

KNIFE_STATUS node_initialize( Node node, double *xyz )
{  
  node->xyz[0] = xyz[0];
  node->xyz[1] = xyz[1];
  node->xyz[2] = xyz[2];

  return KNIFE_SUCCESS;
}

void node_free( Node node )
{
  if ( NULL == node ) return;
  free( node );
}

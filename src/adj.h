
/* adjacency tool for assoicated objects */

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

#ifndef ADJ_H
#define ADJ_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct AdjItem AdjItem;
typedef AdjItem * AdjIterator;
typedef struct AdjStruct AdjStruct;
typedef AdjStruct * Adj;

struct AdjItem {
  int item;
  AdjItem *next;
};

struct AdjStruct {
  int nnode, nadj, chunk_size;
  AdjItem *node2item;
  AdjItem **first;
  AdjItem *current;
  AdjItem *blank;
};

Adj adj_create( int nnode, int nadj, int chunkSize );
void adj_free( Adj );

#define adj_nnode(adj) (adj->nnode)
#define adj_nadj(adj) (adj->nadj)
#define adj_chunk_size(adj) (adj->chunk_size)

Adj adj_resize( Adj, int nnode );

Adj adj_add( Adj, int node, int item );
Adj adj_remove( Adj, int node, int item );

#define adj_valid(iterator) (iterator!=NULL)
#define adj_more(iterator) ((iterator!=NULL)&&(iterator->next != NULL))
#define adj_first(adj,node) \
((node < 0 || node >= adj->nnode)?NULL:adj->first[node])

#define adj_item(iterator) (iterator==NULL?EMPTY:iterator->item)
#define adj_next(iterator) (iterator==NULL?NULL:iterator->next)

KnifeBool adj_exists( Adj, int node, int item );
int adj_degree( Adj, int node );

END_C_DECLORATION

#endif /* ADJ_H */


/* adjacency tool for assoicated objects */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef ADJ_H
#define ADJ_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct AdjItem AdjItem;
struct AdjItem {
  int item;
  AdjItem *next;
};
typedef AdjItem * AdjIterator;

typedef struct AdjStruct AdjStruct;
struct AdjStruct {
  int nnode, nadj, chunk_size;
  AdjItem *node2item;
  AdjItem **first;
  AdjItem *current;
  AdjItem *blank;
};
typedef AdjStruct * Adj;

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

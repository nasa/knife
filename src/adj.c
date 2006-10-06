
/* adjacency tool for assoicated objects */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include "adj.h"

static KNIFE_STATUS adj_allocate_and_init_node2item(Adj *adj);

static KNIFE_STATUS adj_allocate_and_init_node2item(Adj *adj)
{
  int i;
  adj->node2item = (AdjItem *)malloc( adj->nadj * sizeof(AdjItem));
  if (NULL == adj->node2item) {
    printf("%s: %d: malloc failed in adj_allocate_and_init_node_item\n",
	   __FILE__,__LINE__);
    return(KNIFE_MEMORY);
  }
  
  for ( i=0 ; i < (adj->nadj-1) ; i++ ) {
    adj->node2item[i].item = EMPTY;
    adj->node2item[i].next = &adj->node2item[i+1];
  }
  adj->node2item[adj->nadj-1].item = EMPTY;
  adj->node2item[adj->nadj-1].next = NULL;
  
  adj->blank = adj->node2item;
  return(KNIFE_SUCCESS);
}

Adj* adj_create( int nnode, int nadj, int chunk_size )
{
  int node;
  Adj *adj;
  
  adj = malloc( sizeof(Adj) );
  if (NULL == adj) {
    printf("%s: %d: malloc failed in adj_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  adj->nnode      = MAX(nnode,1);
  adj->nadj       = MAX(nadj,1);
  adj->chunk_size = MAX(chunk_size,1);
  
  adj_allocate_and_init_node2item(adj);
       
  adj->current = NULL;
       
  adj->first = (AdjItem **) malloc( adj->nnode * sizeof(AdjItem*) );
  if (NULL == adj->first) {
    printf("%s: %d: malloc failed in adj_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  for ( node=0 ; node<adj->nnode; node++ ) adj->first[node] = NULL; 

  return adj;
}

void adj_free( Adj *adj )
{
  if ( NULL == adj ) return;
  free( adj->node2item );
  free( adj->first );
  free( adj );
}

Adj *adj_resize( Adj *adj, int nnode )
{
  AdjItem *remove;
  AdjItem **new_mem;
  int node, old_size, new_size;
  old_size = adj->nnode;
  new_size = MAX(nnode,1);
  if ( old_size > new_size) {
    for ( node=new_size ; node<old_size; node++ ) {
      while ( NULL != adj->first[node] ) {
	remove = adj->first[node];
	adj->first[node] = remove->next;
	remove->item = EMPTY;
	remove->next = adj->blank;
	adj->blank = remove;
      }
    }
  }
  adj->nnode = new_size;
  new_mem = (AdjItem **) realloc( adj->first, adj->nnode * sizeof(AdjItem*) );
  if (NULL == new_mem) {
    printf("%s: %d: realloc failed in adj_resize\n",__FILE__,__LINE__);
    return NULL;
  }
  adj->first = new_mem;
  for ( node=old_size ; node<adj->nnode; node++ ) adj->first[node] = NULL; 
  return adj;
}

Adj *adj_add( Adj *adj, int node, int item )
{
  AdjItem *oldnode2item;
  AdjItem *oldfirst;
  AdjItem *new;
  int copynode;

  if (node>=adj->nnode || node<0) return NULL;

  if (NULL == adj->blank) {
    adj->nadj += adj->chunk_size;
    oldnode2item = adj->node2item;
    adj_allocate_and_init_node2item(adj);
    for ( copynode = 0 ; copynode < adj->nnode ; copynode++ ) {
      oldfirst = adj->first[copynode];
      adj->first[copynode] = NULL;
      while ( NULL != oldfirst ) {
	new = adj->blank;
	adj->blank = adj->blank->next;
	new->next = adj->first[copynode];
	adj->first[copynode] = new;
	new->item = oldfirst->item;
	oldfirst = oldfirst->next;
      }
    }
    free(oldnode2item);
  }

  adj->current = NULL;

  new = adj->blank;
  adj->blank = adj->blank->next;
  new->next = adj->first[node];
  adj->first[node] = new;
  new->item = item;
  return adj;
}

Adj* adj_remove(Adj *adj, int node, int item)
{
  AdjIterator it;
  AdjItem  *remove, *previous;

  remove = NULL;
  previous = NULL;

  for ( it = adj_first(adj,node); adj_valid(it); it = adj_next(it) ) {
    if (adj_item(it)==item) {
      remove = it;
      break;
    }else{
      previous = it;
    }
  }

  if (remove == NULL) return NULL;
 
  if ( previous == NULL ) {
    adj->first[node] = remove->next;
  }else{
    previous->next = remove->next;
  }

  remove->item = EMPTY;
  remove->next = adj->blank;
  adj->blank = remove;

  return adj;
}

KnifeBool adj_exists( Adj *adj, int node, int item )
{
  AdjIterator it;
  KnifeBool exist;
  exist = FALSE;
  for ( it = adj_first(adj,node); 
	!exist && adj_valid(it); 
	it = adj_next(it)) 
    exist = (item == adj_item(it));
  return exist;
}

int adj_degree(Adj *adj, int node )
{
  AdjIterator it;
  int degree;
  degree =0;
  for ( it = adj_first(adj,node) ; adj_valid(it); it = adj_next(it)) degree++;
  return degree;
}

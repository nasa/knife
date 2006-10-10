
/* storage for a primal tetrahedral grid */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef PRIMAL_H
#define PRIMAL_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct PrimalStruct PrimalStruct;
typedef PrimalStruct * Primal;

END_C_DECLORATION

#include "adj.h"

BEGIN_C_DECLORATION

struct PrimalStruct {

  int nnode;
  double *xyz;

  int nface;
  int *f2n;

  int ncell;
  int *c2n;

  Adj cell_adj;
  Adj face_adj;

  int nedge;
  int *c2e;
  int *e2n;

  int ntri;
  int *c2t;
  int *t2n;

};

#define primal_test_malloc(ptr,fcn)		       \
  if (NULL == (ptr)) {				       \
    printf("%s: %d: malloc failed in %s\n",	       \
	   __FILE__,__LINE__,(fcn));		       \
    return NULL;				       \
  }

Primal primal_create( int nnode, int nface, int ncell );
Primal primal_from_FAST( char *filename );

/* Primal *primal_from_FAST_file( char *filename ); */

void primal_free( Primal );

#define primal_nnode(primal) (primal->nnode)
#define primal_nface(primal) (primal->nface)
#define primal_ncell(primal) (primal->ncell)

#define primal_nedge(primal) (primal->nedge)
#define primal_ntri(primal) (primal->ntri)

KNIFE_STATUS primal_establish_c2e( Primal );
KNIFE_STATUS primal_establish_c2t( Primal );

/* xyz[3] */
KNIFE_STATUS primal_xyz( Primal, int node_index, double *xyz); 

/* face[4] : face[0-2] face node indexes, face[3] boundary group id  */
KNIFE_STATUS primal_face( Primal, int face_index, int *face); 

KNIFE_STATUS primal_cell( Primal, int cell_index, int *nodes); 

KNIFE_STATUS primal_edge( Primal, int edge_index, int *nodes); 
KNIFE_STATUS primal_tri( Primal, int tri_index, int *nodes); 

KNIFE_STATUS primal_find_face_side( Primal, int node0, int node1, 
				    int *other_face_index, int *other_side ); 

KNIFE_STATUS primal_find_cell_side( Primal, int node0, int node1, int node2, 
				    int *other_cell_index, int *other_side ); 

KNIFE_STATUS primal_find_edge ( Primal, int node0, int node1,
				int *edge_index ); 

#define primal_face_side_node0(side) ((0==side)?1:(1==side)?2:(2==side)?0:EMPTY)
#define primal_face_side_node1(side) ((0==side)?2:(1==side)?0:(2==side)?1:EMPTY)

/* 0, 0, 0, 1, 1, 2 */
#define primal_cell_edge_node0(edge) \
  ((3>edge)?0:(5>edge)?1:2)
/* 1, 2, 3, 2, 3, 3 */
#define primal_cell_edge_node1(edge) \
  ((0==edge)?1:(1==edge)?2:(3==edge)?2:3)

#define primal_cell_side_node0(side) ((0==side)?1:(1==side)?0:(2==side)?0:0)
#define primal_cell_side_node1(side) ((0==side)?3:(1==side)?2:(2==side)?3:1)
#define primal_cell_side_node2(side) ((0==side)?2:(1==side)?3:(2==side)?1:2)

END_C_DECLORATION

#endif /* PRIMAL_H */

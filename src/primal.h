
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
#include "set.h"

BEGIN_C_DECLORATION

struct PrimalStruct {

  int nnode0;

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

  int surface_nnode;
  int *surface_node;
  int *surface_volume_node;
};

Primal primal_create( int nnode, int nface, int ncell );
Primal primal_from_fast( char *filename );
Primal primal_from_tri( char *filename );

void primal_free( Primal );

KNIFE_STATUS primal_copy_volume( Primal, 
				 double *x, double *y, double *z,
				 int *c2n );

KNIFE_STATUS primal_copy_boundary( Primal, int face_id, 
				   int nboundnode, int *inode,
				   int leading_dim, int nface, int *f2n );

#define primal_nnode0(primal) (primal->nnode0)
#define primal_nnode(primal) (primal->nnode)
#define primal_nface(primal) (primal->nface)
#define primal_ncell(primal) (primal->ncell)

#define primal_nedge(primal) (primal->nedge)
#define primal_ntri(primal) (primal->ntri)

KNIFE_STATUS primal_establish_all( Primal );

KNIFE_STATUS primal_establish_c2e( Primal );
KNIFE_STATUS primal_establish_c2t( Primal );
KNIFE_STATUS primal_establish_surface_node( Primal );

#define primal_c2e(primal,cell,edge) ((primal)->c2e[(edge)+6*(cell)])
#define primal_c2t(primal,cell,tri)  ((primal)->c2t[(tri) +4*(cell)])

#define primal_surface_nnode(primal) ((primal)->surface_nnode)
#define primal_surface_node(primal,volume_node) \
  ((primal)->surface_node[(volume_node)])
#define primal_surface_volume_node(primal,surface_node) \
  ((primal)->surface_volume_node[(surface_node)])

/* xyz[3] */
KNIFE_STATUS primal_xyz( Primal, int node_index, double *xyz); 

/* face[4] : face[0-2] face node indexes, face[3] boundary group id  */
KNIFE_STATUS primal_face( Primal, int face_index, int *face); 
#define primal_face_adj(primal)  ((primal)->face_adj)

KNIFE_STATUS primal_max_face_id( Primal, int *max_face_id); 

KNIFE_STATUS primal_cell( Primal, int cell_index, int *nodes); 
KNIFE_STATUS primal_cell_center( Primal, int cell_index, double *xyz); 
#define primal_cell_adj(primal)  ((primal)->cell_adj)

KNIFE_STATUS primal_edge( Primal, int edge_index, int *nodes); 
KNIFE_STATUS primal_edge_center( Primal, int edge_index, double *xyz); 

KNIFE_STATUS primal_tri( Primal, int tri_index, int *nodes); 
KNIFE_STATUS primal_tri_center( Primal, int tri_index, double *xyz); 

KNIFE_STATUS primal_find_face_side( Primal, int node0, int node1, 
				    int *face_index, int *side ); 

KNIFE_STATUS primal_find_cell_side( Primal, int node0, int node1, int node2, 
				    int *cell_index, int *side ); 

KNIFE_STATUS primal_find_cell_edge( Primal, int cell, int edge, int *cell_edge);

KNIFE_STATUS primal_find_edge ( Primal, int node0, int node1,
				int *edge_index ); 

KNIFE_STATUS primal_find_tri( Primal, int node0, int node1, int node2, 
			      int *tri ); 

KNIFE_STATUS primal_find_tri_side( Primal, int tri, int node0, int node1,
				   int *tri_side );

#define primal_face_side_node0(side) ((0==side)?1:(1==side)?2:(2==side)?0:EMPTY)
#define primal_face_side_node1(side) ((0==side)?2:(1==side)?0:(2==side)?1:EMPTY)

/* 0, 0, 0, 1, 1, 2 */
#define primal_cell_edge_node0(edge) \
  ((3>edge)?0:(5>edge)?1:2)
/* 1, 2, 3, 2, 3, 3 */
#define primal_cell_edge_node1(edge) \
  ((0==edge)?1:(1==edge)?2:(3==edge)?2:3)

/* 2, 3, 1, 0, 2, 0 */
#define primal_cell_edge_left_side(edge) \
  ((0==edge)?2:(1==edge)?3:(2==edge)?1:(3==edge)?0:(4==edge)?2:0)
/* 3, 1, 2, 3, 0, 1 */
#define primal_cell_edge_right_side(edge) \
  ((0==edge)?3:(1==edge)?1:(2==edge)?2:(3==edge)?3:(4==edge)?0:1)

#define primal_cell_side_node0(side) ((0==side)?1:(1==side)?0:(2==side)?0:0)
#define primal_cell_side_node1(side) ((0==side)?3:(1==side)?2:(2==side)?3:1)
#define primal_cell_side_node2(side) ((0==side)?2:(1==side)?3:(2==side)?1:2)

KNIFE_STATUS primal_scale_about( Primal, 
				 double x, double y, double z, double scale );
KNIFE_STATUS primal_translate( Primal, 
			       double dx, double dy, double dz );
KNIFE_STATUS primal_rotate( Primal, 
			    double nx, double ny, double nz, 
			    double angle_in_radians );
KNIFE_STATUS primal_flip_yz( Primal ); /* (y=>z,z=>-y) */
KNIFE_STATUS primal_flip_zy( Primal ); /* (z=>y,y=>-z) */

KNIFE_STATUS primal_reflect_y( Primal );

KNIFE_STATUS primal_flip_face_normals( Primal );

KNIFE_STATUS primal_apply_massoud( Primal primal, char *massoud_filename );

Primal primal_subset( Primal, Set bcs );

KNIFE_STATUS primal_export_tri( Primal, char *filename );
KNIFE_STATUS primal_export_fast( Primal, char *filename );
KNIFE_STATUS primal_export_tec( Primal, char *filename );
KNIFE_STATUS primal_export_single_zone_tec( Primal, char *filename );
KNIFE_STATUS primal_export_vtk( Primal, char *filename );

END_C_DECLORATION

#endif /* PRIMAL_H */

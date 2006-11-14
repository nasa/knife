
/* fortran API for knife package */

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
#include <string.h>

#include "knife_definitions.h"

#include "primal.h"
#include "surface.h"
#include "domain.h"

#include "node.h"
#include "triangle.h"
#include "loop.h"
#include "poly.h"

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      fflush(stdout);					      \
      *knife_status = code;				      \
      return;						      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				\
  if (NULL == (pointer)) {				\
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	\
    fflush(stdout);					\
    *knife_status = KNIFE_FAILURE;			\
    return;						\
  }

static Primal  surface_primal = NULL;
static Surface surface        = NULL;
static Primal  volume_primal  = NULL;
static Domain  domain         = NULL;
static int partition = EMPTY;

void knife_volume_( int *part_id,
		    int *nnode0, int *nnode, double *x, double *y, double *z,
		    int *nface, int *ncell, int *maxcell, int *c2n, 
		    int *knife_status )
{

  partition = *part_id;

  triangle_set_frame( 10000*partition);
  loop_set_frame( 10000*partition);

  volume_primal = primal_create( *nnode, *nface, *ncell );
  NOT_NULL(volume_primal, "volume_primal NULL");

  volume_primal->nnode0 = *nnode0;

  TRY( primal_copy_volume( volume_primal, x, y, z, *maxcell, c2n ), 
       "primal_copy_volume");

  *knife_status = KNIFE_SUCCESS;
}
void knife_volume__( int *part_id,
		     int *nnode0, int *nnode, double *x, double *y, double *z,
		     int *nface, int *ncell, int *maxcell, int *c2n, 
		     int *knife_status )
{
  knife_volume_( part_id,
		 nnode0, nnode, x, y, z,
		 nface, ncell, maxcell, c2n, 
		 knife_status );
}


void knife_boundary_( int *face_id, int *nodedim, int *inode,
		      int *leading_dim, int *nface, int *f2n, 
		      int *knife_status )
{

  if ( *nodedim > 0 )
    TRY( primal_copy_boundary( volume_primal, *face_id, inode,
			       *leading_dim, *nface, f2n ), 
	 "primal_copy_boundary");

  *knife_status = KNIFE_SUCCESS;
}
void knife_boundary__( int *face_id, int *nodedim, int *inode,
		      int *leading_dim, int *nface, int *f2n, 
		      int *knife_status )
{
  knife_boundary_( face_id, nodedim, inode,
		   leading_dim, nface, f2n, 
		   knife_status );
}

void knife_required_local_dual_( char *knife_input_file_name, 
				 int *nodedim, int *required,
				 int *knife_status )
{
  FILE *f;
  char surface_filename[1025];
  char orientation_string[1025];
  KnifeBool orientation_missing;
  KnifeBool inward_pointing_surface_normal;
  Array active_bcs;
  int *bc, bc_found;

  if ( *nodedim != primal_nnode(volume_primal)  )
    {
      printf("%s: %d: knife_required_local_dual_ wrong nnode %d %d\n",
	     __FILE__,__LINE__,*nodedim,domain_npoly( domain ));
      *knife_status = KNIFE_ARRAY_BOUND;
      return;
    }

  f = NULL;
  f = fopen(knife_input_file_name, "r");
  if ( NULL == f ) printf("filename: %s\n",knife_input_file_name);
  NOT_NULL(f , "could not open knife_input_file_name");

  fscanf( f, "%s\n", surface_filename);
  surface_primal = primal_from_FAST( surface_filename );
  if ( NULL == surface_primal ) 
    printf("surface filename: %s\n",surface_filename);
  NOT_NULL(surface_primal, "surface_primal NULL");
  
  inward_pointing_surface_normal = TRUE;
  orientation_missing = TRUE;
  
  fscanf( f, "%s\n", orientation_string );
  if( strcmp(orientation_string,"outward") == 0 ) {
    orientation_missing = FALSE;
    inward_pointing_surface_normal = FALSE;
  }
  if( strcmp(orientation_string,"inward") == 0 ) {
    orientation_missing = FALSE;
    inward_pointing_surface_normal = TRUE;
  }

  if (orientation_missing)
    {
      printf("%s: %d: knife input file error: orientation\n",__FILE__,__LINE__);
      *knife_status = KNIFE_FILE_ERROR;
      return;
    }

  active_bcs = array_create(10,10);
  NOT_NULL(active_bcs, "active_bcs NULL");

  while ( !feof( f ) )
    {
      bc = (int *) malloc( sizeof(int) );
      NOT_NULL( bc , "bc NULL" );
      bc_found = fscanf( f, "%d", bc );
      if ( 1 == bc_found )
	{
	  TRY( array_add( active_bcs, bc ), "array_add bc");
	}
      else
	{
	  free(bc);
	}
    }

  surface = surface_from( surface_primal, active_bcs, 
			  inward_pointing_surface_normal );
  NOT_NULL(surface, "surface NULL");

  TRY( primal_establish_all( volume_primal ), "primal_establish_all" );

  domain = domain_create( volume_primal, surface );
  NOT_NULL(domain, "domain NULL");

  TRY( domain_required_local_dual( domain, required ), 
       "domain_required_local_dual" );

  *knife_status = KNIFE_SUCCESS;
}
void knife_required_local_dual__( char *knife_input_file_name,
				  int *nodedim, int *required,
				  int *knife_status )
{
  knife_required_local_dual_( knife_input_file_name,
			      nodedim, required,
			      knife_status );
}

void knife_cut_( int *nodedim, int *required,
		 int *knife_status )
{
  char tecplot_file_name[1025];
  if ( *nodedim != primal_nnode(volume_primal) )
    {
      printf("%s: %d: knife_cut_ wrong nnode %d %d\n",
	     __FILE__,__LINE__,*nodedim,domain_npoly( domain ));
      *knife_status = KNIFE_ARRAY_BOUND;
      return;
    }

  TRY( domain_create_dual( domain, required ), "domain_required_local_dual" );

  TRY( domain_boolean_subtract( domain ), "boolean subtract" );

  if (FALSE) 
    {
      sprintf( tecplot_file_name, "knife_cut%03d.t", partition );
      domain_tecplot( domain, tecplot_file_name );
    }

  *knife_status = KNIFE_SUCCESS;
}
void knife_cut__( int *nodedim, int *required,
		 int *knife_status )
{
  knife_cut_( nodedim, required, 
	      knife_status );
}

void knife_dual_topo_( int *nodedim, int *topo,
		       int *knife_status )
{
  int node;

  if ( *nodedim != domain_npoly( domain ) )
    {
      printf("%s: %d: knife_dual_topo_ wrong nnode %d %d\n",
	     __FILE__,__LINE__,*nodedim,domain_npoly( domain ));
      *knife_status = KNIFE_ARRAY_BOUND;
      return;
    }

  for ( node = 0; node < domain_npoly(domain); node++ )
    {
      topo[node] = domain_topo(domain,node);
    }

  *knife_status = KNIFE_SUCCESS;
}
void knife_dual_topo__( int *nodedim, int *topo,
			int *knife_status )
{
  knife_dual_topo_( nodedim, topo,
		    knife_status );
}

void knife_dual_centroid_volume_( int *node, 
				  double *x, double *y, double *z, 
				  double *volume,
				  int *knife_status )
{
  double xyz[3], center[3];
  Poly poly;

  TRY( primal_xyz(domain_primal(domain),(*node)-1,xyz), "primal_xyz" );

  center[0] = xyz[0];
  center[1] = xyz[1];
  center[2] = xyz[2];

  poly = domain_poly(domain,(*node)-1);
  NOT_NULL( poly, "poly NULL" );

  *knife_status = poly_centroid_volume(poly,xyz,center,volume);

  *x = center[0];
  *y = center[1];
  *z = center[2];
}
void knife_dual_centroid_volume__( int *node, 
				   double *x, double *y, double *z, 
				   double *volume,
				   int *knife_status )
{
  knife_dual_centroid_volume_( node, 
			       x, y, z, 
			       volume,
			       knife_status );
}

void knife_number_of_triangles_between_( int *node1, int *node2,
					 int *nsubtri,
					 int *knife_status )
{
  int n;
  int edge;
  Poly poly;
  Node node;

  poly = domain_poly( domain, (*node1)-1 );
  NOT_NULL(poly, "node1 poly NULL in knife_number_of_triangles_between_");

  TRY( primal_find_edge( volume_primal, (*node1)-1, (*node2)-1, &edge ), 
       "no edge found by primal_edge_between"); 

  node = domain_node_at_edge_center( domain, edge );
  NOT_NULL(node, "edge node NULL in knife_number_of_triangles_between_");

  TRY( poly_nsubtri_about( poly, node, &n ), "poly_nsubtri_about" );
  
  *nsubtri = n;
  *knife_status = KNIFE_SUCCESS;
}
void knife_number_of_triangles_between__( int *node1, int *node2,
					  int *nsubtri,
					  int *knife_status )
{
  knife_number_of_triangles_between_( node1, node2,
				      nsubtri,
				      knife_status );
}

void knife_triangles_between_( int *node1, int *node2,
                               int *nsubtri,
                               double *triangle_node0,
                               double *triangle_node1,
                               double *triangle_node2,
                               int *knife_status )
{
  int edge;
  Poly poly;
  Node node;

  poly = domain_poly( domain, (*node1)-1 );
  NOT_NULL(poly, "node1 poly NULL in knife_triangles_between_");

  TRY( primal_find_edge( volume_primal, (*node1)-1,  (*node2)-1, &edge ), 
       "no edge found by primal_edge_between"); 

  node = domain_node_at_edge_center( domain, edge );
  NOT_NULL(node, "edge node NULL in knife_triangles_between_");

  TRY( poly_subtri_about( poly, node, *nsubtri, 
			  triangle_node0, triangle_node1, triangle_node2 ), 
       "poly_nsubtri_about" );
  
  *knife_status = KNIFE_SUCCESS;
}
void knife_triangles_between__( int *node1, int *node2,
				int *nsubtri,
				double *triangle_node0,
				double *triangle_node1,
				double *triangle_node2,
				int *knife_status )
{
  knife_triangles_between_( node1, node2,
			    nsubtri,
			    triangle_node0,
			    triangle_node1,
			    triangle_node2,
			    knife_status );
}

void knife_number_of_surface_triangles_( int *node,
					 int *nsubtri,
					 int *knife_status )
{
  int n;
  Poly poly;

  poly = domain_poly( domain, (*node)-1 );
  NOT_NULL(poly, "poly NULL in knife_number_of_surface_triangles_");

  TRY( poly_surface_nsubtri( poly, &n ), "poly_nsubtri_about" );
  
  *nsubtri = n;
  *knife_status = KNIFE_SUCCESS;
}
void knife_number_of_surface_triangles__( int *node,
					  int *nsubtri,
					  int *knife_status )
{
  knife_number_of_surface_triangles_( node,
				      nsubtri,
				      knife_status );
}

void knife_surface_triangles_( int *node,
                               int *nsubtri,
                               double *triangle_node0,
                               double *triangle_node1,
                               double *triangle_node2,
                               int *knife_status )
{
  Poly poly;

  poly = domain_poly( domain, (*node)-1 );
  NOT_NULL(poly, "poly NULL in knife_surface_triangles_");

  TRY( poly_surface_subtri( poly, *nsubtri, 
			    triangle_node0, triangle_node1, triangle_node2 ), 
       "poly_nsubtri_about" );
  
  *knife_status = KNIFE_SUCCESS;
}
void knife_surface_triangles__( int *node,
                               int *nsubtri,
                               double *triangle_node0,
                               double *triangle_node1,
                               double *triangle_node2,
                               int *knife_status )
{
  knife_surface_triangles_( node,
			    nsubtri,
			    triangle_node0,
			    triangle_node1,
			    triangle_node2,
			    knife_status );
}

void knife_number_of_boundary_triangles_( int *node, int *face,
					  int *nsubtri,
					  int *knife_status )
{
  int n;
  Poly poly;

  poly = domain_poly( domain, (*node)-1 );
  NOT_NULL(poly, "poly NULL in knife_number_of_boundary_triangles_");

  TRY( poly_boundary_nsubtri( poly, (*face)-1, &n ), "poly_nsubtri_about" );
  
  *nsubtri = n;
  *knife_status = KNIFE_SUCCESS;
}
void knife_number_of_boundary_triangles__( int *node, int *face,
					   int *nsubtri,
					   int *knife_status )
{
  knife_number_of_boundary_triangles_( node, face,
				       nsubtri,
				       knife_status ); 
}

void knife_boundary_triangles_( int *node, int *face,
                               int *nsubtri,
                               double *triangle_node0,
                               double *triangle_node1,
                               double *triangle_node2,
                               int *knife_status )
{
  Poly poly;

  poly = domain_poly( domain, (*node)-1 );
  NOT_NULL(poly, "poly NULL in knife_boundary_triangles_");

  TRY( poly_boundary_subtri( poly, (*face)-1, *nsubtri, 
			    triangle_node0, triangle_node1, triangle_node2 ), 
       "poly_nsubtri_about" );
  
  *knife_status = KNIFE_SUCCESS;
}
void knife_boundary_triangles__( int *node, int *face,
				 int *nsubtri,
				 double *triangle_node0,
				 double *triangle_node1,
				 double *triangle_node2,
				 int *knife_status )
{
  knife_boundary_triangles_( node, face,
			     nsubtri,
			     triangle_node0,
			     triangle_node1,
			     triangle_node2,
			     knife_status );
}


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
		    int *nnode, double *x, double *y, double *z,
		    int *nface, int *ncell, int *maxcell, int *c2n, 
		    int *knife_status )
{

  partition = *part_id;

  volume_primal = primal_create( *nnode, *nface, *ncell );
  NOT_NULL(volume_primal, "volume_primal NULL");

  TRY( primal_copy_volume( volume_primal, x, y, z, *maxcell, c2n ), 
       "primal_copy_volume");

  fflush(stdout);
  *knife_status = KNIFE_SUCCESS;
}

void knife_boundary_( int *face_id, int *nodedim, int *inode,
		      int *leading_dim, int *nface, int *f2n, 
		      int *knife_status )
{

  if ( *nodedim > 0 )
    TRY( primal_copy_boundary( volume_primal, *face_id, inode,
			       *leading_dim, *nface, f2n ), 
	 "primal_copy_boundary");

  fflush(stdout);
  *knife_status = KNIFE_SUCCESS;
}

void knife_cut_( char *knife_input_file_name, 
		 int *knife_status )
{
  FILE *f;
  char surface_filename[1025];
  char orientation_string[1025];
  KnifeBool orientation_missing;
  KnifeBool inward_pointing_surface_normal;
  Array active_bcs;
  int *bc, bc_found;
  char tecplot_file_name[1025];

  f = NULL;
  f = fopen(knife_input_file_name, "r");
  NOT_NULL(f , "could not open knife_input_file_name");
  
  fscanf( f, "%s\n", surface_filename);
  printf(" knife surface filename %s\n", surface_filename);
  surface_primal = primal_from_FAST( surface_filename );
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

  printf(" knife surface filename %s\n", surface_filename);

  if ( inward_pointing_surface_normal ) 
    {
      printf(" knife surface points into the domain\n");
    }
  else
    {
      printf(" knife surface points out of the domain\n");
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
	  printf(" knife surface bc %d active\n",*bc);
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

  TRY( domain_required_dual( domain ), "domain_all_dual" );

  TRY( domain_boolean_subtract( domain ), "boolean subtract" );

  sprintf( tecplot_file_name, "knife_cut%03d.t", partition );
  printf(" knife producing %s \n",tecplot_file_name);
  domain_tecplot( domain, tecplot_file_name );

  fflush(stdout);
  *knife_status = KNIFE_SUCCESS;
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

  fflush(stdout);
  *knife_status = KNIFE_SUCCESS;
}

void knife_centroid_volume_( int *node, 
			     double *x, double *y, double *z, double *volume,
			     int *knife_status )
{
  double xyz[3], center[3];

  primal_xyz(domain_primal(domain),*node,xyz);

  center[0] = xyz[0];
  center[1] = xyz[1];
  center[2] = xyz[2];

  poly_centroid_volume(domain_poly(domain,*node),xyz,center,volume);
  *x = center[0];
  *y = center[1];
  *z = center[2];

  fflush(stdout);
  *knife_status = KNIFE_SUCCESS;
}


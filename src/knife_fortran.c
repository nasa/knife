
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

#include "knife_definitions.h"

#include "primal.h"
#include "surface.h"
#include "domain.h"

#define NOT_NULL(pointer,msg)				\
  if (NULL == (pointer)) {				\
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	\
    return;						\
  }

static Primal  surface_primal = NULL;
static Surface surface        = NULL;
static Primal  volume_primal  = NULL;
static int partition = EMPTY;

void knife_create_( int *part_id, char *project, 
		    int *nnode, double *x, double *y, double *z,
		    int *nface, int *ncell, int *maxcell, int *c2n )
{
  char surface_filename[1025];
  KnifeBool inward_pointing_surface_normal;
  Array active_bcs;
  int *bc;

  partition = *part_id;

  sprintf( surface_filename, "%s_surface.fgrid",project  );
  surface_primal = primal_from_FAST( surface_filename );
  NOT_NULL(surface_primal, "surface_primal NULL");

  active_bcs = array_create(10,10);
  bc = (int *) malloc( sizeof(int) ); *bc = 1; array_add( active_bcs, bc );
  bc = (int *) malloc( sizeof(int) ); *bc = 2; array_add( active_bcs, bc );

  inward_pointing_surface_normal = TRUE;

  surface = surface_from( surface_primal, active_bcs, 
			  inward_pointing_surface_normal );
  NOT_NULL(surface, "surface NULL");

  volume_primal = primal_create( *nnode, *nface, *ncell );
  NOT_NULL(volume_primal, "volume_primal NULL");

  primal_copy_arrays( volume_primal, x, y, z, *maxcell, c2n );
}

void knife_create_boundary_( int *face_id, int *nnode, int *nodedim, int *inode,
			     int *nface, int *dim1, int *dim2, int *f2n )
{

  if (*nface > *dim2 )
    {
      printf("dim2 array bound %d %d\n",*nface,*dim2);
      return;
    }

  if (*nnode > *nodedim )
    {
      printf("nodedim array bound %d %d\n",*nnode,*nodedim);
      return;
    }

  primal_copy_boundary( volume_primal, *face_id,
			*nnode, inode,
			*nface, *dim1, f2n );
}

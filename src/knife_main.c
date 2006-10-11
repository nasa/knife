/* main driver program for knife package */

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
#include <unistd.h> /* for sleep */
#include <string.h>
#include "knife_definitions.h"
#include "array.h"
#include "primal.h"
#include "surface.h"
#include "domain.h"

int main( int argc, char *argv[] )
{
  int argument;

  char surface_filename[1025];
  Primal surface_primal;
  Array active_bcs;
  int *bc;
  Surface surface;

  char volume_filename[1025];
  Primal volume_primal;
  Domain dual_domain;

  sprintf( surface_filename, "not_set" );
  active_bcs = array_create(10,10);

  argument = 1;
  while ( argument < argc )
    {
      if( strcmp(argv[argument],"-s") == 0 ) {
	argument++; 
	sprintf( surface_filename, "%s", argv[argument] );
	printf("-s %s\n", surface_filename);
      }

      if( strcmp(argv[argument],"-v") == 0 ) {
	argument++; 
	sprintf( volume_filename, "%s", argv[argument] );
	printf("-v %s\n", volume_filename);
      }

      if( strcmp(argv[argument],"-b") == 0 ) {
	argument++;
	bc = (int *) malloc( sizeof(int) );
	*bc = atoi(argv[argument]);
	array_add( active_bcs, bc );
	printf("-b %d\n", *bc );
      }

      argument++;
    }

  surface_primal = primal_from_FAST( surface_filename );
  surface = surface_from( surface_primal, active_bcs );

  volume_primal = primal_from_FAST( volume_filename );

  dual_domain = domain_create( volume_primal, surface );
  domain_dual_elements( tet_domain );

  domain_boolean_subtract( dual_domain );

  /* sleep(2); */

  return 0;
}


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
#include <string.h>
#include "knife_definitions.h"
#include "array.h"
#include "primal.h"
#include "surface.h"

int main( int argc, char *argv[] )
{
  int argument;

  char surface_filename[1025];
  Array active_bcs;
  int *bc;
  Primal primal;
  Surface surface;

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

      if( strcmp(argv[argument],"-b") == 0 ) {
	argument++;
	bc = (int *) malloc( sizeof(int) );
	*bc = atoi(argv[argument]);
	array_add( active_bcs, bc );
	printf("-b %d\n", *bc );
      }

      argument++;
    }

  primal = primal_from_FAST( surface_filename );
  surface = surface_from( primal, active_bcs );

  return 0;
}


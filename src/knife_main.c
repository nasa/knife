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

int main( int argc, char *argv[] )
{
  int argument;

  char surface_filename[1025];

  sprintf( surface_filename, "not_set" );

  argument = 1;
  while ( argument < argc )
    {
      if( strcmp(argv[argument],"-s") == 0 ) {
	argument++; 
	sprintf( surface_filename, "%s", argv[argument] );
	printf("-s %s\n", surface_filename);
      }

      argument++;
    }
  return 0;
}


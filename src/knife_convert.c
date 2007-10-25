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
#include "primal.h"

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

int main( int argc, char *argv[] )
{
  char fast_filename[1025];
  char tri_filename[1025];
  Primal surface_primal;

  if ( 3 != argc ) 
    {
      printf("usage : %s input.fgrid output.tri\n", argv[0] );
      
      return 1;
    }


  sprintf( fast_filename, "%s", argv[1] );
  printf("fast input file %s\n", fast_filename);

  sprintf( tri_filename, "%s", argv[2] );
  printf("tri input file %s\n", tri_filename);


  surface_primal = primal_from_FAST( fast_filename );
  NOT_NULL(surface_primal, "surface_primal NULL");

  TRY( primal_export_tri( surface_primal, tri_filename ), 
       "primal_export_tri failed in main")

  return 0;
}


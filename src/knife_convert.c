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

  if ( 3 > argc ) 
    {
      printf("usage : %s input.fgrid output.tri [ faceId ... ]\n", argv[0] );
      
      return 1;
    }


  sprintf( fast_filename, "%s", argv[1] );
  printf("fast input file %s\n", fast_filename);

  sprintf( tri_filename, "%s", argv[2] );
  printf("tri output file %s\n", tri_filename);

  surface_primal = primal_from_fast( fast_filename );
  NOT_NULL(surface_primal, "surface_primal NULL");

  if ( 3 < argc ) 
    {
      Primal subset;
      Set bcs;
      int argument;
      int bc;
      bcs = set_create(10,10);
      NOT_NULL(bcs, "Set bcs NULL");
      for ( argument = 3; argument < argc ; argument++ )
	{
	  bc = atoi(argv[argument]);
	  printf("bc %d\n",bc);
	  TRY( set_insert( bcs, bc ), 
	       "set_insert bc into bcs");
	}
      subset = primal_subset( surface_primal, bcs );
      NOT_NULL(subset, "surface_primal NULL");
      primal_free( surface_primal );
      surface_primal = subset;
    }

  TRY( primal_export_tri( surface_primal, tri_filename ), 
       "primal_export_tri failed in main")

  TRY( primal_export_fast( surface_primal, NULL ), 
       "primal_export_fast failed in main")

  TRY( primal_export_tec( surface_primal, NULL ), 
       "primal_export_tec failed in main")

  return 0;
}


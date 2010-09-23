
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

int main( int argc, char *argv[] )
{
  Primal primal;

  if ( 2 > argc ) 
    {
      printf("usage : %s input.{fgrid|tri} [ faceId ... ]\n", argv[0] );
      
      return 1;
    }

  primal = primal_from_file( argv[1] );
  TNS(primal, "primal NULL");

  if ( 2 < argc ) 
    {
      Primal subset;
      Set bcs;
      int argument;
      int bc;
      bcs = set_create(10,10);
      TNS(bcs, "Set bcs NULL");
      for ( argument = 2; argument < argc ; argument++ )
	{
	  bc = atoi(argv[argument]);
	  printf("bc %d\n",bc);
	  TSS( set_insert( bcs, bc ), 
	       "set_insert bc into bcs");
	}
      subset = primal_subset( primal, bcs );
      TNS(subset, "primal NULL");
      primal_free( primal );
      primal = subset;
    }

  TSS( primal_export_tri( primal, NULL ), 
       "primal_export_tri failed in main")

  TSS( primal_export_fast( primal, NULL ), 
       "primal_export_fast failed in main")

  return 0;
}


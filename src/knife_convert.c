
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
  char filename[1025];
  int end_of_string;
  Primal surface_primal;
  Surface surface;

  if ( 2 > argc ) 
    {
      printf("usage : %s input.{fgrid|tri} [ faceId ... ]\n", argv[0] );
      
      return 1;
    }

  if ( 0 == strncmp( argv[1], "-i", 2 ) )
    {
      sprintf( filename, "%s", argv[2] );
      TSS( primal_interrogate_tri( filename ), "interrogation");
      return 0;
    }

  sprintf( filename, "%s", argv[1] );

  end_of_string = strlen(filename);
  if( strcmp(&filename[end_of_string-3],"tri") == 0 ) {
    printf("tri input file %s\n", filename);
    surface_primal = primal_from_tri( filename );
  } else if( strcmp(&filename[end_of_string-3],"rid") == 0 ) {
    printf("fast input file %s\n", filename);
    surface_primal = primal_from_fast( filename );
  } else {
    printf("input file name extension unknown %s\n", filename);
    return 1;
  }
  
  TNS(surface_primal, "surface_primal NULL");

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
      subset = primal_subset( surface_primal, bcs );
      TNS(subset, "surface_primal NULL");
      primal_free( surface_primal );
      surface_primal = subset;
    }

  TSS( primal_export_tri( surface_primal, NULL ), 
       "primal_export_tri failed in main")

  TSS( primal_export_fast( surface_primal, NULL ), 
       "primal_export_fast failed in main")

  TSS( primal_export_tec( surface_primal, NULL ), 
       "primal_export_tec failed in main")

  TSS( primal_export_vtk( surface_primal, NULL ), 
       "primal_export_vtk failed in main");

  if( strcmp(filename,"inviscid.fgrid") == 0 )
    {
      TSS( primal_apply_massoud( surface_primal, 
				 "Rubberize/model.tec.1.sd1", TRUE ), 
	   "primal_apply_massoud failed in main");

      TSS( primal_export_tec( surface_primal, "pert.t" ), 
	   "primal_export_tec failed in main");
      TSS( primal_export_vtk( surface_primal, "pert.vtu" ), 
	   "primal_export_vtk failed in main");
    }

  surface = surface_from( surface_primal, NULL, TRUE );
  TNS(surface, "surface NULL");

  TSS( surface_export_tec( surface, "massoud.t" ), "surface_export_tec" );

  return 0;
}


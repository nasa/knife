
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

int main( int argc, char *argv[] )
{
  char *input;
  char output[1024];
  char *output_ext;
  Primal primal;

  if ( 2 > argc ) 
    {
      printf("usage : %s input.{fgrid|tri}\n", argv[0] );
      
      return 1;
    }

  input = argv[1];
  primal = primal_from_file( input );
  TNS(primal, "primal NULL");

  sprintf(output, "%s", input);
  output_ext = strrchr(output, '.');
  if ( NULL != output_ext )
    {
      sprintf(output_ext, ".t" );
      TSS( primal_export_tec( primal, output ), 
	   "primal_export_tec failed in main");
      sprintf(output_ext, ".vtk" );
      TSS( primal_export_vtk( primal, output ), 
	   "primal_export_vtk failed in main");
    }
  else
    {
      TSS( primal_export_tec( primal, NULL ), 
	   "primal_export_tec failed in main");
      TSS( primal_export_vtk( primal, NULL ), 
	   "primal_export_vtk failed in main");
    }

  primal_free(primal);

  return 0;
}


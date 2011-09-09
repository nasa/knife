
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

int main( int argc, char *argv[] )
{
  Primal volume_primal, surface_primal;
  Surface surface;
  Domain domain;
  int *required;

  if ( 3 > argc ) 
    {
      printf("usage : %s volume.fgrid cutting-surface.{fgrid|tri}\n", argv[0] );
      return 1;
    }

  volume_primal = primal_from_file( argv[1] );
  TNS(volume_primal, "primal volume NULL");

  surface_primal = primal_from_file( argv[2] );
  TNS(surface_primal, "primal surface NULL");

  surface = surface_from( surface_primal, NULL, FALSE );

  TSS( primal_establish_all( volume_primal ), "primal_establish_all" );

  domain = domain_create( volume_primal, surface );
  TNS(domain, "domain NULL");

  required = (int *)malloc( primal_nnode(volume_primal) * sizeof(int) );
  TNS(required, "required NULL");

  TSS( domain_required_local_dual( domain, required ), 
       "domain_required_local_dual" );

  domain_free(domain);
  surface_free(surface);
  primal_free(surface_primal);
  primal_free(volume_primal);

  return 0;
}


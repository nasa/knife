
/* Copyright 2007 United States Government as represented by the
 * Administrator of the National Aeronautics and Space
 * Administration. No copyright is claimed in the United States under
 * Title 17, U.S. Code.  All Other Rights Reserved.
 *
 * The knife platform is licensed under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
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
      printf("\n");
      printf("Copyright 2007 United States Government as represented by the\n");
      printf("Administrator of the National Aeronautics and Space\n");
      printf("Administration. No copyright is claimed in the United States under\n");
      printf("Title 17, U.S. Code.  All Other Rights Reserved.\n");
      printf("\n");
      printf("The knife platform is licensed under the Apache License, Version\n");
      printf("2.0 (the \"License\"); you may not use this file except in compliance\n");
      printf("with the License. You may obtain a copy of the License at\n");
      printf("http://www.apache.org/licenses/LICENSE-2.0.\n");
      printf("\n");
      printf("Unless required by applicable law or agreed to in writing, software\n");
      printf("distributed under the License is distributed on an \"AS IS\" BASIS,\n");
      printf("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or\n");
      printf("implied. See the License for the specific language governing\n");
      printf("permissions and limitations under the License.\n");

      return 1;
    }

  printf( "volume primal %s\n", argv[1]);
  volume_primal = primal_from_file( argv[1] );
  TNS(volume_primal, "primal volume NULL");

  printf( "surface primal %s\n", argv[2]);
  surface_primal = primal_from_file( argv[2] );
  TNS(surface_primal, "primal surface NULL");

  printf( "surface\n");
  surface = surface_from( surface_primal, NULL, FALSE );
  TSS( primal_establish_all( volume_primal ), "primal_establish_all" );

  printf( "domain\n");
  domain = domain_create( volume_primal, surface );
  TNS(domain, "domain NULL");

  printf( "required\n");
  required = (int *)malloc( primal_nnode(volume_primal) * sizeof(int) );
  TNS(required, "required NULL");

  TSS( domain_required_local_dual( domain, required ), 
       "domain_required_local_dual" );

  printf( "create duals\n");
  TSS( domain_create_dual( domain, required ), "domain_required_local_dual" );

  printf( "subtract\n");
  TSS( domain_boolean_subtract( domain ), "boolean subtract" );

  printf( "free\n");
  free( required );
  domain_free(domain);
  surface_free(surface);
  primal_free(surface_primal);
  primal_free(volume_primal);

  printf( "done.\n");
  return 0;
}


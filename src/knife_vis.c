
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

int main( int argc, char *argv[] )
{
  char *input;
  char output[1024];
  char *output_ext;
  Primal primal;

  if ( 2 > argc ) 
    {
      printf("usage : %s input.{fgrid|tri}\n", argv[0] );
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


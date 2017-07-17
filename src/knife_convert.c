
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

int main( int argc, char *argv[] )
{
  Primal primal;

  if ( 2 > argc ) 
    {
      printf("usage : %s input.{fgrid|tri} [ faceId ... ]\n", argv[0] );
      
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


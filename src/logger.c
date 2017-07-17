
/* a logging facility */

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

#include <time.h>

#include "logger.h"

static FILE *file = NULL;
static time_t start_time;
static clock_t last_clock_time;
static int verbosity = 0;

KNIFE_STATUS logger_verbosity( int verbosity_arg )
{
  verbosity = verbosity_arg;
  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_partition( int part_id )
{
  char filename[1024];

  sprintf( filename, "knife_log_%05d", part_id );

  TSS( logger_filename( filename ), "set file name" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_filename( char *filename )
{
  FILE *file_pointer;

  file_pointer = fopen(filename, "wt" );
  TNS( file_pointer, "could not open file");

  TSS( logger_file_pointer( file_pointer ), "set file pointer" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_file_pointer( FILE *file_pointer )
{
  file = file_pointer;

  start_time = time( (time_t *)NULL );
  last_clock_time = clock(  );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_message( int level, char *message )
{
  time_t time_now;
  clock_t clock_time;

  if ( (verbosity >= level) && (NULL != file)  )
    {
      time_now = time( (time_t *)NULL );
      clock_time = clock(  );      
      fprintf( file, "%10.4f %10.4f %s\n", 
	       difftime( time_now, start_time ), 
	       ((double)(clock_time-last_clock_time))/((double)CLOCKS_PER_SEC),
	       message );
      fflush( file );
      last_clock_time = clock_time;
    } 

  return KNIFE_SUCCESS;
}



/* a logging facility */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <time.h>

#include "logger.h"

static FILE *file = NULL;
static time_t start_time;
static time_t last_time;

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

  time( &start_time );
  last_time = start_time;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_message( char *message )
{
  time_t time_now;

  if ( NULL != file )
    {
      time( &time_now );      
      fprintf( file, "%8.4f %8.4f %s\n", 
	       difftime( time_now, start_time ), 
	       difftime( time_now, last_time ), 
	       message );
      fflush( file );
      last_time = time_now;
    } 

  return KNIFE_SUCCESS;
}


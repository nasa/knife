
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

KNIFE_STATUS logger_filename( char *filename )
{
  static FILE *file_pointer;

  file_pointer = fopen(filename, "wt" );
  TNS( file_pointer, "could not open file");

  TSS( logger_file_pointer( file_pointer ), "set file pointer" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_file_pointer( FILE *file_pointer )
{
  file = file_pointer;

  time( &start_time );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS logger_message( char *message )
{
  time_t time_now;
  if ( NULL != file )
    {
      time( &time_now );      
      fprintf( file, "%8.4f %s", difftime( time_now, start_time ), message );
    } 
  return KNIFE_SUCCESS;
}


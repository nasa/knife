
/* a logging facility */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>

#include "knife_definitions.h"

BEGIN_C_DECLORATION

KNIFE_STATUS logger_filename( char *filename );
KNIFE_STATUS logger_file_pointer( FILE *file_pointer );
KNIFE_STATUS logger_message( char *message );

END_C_DECLORATION

#endif /* LOGGER_H */

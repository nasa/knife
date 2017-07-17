
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

#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>

#include "knife_definitions.h"

BEGIN_C_DECLORATION

KNIFE_STATUS logger_verbosity( int verbosity );
KNIFE_STATUS logger_partition( int part_id );
KNIFE_STATUS logger_filename( char *filename );
KNIFE_STATUS logger_file_pointer( FILE *file_pointer );
KNIFE_STATUS logger_message( int level, char *message );

END_C_DECLORATION

#endif /* LOGGER_H */

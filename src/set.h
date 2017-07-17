
/* growable set (list) of ints */

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

#ifndef SET_H
#define SET_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct SetStruct SetStruct;
typedef SetStruct * Set;

struct SetStruct {
  int actual, allocated, chunk;
  int *data;
};

Set set_create( int guess, int chunk );
void set_free( Set );

#define set_size(set) (NULL==(set)?0:(set)->actual)

KNIFE_STATUS set_insert( Set, int );
KNIFE_STATUS set_remove( Set, int );

#define set_item( set,indx ) \
  (((indx)>=0 && (indx) < (set)->actual)?(set)->data[(indx)]:EMPTY)

KnifeBool set_contains( Set, int target );
int set_index_of( Set, int );

KNIFE_STATUS set_echo( Set );

END_C_DECLORATION

#endif /* SET_H */

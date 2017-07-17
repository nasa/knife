
/* growable list of objects (void pointers) */

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

#ifndef ARRAY_H
#define ARRAY_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef void * ArrayItem;
typedef struct ArrayStruct ArrayStruct;
typedef ArrayStruct * Array;

struct ArrayStruct {
  int actual, allocated, chunk;
  ArrayItem *data;
};

Array array_from( ArrayItem *data, int size );
Array array_create( int guess, int chunk );
void array_free( Array );

#define array_size(array) (NULL==(array)?0:(array)->actual)

KNIFE_STATUS array_add( Array, ArrayItem );
KNIFE_STATUS array_add_uniquely( Array, ArrayItem );
KNIFE_STATUS array_remove( Array, ArrayItem );

#define array_item( array,indx ) \
  (((indx)>=0 && (indx) < (array)->actual)?(array)->data[(indx)]:NULL)

KnifeBool array_contains_item( Array, ArrayItem );
int array_index_of( Array, ArrayItem );

END_C_DECLORATION

#endif /* ARRAY_H */

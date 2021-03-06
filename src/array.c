
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

#include <stdlib.h>
#include <stdio.h>
#include "array.h"

Array array_from( ArrayItem *data, int size )
{
  Array array;
  
  if (NULL == data ) {
    printf("%s: %d: NULL data passed to array_from\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  array = (Array) malloc( sizeof(ArrayStruct) );
  if (NULL == array) {
    printf("%s: %d: malloc failed in array_from\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  array->actual     = size;
  array->allocated  = size;
  array->chunk      = 1000;
       
  array->data = data;

  return array;
}

Array array_create( int guess, int chunk )
{
  Array array;
  
  array = (Array)malloc( sizeof(ArrayStruct) );
  if (NULL == array) {
    printf("%s: %d: malloc failed in array_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  array->actual     = 0;
  array->allocated  = MAX(guess,1);
  array->chunk      = MAX(chunk,1);
       
  array->data = NULL;
  
  return array;
}

void array_free( Array array )
{
  if ( NULL == array ) return;
  if ( NULL != array->data ) free( array->data );
  free( array );
}

KNIFE_STATUS array_add( Array array, ArrayItem item )
{
  ArrayItem *new_data;

  if ( NULL == array->data )
    {
      array->data = (ArrayItem *) malloc( array->allocated * 
					  sizeof(ArrayItem) );
      if (NULL == array->data) {
	printf("%s: %d: malloc failed in array_add\n",
	       __FILE__,__LINE__);
	return KNIFE_MEMORY; 
      }
    }

  if (array->actual >= array->allocated)
    {
      array->allocated += array->chunk;
      new_data = (ArrayItem *) realloc( array->data, 
					array->allocated * sizeof(ArrayItem) );
      if (NULL == new_data) {
	printf("%s: %d: realloc failed in array_add\n",
	       __FILE__,__LINE__);
	array->allocated -= array->chunk;
	return KNIFE_MEMORY; 
      }
      array->data = new_data;
    }

  array->data[array->actual] = item;
  array->actual++;
  return KNIFE_SUCCESS;
}

KNIFE_STATUS array_add_uniquely( Array array, ArrayItem item )
{

  if ( array_contains_item( array, item ) ) return KNIFE_SUCCESS;
  return array_add( array, item );
}

KNIFE_STATUS array_remove( Array array, ArrayItem item )
{
  int indx;
  int sweep;
  KnifeBool found;

  if ( NULL == item ) return KNIFE_NULL;

  found = FALSE;
  for ( indx = 0 ; indx < array_size(array) ; indx++ )
    if ( item == array_item( array,indx ) ) 
      {
	found = TRUE;
	for ( sweep = indx+1 ; sweep < array_size(array) ; sweep++ )
	  array->data[sweep-1] = array->data[sweep];
	array->actual--;
      }
  
  return ( found ? KNIFE_SUCCESS : KNIFE_NOT_FOUND );
}

KnifeBool array_contains_item( Array array, ArrayItem target )
{
  return (KnifeBool)( EMPTY != array_index_of( array, target ) ? TRUE : FALSE );
}

int array_index_of( Array array, ArrayItem target )
{
  int indx;

  for ( indx = 0 ; indx < array_size(array) ; indx++ )
    if ( target == array_item(array, indx) ) return indx;
 
  return EMPTY;
}


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

#include <stdlib.h>
#include <stdio.h>
#include "set.h"

Set set_create( int guess, int chunk )
{
  Set set;
  
  set = (Set)malloc( sizeof(SetStruct) );
  if (NULL == set) {
    printf("%s: %d: malloc failed in set_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  set->actual     = 0;
  set->allocated  = MAX(guess,1);
  set->chunk      = MAX(chunk,1);
       
  set->data = NULL;
  
  return set;
}

void set_free( Set set )
{
  if ( NULL == set ) return;
  if ( NULL != set->data ) free( set->data );
  free( set );
}

KNIFE_STATUS set_insert( Set set, int item )
{
  int *new_data;

  if ( set_contains( set, item ) ) return KNIFE_SUCCESS;

  if ( NULL == set->data )
    {
      set->data = (int *) malloc( set->allocated * 
				  sizeof(int) );
      if (NULL == set->data) {
	printf("%s: %d: malloc failed in set_insert\n",
	       __FILE__,__LINE__);
	return KNIFE_MEMORY; 
      }
    }

  if (set->actual >= set->allocated)
    {
      set->allocated += set->chunk;
      new_data = (int *) realloc( set->data, 
				  set->allocated * sizeof(int) );
      if (NULL == new_data) {
	printf("%s: %d: realloc failed in set_add\n",
	       __FILE__,__LINE__);
	set->allocated -= set->chunk;
	return KNIFE_MEMORY;
      }
      set->data = new_data;
    }

  set->data[set->actual] = item;
  set->actual++;
  return KNIFE_SUCCESS;
}

KNIFE_STATUS set_remove( Set set, int item )
{
  int indx;
  int sweep;
  KnifeBool found;

  found = FALSE;
  for ( indx = 0 ; indx < set_size(set) ; indx++ )
    if ( item == set_item( set, indx ) ) 
      {
	found = TRUE;
	for ( sweep = indx+1 ; sweep < set_size(set) ; sweep++ )
	  set->data[sweep-1] = set->data[sweep];
	set->actual--;
      }
  
  return ( found ? KNIFE_SUCCESS : KNIFE_NOT_FOUND );
}

KnifeBool set_contains( Set set, int target )
{
  return (KnifeBool)( EMPTY != set_index_of( set, target ) ? TRUE : FALSE );
}

int set_index_of( Set set, int target )
{
  int indx;

  for ( indx = 0 ; indx < set_size(set) ; indx++ )
    if ( target == set_item(set, indx) ) return indx;
 
  return EMPTY;
}

KNIFE_STATUS set_echo( Set set )
{
  int indx;

  if ( NULL == set) return KNIFE_NULL;

  for ( indx = 0 ; indx < set_size(set) ; indx++ )
    printf(" %6d: %6d\n", indx, set_item(set, indx) );
 
  return KNIFE_SUCCESS;
}

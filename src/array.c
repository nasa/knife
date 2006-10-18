
/* growable list of objects (void pointers) */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
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
  free( array->data );
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

KnifeBool array_contains_int( Array array, int target )
{
  int indx;

  for ( indx = 0 ; indx < array_size(array) ; indx++ )
    if (target == *((int *)array_item(array, indx))) return TRUE;
 
  return FALSE;
}

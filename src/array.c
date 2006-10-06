
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
       
  array->data = (ArrayItem *) malloc( array->allocated * sizeof(ArrayItem) );
  if (NULL == array->data) {
    printf("%s: %d: malloc failed in array_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

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

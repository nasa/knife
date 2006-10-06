
/* growable list of objects (void pointers) */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef ARRAY_H
#define ARRAY_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct ArrayStruct ArrayStruct;
struct ArrayStruct {
  int size, allocated, chunk;
  void **data
};
typedef ArrayStruct * Array;

Array array_create( int guess, int chunk );
void array_free( Array );

#define array_size(array) (array->size)

KNIFE_STATUS array_add( Array, void * );
void * array_item( Array, int index );

END_C_DECLORATION

#endif /* ARRAY_H */

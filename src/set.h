
/* growable set (list) of ints */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
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

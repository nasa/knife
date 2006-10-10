
/* three subnodes and three sides */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SUBTRI_H
#define SUBTRI_H

#include "knife_definitions.h"
#include "subnode.h"

BEGIN_C_DECLORATION

typedef struct SubtriStruct SubtriStruct;
typedef SubtriStruct * Subtri;

typedef void * Side;

struct SubtriStruct {
  Subnode n0, n1, n2;
  Side s0, s1, s2;
};

Subtri subtri_create( Subnode n0, Subnode n1, Subnode n2,
		      Side s0, Side s1, Side s2 );
void subtri_free( Subtri );

END_C_DECLORATION

#endif /* SUBTRI_H */

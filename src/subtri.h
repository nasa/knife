
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

BEGIN_C_DECLORATION
typedef struct SubtriStruct SubtriStruct;
typedef SubtriStruct * Subtri;
END_C_DECLORATION

#include "subnode.h"

BEGIN_C_DECLORATION

struct SubtriStruct {
  Subnode n0, n1, n2;
};

Subtri subtri_create( Subnode n0, Subnode n1, Subnode n2 );
void subtri_free( Subtri );

#define subtri_n0(subtri) ((subtri)->n0)
#define subtri_n1(subtri) ((subtri)->n1)
#define subtri_n2(subtri) ((subtri)->n2)

KNIFE_STATUS subtri_bary( Subtri, Subnode, double *bary );

END_C_DECLORATION

#endif /* SUBTRI_H */


/* volume defined by a watertight collection of triangles */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef POLY_H
#define POLY_H

#include "knife_definitions.h"
#include "array.h"

BEGIN_C_DECLORATION

typedef struct PolyStruct PolyStruct;
struct PolyStruct {
  Array triangles;
};
typedef PolyStruct * Poly;

Poly poly_create( void );
void poly_free( Poly );

END_C_DECLORATION

#endif /* POLY_H */

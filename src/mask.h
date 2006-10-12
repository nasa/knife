
/* a triangle wrapper that extends to include orientation and active subtri */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef MASK_H
#define MASK_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct MaskStruct MaskStruct;
typedef MaskStruct * Mask;
END_C_DECLORATION

#include "triangle.h"

BEGIN_C_DECLORATION
struct MaskStruct {
  Triangle triangle;
  KnifeBool inward_pointing_normal;
  KnifeBool *active;
};

Mask mask_create( Triangle, KnifeBool inward_pointing_normal );
void mask_free( Mask );

#define mask_triangle( mask )((mask)->triangle)

END_C_DECLORATION

#endif /* MASK_H */

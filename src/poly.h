
/* volume defined by a watertight collection of masks */

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

#include <stdlib.h>
#include <stdio.h>
#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct PolyStruct PolyStruct;
typedef PolyStruct * Poly;
END_C_DECLORATION

#include "array.h"
#include "triangle.h"
#include "mask.h"

BEGIN_C_DECLORATION

struct PolyStruct {
  Array mask;
};

Poly poly_create( void );
KNIFE_STATUS poly_initialize( Poly );
void poly_free( Poly );

KNIFE_STATUS poly_add_triangle( Poly, Triangle, 
				KnifeBool inward_pointing_normal );

#define poly_add_mask( poly, new_mask )		\
  array_add( (poly)->mask, (ArrayItem)(new_mask) )
#define poly_nmask( poly )		\
  array_size( (poly)->mask )
#define poly_mask( poly, mask_index )		\
  ((Mask)array_item( (poly)->mask, (mask_index) ))

KNIFE_STATUS poly_tecplot_zone( Poly, FILE * );


END_C_DECLORATION

#endif /* POLY_H */

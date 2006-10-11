
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

BEGIN_C_DECLORATION
typedef struct PolyStruct PolyStruct;
typedef PolyStruct * Poly;
END_C_DECLORATION

#include "array.h"

BEGIN_C_DECLORATION

struct PolyStruct {
  Array triangle;
};

Poly poly_create( void );
KNIFE_STATUS poly_initialize( Poly );
void poly_free( Poly );

#define poly_add_triangle( poly, new_triangle )		\
  array_add( (poly)->triangle, (ArrayItem)(new_triangle) )
#define poly_ntriangle( poly )		\
  array_size( (poly)->triangle )
#define poly_triangle( poly, triangle_index )		\
  ((Triangle)array_item( (poly)->triangle, (triangle_index) ))



END_C_DECLORATION

#endif /* POLY_H */


/* collection of segments and triangles defining a surface */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef SURFACE_H
#define SURFACE_H

#include "knife_definitions.h"
#include "array.h"
#include "primal.h"

BEGIN_C_DECLORATION

typedef struct SurfaceStruct SurfaceStruct;
struct SurfaceStruct {
  Array segments;
  Array triangles;
};
typedef SurfaceStruct * Surface;

Surface surface_from( Primal, Array of_bcs );

Surface surface_create( void );
void surface_free( Surface );

END_C_DECLORATION

#endif /* SURFACE_H */

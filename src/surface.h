
/* collection of segments and triangles to define a surface */

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
#include "primal.h"
#include "segment.h"
#include "triangle.h"

BEGIN_C_DECLORATION

typedef struct SurfaceStruct SurfaceStruct;
struct SurfaceStruct {
  Array segments;
  Array triangles;
};
typedef SurfaceStruct * Surface;

Surface surface_from( Primal primal );

END_C_DECLORATION

#endif /* SURFACE_H */

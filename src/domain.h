
/* domain for PDE solvers */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef DOMAIN_H
#define DOMAIN_H

#include "knife_definitions.h"
#include "primal.h"
#include "surface.h"
#include "poly.h"
#include "triangle.h"

BEGIN_C_DECLORATION

typedef struct DomainStruct DomainStruct;
struct DomainStruct {

  Primal primal;

  Surface surface;

  int npoly;
  PolyStruct *poly;

  int ntriangle;
  TriangleStruct *triangles;

};
typedef DomainStruct * Domain;

#define domain_test_malloc(ptr,fcn)		       \
  if (NULL == (ptr)) {				       \
    printf("%s: %d: malloc failed in %s\n",	       \
	   __FILE__,__LINE__,(fcn));		       \
    return NULL;				       \
  }

Domain domain_create( Primal, Surface );

void domain_free( Domain );

END_C_DECLORATION

#endif /* DOMAIN_H */

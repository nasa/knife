
/* storage for a primal tetrahedral grid */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#ifndef PRIMAL_H
#define PRIMAL_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION

typedef struct Primal Primal;
struct Primal {

  int nnode;
  double *xyz;

  int ncell;
  int *c2n;

  int nface;
  int *f2n;

  Adj *cellAdj;
  Adj *faceAdj;

  int nedge;
  int *c2e;
  int *e2n;

  int ntri;
  int *c2t;
  int *t2n;

};

Primal *primal_create( int nnode, int ncell, int nface );

/* Primal *primal_from_FAST_file( char *filename ); */

Primal *primal_free( Primal * );

END_C_DECLORATION

#endif /* PRIMAL_H */

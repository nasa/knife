
/* storage for a primal tetrahedral grid */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <stdlib.h>
#include <stdio.h>
#include "primal.h"


Primal *primal_create(int nnode, int ncell, int nface);
{
  Primal *primal

    adj = malloc( sizeof(Adj) );

  return primal;
}

void primal_free( Primal *primal )
{
  if ( NULL == primal ) return;

  free( primal->xyz );
  free( primal->c2n );
  free( primal->f2n );

  adj_free( primal->cell_adj );
  adj_free( primal->face_adj );

  if ( NULL != primal->c2e ) free( primal->c2e );
  if ( NULL != primal->e2n ) free( primal->e2n );

  if ( NULL != primal->c2t ) free( primal->c2t );
  if ( NULL != primal->t2n ) free( primal->t2n );

  free( primal );
}

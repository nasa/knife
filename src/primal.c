
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

#define primal_test_malloc(ptr,fcn)		       \
  if (NULL == (void)(ptr)) {			       \
    printf("%s: %d: malloc failed in %s\n",	       \
	   __FILE__,__LINE__,(fcn));		       \
    return NULL;				       \
  }

Primal primal_create(int nnode, int ncell, int nface);
{
  Primal primal;

  primal = malloc( sizeof(Adj) );
  primal_test_malloc(primal,"primal_create");

  primal->nnode = nnode;
  primal->xyz = (double *)malloc(3 * MAX(primal->nnode,1) * sizeof(double));
  primal_test_malloc(primal->xyz,"primal_create xyz");

  primal->ncell = ncell;
  primal->c2n = (int *)malloc(4 * MAX(grid->ncell,1) * sizeof(int));
  primal_test_malloc(primal->c2n,"primal_create c2n");

  primal->nface = nface;
  primal->n2f = (int *)malloc(4 * MAX(grid->nface,1) * sizeof(int));
  primal_test_malloc(primal->n2f,"primal_create f2n");

  primal->cell_adj = adj_create( primal->nnode, primal->ncell, 1000 );
  primal->face_adj = adj_create( primal->nnode, primal->nface, 1000 );

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

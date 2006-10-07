
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

Primal primal_create(int nnode, int nface, int ncell)
{
  Primal primal;

  primal = (Primal) malloc( sizeof(PrimalStruct) );
  primal_test_malloc(primal,"primal_create primal");

  primal->nnode = nnode;
  primal->xyz = (double *)malloc(3 * MAX(primal->nnode,1) * sizeof(double));
  primal_test_malloc(primal->xyz,"primal_create xyz");

  primal->nface = nface;
  primal->f2n = (int *)malloc(4 * MAX(primal->nface,1) * sizeof(int));
  primal_test_malloc(primal->f2n,"primal_create f2n");

  primal->ncell = ncell;
  primal->c2n = (int *)malloc(4 * MAX(primal->ncell,1) * sizeof(int));
  primal_test_malloc(primal->c2n,"primal_create c2n");

  primal->face_adj = adj_create( primal->nnode, primal->nface, 1000 );
  primal->cell_adj = adj_create( primal->nnode, primal->ncell, 1000 );

  primal->nedge = EMPTY;
  primal->c2e = NULL;
  primal->e2n = NULL;

  primal->ntri = EMPTY;
  primal->c2t = NULL;
  primal->t2n = NULL;

  return primal;
}

Primal primal_from_FAST( char *filename )
{
  Primal primal;
  int nnode, nface, ncell;
  int i;
  FILE *file;

  file = fopen(filename,"r");
  fscanf(file,"%d %d %d",&nnode,&nface,&ncell);
  primal = primal_create( nnode, nface, ncell );

  for( i=0; i<nnode ; i++ ) fscanf(file,"%lf",&(primal->xyz[0+3*i]));
  for( i=0; i<nnode ; i++ ) fscanf(file,"%lf",&(primal->xyz[1+3*i]));
  for( i=0; i<nnode ; i++ ) fscanf(file,"%lf",&(primal->xyz[2+3*i]));

  for( i=0; i<nface ; i++ ) {
    fscanf(file,"%d",&(primal->f2n[0+4*i]));
    fscanf(file,"%d",&(primal->f2n[1+4*i]));
    fscanf(file,"%d",&(primal->f2n[2+4*i]));
    primal->f2n[0+4*i]--;
    primal->f2n[1+4*i]--;
    primal->f2n[2+4*i]--;
  }

  for( i=0; i<nface ; i++ ) {
    fscanf(file,"%d",&(primal->f2n[3+4*i]));
  }

  for( i=0; i<ncell ; i++ ) {
    fscanf(file,"%d",&(primal->c2n[0+4*i]));
    fscanf(file,"%d",&(primal->c2n[1+4*i]));
    fscanf(file,"%d",&(primal->c2n[2+4*i]));
    fscanf(file,"%d",&(primal->c2n[3+4*i]));
    primal->c2n[0+4*i]--;
    primal->c2n[1+4*i]--;
    primal->c2n[2+4*i]--;
    primal->c2n[3+4*i]--;
  }

  fclose(file);

  return primal;
}


void primal_free( Primal primal )
{
  if ( NULL == primal ) return;

  free( primal->xyz );
  free( primal->f2n );
  free( primal->c2n );

  adj_free( primal->face_adj );
  adj_free( primal->cell_adj );

  if ( NULL != primal->c2e ) free( primal->c2e );
  if ( NULL != primal->e2n ) free( primal->e2n );

  if ( NULL != primal->c2t ) free( primal->c2t );
  if ( NULL != primal->t2n ) free( primal->t2n );

  free( primal );
}

KNIFE_STATUS primal_face( Primal primal, int face_index, int *face)
{
  if (face_index < 0 || face_index >= primal_nface(primal) ) 
    return KNIFE_ARRAY_BOUND;

  face[0] = primal->f2n[0+4*face_index];
  face[1] = primal->f2n[1+4*face_index];
  face[2] = primal->f2n[2+4*face_index];
  face[3] = primal->f2n[3+4*face_index];

  return KNIFE_SUCCESS;
}

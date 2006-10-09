
/* domain for PDE solvers */

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
#include "domain.h"
#include "cut.h"

Domain domain_create( Primal primal, Surface surface)
{
  Domain domain;

  domain = (Domain) malloc( sizeof(DomainStruct) );
  if (NULL == domain) {
    printf("%s: %d: malloc failed in domain_create\n",__FILE__,__LINE__);
    return NULL;
  }

  domain->primal = primal;
  domain->surface = surface;

  domain->npoly = EMPTY;
  domain->poly = NULL;

  domain->ntriangle = EMPTY;
  domain->triangle = NULL;

  return domain;
}

void domain_free( Domain domain )
{
  if ( NULL == domain ) return;
  
  if ( NULL != domain->poly ) free(domain->poly);
  if ( NULL != domain->triangle ) free(domain->triangle);

  free(domain);
}

KNIFE_STATUS domain_tetrahedral_elements( Domain domain )
{
  domain->npoly = primal_ncell(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,
		     "domain_tetrahedral_elements poly");

  domain->ntriangle = primal_ntri(domain->primal);
  domain->triangle = (TriangleStruct *)malloc( domain->ntriangle * 
					       sizeof(TriangleStruct));
  domain_test_malloc(domain->triangle,
		     "domain_tetrahedral_elements triangle");
  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_dual_elements( Domain domain )
{
  domain->npoly = primal_nnode(domain->primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->poly,"domain_dual_elements poly");
  
  domain->ntriangle = 12*primal_ncell(domain->primal) + 
                       6*primal_nface(domain->primal);
  domain->triangle = (TriangleStruct *)malloc( domain->ntriangle * 
					       sizeof(TriangleStruct));
  domain_test_malloc(domain->triangle,"domain_dual_elements triangle");
  return (KNIFE_SUCCESS);
}

KNIFE_STATUS domain_boolean_subtract( Domain domain )
{
  int triangle_index;
  int i;

  for ( triangle_index = 0;
	triangle_index < domain->ntriangle; 
	triangle_index++)
    {
      for (i=0;i<surface_ntriangle(domain->surface);i++)
	{
	  cut_between( domain_triangle(domain,triangle_index),
		       surface_triangle(domain->surface,i) );
	}
    }

  return (KNIFE_SUCCESS);
}


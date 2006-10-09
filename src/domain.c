
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

Domain domain_create( Primal primal, Surface surface)
{
  Domain domain;

  domain = (Domain) malloc( sizeof(DomainStruct) );
  domain_test_malloc(domain,"domain_create domain");

  domain->primal = primal;
  domain->surface = surface;

  domain->npoly = primal_ncell(primal);
  domain->poly = (PolyStruct *)malloc(domain->npoly * sizeof(PolyStruct));
  domain_test_malloc(domain->npoly,"domain_create poly");

  domain->ntriangle = primal_ncell(primal);
  domain->triangles = (TriangleStruct *)malloc( domain->ntriangle * 
					      sizeof(TriangleStruct));
  domain_test_malloc(domain->ntriangle,"domain_create triangles");

  return domain;
}

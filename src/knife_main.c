/* main driver program for knife package */

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
#include "knife_definitions.h"

int main( int argc, char *argv[] )
{
  int i;
  for(i=0;i<argc;i++) printf("%s\n",argv[i]);
  return 0;
}


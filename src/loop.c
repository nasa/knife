
/* loop of subtri sides used for provable boundary recovery */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include "loop.h"

static int loop_tecplot_frame = 0;

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      loop_tecplot(loop);                                     \
      return code;					      \
    }							      \
  }

#define NOT_NULL(pointer,msg)				      \
  if (NULL == (pointer)) {				      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

Loop loop_create( void )
{
  Loop loop;
  
  loop = (Loop) malloc( sizeof(LoopStruct) );
  if (NULL == loop) {
    printf("%s: %d: malloc failed in loop_create self\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  loop->nside = 0;
  loop->allocated = 10;

  loop->side = (Subnode *) malloc( 2 * loop->allocated * sizeof(Subnode) );
  if (NULL == loop->side) {
    printf("%s: %d: malloc failed in loop_create side\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  loop->node0 = NULL;
  loop->node1 = NULL;

  loop->subtri = NULL;

  return loop;
}

void loop_free( Loop loop )
{
  if ( NULL == loop ) return;
  if ( NULL != loop->side ) free(loop->side);
  if ( NULL != loop->subtri ) array_free(loop->subtri);
  free( loop );
}

KNIFE_STATUS loop_set_frame( int frame )
{
  loop_tecplot_frame = frame;
  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_add_side( Loop loop, Subnode node0, Subnode node1 )
{
  Subnode *new_side;

  if (loop->nside >= loop->allocated)
    {
      loop->allocated += 10;
      new_side = (Subnode *) realloc( loop->side, 
				      2 * loop->allocated * sizeof(Subnode) );
      if (NULL == new_side) {
	printf("%s: %d: realloc failed in loop_add_side\n",
	       __FILE__,__LINE__);
	loop->allocated -= 10;
	return KNIFE_MEMORY;
      }
      loop->side = new_side;
    }

  loop->side[0+2*loop->nside] = node0;
  loop->side[1+2*loop->nside] = node1;

  loop->nside++;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_remove_side( Loop loop, Subnode node0, Subnode node1 )
{
  int existing_side;
  int sweep;
  KnifeBool found;

  found = FALSE;
  for ( existing_side = 0; existing_side < loop_nside(loop) ; existing_side++ )
    if ( node0 == loop->side[0+2*existing_side] &&
	 node1 == loop->side[1+2*existing_side] ) 
      {
	found = TRUE;
	for ( sweep = existing_side+1 ; sweep < loop_nside(loop) ; sweep++ )
	  {
	    loop->side[0+2*(sweep-1)] = loop->side[0+2*sweep];
	    loop->side[1+2*(sweep-1)] = loop->side[1+2*sweep];
	  }
	loop->nside--;
      }
  
  return ( found ? KNIFE_SUCCESS : KNIFE_NOT_FOUND );
}

KNIFE_STATUS loop_add_to_front( Loop loop, Subnode node0, Subnode node1 )
{
  KNIFE_STATUS remove_status;
  
  remove_status = loop_remove_side( loop, node1, node0 );
  if (KNIFE_NOT_FOUND == remove_status)
    {
      TRY( loop_add_side( loop, node0, node1 ), "add side");
    }
  else
    {
      TRY( remove_status, "remove side");
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_add_subtri( Loop loop, Subtri subtri )
{
  if ( NULL == loop->subtri ) loop->subtri = array_create(10,10);

  array_add(loop->subtri, (ArrayItem)subtri);
  TRY( loop_add_to_front(loop, subtri_n1(subtri), subtri_n2(subtri)), "fnt0");
  TRY( loop_add_to_front(loop, subtri_n2(subtri), subtri_n0(subtri)), "fnt1");
  TRY( loop_add_to_front(loop, subtri_n0(subtri), subtri_n1(subtri)), "fnt2");
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_subnode_after( Loop loop, Subnode node0, Subnode *node1)
{
  int side_index;

  for ( side_index = 0 ; side_index < loop_nside(loop) ; side_index++ )
    if ( loop->side[0+2*side_index] == node0 )
      {
	*node1 = loop->side[1+2*side_index];
	return KNIFE_SUCCESS;
      }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS loop_split( Loop old_loop, Subnode node0, Subnode node1, 
			 Loop *new_loop )
{
  Subnode move0, move1;
  Loop loop;

  loop = old_loop; /* for tecplot in TRY( ) macro */

  *new_loop = loop_create(  );

  NOT_NULL( *new_loop, "new loop creation in split" );

  move0 = node0;
  while (move0 != node1)
    {
      TRY( loop_subnode_after( old_loop, move0, &move1 ), "node after, split");
      TRY( loop_remove_side(   old_loop, move0,  move1 ), "remove move" );
      TRY( loop_add_side(     *new_loop, move0,  move1 ), "add move" );
      move0 = move1;
    }

  TRY( loop_add_to_front( old_loop, node0, node1 ), "old_loop add hard side" );
  old_loop->node0 = node0;
  old_loop->node1 = node1;
  TRY( loop_add_to_front(*new_loop, node1, node0 ), "new_loop add hard side" );
  (*new_loop)->node0 = node1;
  (*new_loop)->node1 = node0;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_three_subnodes( Loop loop, 
				  Subnode *subnode0, 
				  Subnode *subnode1, 
				  Subnode *subnode2  )
{
  int side_index;

  if ( 3 != loop_nside(loop) )
    {
      printf("%s: %d: loop_three_subnodes: nside %d != 0\n",__FILE__,__LINE__,
	     loop_nside(loop));
      return KNIFE_FAILURE;
    }

  (*subnode0) = loop->side[0+2*0];
  (*subnode1) = loop->side[1+2*0];

  for ( side_index = 0 ; side_index < loop_nside(loop) ; side_index++ )
    {
      if ( (*subnode0) != loop->side[0+2*side_index] &&
	   (*subnode1) != loop->side[0+2*side_index] )
	{
	  (*subnode2) = loop->side[0+2*side_index];
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_FAILURE;
}

KNIFE_STATUS loop_triangulate( Loop loop, Triangle triangle )
{
  int side_index;
  double area, best_area;
  Subnode subnode, best_subnode;
  Loop loop0, loop1;

  Subtri subtri;
  Subnode subnode0, subnode1, subnode2;

  if ( NULL == loop->node0 || NULL == loop->node1)
    {
      printf("call loop_split before loop_triangulate\n");
      return KNIFE_NULL;
    }

  if ( 0 == loop_nside(loop) ) return KNIFE_SUCCESS;

  if ( 3 > loop_nside(loop) )
    {
      printf("%s: %d: loop_triangulate: nside %d != 0\n",__FILE__,__LINE__,
	     loop_nside(loop));
      return KNIFE_FAILURE;
    }

  if ( 3 == loop_nside(loop) )
    {
      TRY( loop_three_subnodes( loop, &subnode0, &subnode1, &subnode2 ),
	   "loop_three_subnodes" );
      area = subnode_area( subnode0, subnode1, subnode2 );
      if ( area <= 0.0 )
	{
	  printf("%s: %d: loop_triangulate: %.16e\n",__FILE__,__LINE__,area);
	  loop_tecplot(loop);
	  return KNIFE_NEG_AREA;
	}
      TRY( loop_add_to_front( loop, subnode2, subnode1 ), "front 0");
      TRY( loop_add_to_front( loop, subnode0, subnode2 ), "front 1");
      TRY( loop_add_to_front( loop, subnode1, subnode0 ), "front 2");
      if ( 0 != loop_nside(loop) )
	{
	  printf("%s: %d: loop_triangulate: nside %d != 0\n",__FILE__,__LINE__,
		 loop_nside(loop));
	  loop_tecplot(loop);
	  return KNIFE_FAILURE;
	}
      subtri = subtri_create( subnode0, subnode1, subnode2 );
      NOT_NULL( subtri, "subtri creation failed" );
      TRY( triangle_add_subtri(triangle,subtri), "add new subtri");
      
      return KNIFE_SUCCESS;
    }

  best_area = 999.0;
  best_subnode = NULL;

  for ( side_index = 0 ; side_index < loop_nside(loop) ; side_index++ )
    {
      subnode = loop->side[0+2*side_index];
      if ( loop->node0 == subnode || loop->node1 == subnode ) continue;
      area = subnode_area( loop->node0, loop->node1, subnode );
      if ( area > 0.0 && area < best_area )
	{
	  best_area = area;
	  best_subnode = subnode;
	}
    }

  if ( NULL == best_subnode )
    {
      printf("%s: %d: loop_triangulate: best subnode not found\n",
	     __FILE__,__LINE__);
      loop_tecplot(loop);
      return KNIFE_NOT_FOUND;
    }

  subnode0 = loop->node0;
  subnode1 = loop->node1;

  TRY( loop_split( loop, best_subnode, subnode0, &loop0 ), "split0" );

  TRY( loop_split( loop, subnode1, best_subnode, &loop1 ), "split1" );

  TRY( loop_triangulate( loop,  triangle ), "fill loop" );
  TRY( loop_triangulate( loop0, triangle ), "fill loop0" );
  TRY( loop_triangulate( loop1, triangle ), "fill loop1" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS loop_tecplot( Loop loop )
{
  FILE *f;
  int side_index;
  int subtri_index;
  Subnode subnode;
  double xyz[3];
  double uvw[3];

  char filename[1025];

  loop_tecplot_frame++;

  sprintf(filename, "loop%08d.t",loop_tecplot_frame );
  printf("producing %s\n",filename);

  if ( 0 == loop_nside(loop) ) return KNIFE_SUCCESS;

  f = fopen(filename, "w");

  fprintf(f,"title=loop_geometry\nvariables=v,w,x,y,z\n");
  if ( 0 == loop_nside(loop))
    {
      fclose(f);
      return KNIFE_SUCCESS;
    }

  fprintf(f, "zone t=loop, i=%d, j=%d, f=fepoint, et=triangle\n",
	  2*loop_nside(loop), loop_nside(loop) );

  subnode = loop->node0;
  if ( NULL != subnode )
    {
      subnode_uvw(subnode,uvw);
      subnode_xyz(subnode,xyz);
      fprintf(f, "#node0 %.16e %.16e %.16e %.16e %.16e\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
    }

  subnode = loop->node1;
  if ( NULL != subnode )
    {
      subnode_uvw(subnode,uvw);
      subnode_xyz(subnode,xyz);
      fprintf(f, "#node1 %.16e %.16e %.16e %.16e %.16e\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
    }

  for ( side_index = 0;
	side_index < loop_nside(loop); 
	side_index++)
    {
      subnode = loop->side[0+2*side_index];
      subnode_uvw(subnode,uvw);
      subnode_xyz(subnode,xyz);
      fprintf(f, " %.16e %.16e %.16e %.16e %.16e\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );

      subnode = loop->side[1+2*side_index];
      subnode_uvw(subnode,uvw);
      subnode_xyz(subnode,xyz);
      fprintf(f, " %.16e %.16e %.16e %.16e %.16e\n",
	      uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
    }

  for ( side_index = 0;
	side_index < loop_nside(loop); 
	side_index++)
    {
      fprintf(f, "%6d %6d %6d\n", 
	      1+2*side_index, 2+2*side_index, 2+2*side_index);
    }

  if ( 0 != array_size(loop->subtri) )
    {
      fprintf(f, "zone t=loop_subtri, i=%d, j=%d, f=fepoint, et=triangle\n",
	      3*array_size(loop->subtri), array_size(loop->subtri) );

      for ( subtri_index = 0;
	    subtri_index < array_size(loop->subtri); 
	    subtri_index++)
	{
	  subnode = subtri_n0((Subtri)array_item(loop->subtri,subtri_index));
	  subnode_uvw(subnode,uvw);
	  subnode_xyz(subnode,xyz);
	  fprintf(f, " %.16e %.16e %.16e %.16e %.16e\n",
		  uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );

	  subnode = subtri_n1((Subtri)array_item(loop->subtri,subtri_index));
	  subnode_uvw(subnode,uvw);
	  subnode_xyz(subnode,xyz);
	  fprintf(f, " %.16e %.16e %.16e %.16e %.16e\n",
		  uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );

	  subnode = subtri_n2((Subtri)array_item(loop->subtri,subtri_index));
	  subnode_uvw(subnode,uvw);
	  subnode_xyz(subnode,xyz);
	  fprintf(f, " %.16e %.16e %.16e %.16e %.16e\n",
		  uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
	}
      for ( subtri_index = 0;
	    subtri_index < array_size(loop->subtri); 
	    subtri_index++)
	fprintf(f, "%6d %6d %6d\n", 
		1+3*subtri_index, 2+3*subtri_index, 3+3*subtri_index);
    }

  fclose(f);

  return KNIFE_SUCCESS;
}

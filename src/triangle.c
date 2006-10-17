
/* defined by three segments and their three shared nodes */

/* $Id$ */

/* Michael A. Park (Mike Park)
 * Computational AeroSciences Branch
 * NASA Langley Research Center
 * Hampton, VA 23681
 * Phone:(757) 864-6604
 * Email: Mike.Park@NASA.Gov
 */

#include <math.h>
#include "triangle.h"

static int triangle_eps_frame = 0;
static int triangle_tecplot_frame = 0;

#define POSITIVE_AREA( subtri )					\
  if (FALSE) {							\
    if (subtri_reference_area(subtri) < 0.0 ) {			\
      triangle_eps(triangle);					\
      triangle_tecplot(triangle);				\
      triangle_examine_subnodes(triangle);			\
      printf("%s: %d: neg area %e\n",				\
	     __FILE__,__LINE__,subtri_reference_area(subtri));	\
      return KNIFE_NEG_AREA;					\
    }								\
  }

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      triangle_eps(triangle);				      \
      triangle_tecplot(triangle);			      \
      return code;					      \
    }							      \
  }

#define TRYN(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      triangle_eps(triangle);				      \
      triangle_tecplot(triangle);			      \
      return NULL;					      \
    }							      \
  }
#define NOT_NULL(pointer,msg)				      \
  if (NULL == pointer){					      \
    printf("%s: %d: %s\n",__FILE__,__LINE__,(msg));	      \
    return KNIFE_NULL;					      \
  }

Triangle triangle_create(Segment segment0, Segment segment1, Segment segment2,
			 int boundary_face_index )
{
  Triangle triangle;
  
  triangle = (Triangle)malloc( sizeof(TriangleStruct) );
  if (NULL == triangle) {
    printf("%s: %d: malloc failed in triangle_create\n",
	   __FILE__,__LINE__);
    return NULL; 
  }

  triangle_initialize(triangle, 
		      segment0, segment1, segment2, boundary_face_index);

  return triangle;
}

KNIFE_STATUS triangle_initialize(Triangle triangle,
				 Segment segment0, 
				 Segment segment1, 
				 Segment segment2,
				 int boundary_face_index )
{
  Subnode subnode0, subnode1, subnode2;

  triangle->boundary_face_index = boundary_face_index;

  triangle->segment[0] = segment0;
  triangle->segment[1] = segment1;
  triangle->segment[2] = segment2;

  triangle->node0 = segment_common_node( segment1, segment2 );
  NOT_NULL(triangle->node0,"common node0 NULL in triangle_initialize");
  triangle->node1 = segment_common_node( segment0, segment2 );
  NOT_NULL(triangle->node1,"common node1 NULL in triangle_initialize");
  triangle->node2 = segment_common_node( segment0, segment1 );
  NOT_NULL(triangle->node2,"common node2 NULL in triangle_initialize");

  subnode0 = subnode_create( 1.0, 0.0, 0.0, triangle->node0, NULL );
  subnode1 = subnode_create( 0.0, 1.0, 0.0, triangle->node1, NULL );
  subnode2 = subnode_create( 0.0, 0.0, 1.0, triangle->node2, NULL );

  triangle->subnode = array_create( 3, 50 );
  NOT_NULL(triangle->subnode, "triangle->subnode NULL in init");

  triangle_add_subnode( triangle, subnode0 );
  triangle_add_subnode( triangle, subnode1 );
  triangle_add_subnode( triangle, subnode2 );

  triangle->subtri  = array_create( 1, 50 );
  NOT_NULL(triangle->subtri, "triangle->subtri NULL in init");

  triangle_add_subtri( triangle, 
		       subtri_create( subnode0, subnode1, subnode2 ) );

  triangle->cut = array_create( 1, 50 );
  NOT_NULL(triangle->cut, "triangle->cut NULL in init");

  return KNIFE_SUCCESS;
}

void triangle_free( Triangle triangle )
{
  if ( NULL == triangle ) return;
  array_free( triangle->subnode );
  array_free( triangle->subtri );
  array_free( triangle->cut );
  free( triangle );
}

void triangle_echo( Triangle triangle )
{
  printf("node0 %d node1 %d node2 %d\n",
	 triangle->node0->indx,
	 triangle->node1->indx,
	 triangle->node2->indx);
}

KNIFE_STATUS triangle_extent( Triangle triangle, 
			      double *center, double *diameter )
{
  double dx, dy, dz;
  int i;

  for(i=0;i<3;i++)
    center[i] = ( triangle->node0->xyz[i] + 
		  triangle->node1->xyz[i] + 
		  triangle->node2->xyz[i] ) / 3.0;
  dx = triangle->node0->xyz[0] - center[0];
  dy = triangle->node0->xyz[1] - center[1];
  dz = triangle->node0->xyz[2] - center[2];
  *diameter = sqrt(dx*dx+dy*dy+dz*dz);
  dx = triangle->node1->xyz[0] - center[0];
  dy = triangle->node1->xyz[1] - center[1];
  dz = triangle->node1->xyz[2] - center[2];
  *diameter = MAX(*diameter,sqrt(dx*dx+dy*dy+dz*dz));
  dx = triangle->node2->xyz[0] - center[0];
  dy = triangle->node2->xyz[1] - center[1];
  dz = triangle->node2->xyz[2] - center[2];
  *diameter = MAX(*diameter,sqrt(dx*dx+dy*dy+dz*dz));
  
  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_neighbor( Triangle triangle, Segment segment, 
				Triangle *other )
{
  if (2 != segment_ntriangle(segment)) return KNIFE_UNIQUENESS;

  if ( triangle == segment_triangle(segment, 0) )
    {
      *other = segment_triangle(segment, 1);
      return KNIFE_SUCCESS;
    }

  if ( triangle == segment_triangle(segment, 1) )
    {
      *other = segment_triangle(segment, 0);
      return KNIFE_SUCCESS;
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_triangulate_cuts( Triangle triangle )
{
  int cut_index;
  Cut cut;
  Subnode subnode0, subnode1;
  Subtri subtri;

  double min_area;

  /* insert all nodes once (uniquely) */
  /* Delaunay poroperty is maintained with swaps after each insert */
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    TRY( triangle_insert_unique_subnode(triangle, cut_intersection0(cut) ),
	 "NULL subnode0 in triangle_triangulate_cuts" );
    TRY( triangle_insert_unique_subnode(triangle, cut_intersection1(cut) ),
	 "NULL subnode1 in triangle_triangulate_cuts" );
  }

  /* recover all cuts as subtriangle sides */
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    subnode0 = triangle_subnode_with_intersection(triangle, 
						  cut_intersection0(cut));
    subnode1 = triangle_subnode_with_intersection(triangle, 
						  cut_intersection1(cut));
    TRY( triangle_recover_side(triangle, subnode0, subnode1 ), "edge recovery");
  }

  /* verify that all cuts are now subtriangle sides (redundant) */
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    subnode0 = triangle_subnode_with_intersection(triangle, 
						  cut_intersection0(cut));
    subnode1 = triangle_subnode_with_intersection(triangle, 
						  cut_intersection1(cut));
    TRY( triangle_subtri_with_subnodes( triangle, 
					subnode0, subnode1, 
					&subtri ), "edge was not recovered" );
  }

  /* verify that all subtriangle have a positive area in uvw space */
  min_area = triangle_min_subtri_area( triangle );
  if (min_area < 0.0) 
    {
      printf("frames eps %d tecplot%d\n",
	     triangle_eps_frame,triangle_tecplot_frame);
      triangle_eps(triangle);
      triangle_tecplot(triangle);
      printf("%s: %d: area %30.20e\n",__FILE__,__LINE__,min_area);
      return KNIFE_NEG_AREA;
    }

  return KNIFE_SUCCESS;
}


Subnode triangle_subnode_with_intersection( Triangle triangle, 
					    Intersection intersection)
{
  int subnode_index;
  Subnode subnode;

  if( NULL == triangle || NULL == intersection ) return NULL;

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    {
      subnode = triangle_subnode(triangle, subnode_index);
      if ( intersection == subnode_intersection(subnode) ) return subnode;
    }

  return NULL;
}

KNIFE_STATUS triangle_insert_unique_subnode( Triangle triangle, 
					     Intersection intersection)
{
  Subnode subnode;
  double uvw[3];

  if ( NULL != triangle_subnode_with_intersection( triangle,intersection) )
    return KNIFE_SUCCESS;

  intersection_uvw(intersection,triangle,uvw);
  subnode = subnode_create( uvw[0], uvw[1], uvw[2], NULL, intersection );
  TRY( triangle_add_subnode( triangle, subnode ), "add subnode" );
  TRY( triangle_insert( triangle, subnode), "insert" );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_enclosing_subtri( Triangle triangle, Subnode subnode,
					Subtri *enclosing_subtri, 
					double *enclosing_bary )
{
  int subtri_index;

  Subtri subtri;
  double bary[3];
  double min_bary;
  
  Subtri best_subtri;
  double best_min_bary;

  if( NULL == triangle ) return KNIFE_NULL;

  best_subtri = triangle_subtri(triangle, 0);
  subtri_bary(best_subtri, subnode, bary);
  best_min_bary = MIN3(bary);

  for ( subtri_index = 1;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      subtri_bary(subtri, subnode, bary);
      min_bary = MIN3(bary);
      if (min_bary > best_min_bary)
	{
	  best_min_bary = min_bary;
	  best_subtri = subtri;
	}
    }

  if ( -1.0e-12 > best_min_bary ) 
    {
      printf("subnode u %f v %f w %f frame %d\n",
	     subnode->uvw[0],subnode->uvw[1],subnode->uvw[2],
	     triangle_eps_frame);
      triangle_eps(triangle);
      printf("%s: %d: triangle_enclosing_subtri %30.20e\n",
	     __FILE__,__LINE__,best_min_bary);
      return KNIFE_NOT_FOUND;
    }

  *enclosing_subtri = best_subtri;
  subtri_bary(*enclosing_subtri, subnode, enclosing_bary);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_insert( Triangle triangle, Subnode subnode)
{
  Subtri subtri;
  double bary[3];
  double min_bary;
  double side_tolerence;
  int insert_side;

  if( NULL == triangle ) return KNIFE_NULL;

  subtri = NULL;  

  TRY( triangle_enclosing_subtri( triangle, subnode, &subtri, bary ), 
       "triangle_enclosing_subtri not found" );

  side_tolerence = 1.0e-12;
  min_bary = MIN3(bary);

  if ( min_bary < side_tolerence )
    {
      insert_side = EMPTY;
      if (bary[0] <= bary[1] && bary[0] <= bary[2]) insert_side = 0;
      if (bary[1] <= bary[0] && bary[1] <= bary[2]) insert_side = 1;
      if (bary[2] <= bary[0] && bary[2] <= bary[1]) insert_side = 2;
      switch (insert_side) {
      case 0:
	TRY( triangle_insert_into_side(triangle, subnode,
				       subtri_n1(subtri),
				       subtri_n2(subtri) ), "side0" ); 
	break;
      case 1:
	TRY( triangle_insert_into_side(triangle, subnode,
				       subtri_n2(subtri),
				       subtri_n0(subtri) ), "side1" ); 
	break;
      case 2:
	TRY( triangle_insert_into_side(triangle, subnode,
				       subtri_n0(subtri),
				       subtri_n1(subtri) ), "side2" ); 
	break;
      default:
	printf("%s: %d: EMPTY insert side\n",__FILE__,__LINE__);
	return KNIFE_FAILURE;
      }
    }
  else
    {
      TRY( triangle_insert_into_center(triangle, subnode, subtri), "center" );  
    }
  
  TRY( triangle_delaunay(triangle, subnode ), "delaunay" );  

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_insert_into_side(Triangle triangle, Subnode new_node,
				       Subnode n0, Subnode n1)
{
  Subtri existing_subtri;
  Subtri new_subtri;

  if( NULL == triangle ) return KNIFE_NULL;

  existing_subtri = NULL;

  if (KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, n0, n1, 
						      &existing_subtri))
    {
      new_subtri = subtri_shallow_copy(existing_subtri);
      triangle_add_subtri(triangle,new_subtri);
      subtri_replace_node(existing_subtri, n0, new_node);
      subtri_replace_node(new_subtri,      n1, new_node);
      POSITIVE_AREA( existing_subtri );
      POSITIVE_AREA( new_subtri );
    }

  if (KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, n1, n0, 
						      &existing_subtri))
    {
      new_subtri = subtri_shallow_copy(existing_subtri);
      triangle_add_subtri(triangle,new_subtri);
      subtri_replace_node(existing_subtri, n0, new_node);
      subtri_replace_node(new_subtri,      n1, new_node);
      POSITIVE_AREA( existing_subtri );
      POSITIVE_AREA( new_subtri );
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_insert_into_center( Triangle triangle, 
					  Subnode new_node, Subtri subtri0 )
{
  Subtri subtri1, subtri2;

  if( NULL == triangle ) return KNIFE_NULL;

  subtri1 = subtri_shallow_copy( subtri0 );
  subtri2 = subtri_shallow_copy( subtri0 );

  triangle_add_subtri(triangle,subtri1);
  triangle_add_subtri(triangle,subtri2);
      
  subtri_replace_node(subtri0, subtri_n0(subtri0), new_node);
  subtri_replace_node(subtri1, subtri_n1(subtri1), new_node);
  subtri_replace_node(subtri2, subtri_n2(subtri2), new_node);

  POSITIVE_AREA( subtri0 );
  POSITIVE_AREA( subtri1 );
  POSITIVE_AREA( subtri2 );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_subtri_index( Triangle triangle, Subtri subtri,
				    int *subtri_index )
{
  int canidate;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( canidate = 0;
	canidate < triangle_nsubtri(triangle); 
	canidate++)
    {
      if ( subtri == triangle_subtri(triangle, canidate) )
	{
	  *subtri_index = canidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subnode_index( Triangle triangle, Subnode subnode,
				    int *subnode_index )
{
  int canidate;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( canidate = 0;
	canidate < triangle_nsubnode(triangle); 
	canidate++)
    {
      if ( subnode == triangle_subnode(triangle, canidate) )
	{
	  *subnode_index = canidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subtri_with_subnodes( Triangle triangle, 
					    Subnode n0, Subnode n1,
					    Subtri *subtri )
{
  Subtri canidate;
  int subtri_index;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      canidate = triangle_subtri(triangle, subtri_index);
      if ( subtri_has2(canidate,n0,n1) )
	{
	  *subtri = canidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subtri_index_with_nodes( Triangle triangle, 
					       Node n0, Node n1,
					       int *subtri_index )
{
  Subtri subtri;
  int canidate;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( canidate = 0;
	canidate < triangle_nsubtri(triangle); 
	canidate++)
    {
      subtri = triangle_subtri(triangle, canidate);
      if ( ( n0 == subtri->n0->node ||
	     n0 == subtri->n1->node ||
	     n0 == subtri->n2->node ) &&
	   ( n1 == subtri->n0->node ||
	     n1 == subtri->n1->node ||
	     n1 == subtri->n2->node ))
	{
	  *subtri_index = canidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_cut_with_subnodes( Triangle triangle, 
					 Subnode n0, Subnode n1,
					 Cut *cut )
{
  Cut canidate;
  int cut_index;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++)
    {
      canidate = triangle_cut(triangle, cut_index);
      if ( cut_has_intersections( canidate,
				  subnode_intersection(n0),
				  subnode_intersection(n1) ) )
	{
	  *cut = canidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subtri_with_intersections( Triangle triangle, 
						 Intersection i0,
						 Intersection i1,
						 Subtri *subtri )
{
  Subnode s0, s1;

  if( NULL == triangle ) return KNIFE_NULL;

  s0 = triangle_subnode_with_intersection( triangle, i0 );
  s1 = triangle_subnode_with_intersection( triangle, i1 );

   if( NULL == s0 ||  NULL == s1 ) return KNIFE_NOT_FOUND;

   return triangle_subtri_with_subnodes(triangle, s0, s1, subtri);
}

KNIFE_STATUS triangle_first_blocking_side( Triangle triangle, 
					   Subnode node2, Subnode node3,
					   Subnode *s0, Subnode *s1 )
{
  Subtri subtri;
  int subtri_index;
  Subnode n0, node0, node1;
  double right_area, left_area;
  double best_area;
  Subnode best_s0, best_s1;
  double min_area;

  if( NULL == triangle ) return KNIFE_NULL;

  best_area = -999.0;
  best_s0 = NULL;
  best_s1 = NULL;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      if ( subtri_has1(subtri,node2) )
	{
	  if ( subtri_has1(subtri,node3) ) return KNIFE_RECOVERED;
	  TRY( subtri_orient( subtri, node2, &n0, &node0, &node1 ),
	       "orient in block" );
	  right_area = subnode_area( node1, node2, node3 );
	  left_area = subnode_area( node0, node3, node2 );
	  min_area = MIN(right_area, left_area);
	  if ( min_area > best_area )
	    { 
	      best_area = min_area;
	      best_s0 = node0;
	      best_s1 = node1;
	    }
	}
    }

  if ( best_area < 1.0e-14 )
    {    
      printf("%s: %d: best area %e\n",__FILE__,__LINE__,best_area);
      return KNIFE_NOT_FOUND;
    }

  *s0 = best_s0;
  *s1 = best_s1;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_eps( Triangle triangle)
{
  int subtri_index;
  Subtri subtri;
  Subnode node;
  FILE *f;
  f = fopen("gnuplot_mesh_command","w");
  fprintf(f,"reset\n");
  fprintf(f,"set term postscript eps\n");
  fprintf(f,"set output 'frame%04d.eps'\n",triangle_eps_frame);

  triangle_eps_frame++;

  fprintf(f,"set size ratio -1\n");
  fprintf(f,"set xlabel 'V'\n");
  fprintf(f,"set ylabel 'W'\n");
  fprintf(f,"plot [-0.1:1.1] [-0.1:1.1] '-' title '' with lines lw 0.5\n");

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      node = subtri->n0;
      fprintf(f,"%25.15f %25.15f\n",node->uvw[1],node->uvw[2]);
      node = subtri->n1;
      fprintf(f,"%25.15f %25.15f\n",node->uvw[1],node->uvw[2]);
      node = subtri->n2;
      fprintf(f,"%25.15f %25.15f\n",node->uvw[1],node->uvw[2]);
      node = subtri->n0;
      fprintf(f,"%25.15f %25.15f\n",node->uvw[1],node->uvw[2]);
      fprintf(f,"\n\n");
    }

  fprintf(f,"e\n");

  fclose(f);
  system("gnuplot gnuplot_mesh_command; rm gnuplot_mesh_command");
  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_tecplot( Triangle triangle)
{
  int subnode_index;
  Subnode subnode;
  double xyz[3];
  int subtri_index;
  Subtri subtri;
  int node0, node1, node2;
  char filename[1025];
  FILE *f;

  sprintf(filename, "frame%04d.t",triangle_tecplot_frame );
  triangle_tecplot_frame++;

  f = fopen(filename, "w");

  fprintf(f,"title=triangle_geometry\nvariables=x,y,z,v,w\n");
  fprintf(f, "zone t=poly, i=%d, j=%d, f=fepoint, et=triangle\n",
	  triangle_nsubnode(triangle), triangle_nsubtri(triangle) );

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    {
      subnode = triangle_subnode(triangle, subnode_index);
      TRY( subnode_xyz( subnode, xyz ), "tecplot subnode xyz");
      fprintf(f, " %20.25f %20.25f %20.25f %20.25f %20.25f\n",
	      xyz[0], xyz[1], xyz[2], subnode_v(subnode), subnode_w(subnode) );
    }

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      TRY( triangle_subnode_index( triangle, subtri->n0, &node0), "tec sn0");
      TRY( triangle_subnode_index( triangle, subtri->n1, &node1), "tec sn1");
      TRY( triangle_subnode_index( triangle, subtri->n2, &node2), "tec sn2");
      fprintf(f, "%6d %6d %6d\n", 1+node0, 1+node1, 1+node2);
    }

  fclose(f);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_delaunay( Triangle triangle, Subnode subnode )
{
   Subtri subtri;
  int subtri_index;
  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      if ( subtri_has1(subtri,subnode) )
	{
	  TRY( triangle_suspect_edge( triangle, subnode, subtri), "suspect");
	}
    }
  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_suspect_edge( Triangle triangle, 
				    Subnode subnode, Subtri subtri )
{
  Subnode n0,n1,n2;
  Subtri other;
  Cut cut;
  Subnode o0,o1,o2;
  double xyz0[3], xyz1[3], xyz2[3], xyz3[3];
  double volume;

  TRY( subtri_orient( subtri, subnode, &n0, &n1, &n2 ), "orient");
  if ( KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, n2, n1, 
						       &other ) )
    if ( KNIFE_NOT_FOUND == triangle_cut_with_subnodes( triangle, n2, n1, 
							&cut ) )
      {
	subtri_orient( other, n2, &o0, &o1, &o2 );
	xyz0[0] = subnode_v(n0);
	xyz0[1] = subnode_w(n0);
 
	xyz1[0] = subnode_v(n1);
	xyz1[1] = subnode_w(n1);

	xyz2[0] = subnode_v(n2);
	xyz2[1] = subnode_w(n2);

	xyz3[0] = subnode_v(o2);
	xyz3[1] = subnode_w(o2);

	xyz0[2] = xyz0[0]*xyz0[0]+xyz0[1]*xyz0[1];
	xyz1[2] = xyz1[0]*xyz1[0]+xyz1[1]*xyz1[1];
	xyz2[2] = xyz2[0]*xyz2[0]+xyz2[1]*xyz2[1];
	xyz3[2] = xyz3[0]*xyz3[0]+xyz3[1]*xyz3[1];

	volume = intersection_volume6(xyz0,xyz1,xyz2,xyz3);
	if ( volume < 0.0 )
	  {
	    TRY( triangle_swap_side(triangle,n1,n2), "swap");
	    TRY( triangle_subtri_with_subnodes(triangle, n1, o2, &other),"on1");
	    TRY( triangle_suspect_edge( triangle, subnode, other ), "sn1" );
	    TRY( triangle_subtri_with_subnodes(triangle, o2, n2, &other),"on2");
	    TRY( triangle_suspect_edge( triangle, subnode, other ), "sn2" );
	  }
      }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_swap_side( Triangle triangle, 
				 Subnode node0, Subnode node1 )
{
  Subtri subtri0, subtri1;
  Subnode node2, node3;
  Subnode n0, n1, n2;

  TRY( triangle_subtri_with_subnodes(triangle, node0, node1, &subtri0 ), "s0" );
  TRY( triangle_subtri_with_subnodes(triangle, node1, node0, &subtri1 ), "s1" );

  TRY( subtri_orient( subtri0, node0, &n0, &n1, &n2 ), "orient0");
  node2 = n2;
  TRY( subtri_orient( subtri1, node1, &n0, &n1, &n2 ), "orient1");
  node3 = n2;

  subtri0->n0 = node1;
  subtri0->n1 = node2;
  subtri0->n2 = node3;

  subtri1->n0 = node0;
  subtri1->n1 = node3;
  subtri1->n2 = node2;

  POSITIVE_AREA( subtri0 );
  POSITIVE_AREA( subtri1 );

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_recover_side( Triangle triangle, 
				    Subnode node0, Subnode node1 )
{
  KNIFE_STATUS blocking_code;
  Subnode side0, side1;

  if ( NULL == node0 || NULL == node1 ) return KNIFE_NULL;
  
  blocking_code = triangle_first_blocking_side( triangle, node0, node1, 
						&side0, &side1 );
  
  /* return successfully if the subtriangle already exsits */
  if ( KNIFE_RECOVERED == blocking_code ) return KNIFE_SUCCESS;
  TRY( blocking_code, "first blocking side not found" );

  TRY( triangle_swap_side( triangle, side0, side1 ), "swap in recover" ); 

  return triangle_recover_side(triangle, node0, node1);
}

double triangle_min_subtri_area( Triangle triangle )
{
  Subtri subtri;
  int subtri_index;
  double min_area;

  if( NULL == triangle ) return KNIFE_NULL;

  subtri = triangle_subtri(triangle, 0);
  min_area = subtri_reference_area( subtri );

  for ( subtri_index = 1;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      min_area = MIN(min_area,subtri_reference_area( subtri ));
    }

  return min_area;
}

void triangle_examine_subnodes(Triangle triangle)
{
  int subnode_index;						
  int other_index;
  double diff;
  for ( subnode_index = 0;					
	subnode_index < triangle_nsubnode(triangle);	
	subnode_index++)					
    {							
      printf("%2d   u %f v %f w %f\n",	subnode_index,
	     triangle_subnode(triangle,subnode_index)->uvw[0],
	     triangle_subnode(triangle,subnode_index)->uvw[1],
	     triangle_subnode(triangle,subnode_index)->uvw[2]);
      for ( other_index = 0;					
	    other_index < triangle_nsubnode(triangle);	
	    other_index++)					
	{
	  diff = ABS(triangle_subnode(triangle,  other_index)->uvw[0] -
		     triangle_subnode(triangle,subnode_index)->uvw[0]) +
	    ABS(triangle_subnode(triangle,  other_index)->uvw[1]-
		triangle_subnode(triangle,subnode_index)->uvw[1]) +
	    ABS(triangle_subnode(triangle,  other_index)->uvw[2]-
		triangle_subnode(triangle,subnode_index)->uvw[2]);
	  if (subnode_index != other_index && diff < 1.0e-8)
	    printf("  %2d u %f v %f w %f\n",	other_index,
		   triangle_subnode(triangle,other_index)->uvw[0],
		   triangle_subnode(triangle,other_index)->uvw[1],
		   triangle_subnode(triangle,other_index)->uvw[2]);
	}
    }
}

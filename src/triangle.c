
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
#include "loop.h"

static int triangle_eps_frame = 0;
static int triangle_tecplot_frame = 0;
static int triangle_export_frame = 0;

#define POSITIVE_AREA( subtri )					\
  if (TRUE) {							\
    if (subtri_reference_area(subtri) <= 0.0 ) {		\
      printf("%s: %d: neg area %30.22e\n",			\
	     __FILE__,__LINE__,subtri_reference_area(subtri));	\
      subtri_echo( subtri );					\
      return KNIFE_NEG_AREA;					\
    }								\
  }

#define TRY(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
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
      triangle_tecplot(triangle);			      \
      return NULL;					      \
    }							      \
  }

#define TRYQ(fcn,msg)					      \
  {							      \
    int code;						      \
    code = (fcn);					      \
    if (KNIFE_SUCCESS != code){				      \
      printf("%s: %d: %d %s\n",__FILE__,__LINE__,code,(msg)); \
      return code;					      \
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
  NOT_NULL(subnode0, "NULL sn0");
  subnode1 = subnode_create( 0.0, 1.0, 0.0, triangle->node1, NULL );
  NOT_NULL(subnode1, "NULL sn1");
  subnode2 = subnode_create( 0.0, 0.0, 1.0, triangle->node2, NULL );
  NOT_NULL(subnode2, "NULL sn2");

  triangle->subnode = array_create( 3, 50 );
  NOT_NULL(triangle->subnode, "triangle->subnode NULL in init");

  TRY( triangle_add_subnode( triangle, subnode0 ), "add sn0");
  TRY( triangle_add_subnode( triangle, subnode1 ), "add sn1");
  TRY( triangle_add_subnode( triangle, subnode2 ), "add sn2");

  triangle->subtri  = array_create( 1, 50 );
  NOT_NULL(triangle->subtri, "triangle->subtri NULL in init");

  TRY( triangle_add_subtri( triangle, 
			    subtri_create( subnode0, subnode1, subnode2 ) ),
       "add st");

  triangle->cut = array_create( 1, 50 );
  NOT_NULL(triangle->cut, "triangle->cut NULL in init");

  return KNIFE_SUCCESS;
}

void triangle_free( Triangle triangle )
{
  int i;
  if ( NULL == triangle ) return;

  for ( i = 0; i < triangle_nsubnode(triangle); i++) 
    subnode_free( triangle_subnode( triangle, i ) );
  array_free( triangle->subnode );

  for ( i = 0; i < triangle_nsubtri(triangle); i++) 
    subtri_free( triangle_subtri( triangle, i ) );
  array_free( triangle->subtri );

  /* FIXME find a consistant way to free cuts and intersections */
  array_free( triangle->cut );

  free( triangle );
}

int triangle_segment_index( Triangle triangle, Segment segment )
{
  if ( NULL == triangle ) return EMPTY;
  if ( NULL == segment ) return EMPTY;
  if ( triangle_segment(triangle,0) == segment ) return 0;
  if ( triangle_segment(triangle,1) == segment ) return 1;
  if ( triangle_segment(triangle,2) == segment ) return 2;
  return EMPTY;
}

int triangle_node_index( Triangle triangle, Node node )
{
  if ( NULL == triangle ) return EMPTY;
  if ( NULL == node ) return EMPTY;
  if ( triangle_node0(triangle) == node ) return 0;
  if ( triangle_node1(triangle) == node ) return 1;
  if ( triangle_node2(triangle) == node ) return 2;
  return EMPTY;
}

KNIFE_STATUS triangle_set_frame( int frame )
{
  triangle_eps_frame     = frame;
  triangle_tecplot_frame = frame;
  triangle_export_frame  = frame;
  return KNIFE_SUCCESS;
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
  KNIFE_STATUS recover_status;
  KnifeBool *cut_recovered;
  KnifeBool improvement;
  int subnode_index;
  double t_limit;
  double side_tolerence;

  /* insert all nodes once (uniquely) */
  /* Delaunay poroperty is maintained with swaps after each insert */

  /* add to boundary first */

  side_tolerence = 2.0; /* force into side */
  for ( t_limit = 0.45 ; t_limit >= -0.11 ; t_limit -= 0.1 ) 
    for ( cut_index = 0;
	  cut_index < triangle_ncut(triangle); 
	  cut_index++) {
      cut = triangle_cut(triangle,cut_index);
      NOT_NULL(cut,"triangle_triangulate_cuts: cut NULL");
      NOT_NULL(cut_intersection0(cut),"triangle_triangulate_cuts: int0");
      NOT_NULL(cut_intersection1(cut),"triangle_triangulate_cuts: int1");
      if ( triangle != intersection_triangle( cut_intersection0(cut) ) &&
	   intersection_t( cut_intersection0(cut) ) > t_limit &&
	   intersection_t( cut_intersection0(cut) ) < (1.0-t_limit) )
	TRY( triangle_insert_unique_subnode( triangle, cut_intersection0(cut),
					     side_tolerence ),
	     "insert side subnode0 in triangle_triangulate_cuts" );
      if ( triangle != intersection_triangle( cut_intersection1(cut) ) &&
	   intersection_t( cut_intersection1(cut) ) > t_limit &&
	   intersection_t( cut_intersection1(cut) ) < (1.0-t_limit)  )
	TRY( triangle_insert_unique_subnode( triangle, cut_intersection1(cut),
					     side_tolerence ),
	     "insert subnode1 in triangle_triangulate_cuts" );
    }

  /* now triangle interior */
  side_tolerence = 1.0e-15; /* allow into side rarely */
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    if ( triangle == intersection_triangle( cut_intersection0(cut) ) )
      TRY( triangle_insert_unique_subnode( triangle, cut_intersection0(cut),
					   side_tolerence ),
	   "insert subnode0 in triangle_triangulate_cuts" );
    if ( triangle == intersection_triangle( cut_intersection1(cut) ) )
      TRY( triangle_insert_unique_subnode( triangle, cut_intersection1(cut),
					   side_tolerence ),
	   "insert subnode1 in triangle_triangulate_cuts" );
  }

  /* recover all cuts as subtriangle sides */

  cut_recovered = (KnifeBool *) malloc( triangle_ncut(triangle) * 
					sizeof(KnifeBool) );
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut_recovered[cut_index] = FALSE;
  }

  improvement = TRUE;
  while( improvement )
    {
      improvement = FALSE;
      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++) 
	if ( !cut_recovered[cut_index] ) 
	  {
	    cut = triangle_cut(triangle,cut_index);
	    subnode0 = triangle_subnode_with_intersection(triangle, 
							cut_intersection0(cut));
	    subnode1 = triangle_subnode_with_intersection(triangle, 
						        cut_intersection1(cut));
	    /* try both orientations */
	    recover_status = 
	      triangle_recover_side(triangle, subnode0, subnode1 );
	    if ( KNIFE_NOT_IMPROVED == recover_status )
	      recover_status = 
		triangle_recover_side(triangle, subnode1, subnode0 );

	    if ( KNIFE_SUCCESS == recover_status )
	      {
		cut_recovered[cut_index] = TRUE;
		improvement = TRUE;
		if (TRUE)
		  {
		    TRY( triangle_delaunay( triangle, subnode0 ), "re-d 0");
		    TRY( triangle_delaunay( triangle, subnode1 ), "re-d 1");
		  }
	      }
	    if ( KNIFE_NOT_IMPROVED != recover_status )
	      TRY( recover_status, "recover_side failure" );

	  }
    }

  /* enforce delaunay after recovery */
  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    {
      TRY( triangle_delaunay( triangle, 
			      triangle_subnode(triangle,subnode_index ) ),
	   "re-d");
    }

  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) 
    if ( !cut_recovered[cut_index] ) 
      {
	cut = triangle_cut(triangle,cut_index);
	subnode0 = triangle_subnode_with_intersection(triangle, 
						      cut_intersection0(cut));
	subnode1 = triangle_subnode_with_intersection(triangle, 
						      cut_intersection1(cut));
	recover_status = triangle_provable_recovery(triangle,subnode0,subnode1);
	if ( KNIFE_SUCCESS != recover_status )
	  {
	    free(cut_recovered);
	    printf("%s: %d: triangle_provable_recovery failed\n",
		   __FILE__,__LINE__);
	    return recover_status;
	  }
	/* enforce delaunay after recovery */
	for ( subnode_index = 0;
	      subnode_index < triangle_nsubnode(triangle); 
	      subnode_index++)
	  {
	    TRY( triangle_delaunay( triangle, 
				    triangle_subnode(triangle,subnode_index ) ),
		 "re-d");
	  }
      }

  free(cut_recovered);

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
  return triangle_verify_subtri_area( triangle );
}

KNIFE_STATUS triangle_shewchuk( Triangle triangle )
{
  int status;
  char command[1025];

  int cut_index;
  Cut cut;
  Subnode subnode0, subnode1;
  Subtri subtri;

  if ( 0 == triangle_ncut(triangle) ) return KNIFE_SUCCESS;

  TRY( triangle_export( triangle ), "export" );
  sprintf(command, "triangle -q -S -p triangle%08d > triangle%08d.out",
	  triangle_export_frame, triangle_export_frame );
  status = system( command );
  if (0 != status) 
    {
      printf("%s: %d: triangle exit code %d\n",__FILE__,__LINE__,status);
      return KNIFE_FAILURE;
    }
  TRY( triangle_import( triangle, NULL ), "import" );

  triangle_tecplot( triangle );

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
  return triangle_verify_subtri_area( triangle );
}

KNIFE_STATUS triangle_area_normal( Triangle triangle, 
				   double *area, double *normal )
{
  double edge1[3], edge2[3];
  double length;

  edge1[0] = triangle_xyz1(triangle)[0] - triangle_xyz0(triangle)[0];
  edge1[1] = triangle_xyz1(triangle)[1] - triangle_xyz0(triangle)[1];
  edge1[2] = triangle_xyz1(triangle)[2] - triangle_xyz0(triangle)[2];

  edge2[0] = triangle_xyz2(triangle)[0] - triangle_xyz0(triangle)[0];
  edge2[1] = triangle_xyz2(triangle)[1] - triangle_xyz0(triangle)[1];
  edge2[2] = triangle_xyz2(triangle)[2] - triangle_xyz0(triangle)[2];

  normal[0] = edge1[1]*edge2[2] - edge1[2]*edge2[1];
  normal[1] = edge1[2]*edge2[0] - edge1[0]*edge2[2];
  normal[2] = edge1[0]*edge2[1] - edge1[1]*edge2[0];
  
  (*area) = sqrt( normal[0]*normal[0] +
		  normal[1]*normal[1] +
		  normal[2]*normal[2] );

  if ( 0.0 == (*area) )
    {
      printf("%s: %d: triangle area %15.7e norm %15.7e %15.7e %15.7e\n",
	     __FILE__,__LINE__,(*area), normal[0], normal[1], normal[2]);
      return KNIFE_SUCCESS;
    }

  normal[0] /= (*area);
  normal[1] /= (*area);
  normal[2] /= (*area);
  
  (*area) *= 0.5;

  length = sqrt( normal[0]*normal[0] + 
		 normal[1]*normal[1] + 
		 normal[2]*normal[2]);

  if ( ABS( length - 1.0 ) > 0.1 )
    {
      printf("%s: %d: triangle area %15.7e norm len %10.2e out of tolerance\n",
	     __FILE__,__LINE__,(*area), length);
      normal[0] = 0.0;
      normal[1] = 0.0;
      normal[2] = 0.0;
    }

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_verify_subtri_area( Triangle triangle )
{
  double min_area;
  
  min_area = triangle_min_subtri_area( triangle );

  while (min_area <= 0.0)
    {
      printf("%s: %d: improving area %e\n",__FILE__,__LINE__,min_area);
      triangle_tecplot(triangle);
      
      TRY( triangle_swap_neg_area( triangle ), 
	   "neg area swap failed to improve triangle" );
      
      min_area = triangle_min_subtri_area( triangle );

      printf("%s: %d: min area now %e\n",__FILE__,__LINE__,min_area);
    }

  return ( (min_area <= 0.0) ? KNIFE_NEG_AREA : KNIFE_SUCCESS );

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
					     Intersection intersection, 
					     double side_tolerence )
{
  Subnode subnode;
  double uvw[3];

  if( NULL == triangle || NULL == intersection ) return KNIFE_NULL;

  if ( NULL != triangle_subnode_with_intersection( triangle,intersection) )
    return KNIFE_SUCCESS;

  TRY( intersection_uvw(intersection,triangle,uvw), "intersection uvw" );
  subnode = subnode_create( uvw[0], uvw[1], uvw[2], NULL, intersection );
  NOT_NULL( subnode, "new subnode NULL");
  TRY( triangle_add_subnode( triangle, subnode ), "add subnode" );
  TRY( triangle_insert( triangle, subnode, side_tolerence), "insert" );

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

  if ( -1.0 > best_min_bary ) /* cannot happen? */
    {
      printf("subnode u %e v %e w %e\n",
	     subnode->uvw[0],subnode->uvw[1],subnode->uvw[2]);
      triangle_tecplot(triangle);
      printf("%s: %d: triangle_enclosing_subtri %30.20e\n",
	     __FILE__,__LINE__,best_min_bary);
      return KNIFE_NOT_FOUND;
    }

  *enclosing_subtri = best_subtri;
  subtri_bary(*enclosing_subtri, subnode, enclosing_bary);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_insert( Triangle triangle, Subnode subnode, 
			      double side_tolerence)
{
  Subtri subtri;
  double bary[3];
  double min_bary;
  int insert_side;

  if( NULL == triangle ) return KNIFE_NULL;

  subtri = NULL;  

  TRY( triangle_enclosing_subtri( triangle, subnode, &subtri, bary ), 
       "triangle_enclosing_subtri not found" );

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
      TRY( triangle_add_subtri(triangle,new_subtri), "add new st01");
      subtri_replace_node(existing_subtri, n0, new_node);
      subtri_replace_node(new_subtri,      n1, new_node);
      POSITIVE_AREA( existing_subtri );
      POSITIVE_AREA( new_subtri );
    }

  if (KNIFE_SUCCESS == triangle_subtri_with_subnodes( triangle, n1, n0, 
						      &existing_subtri))
    {
      new_subtri = subtri_shallow_copy(existing_subtri);
      TRY( triangle_add_subtri(triangle,new_subtri), "add new st10");
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

  TRY( triangle_add_subtri(triangle,subtri1), "add new st cent 1");
  TRY( triangle_add_subtri(triangle,subtri2), "add new st cent 2");
      
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
  int candidate;

  if( NULL == triangle ) return KNIFE_NULL;
  if( NULL == subtri ) return KNIFE_NULL;

  for ( candidate = 0;
	candidate < triangle_nsubtri(triangle); 
	candidate++)
    {
      if ( subtri == triangle_subtri(triangle, candidate) )
	{
	  *subtri_index = candidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subnode_index( Triangle triangle, Subnode subnode,
				    int *subnode_index )
{
  int candidate;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( candidate = 0;
	candidate < triangle_nsubnode(triangle); 
	candidate++)
    {
      if ( subnode == triangle_subnode(triangle, candidate) )
	{
	  *subnode_index = candidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subtri_with_subnodes( Triangle triangle, 
					    Subnode n0, Subnode n1,
					    Subtri *subtri )
{
  Subtri candidate;
  int subtri_index;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      candidate = triangle_subtri(triangle, subtri_index);
      if ( subtri_has2(candidate,n0,n1) )
	{
	  *subtri = candidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
}

KNIFE_STATUS triangle_subtri_index_with_subnodes( Triangle triangle, 
						  Subnode n0, Subnode n1,
						  int *subtri_index )
{
  Subtri candidate;
  int candidate_index;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( candidate_index = 0;
	candidate_index < triangle_nsubtri(triangle); 
	candidate_index++)
    {
      candidate = triangle_subtri(triangle, candidate_index);
      if ( subtri_has2(candidate,n0,n1) )
	{
	  *subtri_index = candidate_index;
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
  int candidate;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( candidate = 0;
	candidate < triangle_nsubtri(triangle); 
	candidate++)
    {
      subtri = triangle_subtri(triangle, candidate);
      if ( ( n0 == subtri->n0->node ||
	     n0 == subtri->n1->node ||
	     n0 == subtri->n2->node ) &&
	   ( n1 == subtri->n0->node ||
	     n1 == subtri->n1->node ||
	     n1 == subtri->n2->node ))
	{
	  *subtri_index = candidate;
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

KNIFE_STATUS triangle_subtri_index_with_intersections( Triangle triangle, 
						       Intersection i0,
						       Intersection i1,
						       int *subtri_index )
{
  Subnode s0, s1;

  if( NULL == triangle ) return KNIFE_NULL;

  s0 = triangle_subnode_with_intersection( triangle, i0 );
  s1 = triangle_subnode_with_intersection( triangle, i1 );

   if( NULL == s0 ||  NULL == s1 ) return KNIFE_NOT_FOUND;

   return triangle_subtri_index_with_subnodes(triangle, s0, s1, subtri_index);
}

KNIFE_STATUS triangle_cut_with_subnodes( Triangle triangle, 
					 Subnode n0, Subnode n1,
					 Cut *cut )
{
  Cut candidate;
  int cut_index;

  if( NULL == triangle ) return KNIFE_NULL;

  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++)
    {
      candidate = triangle_cut(triangle, cut_index);
      if ( cut_has_intersections( candidate,
				  subnode_intersection(n0),
				  subnode_intersection(n1) ) )
	{
	  *cut = candidate;
	  return KNIFE_SUCCESS;
	}
    }

  return KNIFE_NOT_FOUND;
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
      printf("from %20.17f %20.17f %20.17f\n  to %20.17f %20.17f %20.17f\n",
	     subnode_u( node2 ), subnode_v( node2 ), subnode_w( node2 ),
	     subnode_u( node3 ), subnode_v( node3 ), subnode_w( node3 ) );
      printf("%s: %d: best area %e\n",__FILE__,__LINE__,best_area);
      return KNIFE_NOT_FOUND;
    }

  *s0 = best_s0;
  *s1 = best_s1;

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_next_blocking_side( Triangle triangle, 
					  Subnode node0, Subnode node1,
					  Subnode target0, Subnode target1,
					  Subnode *s0, Subnode *s1 )
{
  Subnode node2;
  Subnode n0, n1;
  Subtri subtri;
  double area0, area1;

  if( NULL == triangle ) return KNIFE_NULL;

  TRY( triangle_subtri_with_subnodes( triangle, node0, node1, &subtri ),"nst"); 
  
  TRY( subtri_orient( subtri, node0, &n0, &n1, &node2 ),
       "orient in next block" );

  if ( target1 == node2 ) return KNIFE_NO_MAS;

  area0 = subnode_area( target0, target1, node2 );
  area1 = subnode_area( target1, target0, node2 );

  if ( area0 == 0.0 && area1 == 0.0 ) return KNIFE_SINGULAR;
  if ( area0 >  0.0 && area1 >  0.0 ) return KNIFE_SINGULAR;
  if ( area0 <  0.0 && area1 <  0.0 ) return KNIFE_SINGULAR;

  if ( area1 < area0 )
    {
      *s0 = node1;
      *s1 = node2;
    }
  else
    {
      *s0 = node2;
      *s1 = node0;
    }

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

  triangle_eps_frame++;
  fprintf(f,"set output 'triangle%08d.eps'\n",triangle_eps_frame);

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

  int cut_index;
  Cut cut;
  Intersection intersection;
  Node node;
  int i;
  Segment segment;

  double uvw[3];

  char filename[1025];
  FILE *f;

  triangle_tecplot_frame++;
  sprintf(filename, "triangle%08d.t",triangle_tecplot_frame );
  printf("producing %s\n",filename);

  f = fopen(filename, "w");

  fprintf(f,"title=triangle_geometry\nvariables=v,w,x,y,z\n");
  fprintf(f, "zone t=subtri, i=%d, j=%d, f=fepoint, et=triangle\n",
	  triangle_nsubnode(triangle), triangle_nsubtri(triangle) );

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    {
      subnode = triangle_subnode(triangle, subnode_index);
      NOT_NULL( subnode, "tecplot subnode NULL" );
      TRYQ( subnode_xyz( subnode, xyz ), "tecplot subnode xyz");
      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
	      subnode_v(subnode), subnode_w(subnode), xyz[0], xyz[1], xyz[2] );
    }

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      TRYQ( triangle_subnode_index( triangle, subtri->n0, &node0), "tec sn0");
      TRYQ( triangle_subnode_index( triangle, subtri->n1, &node1), "tec sn1");
      TRYQ( triangle_subnode_index( triangle, subtri->n2, &node2), "tec sn2");
      fprintf(f, "%6d %6d %6d\n", 1+node0, 1+node1, 1+node2);
    }
  
  if ( 0 < triangle_ncut(triangle) )
    {
      fprintf(f, "zone t=cut, i=%d, j=%d, f=fepoint, et=triangle\n",
	      2*triangle_ncut(triangle), triangle_ncut(triangle) );

      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++)
	{
	  cut = triangle_cut(triangle, cut_index);
	  NOT_NULL( cut, "tecplot cut NULL" );
	  intersection = cut_intersection0(cut);
	  NOT_NULL( intersection, "tecplot intersection0 NULL" );
	  TRYQ( intersection_uvw( intersection, triangle, uvw ), "int uvw" );
	  TRYQ( intersection_xyz( intersection, xyz ), "int xyz" );
	  fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		  uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
	  intersection = cut_intersection1(cut);
	  NOT_NULL( intersection, "tecplot intersection1 NULL" );
	  TRYQ( intersection_uvw( intersection, triangle, uvw ), "int uvw" );
	  TRYQ( intersection_xyz( intersection, xyz ), "int xyz" );
	  fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		  uvw[1], uvw[2], xyz[0], xyz[1], xyz[2] );
	}

      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++)
	{
	  fprintf(f, "%6d %6d %6d\n", 
		  1+2*cut_index, 2+2*cut_index, 2+2*cut_index);
	}
    }

  fprintf(f, "zone t=parent, i=%d, j=%d, f=fepoint, et=triangle\n",
	  3*triangle_nsubnode(triangle), triangle_nsubnode(triangle) );

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++ )
    {
      subnode = triangle_subnode(triangle, subnode_index);
      node = subnode_node( subnode );
      if ( NULL != node )
	for ( i = 0 ; i < 3; i++ )
	  fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		  subnode_v(subnode), subnode_w(subnode), 
		  node_x(node), node_y(node), node_z(node) );
      intersection = subnode_intersection( subnode );
      if ( NULL != intersection )
	{
	  if ( triangle == intersection_triangle( intersection ) )
	    {
	      segment = intersection_segment( intersection );
	      node = segment_node0(segment);
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	      node = segment_node1(segment);
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	    }
	  else
	    {
	      node = triangle_node0(intersection_triangle( intersection ));
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	      node = triangle_node1(intersection_triangle( intersection ));
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	      node = triangle_node2(intersection_triangle( intersection ));
	      fprintf(f, " %20.17f %20.17f %.16e %.16e %.16e\n",
		      subnode_v(subnode), subnode_w(subnode),
		      node_x(node), node_y(node), node_z(node) );
	    }
	}
    }

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    {
      fprintf(f, "%6d %6d %6d\n", 
	      1+3*subnode_index, 2+3*subnode_index, 3+3*subnode_index);
    }

  fclose(f);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_export( Triangle triangle)
{
  char filename[1025];
  FILE *f;

  Array seg[3];
  Array ints;
  int cut_index;
  Cut cut;
  int i;
  int sorted, indx;
  int indx0, indx1;
  Intersection largest;
  Intersection intersection;
  int nseg;
  double uvw[3], uvw0[3], uvw1[3];

  for (i = 0; i<3; i++)
    {
      seg[i] = array_create( 10, 10 );
      for ( cut_index = 0;
	    cut_index < triangle_ncut(triangle); 
	    cut_index++) {
	cut = triangle_cut(triangle,cut_index);
	intersection = cut_intersection0(cut);
	if (triangle_segment(triangle,i) == intersection_segment(intersection)) 
	  array_add_uniquely( seg[i], (ArrayItem)intersection );
	intersection = cut_intersection1(cut);
	if (triangle_segment(triangle,i) == intersection_segment(intersection)) 
	  array_add_uniquely( seg[i], (ArrayItem)intersection );
      }
      for ( sorted = array_size(seg[i]) ; sorted > 1 ; sorted-- )
	{
	  largest = (Intersection)array_item(seg[i],0);
	  for ( indx = 1; indx < sorted; indx++ )
	    if ( intersection_t( (Intersection)array_item(seg[i],indx) ) >
		 intersection_t( largest ) )
	      largest = (Intersection)array_item(seg[i],indx);
	  array_remove( seg[i], largest );
	  array_add( seg[i], largest );
	}
    }

  triangle_export_frame++;

  sprintf(filename, "triangle%08d.poly",triangle_export_frame );
  printf("exporting %s\n",filename);


  f = fopen(filename, "w");

  ints = array_create( 10, 10 );
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    array_add_uniquely( ints, (ArrayItem)cut_intersection0(cut) );
    array_add_uniquely( ints, (ArrayItem)cut_intersection1(cut) );
  }

  fprintf(f, "%d %d %d %d\n",3+array_size(ints),2,0,0);

  /* corner nodes */
  fprintf(f, "%3d %.20e %.20e\n",1,0.0,0.0);
  fprintf(f, "%3d %.20e %.20e\n",2,1.0,0.0);
  fprintf(f, "%3d %.20e %.20e\n",3,0.0,1.0);

  for ( i = 0 ; i < array_size(ints) ; i++ )
    {
      intersection = (Intersection) array_item( ints, i );
      TRY( intersection_uvw( intersection, triangle, uvw ), "int uvw" );
      fprintf(f, "%3d %.20e %.20e\n",i+4,uvw[1],uvw[2]);
    }

  nseg = triangle_ncut(triangle) + 
    MAX(1,array_size(seg[0])+1) +
    MAX(1,array_size(seg[1])+1) +
    MAX(1,array_size(seg[2])+1);

  fprintf(f, "%d %d\n",nseg,0);

  nseg = 0;
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    nseg++;
    fprintf( f, "%3d %3d %3d\n",nseg,
	     4+array_index_of( ints, (ArrayItem)cut_intersection0(cut)),
	     4+array_index_of( ints, (ArrayItem)cut_intersection1(cut)) );
  }
  
  for (i = 0; i<3; i++)
    for ( indx = 1; indx < array_size(seg[i]); indx++ )
      {
	nseg++;
	fprintf( f, "%3d %3d %3d\n",nseg,
		 4+array_index_of( ints, array_item(seg[i],indx-1) ),
		 4+array_index_of( ints, array_item(seg[i],indx)   ) );
      }


  i = 0;
  if ( 0 == array_size(seg[i]) )
    {
      nseg++;
      fprintf( f, "%3d %3d %3d\n",nseg, 2, 3 );
    }
  else
    {
      intersection = array_item(seg[i],0);
      intersection_uvw( intersection, triangle, uvw0 );
      indx0 = 4+array_index_of( ints, intersection );

      intersection = array_item(seg[i],array_size(seg[i])-1);
      intersection_uvw( intersection, triangle, uvw1 );
      indx1 = 4+array_index_of( ints, intersection );
      if ( uvw1[2] > uvw0[2] )
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 2, indx0 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx1, 3 );
	}
      else
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 2, indx1 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx0, 3 );
	}
    }


  i = 1;
  if ( 0 == array_size(seg[i]) )
    {
      nseg++;
      fprintf( f, "%3d %3d %3d\n",nseg, 3, 1 );
    }
  else
    {
      intersection = array_item(seg[i],0);
      intersection_uvw( intersection, triangle, uvw0 );
      indx0 = 4+array_index_of( ints, intersection );

      intersection = array_item(seg[i],array_size(seg[i])-1);
      intersection_uvw( intersection, triangle, uvw1 );
      indx1 = 4+array_index_of( ints, intersection );
      if ( uvw1[0] > uvw0[0] )
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 3, indx0 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx1, 1 );
	}
      else
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 3, indx1 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx0, 1 );
	}
    }


  i = 2;
  if ( 0 == array_size(seg[i]) )
    {
      nseg++;
      fprintf( f, "%3d %3d %3d\n",nseg, 1, 2 );
    }
  else
    {
      intersection = array_item(seg[i],0);
      intersection_uvw( intersection, triangle, uvw0 );
      indx0 = 4+array_index_of( ints, intersection );

      intersection = array_item(seg[i],array_size(seg[i])-1);
      intersection_uvw( intersection, triangle, uvw1 );
      indx1 = 4+array_index_of( ints, intersection );
      if ( uvw1[1] > uvw0[1] )
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 1, indx0 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx1, 2 );
	}
      else
	{
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, 1, indx1 );
	  nseg++; fprintf( f, "%3d %3d %3d\n", nseg, indx0, 2 );
	}
    }


  fprintf(f, "%d\n",0);

  fclose(f);

  return KNIFE_SUCCESS;
}

KNIFE_STATUS triangle_import( Triangle triangle, char *file_name )
{
  char import_file_name[1025];
  FILE *f;
  Subnode subnode0, subnode1, subnode2;
  int subnode_index, subtri_index, cut_index;
  Cut cut;
  Array ints;
  int nsubtri, node_per_face, nattr;
  int indx, n0,n1,n2;
  Intersection intersection;
  double uvw[3];
  Subnode subnode;
  Subtri subtri;

  f = NULL;
  if ( NULL == file_name )
    {
      sprintf(import_file_name, "triangle%08d.1.ele", triangle_export_frame );
      printf("importing %s\n",import_file_name);
      f = fopen(import_file_name, "r");
    }
  else
    {
      printf("importing %s\n",file_name);
      f = fopen(file_name, "r");
    }
  NOT_NULL(f, "NULL file, unable to open");

  for ( subnode_index = 0;
	subnode_index < triangle_nsubnode(triangle); 
	subnode_index++)
    subnode_free( triangle_subnode(triangle,subnode_index ) );
  array_free(triangle->subnode);
  
  ints = array_create( 10, 10 );
  for ( cut_index = 0;
	cut_index < triangle_ncut(triangle); 
	cut_index++) {
    cut = triangle_cut(triangle,cut_index);
    array_add_uniquely( ints, (ArrayItem)cut_intersection0(cut) );
    array_add_uniquely( ints, (ArrayItem)cut_intersection1(cut) );
  }

  triangle->subnode = array_create( 3+array_size(ints), 50 );
  NOT_NULL(triangle->subnode, "triangle->subnode NULL in init");

  subnode0 = subnode_create( 1.0, 0.0, 0.0, triangle->node0, NULL );
  NOT_NULL(subnode0, "NULL sn0");
  array_add( triangle->subnode, subnode0 );

  subnode1 = subnode_create( 0.0, 1.0, 0.0, triangle->node1, NULL );
  NOT_NULL(subnode1, "NULL sn1");
  array_add( triangle->subnode, subnode1 );

  subnode2 = subnode_create( 0.0, 0.0, 1.0, triangle->node2, NULL );
  NOT_NULL(subnode2, "NULL sn2");
  array_add( triangle->subnode, subnode2 );

  for ( subnode_index = 0;
        subnode_index < array_size(ints);
        subnode_index++)
    {
      intersection = (Intersection)array_item(ints,subnode_index);
      intersection_uvw( intersection, triangle, uvw);
      subnode = subnode_create( uvw[0], uvw[1], uvw[2], NULL, intersection );
      array_add( triangle->subnode, subnode );
    }

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++ )
    subtri_free( triangle_subtri(triangle,subnode_index ) );
  array_free(triangle->subtri);

  fscanf(f,"%d %d %d",&nsubtri,&node_per_face,&nattr);

  triangle->subtri = array_create( nsubtri, 50 );

  for ( subtri_index = 0;
	subtri_index < nsubtri; 
	subtri_index++)
    {
      fscanf(f,"%d %d %d %d",&indx,&n0,&n1,&n2);
      subnode0 = (Subnode)array_item(triangle->subnode, n0-1 );
      NOT_NULL(subnode0, "NULL subnode0");
      subnode1 = (Subnode)array_item(triangle->subnode, n1-1 );
      NOT_NULL(subnode1, "NULL subnode1");
      subnode2 = (Subnode)array_item(triangle->subnode, n2-1 );
      NOT_NULL(subnode2, "NULL subnode2");
      subtri = subtri_create( subnode0, subnode1, subnode2 );
      array_add( triangle->subtri, subtri );
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

KnifeBool triangle_swap_positive( Triangle triangle, 
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

  return ( 1.0e-20 <= subnode_area( node1, node2, node3 ) &&
	   1.0e-20 <= subnode_area( node0, node3, node2 ) );
 
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
	if ( volume < 0.0 && triangle_swap_positive(triangle,n1,n2) )
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

  if ( !triangle_swap_positive( triangle, side0, side1 ) )
    return KNIFE_NOT_IMPROVED;

  TRY( triangle_swap_side( triangle, side0, side1 ), "swap in recover" ); 

  return triangle_recover_side(triangle, node0, node1);
}

KNIFE_STATUS triangle_swap_min_area_increase( Triangle triangle,
					      Subnode node0, Subnode node1)
{
  Subtri subtri0, subtri1;
  Subnode n0, n1, n2;
  Subnode node2, node3;

  Cut cut;
  double orig_area, new_area;

  if ( KNIFE_SUCCESS == triangle_cut_with_subnodes( triangle, 
						    node0, node1,
						    &cut )  )
    return KNIFE_NOT_IMPROVED;

  TRY( triangle_subtri_with_subnodes(triangle, node0, node1, &subtri0 ), "s0" );
  TRY( triangle_subtri_with_subnodes(triangle, node1, node0, &subtri1 ), "s1" );

  TRY( subtri_orient( subtri0, node0, &n0, &n1, &n2 ), "orient0");
  node2 = n2;
  TRY( subtri_orient( subtri1, node1, &n0, &n1, &n2 ), "orient1");
  node3 = n2;

  orig_area = MIN( subtri_reference_area( subtri0 ),
		   subtri_reference_area( subtri1 ) );

  new_area = MIN( subnode_area( node1, node2, node3 ),
		  subnode_area( node0, node3, node2 ) );

  if ( new_area > orig_area ) 
    return triangle_swap_side( triangle, node0, node1 );

  return KNIFE_NOT_IMPROVED;
}

KNIFE_STATUS triangle_swap_neg_area( Triangle triangle )
{
  int subtri_index;
  Subtri subtri;
  Subnode node0, node1;

  double area;
  KnifeBool improved;

  improved = FALSE;

  for ( subtri_index = 0;
	subtri_index < triangle_nsubtri(triangle); 
	subtri_index++)
    {
      subtri = triangle_subtri(triangle, subtri_index);
      area = subtri_reference_area( subtri );
      if ( area <= 0.0 )
	{
	  node0 = subtri_n0(subtri); node1 = subtri_n1(subtri);
	  if ( KNIFE_SUCCESS == 
	       triangle_swap_min_area_increase ( triangle,node0,node1 ) )
	    {
	      improved = TRUE;
	      continue;
	    }
	  node0 = subtri_n1(subtri); node1 = subtri_n2(subtri);
	  if ( KNIFE_SUCCESS == 
	       triangle_swap_min_area_increase ( triangle,node0,node1 ) )
	    {
	      improved = TRUE;
	      continue;
	    }
	  node0 = subtri_n2(subtri); node1 = subtri_n0(subtri);
	  if ( KNIFE_SUCCESS == 
	       triangle_swap_min_area_increase ( triangle,node0,node1 ) )
	    {
	      improved = TRUE;
	      continue;
	    }
	}
    }

  return ( improved ? KNIFE_SUCCESS : KNIFE_NOT_IMPROVED );
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
  Subnode subnode;
  Intersection intersection;

  for ( subnode_index = 0;					
	subnode_index < triangle_nsubnode(triangle);	
	subnode_index++)					
    {
      subnode = triangle_subnode(triangle,subnode_index);
      intersection =  subnode_intersection( subnode );
      printf("%2d   u %.17f v %.17f w %.17f sn %p int %p\n",subnode_index,
	     subnode->uvw[0],
	     subnode->uvw[1],
	     subnode->uvw[2],
	     (void *)subnode,
	     (void *)intersection);
      if (NULL != intersection)
	{
	  printf("int u %.17f v %.17f w %.17f t %.17f tri %p seg %p\n", 
		 intersection->uvw[0],
		 intersection->uvw[1],
		 intersection->uvw[2],
		 intersection->t,
		 (void *)(intersection->triangle),
		 (void *)(intersection->segment));
	}
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
	    printf("  %2d u %.17f v %.17f w %.17f\n",	other_index,
		   triangle_subnode(triangle,other_index)->uvw[0],
		   triangle_subnode(triangle,other_index)->uvw[1],
		   triangle_subnode(triangle,other_index)->uvw[2]);
	}
    }
}

KNIFE_STATUS triangle_provable_recovery( Triangle triangle, 
					 Subnode node0, Subnode node1 )
{
  KNIFE_STATUS blocking_code, next_status;
  Subnode side0, side1;
  Subtri subtri;
  Loop loop0, loop1;

  if ( NULL == node0 || NULL == node1 ) return KNIFE_NULL;
  
  blocking_code = triangle_first_blocking_side( triangle, node0, node1, 
						&side0, &side1 );
  
  /* return successfully if the subtriangle already exsits */
  if ( KNIFE_RECOVERED == blocking_code ) return KNIFE_SUCCESS;
  TRY( blocking_code, "first blocking side not found" );

  TRY( triangle_subtri_with_subnodes( triangle, side0, side1, &subtri ),
       "no first subtri found" );

  loop0 = loop_create( );

  NOT_NULL( loop0, "loop creation" );
  TRY( loop_add_subtri( loop0, subtri ), "subtri not added to loop" );
  /* do not delete, loop will tecplot them KNIFE_MEMORY_LEAK */
  TRY( triangle_remove_subtri( triangle, subtri ), "subtri remove" );

  next_status = KNIFE_SUCCESS;

  while ( KNIFE_SUCCESS == next_status )
    {
      next_status = triangle_next_blocking_side( triangle, 
						 side1, side0, 
						 node0, node1,
						 &side0, &side1 );
      if ( KNIFE_NO_MAS == next_status) 
	{
	  TRY( triangle_subtri_with_subnodes( triangle, side1, side0, &subtri ),
	       "no next subtri found" );
	}
      else
	{
	  if ( KNIFE_SUCCESS != next_status )
	    {
	      triangle_tecplot(triangle);
	      loop_tecplot( loop0 );      
	    }
	  TRY( next_status, "next block" );
	  TRY( triangle_subtri_with_subnodes( triangle, side0, side1, &subtri ),
	       "no next subtri found" );
	}
	  
      TRY( loop_add_subtri( loop0, subtri ), "subtri not added to loop" );
      TRY( triangle_remove_subtri( triangle, subtri ), "subtri remove" );
    }

  /* split loop between node0 and node1 */
  TRY( loop_split( loop0, node0, node1, &loop1 ), "loop hard edge");

  TRY( loop_triangulate( loop0, triangle ), "loop0 triangulate");
  TRY( loop_triangulate( loop1, triangle ), "loop1 triangulate");

  loop_free(loop0);
  loop_free(loop1);

  return KNIFE_SUCCESS;
}


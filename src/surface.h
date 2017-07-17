
/* collection of segments and triangles defining a surface */

/* Copyright 2007 United States Government as represented by the
 * Administrator of the National Aeronautics and Space
 * Administration. No copyright is claimed in the United States under
 * Title 17, U.S. Code.  All Other Rights Reserved.
 *
 * The knife platform is licensed under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef SURFACE_H
#define SURFACE_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct SurfaceStruct SurfaceStruct;
typedef SurfaceStruct * Surface;
END_C_DECLORATION

#include "set.h"
#include "primal.h"
#include "node.h"
#include "segment.h"
#include "triangle.h"

BEGIN_C_DECLORATION

struct SurfaceStruct {
  int nnode;
  NodeStruct *node;
  int *primal_node_index;
  int nsegment;
  SegmentStruct *segment;
  int ntriangle;
  TriangleStruct *triangle;
};

Surface surface_from( Primal, Set of_bcs, KnifeBool inward_pointing_normal );

void surface_free( Surface );

#define surface_nnode(surface) ((surface)->nnode)
#define surface_node(surface,node_index) \
  (&((surface)->node[(node_index)]))
#define surface_node_index(surface,this_node) \
  ( (int)( (this_node) - ((surface)->node) ) )

#define surface_nsegment(surface) ((surface)->nsegment)
#define surface_segment(surface,segment_index) \
  (&((surface)->segment[(segment_index)]))

#define surface_ntriangle(surface) ((surface)->ntriangle)
#define surface_triangle(surface,triangle_index) \
  (&((surface)->triangle[(triangle_index)]))
#define surface_triangle_index(surface,this_triangle) \
  ( (int)( (this_triangle) - ((surface)->triangle) ) )

KNIFE_STATUS surface_triangulate( Surface );

KNIFE_STATUS surface_export_array( Surface, double *xyz, int *global, int *t2n);

KNIFE_STATUS surface_export_tec( Surface, char *filename );

END_C_DECLORATION

#endif /* SURFACE_H */

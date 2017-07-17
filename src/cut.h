
/* cut between two Triangles of two Intersections */

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

#ifndef CUT_H
#define CUT_H

#include "knife_definitions.h"

BEGIN_C_DECLORATION
typedef struct CutStruct CutStruct;
typedef CutStruct * Cut;
END_C_DECLORATION

#include "triangle.h"
#include "intersection.h"

BEGIN_C_DECLORATION

struct CutStruct {
  Triangle triangle0, triangle1;
  Intersection intersection0, intersection1;
};

KNIFE_STATUS cut_establish_between( Triangle, Triangle );
void cut_free( Cut );

#define cut_other_triangle(cut,triangle)				\
  ( ((triangle)==(cut)->triangle1) ? (cut)->triangle0 : (cut)->triangle1 )

#define cut_intersection0(cut) ((cut)->intersection0)
#define cut_intersection1(cut) ((cut)->intersection1)

#define cut_has_intersections(cut,int0,int1)				\
  ( ( ((cut)->intersection0==(int0)) && ((cut)->intersection1==(int1)) ) || \
    ( ((cut)->intersection1==(int0)) && ((cut)->intersection0==(int1)) ) )

END_C_DECLORATION

#endif /* CUT_H */

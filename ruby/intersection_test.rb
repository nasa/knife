#!/usr/bin/env ruby
# unit test for intersection

require 'test/unit'
require 'intersection'
require 'triangle'
require 'segment'
require 'node'

class TestIntersection < Test::Unit::TestCase
 
 def set_up
  @node0 = Node.new(  0.0,  0.0,  0.0)
  @node1 = Node.new(  1.0,  0.0,  0.0)
  @node2 = Node.new(  0.0,  1.0,  0.0)
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
  @nodedn = Node.new(0.3,0.3,-1.0)
  @nodeup = Node.new(0.3,0.3, 3.0)
  @nodemd = Node.new(0.3,1.0, 0.0)
  @segment = Segment.new(@nodedn,@nodeup)
 end
 def setup; set_up; end

 def test_of
  miss0 = Node.new(0.6,0.6,-1.0)
  miss1 = Node.new(0.6,0.6, 3.0)
  miss  = Segment.new(miss0,miss1)

  assert_nil Intersection.of(@triangle,miss)

  intersection = Intersection.of(@triangle,@segment)

  assert intersection.is_a?(Intersection), "not an intersection"
  assert_equal @triangle, intersection.triangle
  assert_equal @segment,  intersection.segment
  tol = 1.0e-15
  assert_in_delta 0.25, intersection.t, tol
  assert_in_delta 0.40, intersection.u, tol
  assert_in_delta 0.30, intersection.v, tol
  assert_in_delta 0.30, intersection.w, tol

  assert_equal [intersection], @segment.intersections

  intersection2 = Intersection.of(@triangle,@segment)
  assert( intersection.equal?(intersection2), "non-unique intersections" )
 end

 def test_volume6
  tol = 1.0e-16
  assert_in_delta( 1.0, Intersection.volume6([0.0,0.0,0.0],
                                             [1.0,0.0,0.0],
                                             [0.0,1.0,0.0],
                                             [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Intersection.volume6([0.0,0.0,0.0],
                                             [1.0,0.0,0.0],
                                             [0.0,1.0,0.0],
                                             [0.0,0.0,-1.0]), tol )
  assert_in_delta(-1.0, Intersection.volume6([0.0,0.0,0.0],
                                             [0.0,1.0,0.0],
                                             [1.0,0.0,0.0],
                                             [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Intersection.volume6([1.0,0.0,0.0],
                                             [0.0,0.0,0.0],
                                             [0.0,1.0,0.0],
                                             [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Intersection.volume6([0.0,0.0,0.0],
                                             [1.0,0.0,0.0],
                                             [0.0,0.0,1.0],
                                             [0.0,1.0,0.0]), tol )
 end

 def test_initialize
  intersection = Intersection.new(@triangle,@segment,0.1,0.2,0.3,0.4)

  assert intersection.is_a?(Intersection), "not an intersection"

  assert_equal @triangle, intersection.triangle
  assert_equal @segment,  intersection.segment

  tol = 1.0e-15
  assert_in_delta 0.1, intersection.t, tol
  assert_in_delta 0.2, intersection.u, tol
  assert_in_delta 0.3, intersection.v, tol
  assert_in_delta 0.4, intersection.w, tol
 end

 def test_uvw
  tol = 1.0e-15
  segmentmd = Segment.new(@nodeup,@nodedn)
  segmentup = Segment.new(@nodedn,@nodemd)
  segmentdn = Segment.new(@nodemd,@nodeup)
  intersection = Intersection.of(@triangle,segmentmd)

  u,v,w = intersection.uvw(@triangle)
  assert_in_delta 0.40, u, tol
  assert_in_delta 0.30, v, tol
  assert_in_delta 0.30, w, tol

  triangle = Triangle.new(segmentup,segmentup,segmentup)

  u,v,w = intersection.uvw(triangle)
  assert_in_delta 0.25, u, tol
  assert_in_delta 0.75, v, tol
  assert_in_delta 0.00, w, tol
 end

end

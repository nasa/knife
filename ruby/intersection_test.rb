#!/usr/bin/env ruby
# unit test for intersection

require 'test/unit'
require 'intersection'
require 'triangle'
require 'segment'
require 'node'

class TestIntersection < Test::Unit::TestCase
 
 def test_of
  node0 = Node.new(  0.0,  0.0,  0.0)
  node1 = Node.new(  1.0,  0.0,  0.0)
  node2 = Node.new(  0.0,  1.0,  0.0)

  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  miss0 = Node.new(0.6,0.6,-1.0)
  miss1 = Node.new(0.6,0.6, 3.0)
  miss  = Segment.new(miss0,miss1)

  assert_nil Intersection.of(triangle,miss)

  nodedn = Node.new(0.3,0.3,-1.0)
  nodeup = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodedn,nodeup)

  intersection = Intersection.new(triangle,segment)

  assert intersection.is_a?(Intersection), "not an intersection"
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
  node0 = Node.new(  0.0,  0.0,  0.0)
  node1 = Node.new(  1.0,  0.0,  0.0)
  node2 = Node.new(  0.0,  1.0,  0.0)

  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  nodedn = Node.new(0.3,0.3,-1.0)
  nodeup = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodedn,nodeup)

  intersection = Intersection.new(triangle,segment)

  assert intersection.is_a?(Intersection), "not an intersection"

  assert_equal triangle, intersection.triangle
  assert_equal segment, intersection.segment

  tol = 1.0e-15
  assert_in_delta 0.25, intersection.t, tol
  assert_in_delta 0.4, intersection.u, tol
  assert_in_delta 0.3, intersection.v, tol
  assert_in_delta 0.3, intersection.w, tol
 end

end

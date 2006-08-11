#!/usr/bin/env ruby
# unit test for intersection

require 'test/unit'
require 'intersection'
require 'triangle'
require 'segment'
require 'node'

class TestIntersection < Test::Unit::TestCase
 
 def test_initialize
  node0 = Node.new(  0.0,  0.0,  0.0)
  node1 = Node.new(  1.0,  0.0,  0.0)
  node2 = Node.new(  0.0,  1.0,  0.0)

  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  nodedn = Node.new(0.3,0.3,-1.0)
  nodeup = Node.new(0.3,0.3, 1.0)
  segment = Segment.new(nodedn,nodeup)

  intersection = Intersection.new(triangle,segment)

  assert intersection.is_a?(Intersection), "not an intersection"

  assert_equal triangle, intersection.triangle
  assert_equal segment, intersection.segment
 end

end

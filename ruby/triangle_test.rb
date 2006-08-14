#!/usr/bin/env ruby
# unit test for triangle

require 'test/unit'
require 'node'
require 'segment'
require 'triangle'

# todo
# - test that segments from a continuous loop

class TestTriangle < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def test_initialize
  node0 = Node.new(0.0, 0.1, 0.2)
  node1 = Node.new(1.0, 1.1, 1.2)
  node2 = Node.new(2.0, 2.1, 2.2)

  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  assert triangle.is_a?(Triangle), "not a triangle"

  assert_equal segment0, triangle.segment(0)
  assert_equal segment1, triangle.segment(1)
  assert_equal segment2, triangle.segment(2)

  assert_equal [], triangle.cuts
 end

 def test_node
  node0 = Node.new(0.0, 0.1, 0.2)
  node1 = Node.new(1.0, 1.1, 1.2)
  node2 = Node.new(2.0, 2.1, 2.2)

  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  assert_equal node0, triangle.original_node0
  assert_equal node1, triangle.original_node1
  assert_equal node2, triangle.original_node2
 end

end

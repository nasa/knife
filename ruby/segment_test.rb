#!/usr/bin/env ruby
# unit test for segment

require 'test/unit'
require 'node'
require 'segment'

class TestSegment < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def test_initialize
  node0 = Node.new(0.0, 0.1, 0.2)
  node1 = Node.new(1.0, 1.1, 1.2)

  segment = Segment.new(node0,node1)

  assert segment.is_a?(Segment), "not a segment"

  assert_equal node0, segment.node0
  assert_equal node1, segment.node1

  assert_equal [], segment.intersections
 end

 def test_common_node
  node0 = Node.new(0.0, 0.1, 0.2)
  node1 = Node.new(1.0, 1.1, 1.2)
  node2 = Node.new(2.0, 2.1, 2.2)
  node3 = Node.new(3.0, 3.1, 3.2)
  segment01 = Segment.new(node0,node1)
  segment10 = Segment.new(node1,node0)
  segment12 = Segment.new(node1,node2)
  segment21 = Segment.new(node2,node1)
  segment23 = Segment.new(node2,node3)

  assert_nil segment01.common_node(segment23)

  assert_equal node1, segment01.common_node(segment12)
  assert_equal node1, segment10.common_node(segment12)
  assert_equal node1, segment01.common_node(segment21)
  assert_equal node1, segment10.common_node(segment12)
 end

end

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

  assert_equal node0, segment.node(0)
  assert_equal node1, segment.node(1)

  assert_equal [], segment.intersections
 end

end

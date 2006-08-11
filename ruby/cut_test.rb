#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'cut'
require 'node'
require 'segment'
require 'triangle'

class TestCut < Test::Unit::TestCase
 
 def test_class
  node0 = Node.new(0.0,0.0,0.0)
  node1 = Node.new(1.0,0.0,0.0)
  node2 = Node.new(0.0,1.0,0.0)
  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)
  triangle = Triangle.new(segment0,segment1,segment2)
  cut = Cut.new(triangle,triangle)
  assert cut.is_a?(Cut), "not a Cut"
 end

end

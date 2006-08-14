#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'triangle_node'
require 'node'
require 'segment'
require 'triangle'

class TestTriangleNode < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def set_up
  @node0 = Node.new(  0.0,  0.1,  0.2)
  @node1 = Node.new(  1.0,  0.3,  1.4)
  @node2 = Node.new(  0.5,  1.1,  0.6)
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
 end
 def setup; set_up; end

 def test_initialize_from_node
  triangle_node = TriangleNode.new(@triangle,0)
  assert_in_delta 1.0, triangle_node.u, TOL
  assert_in_delta 0.0, triangle_node.v, TOL
  assert_in_delta 0.0, triangle_node.w, TOL
  triangle_node = TriangleNode.new(@triangle,1)
  assert_in_delta 0.0, triangle_node.u, TOL
  assert_in_delta 1.0, triangle_node.v, TOL
  assert_in_delta 0.0, triangle_node.w, TOL
  triangle_node = TriangleNode.new(@triangle,2)
  assert_in_delta 0.0, triangle_node.u, TOL
  assert_in_delta 0.0, triangle_node.v, TOL
  assert_in_delta 1.0, triangle_node.w, TOL
 end


end

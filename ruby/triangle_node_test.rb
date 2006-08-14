#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'triangle_node'
require 'node'
require 'segment'
require 'triangle'

class TestTriangleNode < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def test_initialize_from_node
  triangle_node = TriangleNode.new(0)
  assert_in_delta 1.0, triangle_node.u, TOL
  assert_in_delta 0.0, triangle_node.v, TOL
  assert_in_delta 0.0, triangle_node.w, TOL
  triangle_node = TriangleNode.new(1)
  assert_in_delta 0.0, triangle_node.u, TOL
  assert_in_delta 1.0, triangle_node.v, TOL
  assert_in_delta 0.0, triangle_node.w, TOL
  triangle_node = TriangleNode.new(2)
  assert_in_delta 0.0, triangle_node.u, TOL
  assert_in_delta 0.0, triangle_node.v, TOL
  assert_in_delta 1.0, triangle_node.w, TOL
 end


end

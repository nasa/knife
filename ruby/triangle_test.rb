#!/usr/bin/env ruby

require 'test/unit'
require 'triangle'

class TestTriangle < Test::Unit::TestCase

 def set_up
  @n0 = [0.0, 0.0, 0.0]
  @n1 = [1.0, 0.0, 0.0]
  @n2 = [0.0, 1.0, 0.0]
  @triangle = Triangle.new(@n0,@n1,@n2)
 end
 def setup; set_up; end

 def test_initial_nodes_and_children
  assert_equal 3, @triangle.nodes.size
  assert_equal @n0, @triangle.nodes[0]
  assert_equal @n1, @triangle.nodes[1]
  assert_equal @n2, @triangle.nodes[2]
  assert_equal 1, @triangle.children.size
  assert_equal [0,1,2], @triangle.children[0]
 end

 def test_add_node
  
 end

 def test_cut_with_no_change
  n0 = [2.0, -1.0, -1.0]
  n1 = [2.0,  1.5, -1.0]
  n2 = [2.0,  1.5,  1.0]
  assert_equal false, @triangle.cut_with(n0,n1,n2)
  assert_equal 3, @triangle.nodes.size
  assert_equal 1, @triangle.children.size
 end

 def test_cut_with_into_two_pieces
  # 2
  # | \  
  # |   +
  # |   | \
  # 0 - + - 1
  n0 = [0.8, -1.0,  1.0]
  n1 = [0.8, -1.0, -2.0]
  n2 = [0.8,  2.0,  1.0]
  assert_equal true, @triangle.cut_with(n0,n1,n2)
  assert_equal [0.8, 0.0, 0.0], @triangle.nodes[3]
  assert_equal [0.8, 0.2, 0.0], @triangle.nodes[4]
 end

 def test_one_norm
  n0 = [0.3, 0.3, 0.0]
  n1 = [0.5, 0.5, 0.2]
  tol = 1.0e-14
  assert_in_delta 0.0, @triangle.one_norm(n0,n0), tol
  assert_in_delta 0.0, @triangle.one_norm(n1,n1), tol
  assert_in_delta 0.6, @triangle.one_norm(n0,n1), tol
  assert_in_delta 0.6, @triangle.one_norm(n0,n1), tol
 end

 def test_add_unique_node
  assert_equal 0, @triangle.add_unique_node(@n0)
  assert_equal 3, @triangle.nodes.size
  node = [0.3, 0.3, 0.0]
  assert_equal 3, @triangle.add_unique_node(node)
  assert_equal 4, @triangle.nodes.size
  assert_equal 3, @triangle.add_unique_node(node)
  assert_equal 4, @triangle.nodes.size
 end


end

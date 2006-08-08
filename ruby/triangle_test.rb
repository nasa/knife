#!/usr/bin/env ruby

require 'test/unit'
require 'triangle'

class TestTriangle < Test::Unit::TestCase

 def array_in_delta(a,b,tol=1.0e-14)
  a.size.times do |i|
   assert_in_delta a[i], b[i], tol, "element #{i}"
  end
 end

 def set_up
  @n0 = [0.0, 0.0, 0.0]
  @n1 = [1.0, 0.0, 0.0]
  @n2 = [0.0, 1.0, 0.0]
  @triangle = Triangle.new(@n0,@n1,@n2)
 end
 def setup; set_up; end

 def test_initial_nodes
  assert_equal 3, @triangle.nodes.size
  assert_equal @n0, @triangle.nodes[0]
  assert_equal @n1, @triangle.nodes[1]
  assert_equal @n2, @triangle.nodes[2]
 end

 def test_center
  tol = 1.0e-15
  assert_in_delta (1.0/3.0), @triangle.center[0], tol
  assert_in_delta (1.0/3.0), @triangle.center[1], tol
  assert_in_delta 0.0, @triangle.center[2], tol
 end

 def test_diameter
  assert_in_delta 0.74543052809918, @triangle.diameter, 1.0e-12
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

 def test_splits_nil_when_misses_top
  n0 = [0.3,  0.3, -1.0]
  n1 = [0.3,  0.3, -2.0]
  assert_nil @triangle.splits(n0,n1)
  assert_nil @triangle.splits(n1,n0)
 end

 def test_splits_nil_when_misses_bottom
  n0 = [0.3,  0.3, -1.0]
  n1 = [0.3,  0.3, -2.0]
  assert_nil @triangle.splits(n0,n1)
  assert_nil @triangle.splits(n1,n0)
 end

 def test_splits_nil_when_misses_side
  n0 = [1.0,  1.0,  1.0]
  n1 = [1.0,  1.0, -1.0]
  assert_nil @triangle.splits(n0,n1)
  assert_nil @triangle.splits(n1,n0)
 end

 def test_splits_returns_a_node_at_center
  n0 = [0.3,  0.3,  1.0]
  n1 = [0.3,  0.3, -1.0]
  target = [0.3,  0.3, 0.0]
  tol = 1.0e-14
  ans = @triangle.splits(n0,n1)
  assert_in_delta target[0], ans[0], tol
  assert_in_delta target[1], ans[1], tol
  assert_in_delta target[2], ans[2], tol
  ans = @triangle.splits(n1,n0)
  assert_in_delta target[0], ans[0], tol
  assert_in_delta target[1], ans[1], tol
  assert_in_delta target[2], ans[2], tol
 end

 def test_splits_returns_a_node_at_offset
  n0 = [0.3,  0.3,  0.5]
  n1 = [0.3,  0.3, -1.5]
  target = [0.3,  0.3, 0.0]
  tol = 1.0e-14
  ans = @triangle.splits(n0,n1)
  assert_in_delta target[0], ans[0], tol
  assert_in_delta target[1], ans[1], tol
  assert_in_delta target[2], ans[2], tol
  ans = @triangle.splits(n1,n0)
  assert_in_delta target[0], ans[0], tol
  assert_in_delta target[1], ans[1], tol
  assert_in_delta target[2], ans[2], tol
 end


 def test_cut_with_no_change
  n0 = [2.0, -1.0, -1.0]
  n1 = [2.0,  1.5, -1.0]
  n2 = [2.0,  1.5,  1.0]
  assert_equal false, @triangle.cut_with(Triangle.new(n0,n1,n2))
  assert_equal 3, @triangle.nodes.size
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
  assert_equal true, @triangle.cut_with(Triangle.new(n0,n1,n2))
  array_in_delta [0.8, 0.0, 0.0], @triangle.nodes[3]
  array_in_delta [0.8, 0.2, 0.0], @triangle.nodes[4]
 end

 def Xtest_it
  node = [0.8, 0.0, 0.0]
  @triangle.sub
  @triangle.add_unique_node(node)
  @triangle.sub
  node = [0.8, 0.2, 0.0]
  @triangle.add_unique_node(node)
  @triangle.sub
  dump
 end

 def dump(filename='dump.t')
  File.open(filename,'w') do |f|
   f.print @triangle.tecplot_header
   f.print @triangle.tecplot_zone
  end
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

 def test_volume6
  tol = 1.0e-16
  assert_in_delta( 1.0, @triangle.volume6([0.0,0.0,0.0],
                                          [1.0,0.0,0.0],
                                          [0.0,1.0,0.0],
                                          [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, @triangle.volume6([0.0,0.0,0.0],
                                          [1.0,0.0,0.0],
                                          [0.0,1.0,0.0],
                                          [0.0,0.0,-1.0]), tol )
  assert_in_delta(-1.0, @triangle.volume6([0.0,0.0,0.0],
                                          [0.0,1.0,0.0],
                                          [1.0,0.0,0.0],
                                          [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, @triangle.volume6([1.0,0.0,0.0],
                                          [0.0,0.0,0.0],
                                          [0.0,1.0,0.0],
                                          [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, @triangle.volume6([0.0,0.0,0.0],
                                          [1.0,0.0,0.0],
                                          [0.0,0.0,1.0],
                                          [0.0,1.0,0.0]), tol )
 end

end

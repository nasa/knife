#!/usr/bin/env ruby
# unit test for ring

require 'test/unit'
require 'ring'

class TestRing< Test::Unit::TestCase

 def test_initialize
  assert Ring.new.is_a?(Ring), "not a ring"
  assert Ring.new.empty?, "not empty"
 end

 def test_insert_segment
  segment = [0, 1]
  ring = Ring.new

  assert_equal ring, ring.insert_segment(segment)
  assert_equal 1, ring.size
  assert_equal segment, ring.first

  assert_equal ring, ring.insert_segment(segment)
  assert_equal 1, ring.size
  assert_equal segment, ring.first

  assert_equal ring, ring.insert_segment(segment.reverse)
  assert_equal 0, ring.size
  assert ring.empty?
 end

 def test_insert_child
  # 2 - 3
  # | \ |
  # 0 - 1

  ring = Ring.new
  assert_equal ring, ring.insert_child([0,1,2])
  assert_equal 3, ring.size
  assert_equal [[0,1],[1,2],[2,0]], ring

  assert_equal ring, ring.insert_child([1,3,2])
  assert_equal 4, ring.size
  assert_equal [[0,1],[2,0],[1,3],[3,2]], ring
 end

 def test_has?
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  joker = Node.new(5.0,5.0)

  ring = Ring.new.insert_segment([node1,node0])
  assert_equal true, ring.has?(node0)
  assert_equal true, ring.has?(node1)
  assert_equal false, ring.has?(joker)
 end

 def test_find_segment_starting_with
  segment = [0, 1]
  ring = Ring.new.insert_segment([1, 2]).insert_segment([0, 1])

  assert_nil ring.find_segment_starting_with(5)
  assert_nil ring.find_segment_starting_with(2)

  assert_equal [0,1], ring.find_segment_starting_with(0)
  assert_equal [1,2], ring.find_segment_starting_with(1)
 end

 def test_order!
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  node2 = Node.new(2.0,0.0)
  segment = [node0, node1]
  ring = Ring.new.insert_segment([node1, node2]).insert_segment([node0, node1])

  assert_equal [[node1,node2],[node0,node1]], ring

  joker = Node.new(5.0,5.0)

  assert_nil ring.order!(joker)
  assert_equal [[node1,node2],[node0,node1]], ring

  assert_nil ring.order!(node1)
  assert_equal [[node1,node2],[node0,node1]], ring

  assert_equal ring, ring.order!(node0)
  assert_equal [[node0,node1],[node1,node2]], ring
 end

 def test_split
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  node2 = Node.new(0.0,1.0)
  node3 = Node.new(1.0,1.0)
  triangle0 = Triangle.new(node0,node1,node2)
  triangle1 = Triangle.new(node2,node1,node3)

  ring0 = Ring.new.insert_triangle(triangle0).insert_triangle(triangle1)

  joker = Node.new(5.0,5.0)
  assert_nil ring0.split!(node0,joker)
  assert_nil ring0.split!(joker,node3)

  ring1 = ring0.split!(node0,node3)
  assert ring1.is_a?(Ring), "not a ring"

  assert_equal [[node0,node1],[node1,node3],[node3,node0]], ring0
  assert_equal [[node3,node2],[node2,node0],[node0,node3]], ring1
 end

 def test_triangulate_one_triangle
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  node2 = Node.new(0.0,1.0)
  triangle = Triangle.new(node0,node1,node2)
  ring = Ring.new.insert_triangle(triangle)
  triangles = ring.triangulate
  assert_equal 1, triangles.size
  assert_equal triangle.n0, triangles.first.n0
  assert_equal triangle.n1, triangles.first.n1
  assert_equal triangle.n2, triangles.first.n2
 end

 def test_triangulate_two_triangles
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  node2 = Node.new(0.0,1.0)
  node3 = Node.new(1.0,1.0)
  ans = [node0, node1, node3, node2, node0]
  triangle0 = Triangle.new(node0,node1,node2)
  triangle1 = Triangle.new(node2,node1,node3)
  ring = Ring.new.insert_triangle(triangle0).insert_triangle(triangle1)
  triangles = ring.triangulate
  assert_equal 2, triangles.size
  assert_equal node0, triangles[0].n0
  assert_equal node1, triangles[0].n1
  assert_equal node3, triangles[0].n2
  assert_equal node0, triangles[1].n0
  assert_equal node3, triangles[1].n1
  assert_equal node2, triangles[1].n2
 end

 def Xtest_from_connection_of_two_triangles
  node0 = Node.new(0.0,0.0)
  node1 = Node.new(1.0,0.0)
  node2 = Node.new(0.0,1.0)
  node3 = Node.new(1.0,1.0)
  ans = [node0, node1, node3, node2, node0]
  triangle0 = Triangle.new(node0,node1,node2)
  triangle1 = Triangle.new(node2,node1,node3)
  connection = [triangle0,triangle1]
  ring = Ring.from_connection(node0,node3,connection)
  assert ring.is_a?(Ring), "not a ring"
  assert_equal ans, ring
 end

 def Xtest_from_connection_of_four_triangles
  node0 = Node.new( 0.0,0.0)
  node1 = Node.new( 1.0,1.0)
  node2 = Node.new( 1.0,2.0)
  node3 = Node.new( 0.0,3.0)
  node4 = Node.new(-1.0,2.0)
  node5 = Node.new(-1.0,1.0)
  ans = [node0, node1, node2, node3, node4, node5, node0]
  triangle0 = Triangle.new(node0,node1,node5)
  triangle1 = Triangle.new(node1,node4,node5)
  triangle2 = Triangle.new(node2,node4,node1)
  triangle3 = Triangle.new(node3,node4,node2)
  connection = [triangle0,triangle1,triangle2,triangle3]
  ring = Ring.from_connection(node0,node3,connection)
  assert_equal ans, ring
 end

end

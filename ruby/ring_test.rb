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
  ring = Ring.new.insert_segment([1,0])
  assert_equal true, ring.has?(0)
  assert_equal true, ring.has?(1)
  assert_equal false, ring.has?(5)
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
  ring = Ring.new.insert_segment([1, 2]).insert_segment([0, 1])

  assert_equal [[1,2],[0,1]], ring

  assert_nil ring.order!(5)
  assert_equal [[1,2],[0,1]], ring

  assert_nil ring.order!(1)
  assert_equal [[1,2],[0,1]], ring

  assert_equal ring, ring.order!(0)
  assert_equal [[0,1],[1,2]], ring
 end

 def test_split

  ring0 = Ring.new.insert_child([0,1,2]).insert_child([2,1,3])

  assert_nil ring0.split!(0,5)
  assert_nil ring0.split!(5,3)

  ring1 = ring0.split!(0,3)
  assert ring1.is_a?(Ring), "not a ring"

  assert_equal [[0,1],[1,3],[3,0]], ring0
  assert_equal [[3,2],[2,0],[0,3]], ring1
 end

 def test_triangulate_one_triangle
  ring = Ring.new.insert_child([0,1,2])
  triangles = ring.triangulate
  assert_equal 1, triangles.size
  assert_equal [0,1,2], triangles.first
 end

 def test_triangulate_two_triangles
  ring = Ring.new.insert_child([0,1,2]).insert_child([2,1,3])
  triangles = ring.triangulate
  assert_equal 2, triangles.size
  assert_equal [0,1,3], triangles[0]
  assert_equal [0,3,2], triangles[1]
 end

end

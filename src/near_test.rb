#!/usr/bin/env ruby
#
# Mobility test for near c lib
#
# $Id$

Dir.chdir ENV['srcdir'] if ENV['srcdir']

require 'RubyExtensionBuilder'

RubyExtensionBuilder.build('Near')

require 'test/unit'
require 'Near/Near'

class TestNear < Test::Unit::TestCase

 def test_store_index
  near = Near.new(5,0,0,0,0)
  assert_equal 5, near.index
 end

 def test_node_point_to_point_clearance
  point1 = Near.new(5,0,0,0,0)
  point2 = Near.new(5,1,0,0,0)
  assert_equal 1, point1.clearance(point2)

  point2 = Near.new(5,2,0,0,0)
  assert_equal 2, point1.clearance(point2)

  point2 = Near.new(5,0,1,0,0)
  assert_equal 1, point1.clearance(point2)

  point2 = Near.new(5,0,0,1,0)
  assert_equal 1, point1.clearance(point2)
 end

 def test_node_clearance_with_sphere_radius
  sphere1 = Near.new(5,0,0,0,1)
  sphere2 = Near.new(5,1,0,0,1)
  assert_equal(-1, sphere1.clearance(sphere2))

  sphere2 = Near.new(5,3,0,0,1)
  assert_equal 1, sphere1.clearance(sphere2)
 end
 
 def test_initialize_with_no_children
  near = Near.new(5,0,0,0,0)
  assert_equal( -1, near.left_index)
  assert_equal( -1, near.right_index)
 end
 
 def test_insert_left_child_first
  near  = Near.new(5,0,0,0,0)
  child = Near.new(6,1,0,0,0)
  assert_equal near, near.insert(child)
  assert_equal 6, near.left_index
  assert_equal(-1, near.right_index)
 end
 
 def test_insert_right_child_second
  near  = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,0)
  child7 = Near.new(7,1,0,0,0)
  assert_equal near, near.insert(child6)
  assert_equal near, near.insert(child7)
  assert_equal 6, near.left_index
  assert_equal 7, near.right_index
 end
 
 def test_insert_left_closer
  near  = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,0)
  child7 = Near.new(7,2,0,0,0)
  child8 = Near.new(8,1,0,0,0)
  assert_equal near, near.insert(child6)
  assert_equal near, near.insert(child7)
  assert_equal near, near.insert(child8)
  assert_equal 6, near.left_index
  assert_equal 7, near.right_index
  assert_equal 8, child6.left_index
 end
 
 def test_insert_right_closer
  near  = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,0)
  child7 = Near.new(7,2,0,0,0)
  child8 = Near.new(8,2,0,0,0)
  assert_equal near, near.insert(child6)
  assert_equal near, near.insert(child7)
  assert_equal near, near.insert(child8)
  assert_equal 6, near.left_index
  assert_equal 7, near.right_index
  assert_equal 8, child7.left_index
 end
 
 def test_initialize_child_radius
  near  = Near.new(5,0,0,0,0)
  assert_equal 0, near.left_radius
  assert_equal 0, near.right_radius
 end

 def test_compute_point_child_radius
  near   = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,0)
  child7 = Near.new(7,2,0,0,0)
  assert_equal near, near.insert(child6)
  assert_equal 1, near.left_radius
  assert_equal 0, near.right_radius
  assert_equal near, near.insert(child7)
  assert_equal 1, near.left_radius
  assert_equal 2, near.right_radius
 end

 def test_compute_sphere_child_radius
  near   = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,2)
  child7 = Near.new(7,4,0,0,1)
  assert_equal near, near.insert(child6)
  assert_equal 3, near.left_radius
  assert_equal near, near.insert(child7)
  assert_equal 5, near.right_radius
 end

 def test_compute_point_child_radius_in_tree
  near   = Near.new(5,0,0,0,0)
  child6 = Near.new(6,1,0,0,0)
  child7 = Near.new(7,8,0,0,0)
  child8 = Near.new(8,3,0,0,0)
  assert_equal near, near.insert(child6)
  assert_equal near, near.insert(child7)
  assert_equal 1, near.left_radius
  assert_equal near, near.insert(child8)
  assert_equal 3, near.left_radius
  assert_equal 8, child6.left_index
  assert_equal 2, child6.left_radius
 end

 def test_single_collisions
  near        = Near.new(5,0,0,0,0)
  smalltarget = Near.new(6,1,0,0,0)
  largetarget = Near.new(7,1,0,0,2)
  assert_equal 0, near.collisions(smalltarget)
  assert_equal [], near.touched(smalltarget)
  assert_equal 1, near.collisions(largetarget)
  assert_equal [5], near.touched(largetarget)
 end

 def test_tree_collisions
  near   = Near.new(5,0,0,0,1)
  child6 = Near.new(6,4,0,0,2)
  child7 = Near.new(7,8,0,0,1)
  assert_equal near, near.insert(child6)
  assert_equal near, near.insert(child7)
  target = Near.new(8,10,0,0,0)
  assert_equal 0, near.collisions(target)
  assert_equal [], near.touched(target)
  target = Near.new(8,10,0,0,2)
  assert_equal 1, near.collisions(target)
  assert_equal [7], near.touched(target)
  target = Near.new(8,10,0,0,5)
  assert_equal 2, near.collisions(target)
  assert_equal [6,7], near.touched(target)
  target = Near.new(8,10,0,0,10)
  assert_equal 3, near.collisions(target)
  assert_equal [6,7,5], near.touched(target)
  target = Near.new(8,5,0,0,1)
  assert_equal 1, near.collisions(target)
  assert_equal [6], near.touched(target)
 end

end

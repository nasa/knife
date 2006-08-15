#!/usr/bin/env ruby
# unit test for triangle

require 'test/unit'
require 'node'
require 'segment'
require 'triangle'
require 'intersection'
require 'cut'

# todo
# - test that segments from a continuous loop

class TestTriangle < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def set_up
  @node0 = Node.new(  0.0,  0.0,  0.0)
  @node1 = Node.new(  1.0,  0.0,  0.0)
  @node2 = Node.new(  0.0,  1.0,  0.0)
 end
 def setup; set_up; end

 def test_initialize
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)

  triangle = Triangle.new(segment0,segment1,segment2)

  assert triangle.is_a?(Triangle), "not a triangle"

  assert_equal segment0, triangle.segment(0)
  assert_equal segment1, triangle.segment(1)
  assert_equal segment2, triangle.segment(2)

  assert_equal 3, triangle.subnodes.size
  assert_equal 1, triangle.subtris.size
 end

 def test_node
  segment0  = Segment.new(@node1,@node2)
  segment0r = Segment.new(@node2,@node1)
  segment1  = Segment.new(@node2,@node0)
  segment1r = Segment.new(@node0,@node2)
  segment2  = Segment.new(@node0,@node1)
  segment2r = Segment.new(@node1,@node0)

  triangle = Triangle.new(segment0,segment1,segment2)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2

  triangle = Triangle.new(segment0r,segment1,segment2)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2

  triangle = Triangle.new(segment0,segment1r,segment2)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2

  triangle = Triangle.new(segment0,segment1,segment2r)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2

  triangle = Triangle.new(segment0r,segment1r,segment2)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2

  triangle = Triangle.new(segment0r,segment1r,segment2r)
  assert_equal @node0, triangle.original_node0
  assert_equal @node1, triangle.original_node1
  assert_equal @node2, triangle.original_node2
 end

 def test_add_unique_subnode
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)

  triangle = Triangle.new(segment0,segment1,segment2)
  nodea = Node.new(0.3,0.3,-1.0)
  nodeb = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodea,nodeb)
  intersection = Intersection.of(triangle,segment)
  
  first_node = triangle.add_unique_subnode(intersection)
  assert_equal 4, triangle.subnodes.size
 
  second_node = triangle.add_unique_subnode(intersection)
  assert_equal 4, triangle.subnodes.size
  assert_equal first_node, second_node

  assert_equal 3, triangle.subtris.size
 end

 def test_add_subnode_into_subtri_side
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)

  triangle = Triangle.new(segment0,segment1,segment2)
  nodea = Node.new(0.3, 0.3,-1.0)
  nodeb = Node.new(0.3, 0.3, 3.0)
  nodec = Node.new(0.3,-1.0, 0.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodea,nodec)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)
  intersection = Intersection.of(cutter,segment2)
  first_node = triangle.add_unique_subnode(intersection)  
  assert_equal 2, triangle.subtris.size
 end

 def test_find_subtri_with
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)

  triangle = Triangle.new(segment0,segment1,segment2)
  assert_nil triangle.find_subtri_with(triangle.subnodes[1],
                                       triangle.subnodes[0])
  assert_equal( triangle.subtris[0], 
                triangle.find_subtri_with(triangle.subnodes[0],
                                          triangle.subnodes[1]) )
  assert_equal( triangle.subtris[0], 
                triangle.find_subtri_with(triangle.subnodes[1],
                                          triangle.subnodes[2]) )
  assert_equal( triangle.subtris[0], 
                triangle.find_subtri_with(triangle.subnodes[2],
                                          triangle.subnodes[0]) )
 end

 def test_cut_into_two_pieces
  # 2
  # | \  
  # |   4
  # |   | \
  # 0 - 3 - 1
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)
  triangle = Triangle.new(segment0,segment1,segment2)

  nodea = Node.new(  0.8, -1.0, -1.0)
  nodeb = Node.new(  0.8,  3.0, -1.0)
  nodec = Node.new(  0.8, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  Cut.between(triangle,cutter)

  assert_equal triangle, triangle.triangulate_cuts
  assert_equal 5, triangle.subnodes.size
  assert_equal 3, triangle.subtris.size
 end

 def test_cut_requiring_reconstruction
  segment0 = Segment.new(@node1,@node2)
  segment1 = Segment.new(@node2,@node0)
  segment2 = Segment.new(@node0,@node1)
  triangle = Triangle.new(segment0,segment1,segment2)

  nodea = Node.new(  0.8, -1.0, -1.0)
  nodeb = Node.new(  0.8,  3.0, -1.0)
  nodec = Node.new(  0.8, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  Cut.between(triangle,cutter)

  nodea = Node.new(  0.5, -1.0, -1.0)
  nodeb = Node.new(  0.5,  3.0, -1.0)
  nodec = Node.new(  0.5, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  Cut.between(triangle,cutter)

  assert_equal triangle, triangle.triangulate_cuts
  triangle.eps
  assert_equal 7, triangle.subnodes.size
  assert_equal 5, triangle.subtris.size
 end

end

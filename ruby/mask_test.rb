#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'mask'
require 'node'
require 'segment'
require 'triangle'
require 'intersection'
require 'cut'

class TestMask < Test::Unit::TestCase

 def set_up
  @node0 = Node.new(  0.0,  0.0,  0.0)
  @node1 = Node.new(  1.0,  0.0,  0.0)
  @node2 = Node.new(  0.0,  1.0,  0.0)
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
  @mask = Mask.new(@triangle)
 end
 def setup; set_up; end

 def test_mask
  mask = Mask.new("tri")
  assert mask.is_a?(Mask), "not a mask"
 end

 def test_initialize_triangle
  mask = Mask.new("tri")
  assert_equal "tri", mask.triangle
 end

 def test_initialize_reversed
  mask = Mask.new("tri")
  assert_equal false, mask.reversed
  mask = Mask.new("tri",true)
  assert_equal true, mask.reversed
 end

 def test_method_missing
  mask = Mask.new("elgnairt")
  assert_equal "triangle", mask.reverse
  assert_equal "elrt", mask.delete("gnai")
 end

 def test_equals
  str = "tri"
  mask0 = Mask.new("tri")
  assert(mask0==str)
 end

 def test_deactivate_all_subtri
  assert_equal [@triangle.subtris[0]], @mask.subtris
  @mask.deactivate_all_subtri
  assert_equal [], @mask.subtris
 end

 def test_paint_inactive
  @mask.deactivate_all_subtri  
  @mask.paint
  assert_equal [], @mask.subtris
 end

 def test_activate_and_active?
  nodea = Node.new(0.3,0.3,-1.0)
  nodeb = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodea,nodeb)
  intersection = Intersection.of(@triangle,segment)
  @mask.add_unique_subnode(intersection)
  @mask.deactivate_all_subtri
  @mask.activate(@triangle.subtris[1])
  assert_equal 1, @mask.subtris.size
  assert_equal @triangle.subtris[1], @mask.subtris[0]
  assert_equal( false, @mask.active?(@triangle.subtris[0]))
  assert_equal( true,  @mask.active?(@triangle.subtris[1]))
  assert_equal( false, @mask.active?(@triangle.subtris[2]))
 end

 def test_paint_none
  nodea = Node.new(0.3,0.3,-1.0)
  nodeb = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodea,nodeb)
  intersection = Intersection.of(@triangle,segment)
  @mask.add_unique_subnode(intersection)
  @mask.deactivate_all_subtri
  @mask.paint
  assert_equal 0, @mask.subtris.size
 end

 def test_paint_them_all
  nodea = Node.new(0.3,0.3,-1.0)
  nodeb = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodea,nodeb)
  intersection = Intersection.of(@triangle,segment)
  @mask.add_unique_subnode(intersection)
  @mask.deactivate_all_subtri
  @mask.activate(@triangle.subtris[1])
  @mask.paint
  assert_equal 3, @mask.subtris.size
 end

 def test_paint_cut_into_two_pieces
  # 2
  # | \  
  # |   4
  # |   | \
  # 0 - 3 - 1
  nodea = Node.new(  0.8, -1.0, -1.0)
  nodeb = Node.new(  0.8,  3.0, -1.0)
  nodec = Node.new(  0.8, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  cut = Cut.between(@triangle,cutter)
  @triangle.triangulate_cuts
  @mask.deactivate_all_subtri
  @mask.activate(@triangle.subtris[2])  
  assert_equal 1, @mask.subtris.size
  @mask.paint
  assert_equal 2, @mask.subtris.size
  assert_equal [@triangle.subtris[0], @triangle.subtris[2]], @mask.subtris
 end

end

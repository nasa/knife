#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'cut'
require 'node'
require 'segment'
require 'triangle'

class TestCut < Test::Unit::TestCase
 
 def set_up
  @node0 = Node.new(  0.0,  0.0,  0.0)
  @node1 = Node.new(  1.0,  0.0,  0.0)
  @node2 = Node.new(  0.0,  1.0,  0.0)
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
 end
 def setup; set_up; end

 def test_non_intersecting_cut
  node0 = Node.new(  2.0, -1.0, -1.0)
  node1 = Node.new(  2.0,  1.5, -1.0)
  node2 = Node.new(  2.0,  1.5,  1.0)
  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)
  triangle = Triangle.new(segment0,segment1,segment2)
  cut = Cut.between(@triangle,triangle)
  assert_nil cut, "phantom cut"
 end

 def test_cut_with_into_two_pieces
  # 2
  # | \  
  # |   4
  # |   | \
  # 0 - 3 - 1
  node0 = Node.new(  0.8, -1.0, -1.0)
  node1 = Node.new(  0.8,  3.0, -1.0)
  node2 = Node.new(  0.8, -1.0,  3.0)
  segment0 = Segment.new(node1,node2)
  segment1 = Segment.new(node2,node0)
  segment2 = Segment.new(node0,node1)
  triangle = Triangle.new(segment0,segment1,segment2)
  cut = Cut.between(@triangle,triangle)
  assert cut.is_a?(Cut), "Cut not returned"
 end

 def test_volume6
  tol = 1.0e-16
  assert_in_delta( 1.0, Cut.volume6([0.0,0.0,0.0],
                                    [1.0,0.0,0.0],
                                    [0.0,1.0,0.0],
                                    [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Cut.volume6([0.0,0.0,0.0],
                                    [1.0,0.0,0.0],
                                    [0.0,1.0,0.0],
                                    [0.0,0.0,-1.0]), tol )
  assert_in_delta(-1.0, Cut.volume6([0.0,0.0,0.0],
                                    [0.0,1.0,0.0],
                                    [1.0,0.0,0.0],
                                    [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Cut.volume6([1.0,0.0,0.0],
                                    [0.0,0.0,0.0],
                                    [0.0,1.0,0.0],
                                    [0.0,0.0,1.0]), tol )
  assert_in_delta(-1.0, Cut.volume6([0.0,0.0,0.0],
                                    [1.0,0.0,0.0],
                                    [0.0,0.0,1.0],
                                    [0.0,1.0,0.0]), tol )
 end

end

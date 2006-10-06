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
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
 end
 def setup; set_up; end

 def test_initialize
  assert @triangle.is_a?(Triangle), "not a triangle"
 end

 def test_initialize_node
  assert_equal @node0, @triangle.node0
  assert_equal @node1, @triangle.node1
  assert_equal @node2, @triangle.node2
 end

 def test_initialize_segment
  assert_equal @segment0, @triangle.segment(0)
  assert_equal @segment1, @triangle.segment(1)
  assert_equal @segment2, @triangle.segment(2)
 end

 def test_initialize_segment_registration
  assert_equal [@triangle], @segment0.triangles
  assert_equal [@triangle], @segment1.triangles
  assert_equal [@triangle], @segment2.triangles
 end

 def test_initialize_subnode
  assert_equal 3, @triangle.subnodes.size
  assert_equal @node0, @triangle.subnodes[0].parent
  assert_equal @node1, @triangle.subnodes[1].parent
  assert_equal @node2, @triangle.subnodes[2].parent
 end

 def test_initialize_subtri
  assert_equal 1, @triangle.subtris.size
  subtri = @triangle.subtris[0]
  assert_equal @segment0, subtri.s0
  assert_equal @segment1, subtri.s1
  assert_equal @segment2, subtri.s2
 end

 def test_initialize_polyhedra
  assert_equal [], @triangle.polyhedra
 end

 def test_initialize_active
  assert_equal true, @triangle.active
 end

 def test_add_unique_subnode
  nodea = Node.new(0.3,0.3,-1.0)
  nodeb = Node.new(0.3,0.3, 3.0)
  segment = Segment.new(nodea,nodeb)
  intersection = Intersection.of(@triangle,segment)
  
  first_node = @triangle.add_unique_subnode(intersection)
  assert_equal 4, @triangle.subnodes.size
 
  second_node = @triangle.add_unique_subnode(intersection)
  assert_equal 4, @triangle.subnodes.size
  assert_equal first_node, second_node

  assert_equal 3, @triangle.subtris.size
 end

 def test_set_subtri_side
  sidenode = Node.new(0.5,0.0,0.0)
  sidesubnode = Subnode.new(0.5,0.5,0.0,sidenode)
  assert_raise RuntimeError do
   @triangle.set_subtri_side(sidenode,@triangle.subnodes[1],"dummy")
  end
  assert_equal @triangle, @triangle.set_subtri_side(@triangle.subnodes[0],
                                                    @triangle.subnodes[2])
  assert_nil @triangle.subtris[0].s1
  assert_equal @triangle, @triangle.set_subtri_side(@triangle.subnodes[0],
                                                    @triangle.subnodes[1],
                                                    "new side")
  assert_equal "new side", @triangle.subtris[0].s2
 end

 def test_add_subnode_into_subtri_side
  sidenode = Node.new(0.5,0.0,0.0)
  sidesubnode = Subnode.new(0.5,0.5,0.0,sidenode)
  @triangle.insert_subnode_into_subtri_side(sidesubnode,
                                            @triangle.subnodes[0],
                                            @triangle.subnodes[1])
  assert_equal 2, @triangle.subtris.size

  assert_equal sidesubnode,           @triangle.subtris[0].n0
  assert_equal @triangle.subnodes[1], @triangle.subtris[0].n1
  assert_equal @triangle.subnodes[2], @triangle.subtris[0].n2

  assert_equal @triangle.subnodes[0], @triangle.subtris[1].n0
  assert_equal sidesubnode,           @triangle.subtris[1].n1
  assert_equal @triangle.subnodes[2], @triangle.subtris[1].n2

  assert_equal @segment0,            @triangle.subtris[0].s0
  assert_equal @triangle.subtris[1], @triangle.subtris[0].s1
  assert_equal @segment2,            @triangle.subtris[0].s2
  assert_equal @triangle.subtris[0], @triangle.subtris[1].s0
  assert_equal @segment1,            @triangle.subtris[1].s1
  assert_equal @segment2,            @triangle.subtris[1].s2
 end

 def test_add_subnode_into_subtri_side_twice
  sidenode = Node.new(0.5,0.0,0.0)
  sidesubnode = Subnode.new(0.5,0.5,0.0,sidenode)
  @triangle.insert_subnode_into_subtri_side(sidesubnode,
                                            @triangle.subnodes[0],
                                            @triangle.subnodes[1])
  centernode = Node.new(0.5,0.0,0.0)
  centersubnode = Subnode.new(0.4,0.4,0.2,centernode)
  @triangle.insert_subnode_into_subtri_side(centersubnode,
                                            @triangle.subnodes[2],
                                            sidesubnode)
  assert_equal 4, @triangle.subtris.size
 end

 def test_add_subnode_into_subtri_side_raises
  sidenode = Node.new(0.5,0.0,0.0)
  sidesubnode = Subnode.new(0.5,0.5,0.0,sidenode)
  assert_raise RuntimeError do
   @triangle.insert_subnode_into_subtri_side("dummy",
                                             sidenode,
                                             @triangle.subnodes[1])
  end
 end

 def test_add_subnode_into_subtri_side_by_cut
  nodea = Node.new(0.3, 0.3,-1.0)
  nodeb = Node.new(0.3, 0.3, 3.0)
  nodec = Node.new(0.3,-1.0, 0.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodea,nodec)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)
  intersection = Intersection.of(cutter,@segment2)
  first_node = @triangle.add_unique_subnode(intersection)  
  assert_equal 2, @triangle.subtris.size
 end

 def test_insert_subnode_into_subtri_interior
  newnode = Node.new(0.3,0.3,0.0)
  newsubnode = Subnode.new(0.4,0.3,0.3,newnode)
  @triangle.insert_subnode_into_subtri_interior(newsubnode,@triangle.subtris[0])
  assert_equal 3, @triangle.subtris.size

  assert_equal newsubnode,            @triangle.subtris[0].n0
  assert_equal @triangle.subnodes[1], @triangle.subtris[0].n1
  assert_equal @triangle.subnodes[2], @triangle.subtris[0].n2

  assert_equal @triangle.subnodes[0], @triangle.subtris[1].n0
  assert_equal newsubnode,            @triangle.subtris[1].n1
  assert_equal @triangle.subnodes[2], @triangle.subtris[1].n2

  assert_equal @triangle.subnodes[0], @triangle.subtris[2].n0
  assert_equal @triangle.subnodes[1], @triangle.subtris[2].n1
  assert_equal newsubnode,            @triangle.subtris[2].n2

  assert_equal @segment0,            @triangle.subtris[0].s0
  assert_equal @triangle.subtris[1], @triangle.subtris[0].s1
  assert_equal @triangle.subtris[2], @triangle.subtris[0].s2

  assert_equal @triangle.subtris[0], @triangle.subtris[1].s0
  assert_equal @segment1,            @triangle.subtris[1].s1
  assert_equal @triangle.subtris[2], @triangle.subtris[1].s2

  assert_equal @triangle.subtris[0], @triangle.subtris[2].s0
  assert_equal @triangle.subtris[1], @triangle.subtris[2].s1
  assert_equal @segment2,            @triangle.subtris[2].s2
 end


 def test_find_subtri_with
  assert_nil @triangle.find_subtri_with(@triangle.subnodes[1],
                                        @triangle.subnodes[0])
  assert_equal( @triangle.subtris[0], 
                @triangle.find_subtri_with(@triangle.subnodes[0],
                                           @triangle.subnodes[1]) )
  assert_equal( @triangle.subtris[0], 
                @triangle.find_subtri_with(@triangle.subnodes[1],
                                           @triangle.subnodes[2]) )
  assert_equal( @triangle.subtris[0], 
                @triangle.find_subtri_with(@triangle.subnodes[2],
                                           @triangle.subnodes[0]) )
 end

 def test_cut_into_two_pieces
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

  assert_equal @triangle, @triangle.triangulate_cuts
  assert_equal 5, @triangle.subnodes.size
  assert_equal 3, @triangle.subtris.size
  assert_equal cut, @triangle.subtris[1].side_with_nodes(@triangle.subnodes[3],
                                                         @triangle.subnodes[4])
 end

 def test_cut_requiring_reconstruction
  nodea = Node.new(  0.8, -1.0, -1.0)
  nodeb = Node.new(  0.8,  3.0, -1.0)
  nodec = Node.new(  0.8, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  Cut.between(@triangle,cutter)

  nodea = Node.new(  0.5, -1.0, -1.0)
  nodeb = Node.new(  0.5,  3.0, -1.0)
  nodec = Node.new(  0.5, -1.0,  3.0)
  segmenta = Segment.new(nodeb,nodec)
  segmentb = Segment.new(nodec,nodea)
  segmentc = Segment.new(nodea,nodeb)
  cutter = Triangle.new(segmenta,segmentb,segmentc)

  Cut.between(@triangle,cutter)

  assert_equal @triangle, @triangle.triangulate_cuts
  assert_equal 7, @triangle.subnodes.size
  assert_equal 5, @triangle.subtris.size
 end

end

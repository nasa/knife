#!/usr/bin/env ruby

require 'test/unit'
require 'polyhedron'
require 'node'
require 'segment'
require 'triangle'
require 'cut'

class TestPolyhedron < Test::Unit::TestCase

 def set_up
  @node0 = Node.new(  0.0,  0.0,  0.0)
  @node1 = Node.new(  1.0,  0.0,  0.0)
  @node2 = Node.new(  0.0,  1.0,  0.0)
  @segment0 = Segment.new(@node1,@node2)
  @segment1 = Segment.new(@node2,@node0)
  @segment2 = Segment.new(@node0,@node1)
  @triangle = Triangle.new(@segment0,@segment1,@segment2)
  @nodea = Node.new(  0.3,  0.3, -1.0)
  @nodeb = Node.new(  0.3,  0.3,  1.0)
  @nodec = Node.new(  0.3, -1.0,  1.0)
  @segmenta = Segment.new(@nodeb,@nodec)
  @segmentb = Segment.new(@nodec,@nodea)
  @segmentc = Segment.new(@nodea,@nodeb)
  @cutter = Triangle.new(@segmenta,@segmentb,@segmentc)
 end
 def setup; set_up; end

 def test_initialize
  poly = Polyhedron.new
  assert poly.triangles.empty?
  assert poly.cutters.empty?
 end

 def test_trim_external_subtri
  poly = Polyhedron.new
  poly.add_triangle(@triangle)

  Cut.between(@triangle,@cutter)
  @triangle.triangulate_cuts
  @cutter.triangulate_cuts

  assert_equal poly, poly.gather_cutters
  assert_equal poly, poly.trim_external_subtri

  assert_equal 1, poly.triangles[0].subtris.size
  assert_equal 1, poly.cutters[0].subtris.size
  assert_equal @node1, poly.triangles[0].subtris[0].n1.parent
  assert_equal @nodeb, poly.cutters[0].subtris[0].n1.parent
 end

 def test_trim_external_subtri_reversed
  poly = Polyhedron.new
  poly.add_reversed_triangle(@triangle)

  Cut.between(@triangle,@cutter)
  @triangle.triangulate_cuts
  @cutter.triangulate_cuts

  poly.gather_cutters
  poly.trim_external_subtri

  assert_equal 1, poly.triangles[0].subtris.size
  assert_equal 1, poly.cutters[0].subtris.size
  assert_equal @node1, poly.triangles[0].subtris[0].n1.parent
  assert_equal @nodea, poly.cutters[0].subtris[0].n0.parent
 end

 def test_paint
  poly = Polyhedron.new
  poly.add_triangle(@triangle)

  Cut.between(@triangle,@cutter)
  @triangle.triangulate_cuts
  @cutter.triangulate_cuts

  poly.gather_cutters
  poly.trim_external_subtri

  assert_equal poly, poly.paint
 end

end

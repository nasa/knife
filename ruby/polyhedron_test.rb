#!/usr/bin/env ruby

require 'test/unit'
require 'polyhedron'

class TestPolyhedron < Test::Unit::TestCase

 def test_initialize
  poly = Polyhedron.new
  assert poly.triangles.empty?
  assert poly.reversed.empty?
 end

end

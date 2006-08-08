#!/usr/bin/env ruby

require 'test/unit'
require 'triangle'

class TestTriangle < Test::Unit::TestCase

 def set_up
  @n0 = [0.0, 0.0, 0.0]
  @n1 = [1.0, 0.0, 0.0]
  @n2 = [0.0, 1.0, 0.0]
  @triangle = Triangle.new(@n0,@n1,@n2)
 end
 def setup; set_up; end

 def test_initial_nodes
  assert_equal @n0, @triangle.nodes[0]
  assert_equal @n1, @triangle.nodes[1]
  assert_equal @n2, @triangle.nodes[2]
 end

end

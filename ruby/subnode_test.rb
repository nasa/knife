#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'subnode'

class TestSubnode < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def test_initialize
  subnode = Subnode.new(0.2,0.3,0.5)
  assert_in_delta 0.2, subnode.u, TOL
  assert_in_delta 0.3, subnode.v, TOL
  assert_in_delta 0.5, subnode.w, TOL
  assert_nil subnode.parent

  subnode = Subnode.new(0.2,0.3,0.5,"dummy parent")
  assert_in_delta 0.2, subnode.u, TOL
  assert_in_delta 0.3, subnode.v, TOL
  assert_in_delta 0.5, subnode.w, TOL
  assert_equal "dummy parent", subnode.parent
 end


end

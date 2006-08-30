#!/usr/bin/env ruby
# unit test for subtri

require 'test/unit'
require 'polytri'

class TestPolytri < Test::Unit::TestCase
 
 def test_initialize
  polytri = Polytri.new("n0", "n1", "n2")
  assert polytri.is_a?(Polytri), "not a polytri"
 end

 def test_initialize_3
  polytri = Polytri.new("n0", "n1", "n2")
  assert_equal "n0", polytri.n0
  assert_equal "n1", polytri.n1
  assert_equal "n2", polytri.n2
  assert_nil polytri.s0
  assert_nil polytri.s1
  assert_nil polytri.s2
 end

 def test_initialize_6
  polytri = Polytri.new("n0", "n1", "n2", "s0", "s1", "s2")
  assert_equal "n0", polytri.n0
  assert_equal "n1", polytri.n1
  assert_equal "n2", polytri.n2
  assert_equal "s0", polytri.s0
  assert_equal "s1", polytri.s1
  assert_equal "s2", polytri.s2
 end

end

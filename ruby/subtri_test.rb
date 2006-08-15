#!/usr/bin/env ruby
# unit test for subtri

require 'test/unit'
require 'subtri'

class TestSubtri < Test::Unit::TestCase
 
 def test_initialize
  subtri = Subtri.new("n0", "n1", "n2")

  assert subtri.is_a?(Subtri), "not a subtri"

  assert_equal "n0", subtri.n0
  assert_equal "n1", subtri.n1
  assert_equal "n2", subtri.n2
 end

end

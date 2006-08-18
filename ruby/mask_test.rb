#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'mask'

class TestMask < Test::Unit::TestCase

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

end

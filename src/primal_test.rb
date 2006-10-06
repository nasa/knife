#!/usr/bin/env ruby
#
# Mobility test for primal c lib
#
# $Id$

Dir.chdir ENV['srcdir'] if ENV['srcdir']

require 'RubyExtensionBuilder'

RubyExtensionBuilder.build('Primal')

require 'test/unit'
require 'Adj/Adj'
require 'Primal/Primal'

class TestPrimal < Test::Unit::TestCase

 def set_up
  @primal = Primal.new(1,2,3)
 end
 def setup ; set_up ; end

 def test_create
  assert_equal 1, @primal.nnode
  assert_equal 2, @primal.nface
  assert_equal 3, @primal.ncell
 end

 def test_create_zero_size
  local_primal = Primal.new(0,0,0)
  assert_equal 1, local_primal.nnode
  assert_equal 1, local_primal.nface
  assert_equal 1, local_primal.ncell
 end
 
end

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
  @primal = Primal.new(4,3,1)
 end
 def setup ; set_up ; end

 def test_create
  assert_equal 4, @primal.nnode
  assert_equal 3, @primal.nface
  assert_equal 1, @primal.ncell
 end
 
end

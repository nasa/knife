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
  @fast_file = 'primal_test.fgrid'
  File.open(@fast_file,'w') do |f|
   f.puts "4 4 1"
   f.puts "0.0\n1.0\n0.0\n0.0"
   f.puts "0.0\n0.0\n1.0\n0.0"
   f.puts "0.0\n0.0\n0.0\n1.0"
   f.puts "2 4 3"
   f.puts "1 3 4"
   f.puts "1 4 2"
   f.puts "1 2 3"
   f.puts "1\n2\n3\n4"
   f.puts "1 2 3 4"
  end
 end
 def setup ; set_up ; end

 def tear_down
  File.delete @fast_file
 end
 def teardown ; tear_down ; end

 def test_create
  assert_equal 4, @primal.nnode
  assert_equal 3, @primal.nface
  assert_equal 1, @primal.ncell
 end
 
 def test_from_fast
  primal = Primal.from_FAST(@fast_file)
  assert_equal 4, primal.nnode
  assert_equal 4, primal.nface
  assert_equal 1, primal.ncell
 end
 
end

#!/usr/bin/env ruby
#
# Mobility test for adj c lib
#
# $Id$

Dir.chdir ENV['srcdir'] if ENV['srcdir']

require 'RubyExtensionBuilder'

RubyExtensionBuilder.build('Adj')

require 'test/unit'
require 'Adj/Adj'

class TestAdj < Test::Unit::TestCase

 def set_up
  @adj = Adj.new(4,6,3)
  @big_node = 10000
 end
 def setup ; set_up ; end

 def test_create
  assert_equal 4, @adj.nnode
  assert_equal 6, @adj.nadj
  assert_equal 3, @adj.chunk_size
 end

 def test_create_zero_size
  local_adj = Adj.new(0,0,0)
  assert_equal 1, local_adj.nnode
  assert_equal 1, local_adj.nadj
  assert_equal 1, local_adj.chunk_size
 end

 def test_add
  assert_nil @adj.add(-1,1)
  assert_not_nil @adj.add(0,1)
  assert_nil @adj.add(4,1)
  assert_nil @adj.add(@big_node,1)
 end

 def test_add_allocates_new_cunk
  6.times {|i| assert_not_nil @adj.add(0,i)}
  assert_equal 6, @adj.nadj
  assert_not_nil  @adj.add(1,0)
  assert_equal 9, @adj.nadj
 end

 def test_resize_keeps_old_elements
  degree = 10
  4.times { |node| degree.times { |i| @adj.add(node,10*i+node) } }
  assert_equal degree, @adj.degree(0) 
  assert_equal degree, @adj.degree(1) 
  assert_equal degree, @adj.degree(2) 
  assert_equal degree, @adj.degree(3) 
 end

 def test_resize_nnodes_increase
  assert_equal 4, @adj.nnode
  assert_not_nil  @adj.resize(9)
  assert_equal 9, @adj.nnode
  assert_equal @adj,  @adj.add( 8, 899 )
  assert_not_nil  @adj.resize(0)
  assert_equal 1, @adj.nnode
 end

 def test_resize_down_without_memory_leak
  assert_equal @adj, @adj.add( 2, 299 )
  assert_not_nil  @adj.resize(0)
  assert_equal 1, @adj.nnode
  6.times {|i| assert_not_nil @adj.add(0,i)}
  assert_equal  6, @adj.nadj
 end

 def test_add_and_remove
  assert_equal false, @adj.exists(1,0)
  assert_nil          @adj.remove(1,0)
  assert_equal @adj,  @adj.add(1,0)
  assert_equal true,  @adj.exists(1,0)
  assert_equal @adj,  @adj.remove(1,0)
  assert_equal false, @adj.exists(1,0)
  assert_nil          @adj.remove(1,0)
 end
 
 def test_multiple_exists
  assert_equal false, @adj.exists(1,198)
  assert_equal @adj,  @adj.add(1,198)
  assert_equal @adj,  @adj.add(2,198)
  assert_equal @adj,  @adj.add(1,199)
  
  assert_equal true,  @adj.exists(1,198)
  assert_equal true,  @adj.exists(1,199)
  @adj.remove(1,198)
  assert_equal false, @adj.exists(1,198)
  assert_equal true,  @adj.exists(1,199)
  @adj.add(1,198)
  assert_equal true,  @adj.exists(1,198)
  assert_equal true,  @adj.exists(1,199)
 end

 def test_degree
  assert_equal    0, @adj.degree(0)
  assert_equal @adj, @adj.add( 0, 299 )
  assert_equal    1, @adj.degree(0)
  assert_equal @adj, @adj.add( 0, 298 )
  assert_equal    2, @adj.degree(0)
 end

end

#!/usr/bin/env ruby
# unit test for node

require 'test/unit'
require 'node'

class TestNode < Test::Unit::TestCase
 
 TOL = 1.0e-14

 def test_initialize
  node = Node.new(1.1, 2.2, 3.3)

  assert node.is_a?(Node), "not a node"

  assert_in_delta 1.1, node[0], TOL
  assert_in_delta 2.2, node[1], TOL
  assert_in_delta 3.3, node[2], TOL
 end

end

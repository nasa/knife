#!/usr/bin/env ruby
#
# Mobility test for node c lib
#


Dir.chdir ENV['srcdir'] if ENV['srcdir']

require 'RubyExtensionBuilder'

RubyExtensionBuilder.build('Node')

require 'test/unit'
require 'Node/Node'

class TestNode < Test::Unit::TestCase

 def test_create
  node = Node.new(0.1,0.2,0.3)
  assert_equal 0.1, node.x
  assert_equal 0.2, node.y
  assert_equal 0.3, node.z
 end

end

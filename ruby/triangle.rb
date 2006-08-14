# defined from 3 segments

require 'triangle_node'

class Triangle

 attr_reader :segments
 attr_reader :cuts

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
  @nodes = [TriangleNode.new(1.0,0.0,0.0), 
            TriangleNode.new(0.0,1.0,0.0), 
            TriangleNode.new(0.0,0.0,0.1)]
  @children = [[0,1,2]]
 end
 
 def segment(index)
  @segments[index]
 end

 def node(index)
  case index
  when 0
   segment(2).node(0)
  when 1
   segment(0).node(0)
  when 2
   segment(1).node(0)
  else
   nil
  end
 end

end

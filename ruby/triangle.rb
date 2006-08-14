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
            TriangleNode.new(0.0,0.0,1.0)]
  @children = [[@nodes[0],@nodes[1],@nodes[2]]]
 end
 
 def segment(index)
  @segments[index]
 end

 def original_node0
   segment(2).node(0)
 end
 def original_node1
   segment(0).node(0)
 end
 def original_node2
   segment(1).node(0)
 end

 def triangulate_cuts
  @cuts.each do |cut|
   insert_cut(cut)
  end
 end

 def insert_cut
  node0 = add_unique_intersection cut.intersection0
  node1 = add_unique_intersection cut.intersection1
 end

 def add_unique_intersection(intersection)
  @nodes.each do |node|
   return node if intersection == node.intersection
  end
  u,v,w = intersection_uvw(intersection)
  node = TriangleNode.new(u,v,w,intersection)
  insert_node(node)
 end

 def intersection_uvw(intersection)
  if self == intersection.triangle
   return intersection.u,intersection.v,intersection.w
  end
  @segments.each do |segment|
   
  end
 end


 def insert_node(node)
  
  node
 end

end

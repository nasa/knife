# defined from 3 segments

require 'triangle_node'

class Triangle

 attr_reader :segments
 attr_reader :cuts

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
  @nodes = [TriangleNode.new(1.0,0.0,0.0,original_node0), 
            TriangleNode.new(0.0,1.0,0.0,original_node1), 
            TriangleNode.new(0.0,0.0,1.0,original_node2)]
  @children = [[@nodes[0],@nodes[1],@nodes[2]]]
 end
 
 def segment(index)
  @segments[index]
 end

 def original_node0
  return segment(2).node(0) if segment(2).node(0) == segment(1).node(0)  
  return segment(2).node(0) if segment(2).node(0) == segment(1).node(1)  
  return segment(2).node(1) if segment(2).node(1) == segment(1).node(0)  
  return segment(2).node(1) if segment(2).node(1) == segment(1).node(1)  
  raise "no common original_node0"
 end
 def original_node1
  return segment(0).node(0) if segment(0).node(0) == segment(2).node(0)  
  return segment(0).node(0) if segment(0).node(0) == segment(2).node(1)  
  return segment(0).node(1) if segment(0).node(1) == segment(2).node(0)  
  return segment(0).node(1) if segment(0).node(1) == segment(2).node(1)  
  raise "no common original_node1"
 end
 def original_node2
  return segment(0).node(0) if segment(0).node(0) == segment(1).node(0)  
  return segment(0).node(0) if segment(0).node(0) == segment(1).node(1)  
  return segment(0).node(1) if segment(0).node(1) == segment(1).node(0)  
  return segment(0).node(1) if segment(0).node(1) == segment(1).node(1)  
  raise "no common original_node2"
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
   return node if intersection == node.parent
  end
  u,v,w = intersection_uvw(intersection)
  node = TriangleNode.new(u,v,w,intersection)
  insert_node(node)
 end

 def intersection_uvw(intersection)
  if self == intersection.triangle
   return intersection.u,intersection.v,intersection.w
  end
  raise "intersection segment not found in intersection_uvw"
 end

 def insert_node(node)
  
  node
 end

end

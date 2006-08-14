# defined from 3 segments

class Triangle

 attr_reader :segments
 attr_reader :cuts

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
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

 def intersection_uvw(intersection)
  if self == intersection.triangle
   return intersection.u,intersection.v,intersection.w
  end
  u=0.0;v=0.0;w=0.0
  u = (    intersection.t) if intersection.segment[1] == original_node0
  v = (    intersection.t) if intersection.segment[1] == original_node1
  w = (    intersection.t) if intersection.segment[1] == original_node2
  u = (1.0-intersection.t) if intersection.segment[0] == original_node0
  v = (1.0-intersection.t) if intersection.segment[0] == original_node1
  w = (1.0-intersection.t) if intersection.segment[0] == original_node2
  return u,v,w
 end

end

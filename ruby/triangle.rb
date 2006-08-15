# defined from 3 segments

require 'subnode'
require 'subtri'

class Triangle

 attr_reader :segments
 attr_reader :cuts
 attr_reader :subnodes
 attr_reader :subtris

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
  @subnodes = [ Subnode.new(1.0,0.0,0.0),
                Subnode.new(0.0,1.0,0.0),
                Subnode.new(0.0,0.0,1.0)]
  @subtris = [ Subtri.new(@subnodes[0],@subnodes[1],@subnodes[2]) ]
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
   node0 = add_unique_subnode(intersection)
   node1 = add_unique_subnode(intersection)
  end
 end

 def add_unique_subnode(intersection)
  @subnodes.each do |subnode|
   return subnode if intersection == subnode.parent
  end
  u,v,w = intersection.uvw(self)
  subnode = Subnode.new(u,v,w,intersection)
  @subnodes << subnode
  subnode
 end

end

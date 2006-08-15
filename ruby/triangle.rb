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
   node0 = add_unique_subnode(cut.intersection0)
   node1 = add_unique_subnode(cut.intersection1)
  end
 end

 def add_unique_subnode(intersection)
  @subnodes.each do |subnode|
   return subnode if intersection == subnode.parent
  end
  u,v,w = intersection.uvw(self)
  subnode = Subnode.new(u,v,w,intersection)
  @subnodes << subnode
  split_subtri_with(subnode)
  subnode
 end

 def split_subtri_with(subnode)
  subtri, bary = enclosing_subtri(subnode)
  side_tolerence = 1.0e-12
  if bary.min < side_tolerence
   case true
   when bary[0] <= bary[1] && bary[0] <= bary[2]
    insert_subnode_into_subtri_side(subnode,subtri.n1,subtri.n2)
   when bary[1] <= bary[0] && bary[1] <= bary[2]
    insert_subnode_into_subtri_side(subnode,subtri.n2,subtri.n0)
   else
    insert_subnode_into_subtri_side(subnode,subtri.n0,subtri.n1)
   end
  else
   insert_subnode_into_subtri_interior(subnode,subtri)
  end
 end

 def enclosing_subtri(subnode)
  enclosing_subtri = @subtris.first
  enclosing_bary = enclosing_subtri.barycentric(subnode)
  greatest_min_bary = enclosing_bary.min
  @subtris.each do |subtri|
   bary = subtri.barycentric(subnode)
   min_bary = bary.min
   if ( min_bary > greatest_min_bary)
    enclosing_subtri = subtri
    enclosing_bary = bary
    greatest_min_bary = min_bary
   end
  end
  return enclosing_subtri, enclosing_bary
 end

 def insert_subnode_into_subtri_interior(subnode,subtri)
  @subtris << Subtri.new(subtri.n0, subnode,   subtri.n2)
  @subtris << Subtri.new(subtri.n0, subtri.n1, subnode)
  subtri.n0 = subnode
  self
 end


end

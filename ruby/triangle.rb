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
   unless find_subtri_with(node0,node1)
    raise "cut is not triangle side"
   end
  end
  self
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

 def insert_subnode_into_subtri_side(subnode,node0,node1)
  @subtris.each do |subtri|
   if ( (node0 == subtri.n0 && node1 == subtri.n1) ||
        (node1 == subtri.n0 && node0 == subtri.n1) )
    @subtris << Subtri.new(subtri.n0, subnode,   subtri.n2)
    subtri.n0 = subnode
    return self
   end
   if ( (node0 == subtri.n1 && node1 == subtri.n2) ||
        (node1 == subtri.n1 && node0 == subtri.n2) )
    @subtris << Subtri.new(subtri.n0, subtri.n1, subnode )
    subtri.n1 = subnode
    return self
   end
   if ( (node0 == subtri.n2 && node1 == subtri.n0) ||
        (node1 == subtri.n2 && node0 == subtri.n0) )
    @subtris << Subtri.new(subnode, subtri.n1, subtri.n2 )
    subtri.n0 = subnode
    return self
   end
  end
  raise "could not find node pairs"
 end

 def insert_subnode_into_subtri_interior(subnode,subtri)
  @subtris << Subtri.new(subtri.n0, subnode,   subtri.n2)
  @subtris << Subtri.new(subtri.n0, subtri.n1, subnode)
  subtri.n0 = subnode
  self
 end

 def find_subtri_with(node0,node1)
  @subtris.each do |canidate|
   return canidate if (((node0 == canidate.n0) && (node1 == canidate.n1 )) ||
                       ((node0 == canidate.n1) && (node1 == canidate.n2 )) ||
                       ((node0 == canidate.n2) && (node1 == canidate.n0 )))
  end
  nil
 end

 def eps(eps_filename = 'triangle.eps')
  temp_file_name = 'gnuplot_mesh_command'
  File.open(temp_file_name,'w') do |f|
   f.puts gnuplot_command(eps_filename)
  end
  system("gnuplot #{temp_file_name}")
  File.delete(temp_file_name)
  self
 end

 def gnuplot_command(eps_filename)
  output = <<-END_OF_HEADER
  reset
  set term postscript eps
  set output '#{eps_filename}'
  set size ratio -1
  set xlabel 'V'
  set ylabel 'W'
  plot [-0.1:1.1] [-0.1:1.1] '-' title '' with lines lw 0.5
  END_OF_HEADER
  @subtris.each do |subtri|
   node = subtri.n0
   output << sprintf("%25.15f %25.15f\n",node.v,node.w)
   node = subtri.n1
   output << sprintf("%25.15f %25.15f\n",node.v,node.w)
   node = subtri.n2
   output << sprintf("%25.15f %25.15f\n",node.v,node.w)
   node = subtri.n0
   output << sprintf("%25.15f %25.15f\n",node.v,node.w)
   output << "\n\n"
  end
  output+"e\n"
 end


end

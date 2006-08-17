# defined from 3 segments

require 'subnode'
require 'subtri'

class Triangle

 attr_reader :node0, :node1, :node2

 attr_reader :segments
 attr_reader :cuts
 attr_reader :subnodes
 attr_reader :subtris

 attr_reader :center, :diameter

 def initialize(segment0,segment1,segment2)
  @node0=segment1.common_node(segment2)
  @node1=segment0.common_node(segment2)
  @node2=segment0.common_node(segment1)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
  @subnodes = [ Subnode.new(1.0,0.0,0.0,@node0),
                Subnode.new(0.0,1.0,0.0,@node1),
                Subnode.new(0.0,0.0,1.0,@node2)]
  @subtris = [ Subtri.new(@subnodes[0],@subnodes[1],@subnodes[2],
                          segment0,segment1,segment2) ]
  cache_geometry
 end
 
 def segment(index)
  @segments[index]
 end

 def cache_geometry
  @center = [ (node0[0]+node1[0]+node2[0])/3.0,
              (node0[1]+node1[1]+node2[1])/3.0,
              (node0[2]+node1[2]+node2[2])/3.0 ]
  l2 = (node0[0]-center[0])**2 + 
       (node0[1]-center[1])**2 + 
       (node0[2]-center[2])**2 
  d2 = (node1[0]-center[0])**2 + 
       (node1[1]-center[1])**2 + 
       (node1[2]-center[2])**2
  l2 = d2 if d2 > l2
  d2 = (node2[0]-center[0])**2 + 
       (node2[1]-center[1])**2 + 
       (node2[2]-center[2])**2
  l2 = d2 if d2 > l2
  @diameter = 1.0001*Math::sqrt(l2)
 end

 def triangulate_cuts
  @cuts.each do |cut|
   subnode0 = add_unique_subnode(cut.intersection0)
   subnode1 = add_unique_subnode(cut.intersection1)
   unless find_subtri_with(subnode0,subnode1)
    raise "cut is not subtriangle side" unless recover_edge(subnode0,subnode1)
   end
  end
  self
 end

 def each_subtri_around_subnode(subnode)
  @subtris.each do |subtri|
   yield subtri if subtri.has?(subnode)
  end
 end

 def first_blocking_subtri_side(subnode0,subnode1)
  each_subtri_around_subnode(subnode0) do |subtri|
   n0, n1, n2 = subtri.orient(subnode0)
   return n2, n1 if (Subtri.right_handed?(n0,n1,subnode1) &&
                     Subtri.right_handed?(n2,n0,subnode1) )
  end
  raise "first_triangle.nil?"
 end

 def recover_edge(subnode0,subnode1)
  left_node,right_node = first_blocking_subtri_side(subnode0,subnode1)
  swap_side(left_node,right_node)
  return true if find_subtri_with(subnode0,subnode1)
  recover_edge(subnode0,subnode1)
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

 def set_subtri_side(node0, node1, new_side = nil)
  raise "subtri side missing for set" unless ( find_subtri_with(node0,node1) || 
                                               find_subtri_with(node1,node0) )
  @subtris.each do |subtri|
   subtri.set_side(node0,node1,new_side)
  end
  self
 end

 def insert_subnode_into_subtri_side(subnode,node0,node1)
  raise "subtri side missing for inss" unless (find_subtri_with(node0,node1) || 
                                               find_subtri_with(node1,node0) )
  @subtris.each do |subtri|
   if ( (node0 == subtri.n0 && node1 == subtri.n1) ||
        (node1 == subtri.n0 && node0 == subtri.n1) )
    @subtris << subtri.dup
    @subtris.last.n1 = subnode
    subtri.n0 = subnode
    @subtris.last.set_side(subnode,subtri.n2)
    subtri.set_side(subnode,subtri.n2)
   end
   if ( (node0 == subtri.n1 && node1 == subtri.n2) ||
        (node1 == subtri.n1 && node0 == subtri.n2) )
    @subtris << subtri.dup
    @subtris.last.n2 = subnode
    subtri.n1 = subnode
    @subtris.last.set_side(subnode,subtri.n0)
    subtri.set_side(subnode,subtri.n0)
   end
   if ( (node0 == subtri.n2 && node1 == subtri.n0) ||
        (node1 == subtri.n2 && node0 == subtri.n0) )
    @subtris << subtri.dup
    @subtris.last.n0 = subnode
    subtri.n2 = subnode
    @subtris.last.set_side(subnode,subtri.n1)
    subtri.set_side(subnode,subtri.n1)
   end
  end
  self
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

 def swap_side(node0,node1)
  subtri0 = find_subtri_with( node0, node1)
  subtri1 = find_subtri_with( node1, node0)
  raise "subtri side not found in swap" if subtri0.nil? || subtri1.nil?
  n0, n1, n2 = subtri0.orient(node0)
  node2 = n2
  n0, n1, n2 = subtri1.orient(node1)
  node3 = n2

  subtri0.n0 = node1
  subtri0.n1 = node2
  subtri0.n2 = node3

  subtri1.n0 = node0
  subtri1.n1 = node3
  subtri1.n2 = node2

  self
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

 def tecplot_header
  'title="cut cell geometry"'+"\n"+'variables="x","y","z","v","w"'+"\n"
 end

 def tecplot_zone(title='surf')
  output = sprintf("zone t=%s, i=%d, j=%d, f=fepoint, et=triangle\n",
                   title, @subnodes.size, @subtris.size)
  @subnodes.each do |subnode|
   output += sprintf("%25.15e%25.15e%25.15e%25.15e%25.15e\n",
                     subnode.x,subnode.y,subnode.z,
                     subnode.v,subnode.w)
  end
  @subtris.each do |subtri|
   output += sprintf(" %6d %6d %6d\n",
                     1+@subnodes.index(subtri.n0),
                     1+@subnodes.index(subtri.n1),
                     1+@subnodes.index(subtri.n2) )
  end
  output
 end

 def dump(filename='dump.t')
  File.open(filename,'w') do |f|
   f.print tecplot_header
   f.print tecplot_zone
  end
 end

 def min_subtri_area
  area = 1.0
  @subtris.each do |subtri|
   area = [area, subtri.area].min
  end
  area
 end

end

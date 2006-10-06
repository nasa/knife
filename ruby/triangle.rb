# defined from 3 segments

require 'subnode'
require 'subtri'

class Triangle

 @@frame=0

 attr_reader :node0, :node1, :node2

 attr_reader :segments
 attr_reader :cuts
 attr_reader :subnodes
 attr_reader :subtris
 attr_reader :polyhedra
 attr_accessor :active
 attr_accessor :boundary_group
 attr_accessor :interior_index
 attr_accessor :quad_rule_index

 attr_reader :center, :diameter

 def initialize(segment0,segment1,segment2)
  @node0=segment1.common_node(segment2)
  @node1=segment0.common_node(segment2)
  @node2=segment0.common_node(segment1)
  @segments = [ segment0, segment1, segment2 ]
  segment0.triangles << self
  segment1.triangles << self
  segment2.triangles << self
  @cuts = Array.new
  @subnodes = [ Subnode.new(1.0,0.0,0.0,@node0),
                Subnode.new(0.0,1.0,0.0,@node1),
                Subnode.new(0.0,0.0,1.0,@node2)]
  @subtris = [ Subtri.new(@subnodes[0],@subnodes[1],@subnodes[2],
                          segment0,segment1,segment2) ]
  @polyhedra = Array.new
  @active = true
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

 def area
  edge1 = [node1.x-node0.x, node1.y-node0.y, node1.z-node0.z]
  edge2 = [node2.x-node0.x, node2.y-node0.y, node2.z-node0.z]
  norm = [ edge1[1]*edge2[2] - edge1[2]*edge2[1],
           edge1[2]*edge2[0] - edge1[0]*edge2[2],
           edge1[0]*edge2[1] - edge1[1]*edge2[0] ] 
  area2 = Math.sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2])
  (0.5*area2)
 end

 def triangulate_cuts_orig
  @cuts.each do |cut|
   subnode0 = add_unique_subnode(cut.intersection0)
   subnode1 = add_unique_subnode(cut.intersection1)
   swap!
   unless find_subtri_with(subnode0,subnode1)
    raise "cut is not subtriangle side" unless recover_edge(subnode0,subnode1)
   end
   set_subtri_side(subnode0, subnode1, cut)
   swap!
  end
  self
 end

 def cut_by
  cutters = Array.new
  @cuts.each do |cut|
   cut.triangles.each do |triangle|
    cutters << triangle unless self == triangle
   end
  end
  cutters.uniq
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
  subtri0 = find_subtri_with(node0,node1)
  subtri1 = find_subtri_with(node1,node0)
  raise "subtri side missing for set" unless ( subtri0 || subtri1 )
  subtri0.set_side(node0,node1,new_side) unless subtri0.nil?
  subtri1.set_side(node1,node0,new_side) unless subtri1.nil?
  self
 end

 def insert_subnode_into_subtri_side(subnode,node0,node1)
  subtri0 = find_subtri_with(node0,node1)
  subtri1 = find_subtri_with(node1,node0)
  raise "subtri side missing for ins" unless ( subtri0 || subtri1  )
  newtri0 = nil
  unless subtri0.nil?
   newtri0 = subtri0.split_side_with(subnode,node0,node1)
   raise "split side0" if newtri0.nil?
   @subtris << newtri0
  end
  newtri1 = nil
  unless subtri1.nil?
   newtri1 = subtri1.split_side_with(subnode,node1,node0)
   raise "split side1" if newtri1.nil?
   @subtris << newtri1
  end

  if ( newtri0 && newtri1 )
   subtri0 = find_subtri_with(node0,subnode)
   subtri1 = find_subtri_with(subnode,node0)
   subtri0.set_side(node0,subnode,subtri1)
   subtri1.set_side(subnode,node0,subtri0)

   subtri0 = find_subtri_with(node1,subnode)
   subtri1 = find_subtri_with(subnode,node1)
   subtri0.set_side(node1,subnode,subtri1)
   subtri1.set_side(subnode,node1,subtri0)
  end
  self
 end

 def insert_subnode_into_subtri_interior(subnode,subtri)
  newtri0 = subtri
  newtri1 = subtri.dup
  newtri2 = subtri.dup

  @subtris << newtri1
  @subtris << newtri2

  newtri0.n0 = subnode
  newtri1.n1 = subnode
  newtri2.n2 = subnode

  raise unless newtri0.set_side(newtri0.n0,newtri0.n1,newtri2)
  raise unless newtri0.set_side(newtri0.n0,newtri0.n2,newtri1)
  neighbor = newtri0.side_with_nodes(newtri0.n1,newtri0.n2)
  if neighbor.respond_to? :set_side
   neighbor.set_side(newtri0.n2,newtri0.n1,newtri0)
  end

  raise unless newtri1.set_side(newtri1.n1,newtri1.n0,newtri2)
  raise unless newtri1.set_side(newtri1.n1,newtri1.n2,newtri0)
  neighbor = newtri1.side_with_nodes(newtri1.n2,newtri1.n0)
  if neighbor.respond_to? :set_side
   neighbor.set_side(newtri1.n0,newtri1.n2,newtri1)
  end

  raise unless newtri2.set_side(newtri2.n2,newtri2.n0,newtri1)
  raise unless newtri2.set_side(newtri2.n2,newtri2.n1,newtri0)
  neighbor = newtri2.side_with_nodes(newtri2.n0,newtri2.n1)
  if neighbor.respond_to? :set_side
   neighbor.set_side(newtri2.n1,newtri2.n0,newtri2)
  end

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

 def find_subtri_with_parents(parent0,parent1)
  @subtris.each do |canidate|
   return canidate if (((parent0 == canidate.n0.parent) && 
                        (parent1 == canidate.n1.parent )) ||
                       ((parent0 == canidate.n1.parent) && 
                        (parent1 == canidate.n2.parent )) ||
                       ((parent0 == canidate.n2.parent) && 
                        (parent1 == canidate.n0.parent )))
  end
  nil
 end

 def neighboring(segment)
  segment.triangles.each do |triangle|
   return triangle unless self == triangle
  end
  nil
 end

 def swap_side(node0,node1)
  subtri0 = find_subtri_with( node0, node1)
  subtri1 = find_subtri_with( node1, node0)

  unless subtri0 == subtri1.side_with_nodes(node1,node0)
   puts "#{node0.v} #{node0.w}"
   puts "#{node1.v} #{node1.w}"
   eps
  end

  raise "no swap side0" unless subtri1 == subtri0.side_with_nodes(node0,node1)
  raise "no swap side1" unless subtri0 == subtri1.side_with_nodes(node1,node0)

  n0, n1, n2 = subtri0.orient(node0)
  node2 = n2
  n0, n1, n2 = subtri1.orient(node1)
  node3 = n2

  side02 = subtri0.side_with_nodes(node0,node2)
  side12 = subtri0.side_with_nodes(node1,node2)
  side03 = subtri1.side_with_nodes(node0,node3)
  side13 = subtri1.side_with_nodes(node1,node3)

  subtri0.n0 = node1
  subtri0.n1 = node2
  subtri0.n2 = node3

  subtri1.n0 = node0
  subtri1.n1 = node3
  subtri1.n2 = node2

  subtri0.s0 = subtri1
  subtri0.s1 = side13
  subtri0.s2 = side12

  subtri1.s0 = subtri0
  subtri1.s1 = side02
  subtri1.s2 = side03

  self
 end

 def swap!
  changed = false
  subtris.each do |subtri|
   changed = true if attempt_swap_if_mr_improves(subtri,subtri.n0,subtri.n1)
   changed = true if attempt_swap_if_mr_improves(subtri,subtri.n1,subtri.n2)
   changed = true if attempt_swap_if_mr_improves(subtri,subtri.n2,subtri.n0)
  end
  return swap! if changed
  self
 end

 def attempt_swap_if_mr_improves(subtri0, node0, node1)
  return false unless subtri0.side_with_nodes(node0,node1).nil?
  return nil if subtri0.nil?
  subtri1 = find_subtri_with( node1, node0)
  return nil if subtri1.nil?
  n0, n1, n2 = subtri0.orient(node0)
  node2 = n2
  n0, n1, n2 = subtri1.orient(node1)
  node3 = n2

  subtri2_mean_ratio = Subtri.mean_ratio(node1, node2, node3)
  subtri3_mean_ratio = Subtri.mean_ratio(node0, node3, node2)

  if ([subtri2_mean_ratio,  subtri3_mean_ratio].min >
      [subtri0.mean_ratio,  subtri1.mean_ratio].min )
   return true if swap_side(node0,node1)
  end

  false
 end

 def eps(eps_filename = sprintf("triangle%04d.eps",@@frame))
  @@frame += 1
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

 def triangulate_cuts
  to_recover = Array.new
  @cuts.each do |cut|
   subnode0 = add_unique_subnode(cut.intersection0)
   delaunayness(subnode0)
   subnode1 = add_unique_subnode(cut.intersection1)
   delaunayness(subnode1)
   to_recover << [subnode0, subnode1, cut]
  end

  to_recover.each do |recover|
   unless find_subtri_with(recover[0],recover[1])
    raise "cut is not subtri side" unless recover_edge(recover[0],recover[1])
   end
   set_subtri_side(recover[0], recover[1], recover[2])
  end

  self
 end

 def delaunayness(subnode)
  sts = Array.new
  @subtris.each do |subtri|
   sts << subtri if subtri.has?(subnode)
  end
  sts.each do |st|
   delaunay_suspect_edge(subnode,st)
  end
 end

 def delaunay_suspect_edge(subnode,st)
  nodes = st.orient(subnode)
  other = find_subtri_with(nodes[2],nodes[1])
  return self if other.nil?
  other_nodes = other.orient(nodes[2])
  node0 = [nodes[0].v, nodes[0].w, nodes[0].v*nodes[0].v+nodes[0].w*nodes[0].w]
  node1 = [nodes[1].v, nodes[1].w, nodes[1].v*nodes[1].v+nodes[1].w*nodes[1].w]
  node2 = [nodes[2].v, nodes[2].w, nodes[2].v*nodes[2].v+nodes[2].w*nodes[2].w]
  node3 = [other_nodes[2].v, other_nodes[2].w,
           other_nodes[2].v*other_nodes[2].v+other_nodes[2].w*other_nodes[2].w]
  vol = Intersection.volume6(node0,node1,node2,node3)
  if ( (vol < 0.0) && 
       Subtri.right_handed?(nodes[0], nodes[1], other_nodes[2] ) &&
       Subtri.right_handed?(nodes[0], other_nodes[2], nodes[2] ) )
   swap_side(nodes[1],nodes[2])
   st0 = find_subtri_with(nodes[1],other_nodes[2])
   delaunay_suspect_edge(subnode,st0) unless st0.nil?
   st1 = find_subtri_with(other_nodes[2],nodes[2])
   delaunay_suspect_edge(subnode,st1) unless st1.nil?
  end

  self
 end

end

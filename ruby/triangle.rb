
require 'ring'

class Triangle

 EMPTY = -1

 attr_reader :nodes, :children

 def initialize(n0,n1,n2)
  @nodes = [n0, n1, n2]
  @children = [[0,1,2]]
 end

 def center
  [ (@nodes[0][0]+@nodes[1][0]+@nodes[2][0])/3.0,
    (@nodes[0][1]+@nodes[1][1]+@nodes[2][1])/3.0,
    (@nodes[0][2]+@nodes[1][2]+@nodes[2][2])/3.0 ]
 end

 def diameter
  l2 = (@nodes[0][0]-center[0])**2 + 
       (@nodes[0][1]-center[1])**2 + 
       (@nodes[0][2]-center[2])**2 
  d2 = (@nodes[1][0]-center[0])**2 + 
       (@nodes[1][1]-center[1])**2 + 
       (@nodes[1][2]-center[2])**2
  l2 = d2 if d2 > l2
  d2 = (@nodes[2][0]-center[0])**2 + 
       (@nodes[2][1]-center[1])**2 + 
       (@nodes[2][2]-center[2])**2
  l2 = d2 if d2 > l2
  1.0001*Math::sqrt(l2)
 end

 def add_unique_node(new_node)
  smallest_one_norm = one_norm(new_node,@nodes[0])
  smallest_index = 0
  @nodes.each_index do |existing_node_index|
   norm = one_norm(new_node,@nodes[existing_node_index])
   if (norm < smallest_one_norm)
    smallest_one_norm = norm
    smallest_index = existing_node_index
   end
  end
  if smallest_one_norm < 1.0e-14
   smallest_index
  else
   @nodes << new_node
   new_node_index = (@nodes.size-1)
   split_children(new_node_index)
   new_node_index 
  end
 end

 def splits(n0,n1)

  # these tetradedral volumes are made of 3 triangle vertexes 
  # and segment endpoints
  top_volume    = volume6(@nodes[0], @nodes[1], @nodes[2], n0)
  bottom_volume = volume6(@nodes[0], @nodes[1], @nodes[2], n1)

  # if signs match, segment is entirely above or below triangle
  return nil if (top_volume > 0.0 && bottom_volume > 0.0 )
  return nil if (top_volume < 0.0 && bottom_volume < 0.0 )
  
  # see if segment passes through triangle
  volume_side2 = volume6(@nodes[0], @nodes[1], n0, n1)
  volume_side0 = volume6(@nodes[1], @nodes[2], n0, n1)
  volume_side1 = volume6(@nodes[2], @nodes[0], n0, n1)

  # if signs match segment is inside triangle
  unless ( (volume_side0 > 0.0 && volume_side1 > 0.0 && volume_side2 > 0.0 ) ||
           (volume_side0 < 0.0 && volume_side1 < 0.0 && volume_side2 < 0.0 ) )
   return nil
  end

  # raise exception if degeneracy detected
  raise 'degeneracy, top_volume zero'    if top_volume.zero?
  raise 'degeneracy, bottom_volume zero' if bottom_volume.zero?
   
  raise 'degeneracy, volume_side0 zero' if volume_side0.zero?
  raise 'degeneracy, volume_side1 zero' if volume_side1.zero?
  raise 'degeneracy, volume_side2 zero' if volume_side2.zero?

  # find intersection point
  total_volume = top_volume - bottom_volume
  s1 = top_volume/total_volume
  s0 = 1.0-s1
  x = s0*n0[0] + s1*n1[0]
  y = s0*n0[1] + s1*n1[1]
  z = s0*n0[2] + s1*n1[2]
  [ x, y, z ]
 end

 def cut_with(triangle)
  new_nodes = Array.new

  new_node = splits(triangle.nodes[0],triangle.nodes[1])
  new_nodes << new_node if new_node
  new_node = splits(triangle.nodes[1],triangle.nodes[2])
  new_nodes << new_node if new_node
  new_node = splits(triangle.nodes[2],triangle.nodes[0])
  new_nodes << new_node if new_node
  
  new_node = triangle.splits(nodes[0],nodes[1])
  new_nodes << new_node if new_node
  new_node = triangle.splits(nodes[1],nodes[2])
  new_nodes << new_node if new_node
  new_node = triangle.splits(nodes[2],nodes[0])
  new_nodes << new_node if new_node
  
  return false if 0 == new_nodes.size
  raise "improper cut: #{new_nodes.join(' ')}" if 2 != new_nodes.size
  index0 = add_unique_node new_nodes[0]
  index1 = add_unique_node new_nodes[1]

  if find_child_with(index0,index1).nil?
   puts "edge missing" unless recover_edge(index0,index1)
  end

  true
 end

 def find_child_with(node0,node1)
  @children.each_with_index do |canidate, index|
   return canidate if (((node0 == canidate[0]) && (node1 == canidate[1] )) ||
                       ((node0 == canidate[1]) && (node1 == canidate[2] )) ||
                       ((node0 == canidate[2]) && (node1 == canidate[0] )))
  end
  nil
 end

 def split_children(new_node_index)
  node = @nodes[new_node_index]
  child_index, bary = enclosing_child(node)
  child = @children[child_index]
  side_tolerence = 1.0e-12
  if bary.min < side_tolerence
   case true
   when bary[0] <= bary[1] && bary[0] <= bary[2]
    insert_node_into_child_side(new_node_index,child[1],child[2])
   when bary[1] <= bary[0] && bary[1] <= bary[2]
    insert_node_into_child_side(new_node_index,child[2],child[0])
   else
    insert_node_into_child_side(new_node_index,child[0],child[1])
   end
  else
   insert_node_into_child_interior(new_node_index,child_index)
  end
 end

 def insert_node_into_child_side(new_index,node0,node1)
  original_children = @children.size
  original_children.times do |indx|
   child = @children[indx]
   if ( (node0 == child[0] && node1 == child[1]) ||
        (node1 == child[0] && node0 == child[1]) )
    @children << [child[0], new_index, child[2]]
    child[0] = new_index
   end
   if ( (node0 == child[1] && node1 == child[2]) ||
        (node1 == child[1] && node0 == child[2]) )
    @children << [child[0], child[1], new_index]
    child[1] = new_index
   end
   if ( (node0 == child[2] && node1 == child[0]) ||
        (node1 == child[2] && node0 == child[0]) )
    @children << [new_index, child[1], child[2]]
    child[2] = new_index
   end
  end
  self
 end

 def insert_node_into_child_interior(new_index,child_index)
  child = @children[child_index]
  @children << [child[0], new_index, child[2]]
  @children << [child[0], child[1], new_index]
  child[0] = new_index
  self
 end

 def recover_edge(node0,node1)
  connection = connect_nodes(node0,node1)
  if connection.empty? || connection.include?(nil)
   puts "recover_edge:connection.empty? #{connection.empty?}"
   puts "recover_edge:connection.include?(nil) #{connection.include?(nil)}"
   return false
  end 
  ring0 = Ring.from_children(connection)
  return nil if ring0.nil?
  ring1 = ring0.split!(node0,node1)
  return nil if ring1.nil?
  @children -= connection
  return false if ring0.triangulate.empty?
  ring0.triangulate.each do |child|
   @children << child
  end
  return false if ring1.triangulate.empty?
  ring1.triangulate.each do |child|
   @children << child
  end
  true
 end

 def connect_nodes(node0,node1)
  first_child = nil
  right_node = nil
  left_node = nil
  each_child_around_node(node0) do |child|
   n0, n1, n2 = orient(child,node0)
   if (right_handed?(n0,n1,node1) &&
       right_handed?(n2,n0,node1) )
    first_child = child
    left_node = n2
    right_node = n1
    break
   end
  end
  if first_child.nil?
   raise "first_child.nil?"
  else
   [first_child] << next_child_in_connection(node0,node1,
                                             left_node,right_node)
  end
 end

 def next_child_in_connection(node0,node1,left_node,right_node)
  child = find_child_with(left_node,right_node)
  return nil if child.nil?
  n0, n1, n2 = orient(child,left_node)
  return child if node1==n2
  if right_handed?(node0,node1,n2)
   [child] << next_child_in_connection(node0,node1,n2,right_node)
  else
   [child] << next_child_in_connection(node0,node1,left_node,n2)
  end
 end

 def orient(child,node)
  case true
  when node==child[0]
   [ child[0], child[1], child[2] ]
  when node==child[1]
   [ child[1], child[2], child[0] ]
  when node==child[2]
   [ child[2], child[0], child[1] ]
  else
   nil 
  end
 end

 def each_child_around_node(node)
  @children.each do |child|
   yield child if child.include?(node)
  end
 end

 def enclosing_child(node)
  enclosing_child_index = 0
  enclosing_bary = barycentric(node,@children[enclosing_child_index])
  greatest_min_bary = enclosing_bary.min
  @children.each_index do |child|
   bary = barycentric(node,@children[child])
   min_bary = bary.min
   if ( min_bary > greatest_min_bary)
    enclosing_child_index = child
    enclosing_bary = bary
    greatest_min_bary = min_bary
   end
  end
  return enclosing_child_index, enclosing_bary
 end

 def barycentric(node,child)
  n0 = triangle_normal(node,@nodes[child[1]],@nodes[child[2]])
  n1 = triangle_normal(node,@nodes[child[2]],@nodes[child[0]])
  n2 = triangle_normal(node,@nodes[child[0]],@nodes[child[1]])
  avg = triangle_normal(@nodes[child[0]],@nodes[child[1]],@nodes[child[2]])
  len = (avg[0]*avg[0] + avg[1]*avg[1] + avg[2]*avg[2])
  avg[0] /= len
  avg[1] /= len
  avg[2] /= len
  a0 = (n0[0]*avg[0] + n0[1]*avg[1] + n0[2]*avg[2])
  a1 = (n1[0]*avg[0] + n1[1]*avg[1] + n1[2]*avg[2])
  a2 = (n2[0]*avg[0] + n2[1]*avg[1] + n2[2]*avg[2])
  [a0, a1, a2]
 end

 def right_handed?(node0,node1,node2)
  normal   = triangle_normal(@nodes[0],@nodes[1],@nodes[2])
  canidate = triangle_normal(@nodes[node0],@nodes[node1],@nodes[node2])
  dot = (normal[0]*canidate[0]+normal[1]*canidate[1]+normal[2]*canidate[2])
  (dot > 1.0e-14)
 end

#extract to node or vertex class?

 def one_norm(n0,n1)
  (n0[0]-n1[0]).abs + (n0[1]-n1[1]).abs + (n0[2]-n1[2]).abs
 end

 def volume6(a,b,c,d)
  m11 = (a[0]-d[0])*((b[1]-d[1])*(c[2]-d[2])-(c[1]-d[1])*(b[2]-d[2]))
  m12 = (a[1]-d[1])*((b[0]-d[0])*(c[2]-d[2])-(c[0]-d[0])*(b[2]-d[2]))
  m13 = (a[2]-d[2])*((b[0]-d[0])*(c[1]-d[1])-(c[0]-d[0])*(b[1]-d[1]))
  det = -( m11 - m12 + m13 )
 end

 def triangle_normal(a,b,c)
  edge1=Array.new(3)
  edge2=Array.new(3)
  edge1[0] = b[0]-a[0]
  edge1[1] = b[1]-a[1]
  edge1[2] = b[2]-a[2]
  edge2[0] = c[0]-a[0]
  edge2[1] = c[1]-a[1]
  edge2[2] = c[2]-a[2]

  norm=Array.new(3)
  norm[0] = edge1[1]*edge2[2] - edge1[2]*edge2[1]
  norm[1] = edge1[2]*edge2[0] - edge1[0]*edge2[2]
  norm[2] = edge1[0]*edge2[1] - edge1[1]*edge2[0]

  norm
 end

 def tecplot_header
  'title="cut cell geometry"'+"\n"+'variables="x","y","z"'+"\n"
 end

 def tecplot_zone(title='surf')
  output = sprintf("zone t=%s, i=%d, j=%d, f=fepoint, et=triangle\n",
                   title, @nodes.size, @children.size)
  @nodes.each do |node|
   output += sprintf("%25.15e%25.15e%25.15e\n",node[0],node[1],node[2])
  end
  @children.each do |child|
   output += sprintf(" %6d %6d %6d\n",1+child[0],1+child[1],1+child[2])
  end
  output
 end

 def dump(filename='dump.t')
  File.open(filename,'w') do |f|
   f.print tecplot_header
   f.print tecplot_zone
  end
 end

end


class Triangle

 EMPTY = -1

 attr_reader :nodes

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
 
  true
 end

 def split_children(new_node_index)
  node = @nodes[new_node_index]
  child, bary = enclosing_child(node)
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
   insert_node_into_child_interior(new_node_index,child)
  end
 end

 def insert_node_into_child_side(new_index,node0,node1)
  original_children = @children.size
  original_children.times do |indx|
   child = @children[indx]
   if ( (node0 == child[0] && node1 == child[1]) ||
        (node1 == child[0] && node0 == child[1]) )
    child[0] = new_index
    @children << [child[0], new_index, child[2]]
   end
   if ( (node0 == child[1] && node1 == child[2]) ||
        (node1 == child[1] && node0 == child[2]) )
    child[1] = new_index
    @children << [child[0], child[1], new_index]
   end
   if ( (node0 == child[2] && node1 == child[0]) ||
        (node1 == child[2] && node0 == child[0]) )
    child[2] = new_index
    @children << [new_index, child[1], child[2]]
   end
  end
  self
 end

 def insert_node_into_child_interior(new_index,child_index)
  child = @children[child_index]
  @children << [child[0], new_index, child[2]]
  @children << [child[0], child[2], new_index]
  child[0] = new_index
  self
 end

 def enclosing_child(node)
  enclosing_child = 0
  enclosing_bary = barycentric(node,@children[enclosing_child])
  greatest_min_bary = enclosing_bary.min
  @children.each_index do |child|
   bary = barycentric(node,@children[child])
   min_bary = bary.min
   if ( min_bary > greatest_min_bary)
    enclosing_child = child
    nclosing_bary = bary
    greatest_min_bary = min_bary
   end
  end
  return enclosing_child, enclosing_bary
 end

 def barycentric(node,child)
  area0 = area2(node,child[1],child[2])
  area1 = area2(node,child[2],child[0])
  area2 = area2(node,child[0],child[1])
  total = area0+area1+area2
  [area0/total, area1/total, area2/total]
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

 def area2(a,b,c)
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

  (norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2])
 end

end

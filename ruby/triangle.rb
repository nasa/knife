
class Triangle

 EMPTY = -1

 attr_reader :nodes

 def initialize(n0,n1,n2)
  @nodes = [n0, n1, n2]
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
   (@nodes.size-1)
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
  new_nodes.each do |new_node|
   add_unique_node new_node
  end

  true
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

end

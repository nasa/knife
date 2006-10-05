
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'

class NodeFinder

 def initialize(grid)
  @grid = grid
  @nodes = Array.new(@grid.nnode)
 end

 def get(node,node_index=nil)
  if @nodes[node].nil?
   raise "NodeFinder Failed" if node_index.nil?
   xyz = @grid.nodeXYZ(node)
   @nodes[node] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
   node_index +=1
  end
  return @nodes[node], node_index
 end

end

class SegmentFinder

 def initialize(nnode)
  @segment = Array.new(nnode)
  @segment.collect! { Hash.new }
 end

 def between(n0,n1)
  i0 = [n0.indx,n1.indx].min
  i1 = [n0.indx,n1.indx].max
  unless @segment[i0].has_key?(i1)
   @segment[i0][i1] = Segment.new(n0,n1)
  end
  @segment[i0][i1]
 end

end

class Tet

 attr_reader :poly

 def initialize(nodes,poly,center,edge_center)
  @nodes = nodes
  @poly = poly
  @center = center
  @edge_center = edge_center
  @face_center = Array.new(4)
  @neighbor = Array.new(4)
  @boundary = Array.new(4)
 end

 def face_index2node_index(face_index)
  case face_index
  when 0; [1,3,2]
  when 1; [0,2,3]
  when 2; [0,3,1]
  when 3; [0,1,2]
  else; nil; end
 end

 def face_nodes2face_index(face_nodes)
  return 0 if @nodes.values_at(1,2,3).sort == face_nodes.sort
  return 1 if @nodes.values_at(0,2,3).sort == face_nodes.sort
  return 2 if @nodes.values_at(0,1,3).sort == face_nodes.sort
  return 3 if @nodes.values_at(0,1,2).sort == face_nodes.sort
  nil
 end

 def face_index2face_nodes(face_index)
  node_index = face_index2node_index(face_index)
  @nodes.values_at(node_index[0],node_index[1],node_index[2])
 end

 def boundary_face(face_index,faceid,xyz,node_index,node_finder)
  face_index2face_nodes(face_index).each do |node|
   new_node, node_index = node_finder.get(node,node_index)
  end
  @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
  node_index +=1
  @boundary[face_index] = faceid
  node_index
 end

 def shares_face_with(face_index, other_tet, xyz, node_index)
  @neighbor[face_index] = other_tet
  @face_center[face_index], node_index = 
   other_tet.get_center(self,face_index2face_nodes(face_index),xyz, node_index)
  node_index
 end

 def get_center(other_tet,face_nodes,xyz,node_index)
  face_index = face_nodes2face_index(face_nodes)
  if @face_center[face_index].nil?
   @neighbor[face_index] = other_tet
   @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
   node_index += 1
  end
  return @face_center[face_index], node_index
 end
 
 EDGE2NODE0 = [0, 0, 0, 1, 1, 2]
 EDGE2NODE1 = [1, 2, 3, 2, 3, 3]
 EDGE2FACE0 = [2, 3, 1, 0, 2, 0]
 EDGE2FACE1 = [3, 1, 2, 3, 0, 1]
 
 def create_dual(segment_finder, node_finder, triangle)

  6.times do |edge_index|
   edge_node = @edge_center[edge_index]

   poly_rev = @poly[EDGE2NODE0[edge_index]]
   poly_fwd = @poly[EDGE2NODE1[edge_index]]
   
   n0 = edge_node
   n1 = @center
   n2 = @face_center[EDGE2FACE0[edge_index]]

   s0 = segment_finder.between(n1,n2)
   s1 = segment_finder.between(n0,n2)
   s2 = segment_finder.between(n0,n1)

   tri = Triangle.new(s0,s1,s2)

   triangle << tri
   poly_fwd.add_triangle tri
   poly_rev.add_reversed_triangle tri

   n0 = edge_node
   n1 = @face_center[EDGE2FACE1[edge_index]]
   n2 = @center

   s0 = segment_finder.between(n1,n2)
   s1 = segment_finder.between(n0,n2)
   s2 = segment_finder.between(n0,n1)

   tri = Triangle.new(s0,s1,s2)

   triangle << tri
   poly_fwd.add_triangle tri
   poly_rev.add_reversed_triangle tri

  end

  @boundary.each_with_index do |faceid, face_index|
   unless faceid.nil?

    face_nodes = face_index2face_nodes(face_index)
    n0 = @face_center[face_index]

    [[face_nodes[0], face_nodes[1]],
     [face_nodes[1], face_nodes[2]],
     [face_nodes[2], face_nodes[0]] ].each do |triangle_side|

     node0 = triangle_side[0]
     node1 = triangle_side[1]

     n1, dummy = node_finder.get(node0)
     n2 = triangle_side2edge_center(node0,node1)

     s0 = segment_finder.between(n1,n2)
     s1 = segment_finder.between(n0,n2)
     s2 = segment_finder.between(n0,n1)
     tri = Triangle.new(s0,s1,s2)
     tri.boundary_group = faceid

     triangle << tri
     @poly[node2index(node0)].add_triangle tri
    
     n1 = triangle_side2edge_center(node0,node1)
     n2, dummy = node_finder.get(node1)
     
     s0 = segment_finder.between(n1,n2)
     s1 = segment_finder.between(n0,n2)
     s2 = segment_finder.between(n0,n1)
     tri = Triangle.new(s0,s1,s2)
     tri.boundary_group = faceid

     triangle << tri
     @poly[node2index(node1)].add_triangle tri
    end # triangle side
   end # !faceid.nil?
  end

  self
 end

 def node2index(node)
  index = @nodes.index(node)
  raise "cell node not found" if index.nil?
  index
 end

 def triangle_side2edge_center(node0, node1)
  i0 = node2index(node0)
  i1 = node2index(node1)
  index0=[i0,i1].min
  index1=[i0,i1].max
  6.times do |edge_index|
   if index0==EDGE2NODE0[edge_index] && index1==EDGE2NODE1[edge_index]
    return @edge_center [edge_index]
   end
  end
  raise "edge_node not found"
 end

 def all_poly_active?
  (@poly[0].active && @poly[1].active && @poly[2].active && @poly[3].active)
 end

end

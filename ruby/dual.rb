
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

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
  @segment.collect! { Array.new(nnode) }
 end

 def between(n0,n1)
  i0 = [n0.indx,n1.indx].min
  i1 = [n0.indx,n1.indx].max
  if @segment[i0][i1].nil?
   @segment[i0][i1] = Segment.new(n0,n1)
  end
  @segment[i0][i1]
 end

end

class Tet

 def initialize(nodes,center,edge_center)
  @nodes = nodes
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
 
 def create_dual(segment_finder, node_finder, triangle, poly)
  6.times do |edge_index|
   edge_node = @edge_center[edge_index]

   node0 = @nodes[EDGE2NODE0[edge_index]]
   node1 = @nodes[EDGE2NODE1[edge_index]]
   
   poly_rev = poly[node0]
   poly_fwd = poly[node1]
   
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

     triangle << tri
     poly[node0].add_triangle tri
    
     n1 = triangle_side2edge_center(node0,node1)
     n2, dummy = node_finder.get(node1)
     
     s0 = segment_finder.between(n1,n2)
     s1 = segment_finder.between(n0,n2)
     s2 = segment_finder.between(n0,n1)
     tri = Triangle.new(s0,s1,s2)

     triangle << tri
     poly[node1].add_triangle tri
    end # triangle side
   end # !faceid.nil?
  end

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

end

class Dual

 EMPTY = -1

 def Dual.from_grid(grid)
  poly = Array.new(grid.nnode)
  grid.nnode.times do |node|
   poly[node] = Polyhedron.new
  end

  #make volume segments
  grid.createConn

  edge_center = Array.new(grid.nconn)
  
  node_index = 0
  grid.nconn.times do |conn_index|
   conn_nodes = grid.conn2Node(conn_index)
   xyz0 = grid.nodeXYZ(conn_nodes[0])
   xyz1 = grid.nodeXYZ(conn_nodes[1])
   edge_center[conn_index] = Node.new( 0.5*(xyz0[0]+xyz1[0]),
                                       0.5*(xyz0[1]+xyz1[1]),
                                       0.5*(xyz0[2]+xyz1[2]), 
                                       node_index )
   node_index += 1
  end

  node_finder = NodeFinder.new(grid)

  tet = Array.new(grid.ncell)
  grid.ncell.times do |cell|
   nodes = grid.cell(cell)
   xyz0 = grid.nodeXYZ(nodes[0])
   xyz1 = grid.nodeXYZ(nodes[1])
   xyz2 = grid.nodeXYZ(nodes[2])
   xyz3 = grid.nodeXYZ(nodes[3])
   center = Node.new( 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]),
                      0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]),
                      0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]), 
                      node_index )
   node_index += 1
   tet[cell] = Tet.new(nodes,center,
                       edge_center.values_at(grid.cell2Conn(cell,0),
                                             grid.cell2Conn(cell,1),
                                             grid.cell2Conn(cell,2),
                                             grid.cell2Conn(cell,3),
                                             grid.cell2Conn(cell,4),
                                             grid.cell2Conn(cell,5)))
  end

  tet.each_with_index do |t,cell|
   4.times do |face_index|
    face_nodes = t.face_index2face_nodes(face_index)
    other_cell = grid.findOtherCellWith3Nodes(face_nodes[0], 
                                              face_nodes[1], 
                                              face_nodes[2], 
                                              cell)
    xyz0 = grid.nodeXYZ(face_nodes[0])
    xyz1 = grid.nodeXYZ(face_nodes[1])
    xyz2 = grid.nodeXYZ(face_nodes[2])
    xyz = [ (xyz0[0]+xyz1[0]+xyz2[0])/3.0,
            (xyz0[1]+xyz1[1]+xyz2[1])/3.0,
            (xyz0[2]+xyz1[2]+xyz2[2])/3.0 ]
    if EMPTY==other_cell
     faceid = grid.findFace(face_nodes[0],face_nodes[1],face_nodes[2])
     raise "boundary missing" if faceid.nil?
     node_index = t.boundary_face(face_index,faceid,xyz,node_index,node_finder)
    else
     node_index = t.shares_face_with(face_index,tet[other_cell],xyz,node_index)
    end
   end
  end

  segment_finder = SegmentFinder.new(node_index)

  triangle = Array.new

  tet.each do |t|
   t.create_dual(segment_finder, node_finder, triangle, poly)
  end

  Dual.new(poly,triangle,grid)
 end

 def initialize(poly=Array.new, triangle=Array.new, grid=nil)
  @poly = poly
  @triangle = triangle
  @grid = grid
 end

 def boolean_subtract(cut_surface)

  self
 end

 def dump_grid_for_fun3d

  File.open('postslice.fun','w') do |f|
  end
 end

 def write_tecplot(filename='dual.t')
  File.open(filename,'w') do |f|
   f.puts 'title="dual geometry"'+"\n"+'variables="x","y","z"'+"\n"
   @poly.each do |poly|
    f.print poly.tecplot_zone if poly.active
   end
  end
  self
 end

end

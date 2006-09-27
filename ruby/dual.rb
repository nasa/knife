
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

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

 def boundary_face(face_index,faceid,xyz)
  @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2] )
  @boundary[face_index] = faceid
 end

 def shares_face_with(face_index, other_tet, xyz)
  @neighbor[face_index] = other_tet
  other_tet.existing_center(self,face_index2face_nodes(face_index),xyz)
 end

 def existing_center(other_tet,face_nodes,xyz)
  face_index = face_nodes2face_index(face_nodes)
  if @face_center[face_index].nil?
   @neighbor[face_index] = other_tet
   @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2] )
  end
  @face_center[face_index]
 end

 def create_dual(segment_finder, triangle, primal_node)

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
  
  grid.nconn.times do |conn_index|
   conn_nodes = grid.conn2Node(conn_index)
   xyz0 = grid.nodeXYZ(conn_nodes[0])
   xyz1 = grid.nodeXYZ(conn_nodes[1])
   edge_center[conn_index] = Node.new( 0.5*(xyz0[0]+xyz1[0]),
                                       0.5*(xyz0[1]+xyz1[1]),
                                       0.5*(xyz0[2]+xyz1[2]) )
  end

  tet = Array.new(grid.ncell)
  grid.ncell.times do |cell|
   nodes = grid.cell(cell)
   xyz0 = grid.nodeXYZ(nodes[0])
   xyz1 = grid.nodeXYZ(nodes[1])
   xyz2 = grid.nodeXYZ(nodes[2])
   xyz3 = grid.nodeXYZ(nodes[3])
   center = Node.new( 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]),
                      0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]),
                      0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]) )
   
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
     t.boundary_face(face_index,faceid,xyz)
    else
     t.shares_face_with(face_index,tet[other_cell],xyz)
    end
   end
  end

  triangle = Array.new

  Dual.new(poly,triangle,grid)
 end

 def initialize(poly=Array.new, trianglex=Array.new, grid=nil)
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
   @poly.each do |poly|
    f.print poly.tecplot_zone if poly.active
   end
  end
  self
 end

end

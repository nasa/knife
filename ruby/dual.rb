
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
 end

end

class Dual

 attr_reader :grid

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


  Dual.new(poly,grid)
 end

 def initialize(poly=Array.new, grid=nil)
  @poly = poly
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

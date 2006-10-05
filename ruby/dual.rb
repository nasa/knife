
class Dual

 EMPTY = -1

 def Dual.from_grid_and_cut_surface(grid,cut_surface)

  puts "primal has #{grid.nnode} nodes"

  touched = Array.new(grid.nnode)

  #make volume segments
  grid.createConn

  puts "primal has #{grid.nconn} edges"

  puts "interrogate primal edges"

  start_time = Time.now
  grid.nconn.times do |conn_index|
   conn_nodes = grid.conn2Node(conn_index)
   xyz0 = grid.nodeXYZ(conn_nodes[0])
   xyz1 = grid.nodeXYZ(conn_nodes[1])
   center = [ 0.5*(xyz0[0]+xyz1[0]), 
              0.5*(xyz0[1]+xyz1[1]), 
              0.5*(xyz0[2]+xyz1[2])]
   dx = xyz0[0]-xyz1[0]
   dy = xyz0[1]-xyz1[1]
   dz = xyz0[2]-xyz1[2]
   diameter = 0.5000001*Math.sqrt(dx*dx+dy*dy+dz*dz)
   probe = Near.new(-1,center[0],center[1],center[2],diameter)
   cut_surface.near_tree.first.touched(probe).each do |index|
    tool = cut_surface.triangles[index]
    t = Intersection.core(tool.node0,tool.node1,tool.node2,xyz0,xyz1)
    unless t.nil?
     touched[conn_nodes[0]] = true
     touched[conn_nodes[1]] = true
     break
    end
   end
  end

  puts "the primal edge interrogation required #{Time.now-start_time} sec"


  Dual.new(poly,triangle,tets,grid)
 end

 def initialize(poly=Array.new, triangles=Array.new, tets=Array.new, grid=nil)
  @poly      = poly
  @cut_poly  = Array.new
  @triangles = triangles
  @tets      = tets
  @grid      = grid
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

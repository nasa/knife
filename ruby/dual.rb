
refine_path = File.expand_path("~/GRIDEX/refine/src")

$:.push refine_path

require 'Near/Near'

class Dual

 EMPTY = -1

 def Dual.grid_duals_touched_by(grid,cut_surface)

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
   cut_surface.triangle_near_tree.first.touched(probe).each do |index|
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
  start_time = Time.now

  grid.ncell.times do |cell_index|
   cell_nodes = grid.cell(cell_index)
   xyz0 = grid.nodeXYZ(cell_nodes[0])
   xyz1 = grid.nodeXYZ(cell_nodes[1])
   xyz2 = grid.nodeXYZ(cell_nodes[2])
   xyz3 = grid.nodeXYZ(cell_nodes[3])
   center = [ 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]), 
              0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]), 
              0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]) ]

   dx = xyz0[0]-center[0];dy = xyz0[1]-center[1];dz = xyz0[2]-center[2]
   diameter = Math.sqrt(dx*dx+dy*dy+dz*dz)

   dx = xyz1[0]-center[0];dy = xyz1[1]-center[1];dz = xyz1[2]-center[2]
   diameter = [diameter,Math.sqrt(dx*dx+dy*dy+dz*dz)].max

   dx = xyz2[0]-center[0];dy = xyz2[1]-center[1];dz = xyz2[2]-center[2]
   diameter = [diameter,Math.sqrt(dx*dx+dy*dy+dz*dz)].max

   dx = xyz3[0]-center[0];dy = xyz3[1]-center[1];dz = xyz3[2]-center[2]
   diameter = [diameter,Math.sqrt(dx*dx+dy*dy+dz*dz)].max
   
   diameter = 1.00000001*diameter

   probe = Near.new(-1,center[0],center[1],center[2],diameter)
   cut_surface.triangle_near_tree.first.touched(probe).each do |index|
    tool = cut_surface.triangles[index]
    hit = false
    
    t = Intersection.core(xyz1,xyz2,xyz3,tool.node0,tool.node1)
    hit = true unless t.nil?

    t = Intersection.core(xyz0,xyz2,xyz3,tool.node0,tool.node1)
    hit = true unless t.nil?

    t = Intersection.core(xyz0,xyz1,xyz3,tool.node0,tool.node1)
    hit = true unless t.nil?

    t = Intersection.core(xyz0,xyz1,xyz2,tool.node0,tool.node1)
    hit = true unless t.nil?

    if hit
     touched[cell_nodes[0]] = true
     touched[cell_nodes[1]] = true
     touched[cell_nodes[2]] = true
     touched[cell_nodes[3]] = true
     break
    end

   end
  end

  puts "the primal face interrogation required #{Time.now-start_time} sec"

  touched_nodes = 0
  touched.each do |node|
   touched_nodes += 1 if node
  end

  puts "duals #{touched_nodes} of #{grid.nnode} required"

 end


 def Dual.from_grid_and_cut_surface(grid,cut_surface)

  touched = grid_duals_touched_by(grid,cut_surface)


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

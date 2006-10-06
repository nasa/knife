
require 'dual_helper'

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

  touched
 end

 def Dual.expand_touched(orig_touched,grid)
  touched = Array.new(grid.nnode)

  start_time = Time.now
  grid.ncell.times do |cell_index|
   cell_nodes = grid.cell(cell_index)
   if ( orig_touched[cell_nodes[0]] ||
        orig_touched[cell_nodes[1]] ||
        orig_touched[cell_nodes[2]] ||
        orig_touched[cell_nodes[3]] )
     touched[cell_nodes[0]] = true
     touched[cell_nodes[1]] = true
     touched[cell_nodes[2]] = true
     touched[cell_nodes[3]] = true
   end
  end
  
  puts "the touch expansion required #{Time.now-start_time} sec"

  touched_nodes = 0
  touched.each do |node|
   touched_nodes += 1 if node
  end

  puts "duals #{touched_nodes} of #{grid.nnode} required"

  touched
 end

 def Dual.from_grid_and_cut_surface(grid,cut_surface)

  GC.disable

  touched = grid_duals_touched_by(grid,cut_surface)
  touched = expand_touched(touched,grid)

  puts "dual has #{grid.nnode} polyhedra"

  poly = Array.new(grid.nnode)
  touched.each_with_index do |required, node_index|
   poly[node_index] = Polyhedron.new if required
  end

  puts "create primal edge centers for dual"

  edge_center = Array.new(grid.nconn)
  
  node_count = 0
  grid.nconn.times do |conn_index|
   conn_nodes = grid.conn2Node(conn_index)
   if touched[conn_nodes[0]] || touched[conn_nodes[1]]
    xyz0 = grid.nodeXYZ(conn_nodes[0])
    xyz1 = grid.nodeXYZ(conn_nodes[1])
    edge_center[conn_index] = Node.new( 0.5*(xyz0[0]+xyz1[0]),
                                        0.5*(xyz0[1]+xyz1[1]),
                                        0.5*(xyz0[2]+xyz1[2]), 
                                        node_count )
    node_count += 1
   end
  end

  puts "create node finder"

  node_finder = NodeFinder.new(grid)

  ntets = 0
  tets = Array.new(grid.ncell)
  grid.ncell.times do |cell|
   nodes = grid.cell(cell)
   if  ( touched[nodes[0]] || touched[nodes[1]] || 
         touched[nodes[2]] || touched[nodes[3]] )
    xyz0 = grid.nodeXYZ(nodes[0])
    xyz1 = grid.nodeXYZ(nodes[1])
    xyz2 = grid.nodeXYZ(nodes[2])
    xyz3 = grid.nodeXYZ(nodes[3])
    center = Node.new( 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]),
                       0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]),
                       0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]), 
                       node_count )
    node_count += 1
    ntets += 1
    tets[cell] = Tet.new(nodes,
                         poly.values_at(nodes[0],nodes[1],nodes[2],nodes[3]),
                         center,
                         edge_center.values_at(grid.cell2Conn(cell,0),
                                               grid.cell2Conn(cell,1),
                                               grid.cell2Conn(cell,2),
                                               grid.cell2Conn(cell,3),
                                               grid.cell2Conn(cell,4),
                                               grid.cell2Conn(cell,5)))
   end
  end

  puts "primal has #{ntets} of #{grid.ncell} tets touched"

  tets.each_with_index do |t,cell|
   next if t.nil?
   4.times do |face_index|
    face_nodes = t.face_index2face_nodes(face_index)
    next unless ( touched[face_nodes[0]] || 
                  touched[face_nodes[1]] || 
                  touched[face_nodes[2]] )
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
     node_count = t.boundary_face(face_index,faceid,xyz,node_count,node_finder)
    else
     raise "tet missing" if tets[other_cell].nil?
     node_count = t.shares_face_with(face_index,tets[other_cell],xyz,node_count)
    end
   end
  end

  puts "create segment finder"

  segment_finder = SegmentFinder.new(node_count)

  puts "explicitly creating dual"

  triangle = Array.new

  count = 0
  start_time = Time.now
  tets.each_with_index do |t,i|
   t.create_dual(segment_finder, node_finder, triangle) unless t.nil?
  end

  puts "the dual construction required #{Time.now-start_time} sec"

  Dual.new(poly,triangle,tets,grid,cut_surface)
 end

 def initialize(poly=Array.new, triangles=Array.new, tets=Array.new, 
                grid=nil, cut_surface=nil)
  @poly        = poly
  @cut_poly    = Array.new
  @triangles   = triangles
  @tets        = tets
  @grid        = grid
  @cut_surface = cut_surface
  gc_pulse
 end

 def gc_pulse
  start_time = Time.now
  print "GC pulse "
  GC.enable
  GC.start
  GC.disable
  puts "required #{Time.now-start_time} sec"
 end

 def boolean_subtract

  start_time = Time.now
  @triangles.each do |triangle|
   center = triangle.center
   diameter = triangle.diameter
   probe = Near.new(-1,center[0],center[1],center[2],diameter)
   @cut_surface.triangle_near_tree.first.touched(probe).each do |index|
    tool = @cut_surface.triangles[index]
    Cut.between(triangle,tool)
   end
  end
  puts "the cuts required #{Time.now-start_time} sec"

  gc_pulse

  start_time = Time.now
  @cut_surface.triangulate
  puts "the cut triangulation required #{Time.now-start_time} sec"

  gc_pulse

  start_time = Time.now
  triangulate
  puts "the volume triangulation required #{Time.now-start_time} sec"

  start_time = Time.now
  gather_cuts
  puts "the gather_cuts required #{Time.now-start_time} sec"

  start_time = Time.now
  trim_external
  puts "the trim_external required #{Time.now-start_time} sec"

  gc_pulse

  start_time = Time.now
  paint
  puts "the painting required #{Time.now-start_time} sec"

  puts "#{@cut_poly.size} of #{@poly.size} polyhedra cut"

  if false
   start_time = Time.now
   section
   puts "the sectioning required #{Time.now-start_time} sec"
  else
   single_section
   puts "SKIP SECTIONING"
  end 

  start_time = Time.now
  mark_exterior
  puts "the exterior determination required #{Time.now-start_time} sec"
  

  gc_pulse

  self
 end

 def triangulate
  @triangles.each do |triangle|
   begin
    triangle.triangulate_cuts
   rescue RuntimeError
    triangle.eps( sprintf('vol%04d.eps',count))
    triangle.dump(sprintf('vol%04d.t',  count))
    puts "#{count} raised `#$!' at "+triangle.center.join(',')
   end
   if triangle.min_subtri_area < 1.0e-15
    triangle.eps( sprintf('vol%04d.eps',count))
    triangle.dump(sprintf('vol%04d.t',  count))
    raise "negative domain subtri area #{triangle.min_subtri_area}"
   end
  end
  self
 end

 def gather_cuts
  @poly.each do |poly|
   unless poly.nil?
    poly.gather_cutters
    if poly.cutters.size > 0
     @cut_poly << poly
    end
   end
  end
  self
 end

 def trim_external
  @cut_poly.each do |poly|
   poly.trim_external_subtri
  end
  self
 end

 def paint
  @cut_poly.each do |poly|
   poly.paint
  end
  self
 end

 def single_section
  @cut_poly.each do |poly|
   poly.single_section
  end
 end

 def section
  @cut_poly.each do |poly|
   poly.section
  end
 end

 def mark_exterior
  @cut_poly.each do |poly|
   poly.mark_exterior
  end

  present = 0
  active = 0
  @poly.each do |poly|
   if !poly.nil?
    present += 1
    if poly.active
     active += 1
    end
   end
  end
  inactive = present-active
  puts "poly: present #{present} active #{active} inactive #{inactive} cut #{@cut_poly.size}"

  self
 end

 def dump_grid_for_fun3d
 end

 def write_tecplot(filename='dual.t')
  File.open(filename,'w') do |f|
   f.puts 'title="dual geometry"'+"\n"+'variables="x","y","z"'+"\n"
   @poly.each do |poly|
    if !poly.nil? && poly.active
     f.print poly.tecplot_zone
    end
   end
  end
  self
 end

end

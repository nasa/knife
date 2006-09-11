
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

class Domain

 PXE_BoundaryEG = 0
 PXE_ExteriorEG = 2

 attr_reader :poly, :triangles, :cut_poly, :grid
 attr_accessor :bflags
 attr_accessor :element_group_sizes

 def Domain.cell2face(face)
  case face
  when 0; [1,3,2]
  when 1; [0,2,3]
  when 2; [0,3,1]
  when 3; [0,1,2]
  else; nil; end
 end

 def Domain.cell_face_index(cell,face)
  return 0 if cell.values_at(1,2,3).sort == face.sort
  return 1 if cell.values_at(0,2,3).sort == face.sort
  return 2 if cell.values_at(0,1,3).sort == face.sort
  return 3 if cell.values_at(0,1,2).sort == face.sort
  nil
 end

 def Domain.from_grid(volume_grid)

  #make volume nodes
  volume_node = Array.new(volume_grid.nnode)

  volume_grid.nnode.times do |node_index|
   xyz = volume_grid.nodeXYZ(node_index)
   volume_node[node_index] = Node.new(xyz[0],xyz[1],xyz[2])
   volume_node[node_index].indx = node_index
  end

  puts "#{volume_grid.nnode} domain nodes created"

  #make volume segments
  volume_grid.createConn

  segment = Array.new(volume_grid.nconn)
  
  volume_grid.nconn.times do |conn_index|
   nodes = volume_grid.conn2Node(conn_index)
   segment[conn_index] = Segment.new(volume_node[nodes.min],
                                     volume_node[nodes.max])
  end

  puts "#{volume_grid.nconn} domain segments created"

  cell2tri = Array.new(4*volume_grid.ncell)
  volume_triangles = Array.new

  volume_poly = Array.new(volume_grid.ncell)

  volume_grid.ncell.times do |cell_index|
   poly = Polyhedron.new
   volume_poly[cell_index] = poly
   cell_nodes = volume_grid.cell(cell_index)
   poly.original_nodes = cell_nodes
   4.times do |face_index|
    if cell2tri[face_index+4*cell_index]
     poly.add_reversed_triangle cell2tri[face_index+4*cell_index]
    else
     tri_index = cell2face(face_index)
     tri_nodes = cell_nodes.values_at(tri_index[0],tri_index[1],tri_index[2])
     segment0 = segment[volume_grid.findConn(tri_nodes[1],tri_nodes[2])]
     segment1 = segment[volume_grid.findConn(tri_nodes[2],tri_nodes[0])]
     segment2 = segment[volume_grid.findConn(tri_nodes[0],tri_nodes[1])]
     tri = Triangle.new( segment0, segment1, segment2)
     volume_triangles << tri
     cell2tri[face_index+4*cell_index] = tri
     poly.add_triangle tri
     other_cell = volume_grid.findOtherCellWith3Nodes(tri_nodes[0],tri_nodes[1],
                                                      tri_nodes[2],cell_index)
     if other_cell >= 0
      indx = cell_face_index(volume_grid.cell(other_cell),
                             tri_nodes) + 4*other_cell
      cell2tri[indx] = tri
     end
    end
   end  
  end

  volume_triangles.each do |triangle|
   faceid = volume_grid.faceId(triangle.node0.indx,
                               triangle.node1.indx,
                               triangle.node2.indx)
   triangle.boundary_group = faceid if faceid
  end

  puts "domain has #{volume_triangles.size} unique tetrahedral sides"

  Domain.new(volume_poly,volume_triangles,volume_grid)
 end

 def initialize(poly,triangles,grid=nil)
  @poly = poly
  @triangles= triangles
  @cut_poly = Array.new

  @grid = grid
 end

 def boolean_subtract(cut_surface)

  start_time = Time.now
  @triangles.each do |triangle|
   center = triangle.center
   diameter = triangle.diameter
   probe = Near.new(-1,center[0],center[1],center[2],diameter)
   cut_surface.near_tree.first.touched(probe).each do |index|
    tool = cut_surface.triangles[index]
    Cut.between(triangle,tool)
   end
  end
  puts "the cuts required #{Time.now-start_time} sec"

  start_time = Time.now
  cut_surface.triangulate
  puts "the cut triangulation required #{Time.now-start_time} sec"

  start_time = Time.now
  triangulate
  puts "the volume triangulation required #{Time.now-start_time} sec"

  start_time = Time.now
  paint
  puts "the painting required #{Time.now-start_time} sec"

  puts "#{@cut_poly.size} of #{@poly.size} ployhedra cut"

  if false
   start_time = Time.now
   section
   puts "the sectioning required #{Time.now-start_time} sec"
  else
   single_section
   puts "SKIP SECTIONING FOR P0 TEST"
  end 

  start_time = Time.now
  mark_exterior
  puts "the exterior determination required #{Time.now-start_time} sec"

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

 def paint
  @poly.each do |poly|
   poly.gather_cutters
   if poly.cutters.size > 0
    poly.trim_external_subtri
    poly.paint
    @cut_poly << poly
   end
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
  self
 end

 def number_of_element_groups
  nElementGroup = 0
  @poly.each do |poly|
   nElementGroup = [ nElementGroup, poly.element_group ].max
  end
  nElementGroup += 1  
 end

 def assign_element_group_index
  @cut_group = number_of_element_groups
  @exterior_group = @cut_group+1
  @bflags << PXE_BoundaryEG
  @bflags << PXE_ExteriorEG
  puts "cut group #{@cut_group} exterior group #{@exterior_group}"
  @element_group_sizes = Array.new @exterior_group+1
  @element_group_sizes.collect! { 0 }
  @poly.each do |poly|
   poly.element_group = @exterior_group unless poly.active
   poly.element_group = @cut_group if poly.cut?
   if poly.cut?
    @element_group_sizes[poly.element_group] += poly.unique_marks.size
   else
    @element_group_sizes[poly.element_group] += 1
   end
  end
  @element_group_sizes.each_with_index do |count,indx| 
   printf("element group %3d has %6d members with %d bflag\n",
          indx,count,@bflags[indx])
  end
  self
 end

 def dump_grid_for_pxa

  assign_element_group_index

  interior_faces = 0
  @triangles.each do |triangle|
   unless triangle.boundary_group
    triangle.indx = interior_faces
    interior_faces += 1
   end
  end
  File.open('postslice.eg','w') do |f|
   f.puts @bflags.size
   @bflags.each_index do |element_group|
    type = Polyhedron::PXE_TetQ1
    type = Polyhedron::PXE_TetCut if element_group == @cut_group
    f.puts type
    f.puts @bflags[element_group]
    f.puts @element_group_sizes[element_group]
    indx = 0
    @poly.each do |poly|
     if element_group == poly.element_group
      poly.indx = indx
      indx +=1
      poly.triangles.each do |triangle|
       if triangle.boundary_group
        f.puts(-triangle.boundary_group)
       else
        f.puts triangle.indx
       end
      end
     end
    end
    @poly.each do |poly|
     f.puts poly.original_nodes.join(' ') if element_group == poly.element_group
    end
   end
  end

  File.open('postslice.if','w') do |f|
   f.puts interior_faces
   @triangles.each do |triangle|
    unless triangle.boundary_group
     raise "interior faces needs two poly" unless 2 == triangle.polyhedra.size
     triangle.polyhedra.each do |poly|
      f.printf("%10d %10d %10d\n", 
               poly.element_group, poly.indx, poly.triangle_index(triangle))
     end
    end
   end
  end

  File.open('postslice.bf','w') do |f|
   nbound = 0
   @triangles.each do |triangle|
    nbound = [nbound,triangle.boundary_group].max if triangle.boundary_group
   end
   f.puts nbound
   puts "number of boundary groups #{nbound}"
   nbound.times do |boundary_index|
    nbface = 0
    @triangles.each do |triangle|
     nbface += 1 if boundary_index == triangle.boundary_group
    end
    f.puts nbface
    puts "boundary group #{boundary_index} has #{nbface} faces"
    @triangles.each do |triangle|
     if boundary_index == triangle.boundary_group
      raise "interior faces needs two poly" unless 1 == triangle.polyhedra.size
      poly = triangle.polyhedra.first
      f.printf("%10d %10d %10d\n", 
               poly.element_group, poly.indx, poly.triangle_index(triangle))
     end
    end
    
   end
  end

 end

 def write_tecplot(filename='domain.t')
  File.open(filename,'w') do |f|
   f.puts 'title="domain geometry"'+"\n"+'variables="x","y","z"'+"\n"
   npoly = 0
   @poly.each do |poly|
    npoly +=1 if poly.cutters.empty? && poly.active
   end
   f.printf("zone t=%s, i=%d, j=%d, f=fepoint, et=tetrahedron\n",
            "uncut", @grid.nnode, npoly)
   @grid.nnode.times do |indx|
    f.puts @grid.nodeXYZ(indx).join(' ')
   end
   @poly.each do |poly|
    if poly.cutters.empty? && poly.active
     f.puts poly.original_nodes.collect{|n|n+1}.join(' ') 
    end
   end
   @poly.each do |poly|
    f.print poly.tecplot_zone unless poly.cutters.empty?
   end
  end
  self
 end

end

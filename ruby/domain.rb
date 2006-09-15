
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

class Domain

 PXE_BoundaryEG = 0
 PXE_InteriorEG = 1
 PXE_ExteriorEG = 2
 PX_BFACE_WHOLE = -1
 PX_BFACE_NULL = -2

 attr_reader :poly, :triangles, :cut_poly, :grid

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
   poly.original_node_indexes = cell_nodes
   poly.original_nodes = cell_nodes.collect {|i| volume_node[i]}
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

 def get_element_groups_from_file(filename ='preslice.eg')
  File.open(filename) do |eg|
   @number_of_element_groups = eg.gets.to_i
   @bflags = Array.new(@number_of_element_groups)
   @bflags.collect! { eg.gets.to_i }
   nelem = eg.gets.to_i
   raise "inconsistant preslice element count" if @poly.size != nelem
   @poly.each do |poly|
    poly.element_group = eg.gets.to_i
   end
  end
 end

 def add_new_element_group(bflag)
  @bflags << bflag
  new_element_group = @number_of_element_groups
  @number_of_element_groups += 1
  return new_element_group
 end

 def assign_element_group_index

  @boundary_cut_group = add_new_element_group(PXE_BoundaryEG)
  @interior_cut_group = add_new_element_group(PXE_InteriorEG)
  @exterior_group     = add_new_element_group(PXE_ExteriorEG)

  @cut_groups = [@boundary_cut_group,@interior_cut_group]

  puts "boundary cut group #{@boundary_cut_group}"
  puts "interior cut group #{@interior_cut_group}"
  puts "exterior group     #{@exterior_group}"

  @element_group_sizes = Array.new(@number_of_element_groups)
  @element_group_sizes.collect! { 0 }
  @poly.each do |poly|
   poly.element_group = @exterior_group unless poly.active
   if poly.cut?
    if poly.has_boundary_triangle?
     poly.element_group = @boundary_cut_group
    else
     poly.element_group = @interior_cut_group
    end
    @element_group_sizes[poly.element_group] += poly.unique_marks.size
   else
    @element_group_sizes[poly.element_group] += 1
   end
  end
  
  puts "before removal"

  @element_group_sizes.each_with_index do |count,indx| 
   printf("element group %3d has %6d members with %d bflag\n",
          indx,count,@bflags[indx])
  end

  remove_empty_element_groups

  puts "after removal"
  @element_group_sizes.each_with_index do |count,indx| 
   printf("element group %3d has %6d members with %d bflag\n",
          indx,count,@bflags[indx])
  end
  
  puts "cut groups #{@cut_groups.join(' ')}"

  self
 end

 def delete_group(element_group)
  @element_group_sizes.delete_at element_group
  @bflags.delete_at element_group
  @number_of_element_groups -= 1

  @boundary_cut_group = nil if element_group == @boundary_cut_group
  @interior_cut_group = nil if element_group == @interior_cut_group
  @exterior_group     = nil if element_group == @exterior_group

  @poly.each do |poly|
   poly.element_group -= 1 if element_group < poly.element_group
  end
  
  @boundary_cut_group -= 1 unless(@boundary_cut_group.nil? || 
                                  element_group > @boundary_cut_group )
  @interior_cut_group -= 1 unless(@interior_cut_group.nil? || 
                                  element_group > @interior_cut_group )
  @exterior_group -= 1     unless(@exterior_group.nil? || 
                                  element_group > @exterior_group )
 end

 def remove_empty_element_groups
  original_groups = @number_of_element_groups
  
  (original_groups-1).downto(0) do |element_group|
   delete_group(element_group) if 0 == @element_group_sizes[element_group]
  end

  @cut_groups = [@boundary_cut_group,@interior_cut_group].compact
  
 end

 def dump_grid_for_pxa

  assign_element_group_index

  interior_faces = 0
  @triangles.each do |triangle|
   if triangle.boundary_group.nil?
    triangle.interior_index = interior_faces
    interior_faces += 1
   end
  end

  nbound = 0
  @triangles.each do |triangle|
   nbound = [nbound,triangle.boundary_group].max if triangle.boundary_group
  end

  File.open('postslice.eg','w') do |f|
   f.puts @number_of_element_groups
   @number_of_element_groups.times do |element_group|
    type = Polyhedron::PXE_TetQ1
    type = Polyhedron::PXE_TetCut if @cut_groups.include? element_group
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
        f.puts triangle.interior_index
       end
      end
     end
    end
    @poly.each do |poly|
     f.puts poly.original_node_indexes.join(' ') if element_group == poly.element_group
    end
   end
  end

  puts "DUMMY INTEGRATION RULES FOR P0 TEST"
  File.open('postslice.eq','w') do |f|
   f.puts @cut_groups.size
   @cut_groups.each do |element_group|
    f.puts element_group
    f.puts @element_group_sizes[element_group]
    @poly.each do |poly|
     poly.dump_integration_rule(f) if element_group == poly.element_group
    end
   end
  end

  File.open('postslice.if','w') do |f|
   f.puts interior_faces
   @triangles.each do |triangle|
    unless triangle.interior_index.nil?
     raise "interior faces needs two poly" unless 2 == triangle.polyhedra.size
     triangle.polyhedra.each do |poly|
      f.printf("%10d %10d %10d\n", 
               poly.element_group, poly.indx, poly.triangle_index(triangle))
     end
    end
   end
  end

  cut_face_rules = 0
  @triangles.each do |triangle|
   if triangle.active && 1 < triangle.subtris.size
    triangle.quad_rule_index = cut_face_rules
    cut_face_rules += 1
   end
  end

  File.open('postslice.iq','w') do |f|

   f.puts interior_faces
   @triangles.each do |triangle|
    unless triangle.interior_index.nil?
     if triangle.active
      if 1 == triangle.subtris.size
       f.puts PX_BFACE_WHOLE
      else
       f.puts triangle.quad_rule_index
      end
     else
      f.puts PX_BFACE_NULL
     end
    end
   end

   # existing face (interior and boundary) quad rules
   # flux uses 2/2*area, 1/2*w

   f.puts cut_face_rules
   @triangles.each do |triangle|
    unless triangle.quad_rule_index.nil?
     area = triangle.area
     mask = triangle.polyhedra.first.triangle_mask(triangle)
     quad = mask.active_subtri_quadrature
     f.puts quad.size
     quad.each do |rule|
      f.puts "#{rule[1]} #{rule[2]} #{rule[3]/area}"
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
   nbound.times do |bi|
    boundary_index = bi+1
    nbface = 0
    @triangles.each do |triangle|
     nbface += 1 if boundary_index == triangle.boundary_group
    end
    f.puts nbface
    puts "boundary group #{boundary_index} has #{nbface} faces"
    @triangles.each do |triangle|
     if boundary_index == triangle.boundary_group
      raise "boundary faces need one poly" unless 1 == triangle.polyhedra.size
      poly = triangle.polyhedra.first
      f.printf("%10d %10d %10d\n", 
               poly.element_group, poly.indx, poly.triangle_index(triangle))
     end
    end
    @triangles.each do |triangle|
     if boundary_index == triangle.boundary_group
      if triangle.active
       if triangle.quad_rule_index.nil?
        f.puts PX_BFACE_WHOLE
       else
        f.puts triangle.quad_rule_index
       end
      else
       f.puts PX_BFACE_NULL
      end
     end
    end 
   end # nbound
  end # postslice.bf

  File.open('postslice.cs','w') do |f|
   f.puts @cut_poly.size
   @cut_poly.each do |poly|
    f.printf("%2d %10d\n",poly.element_group,poly.indx)
   end
   @cut_poly.each do |poly|
     quad = poly.cut_surface_quadrature
     f.puts quad.size
     quad.each do |rule|
      f.puts rule.join(' ')
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
   if npoly > 0
    f.printf("zone t=%s, i=%d, j=%d, f=fepoint, et=tetrahedron\n",
             "uncut", @grid.nnode, npoly)
    @grid.nnode.times do |indx|
     f.puts @grid.nodeXYZ(indx).join(' ')
    end
    @poly.each do |poly|
     if poly.cutters.empty? && poly.active
      f.puts poly.original_node_indexes.collect{|n|n+1}.join(' ') 
     end
    end
   end
   @poly.each do |poly|
    f.print poly.tecplot_zone unless poly.cutters.empty?
   end
  end
  self
 end

end

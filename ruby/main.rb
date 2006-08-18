#!/usr/bin/env ruby

refine_path = File.expand_path("~/GRIDEX/refine/src")

$:.push refine_path

require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

# for Grid...
require 'Adj/Adj'
require 'Line/Line'
require 'Sort/Sort'
require 'Grid/Grid'
require 'GridMath/GridMath'
require 'Near/Near'

initial_time = Time.now

# read in the grid
surface_grid = Grid.from_FAST File.expand_path("~/GRIDEX/refine/test/om6_inv08.fgrid")

# create a global to local index for surface nodes
triangles = 0
surface_g2l = Array.new(surface_grid.nnode)
surface_grid.nface.times do |face_index|
 face = surface_grid.face(face_index)
 if ( 1 == face[3] || 2 == face[3] )
  surface_g2l[face[0]] = true
  surface_g2l[face[1]] = true
  surface_g2l[face[2]] = true
  triangles += 1
 end
end

puts "cut surface has #{triangles} triangless"

node = 0
surface_grid.nnode.times do |node_index|
 if surface_g2l[node_index] 
  surface_g2l[node_index] = node
  node += 1
 end
end

puts "cut surface has #{node} nodes"

# create surface node objects
surface_node = Array.new(node)
node = 0
surface_grid.nnode.times do |node_index|
 if surface_g2l[node_index]
  xyz = surface_grid.nodeXYZ(node_index)
  surface_node[node] = Node.new(xyz[0],xyz[1],xyz[2])
  node += 1
 end
end

# count unique segments
segment = Array.new(node)
node.times { |i| segment[i] = Array.new(node) }

segments = 0
surface_grid.nface.times do |face_index|
 face = surface_grid.face(face_index)
 if ( 1 == face[3] || 2 == face[3] )
  face[0] = surface_g2l[face[0]]
  face[1] = surface_g2l[face[1]]
  face[2] = surface_g2l[face[2]]

  node0 = [face[0],face[1]].min
  node1 = [face[0],face[1]].max
  unless segment[node0][node1]
   segment[node0][node1] = Segment.new(surface_node[node0],surface_node[node1])
   segments += 1
  end

  node0 = [face[1],face[2]].min
  node1 = [face[1],face[2]].max
  unless segment[node0][node1]
   segment[node0][node1] = Segment.new(surface_node[node0],surface_node[node1])
   segments += 1
  end

  node0 = [face[2],face[0]].min
  node1 = [face[2],face[0]].max
  unless segment[node0][node1]
   segment[node0][node1] = Segment.new(surface_node[node0],surface_node[node1])
   segments += 1
  end

 end
end

puts "cut surface has #{segments} segments"

cut_surface = Array.new(triangles)

triangles = 0
surface_grid.nface.times do |face_index|
 face = surface_grid.face(face_index)
 if ( 1 == face[3] || 2 == face[3] )
  face[0] = surface_g2l[face[0]]
  face[1] = surface_g2l[face[1]]
  face[2] = surface_g2l[face[2]]

  node0 = [face[0],face[1]].min
  node1 = [face[0],face[1]].max
  segment2 = segment[node0][node1]

  node0 = [face[1],face[2]].min
  node1 = [face[1],face[2]].max
  segment0 = segment[node0][node1]

  node0 = [face[2],face[0]].min
  node1 = [face[2],face[0]].max
  segment1 = segment[node0][node1]

  cut_surface[triangles] = Triangle.new(segment0,segment1,segment2)
  triangles += 1
 end
end

#build a near tree to speed up searches
start_time = Time.now
near_list = Array.new(cut_surface.size)
cut_surface.each_with_index do |triangle, index|
 center = triangle.center
 diameter = triangle.diameter
 near_list[index] = Near.new(index,center[0],center[1],center[2],diameter)
end
cut_tree = near_list.first
near_list.each_index do |index|
 cut_tree.insert(near_list[index]) if index > 0
end
puts "the tree built in #{Time.now-start_time} sec"

#load up volume to cut
volume_grid = Grid.from_FAST File.expand_path("~/GRIDEX/refine/test/om6box.fgrid")

#make volume nodes
volume_node = Array.new(volume_grid.nnode)

volume_grid.nnode.times do |node_index|
 xyz = volume_grid.nodeXYZ(node_index)
 volume_node[node_index] = Node.new(xyz[0],xyz[1],xyz[2])
end
puts "#{volume_grid.nnode} volume nodes created"

#make volume segments
volume_grid.createConn

segment = Array.new(volume_grid.nconn)

volume_grid.nconn.times do |conn_index|
 nodes = volume_grid.conn2Node(conn_index)
 segment[conn_index] = Segment.new(volume_node[nodes.min],
                                   volume_node[nodes.max])
end
puts "#{volume_grid.nconn} volume segments created"

def cell2face(face)
 case face
 when 0; [1,3,2]
 when 1; [0,2,3]
 when 2; [0,3,1]
 when 3; [0,1,2]
 else; nil; end
end

def cell_face_index(cell,face)
 return 0 if cell.values_at(1,2,3).sort == face.sort
 return 1 if cell.values_at(0,2,3).sort == face.sort
 return 2 if cell.values_at(0,1,3).sort == face.sort
 return 3 if cell.values_at(0,1,2).sort == face.sort
 nil
end

cell2tri = Array.new(4*volume_grid.ncell)
volume_triangles = Array.new

volume_poly = Array.new(volume_grid.ncell)

volume_grid.ncell.times do |cell_index|
 poly = Polyhedron.new
 volume_poly[cell_index] = poly
 cell_nodes = volume_grid.cell(cell_index)
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
    indx = cell_face_index(volume_grid.cell(other_cell),tri_nodes)+4*other_cell
    cell2tri[indx] = tri
   end
  end
 end  
end

puts "volume has #{volume_triangles.size} unique tetrahedral sides"

start_time = Time.now
count = 0
volume_triangles.each do |triangle|
 count += 1 
 printf("%6d of %6d\n",count,volume_triangles.size) if count.divmod(1000)[1]==0
 center = triangle.center
 diameter = triangle.diameter
 probe = Near.new(-1,center[0],center[1],center[2],diameter)
 cut_tree.touched(probe).each do |index|
  tool = cut_surface[index]
  begin
  Cut.between(triangle,tool)
  rescue RuntimeError
   puts "#{count} raised `#$!' at "+triangle.center.join(',')
  end
 end
end
puts "the cuts required #{Time.now-start_time} sec"

start_time = Time.now
count = 0
cut_surface.each do |triangle|
 count += 1 
 printf("%6d of %6d\n",count,cut_surface.size) if count.divmod(100)[1]==0
 begin
  triangle.triangulate_cuts
 rescue RuntimeError
  triangle.eps( sprintf('tri%04d.eps',count))
  triangle.dump(sprintf('tri%04d.t',  count))
  puts "#{count} raised `#$!' at "+triangle.center.join(',')
 end
 puts triangle.min_subtri_area if triangle.min_subtri_area < 1.0e-15
end
puts "the cut triangulation required #{Time.now-start_time} sec"

start_time = Time.now
count = 0
volume_triangles.each do |triangle|
 count += 1 
 printf("%6d of %6d\n",count,volume_triangles.size) if count.divmod(1000)[1]==0
 begin
  triangle.triangulate_cuts
 rescue RuntimeError
  triangle.eps( sprintf('vol%04d.eps',count))
  triangle.dump(sprintf('vol%04d.t',  count))
  puts "#{count} raised `#$!' at "+triangle.center.join(',')
 end
 if triangle.min_subtri_area < 1.0e-15
  puts triangle.min_subtri_area 
  triangle.eps( sprintf('vol%04d.eps',count))
  triangle.dump(sprintf('vol%04d.t',  count))
 end
end
puts "the volume triangulation required #{Time.now-start_time} sec"

File.open('om6_cut_surface.t','w') do |f|
 f.print cut_surface.first.tecplot_header
 cut_surface.each do |triangle|
  f.print triangle.tecplot_zone
 end
end

ncut = 0
volume_poly.each do |poly|
 poly.gather_cutters
 ncut += 1 if poly.cutters.size > 0
end

puts "#{ncut} of #{volume_poly.size} ployhedra cut"

File.open('om6_cut_poly.t','w') do |f|
 f.print volume_poly.first.tecplot_header
 volume_poly.each do |poly|
  f.print poly.tecplot_zone if poly.cutters
 end
end

puts "the entire process required #{Time.now-initial_time} sec"

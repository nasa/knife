#!/usr/bin/env ruby

refine_path = File.expand_path("~/GRIDEX/refine/src")

$:.push refine_path

require 'node'
require 'segment'
require 'triangle'

# for Grid...
require 'Adj/Adj'
require 'Line/Line'
require 'Sort/Sort'
require 'Grid/Grid'
require 'GridMath/GridMath'
require 'Near/Near'

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

volume_grid = Grid.from_FAST File.expand_path("~/GRIDEX/refine/test/om6box.fgrid")

volume = Array.new
volume_grid.ncell.times do |cell_index|
 cell = volume_grid.cell(cell_index)
 volume << Polyhedron.new(volume_grid.nodeXYZ(cell[0]),
                          volume_grid.nodeXYZ(cell[1]),
                          volume_grid.nodeXYZ(cell[2]),
                          volume_grid.nodeXYZ(cell[3]) )
end
puts "volume has #{volume.size} tets"

start_time = Time.now
count = 0
volume.each do |polyhedron|
 count += 1 ; puts "#{count} of #{volume.size}" if count.divmod(100)[1]==0
 center = polyhedron.center
 diameter = polyhedron.diameter
 probe = Near.new(-1,center[0],center[1],center[2],diameter)
 cut_tree.touched(probe).each do |index|
  tool = cut_surface[index]
  begin
  polyhedron.cut_with(tool)
  rescue RuntimeError
   puts "#$! raised at "+polyhedron.center.join(',')
  end
 end
end
puts "the cuts required #{Time.now-start_time} sec"

File.open('cut_om6.t','w') do |f|
 f.print cut_surface.first.tecplot_header
 cut_surface.each do |triangle|
  f.print triangle.tecplot_zone
 end
 volume.each do |poly|
  if poly.cut?
   poly.exterior.each do |triangle|
    f.print triangle.tecplot_zone
   end
  end
 end
end

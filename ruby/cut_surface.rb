
require 'node'
require 'segment'
require 'triangle'

refine_path = File.expand_path("~/GRIDEX/refine/src")

$:.push refine_path

require 'Near/Near'

class CutSurface

 attr_reader :triangles
 attr_reader :grid
 attr_reader :triangle_near_tree
 attr_reader :segment_near_tree

 def CutSurface.from_grid(surface_grid,cut_bcs,reversed=false)

  # create a global to local index for surface nodes
  triangles = 0
  surface_g2l = Array.new(surface_grid.nnode)
  surface_grid.nface.times do |face_index|
   face = surface_grid.face(face_index)
   if cut_bcs.include? face[3]
    surface_g2l[face[0]] = true
    surface_g2l[face[1]] = true
    surface_g2l[face[2]] = true
    triangles += 1
   end
  end

  puts "cut surface has #{triangles} triangles"

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
  node.times { |i| segment[i] = Hash.new }

  segments = 0
  surface_grid.nface.times do |face_index|
   face = surface_grid.face(face_index)
   if cut_bcs.include? face[3]
    face[0] = surface_g2l[face[0]]
    face[1] = surface_g2l[face[1]]
    face[2] = surface_g2l[face[2]]

    if reversed
     temp = face[0]
     face[0] = face[1]
     face[1] = temp
    end

    node0 = [face[0],face[1]].min
    node1 = [face[0],face[1]].max
    unless segment[node0][node1]
     segment[node0][node1] = Segment.new(surface_node[node0],
                                         surface_node[node1])
     segments += 1
    end

    node0 = [face[1],face[2]].min
    node1 = [face[1],face[2]].max
    unless segment[node0][node1]
     segment[node0][node1] = Segment.new(surface_node[node0],
                                         surface_node[node1])
     segments += 1
    end
   
    node0 = [face[2],face[0]].min
    node1 = [face[2],face[0]].max
    unless segment[node0][node1]
     segment[node0][node1] = Segment.new(surface_node[node0],
                                         surface_node[node1])
     segments += 1
    end

   end
  end

  puts "cut surface has #{segments} segments"

  cut_triangles = Array.new(triangles)

  triangles = 0
  surface_grid.nface.times do |face_index|
   face = surface_grid.face(face_index)
   if cut_bcs.include? face[3]
    face[0] = surface_g2l[face[0]]
    face[1] = surface_g2l[face[1]]
    face[2] = surface_g2l[face[2]]

    if reversed
     temp = face[0]
     face[0] = face[1]
     face[1] = temp
    end

    node0 = [face[0],face[1]].min
    node1 = [face[0],face[1]].max
    segment2 = segment[node0][node1]
    
    node0 = [face[1],face[2]].min
    node1 = [face[1],face[2]].max
    segment0 = segment[node0][node1]
    
    node0 = [face[2],face[0]].min
    node1 = [face[2],face[0]].max
    segment1 = segment[node0][node1]
    
    cut_triangles[triangles] = Triangle.new(segment0,segment1,segment2)
    triangles += 1
   end
  end
  
  cut_segments = Array.new(segments)
  segments = 0
  segment.each do |node0_segments|
   node0_segments.each_value do |node0_node1_segment|
    cut_segments[segments] = node0_node1_segment
    segments += 1
   end
  end

  puts "cut surface has #{segments} segments"

  CutSurface.new(cut_triangles,cut_segments,surface_grid)
 end

 def initialize(triangles,segments,grid=nil)
  @triangles = triangles
  @segments  = segments
  @grid = grid
  
  @triangle_near_tree = build_triangle_near_list
  @segment_near_tree = build_segment_near_list
 end

 #build a near tree to speed up searches
 def build_triangle_near_list
  near_list = Array.new(@triangles.size)
  @triangles.each_with_index do |triangle, index|
   center = triangle.center
   diameter = triangle.diameter
   near_list[index] = Near.new(index,center[0],center[1],center[2],diameter)
  end
  near_list.each_index do |index|
   near_list.first.insert(near_list[index]) if index > 0
  end
  near_list
 end

 def build_segment_near_list
  near_list = Array.new(@segments.size)
  @segments.each_with_index do |segment, index|
   xyz0 = segment.node0
   xyz1 = segment.node1
   center = [ 0.5*(xyz0[0]+xyz1[0]), 
              0.5*(xyz0[1]+xyz1[1]), 
              0.5*(xyz0[2]+xyz1[2])]
   dx = xyz0[0]-xyz1[0]
   dy = xyz0[1]-xyz1[1]
   dz = xyz0[2]-xyz1[2]
   diameter = 0.5000001*Math.sqrt(dx*dx+dy*dy+dz*dz)
   near_list[index] = Near.new(index,center[0],center[1],center[2],diameter)
  end
  near_list.each_index do |index|
   near_list.first.insert(near_list[index]) if index > 0
  end
  near_list
 end


 def triangulate
  @triangles.each do |triangle|
   triangle.triangulate_cuts
   if triangle.min_subtri_area < 1.0e-15
    triangle.eps
    raise "negative cut surface subtri area #{triangle.min_subtri_area}"
   end
  end
  self
 end

 def write_tecplot(filename='cut_surface.t')
  File.open(filename,'w') do |f|
   f.print @triangles.first.tecplot_header
   @triangles.each do |triangle|
    f.print triangle.tecplot_zone
   end
  end
  self
 end

end


require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

class NodeFinder

 def initialize(grid)
  @grid = grid
  @nodes = Array.new(@grid.nnode)
 end

 def get(node,node_index=nil)
  if @nodes[node].nil?
   raise "NodeFinder Failed" if node_index.nil?
   xyz = @grid.nodeXYZ(node)
   @nodes[node] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
   node_index +=1
  end
  return @nodes[node], node_index
 end

end

class SegmentFinder

 def initialize(nnode)
  @segment = Array.new(nnode)
  @segment.collect! { Array.new(nnode) }
 end

 def between(n0,n1)
  i0 = [n0.indx,n1.indx].min
  i1 = [n0.indx,n1.indx].max
  if @segment[i0][i1].nil?
   @segment[i0][i1] = Segment.new(n0,n1)
  end
  @segment[i0][i1]
 end

end

class Tet

 attr_reader :poly

 def initialize(nodes,poly,center,edge_center)
  @nodes = nodes
  @poly = poly
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

 def boundary_face(face_index,faceid,xyz,node_index,node_finder)
  face_index2face_nodes(face_index).each do |node|
   new_node, node_index = node_finder.get(node,node_index)
  end
  @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
  node_index +=1
  @boundary[face_index] = faceid
  node_index
 end

 def shares_face_with(face_index, other_tet, xyz, node_index)
  @neighbor[face_index] = other_tet
  @face_center[face_index], node_index = 
   other_tet.get_center(self,face_index2face_nodes(face_index),xyz, node_index)
  node_index
 end

 def get_center(other_tet,face_nodes,xyz,node_index)
  face_index = face_nodes2face_index(face_nodes)
  if @face_center[face_index].nil?
   @neighbor[face_index] = other_tet
   @face_center[face_index] = Node.new( xyz[0], xyz[1], xyz[2], node_index )
   node_index += 1
  end
  return @face_center[face_index], node_index
 end
 
 EDGE2NODE0 = [0, 0, 0, 1, 1, 2]
 EDGE2NODE1 = [1, 2, 3, 2, 3, 3]
 EDGE2FACE0 = [2, 3, 1, 0, 2, 0]
 EDGE2FACE1 = [3, 1, 2, 3, 0, 1]
 
 def create_dual(segment_finder, node_finder, triangle)

  6.times do |edge_index|
   edge_node = @edge_center[edge_index]

   poly_rev = @poly[EDGE2NODE0[edge_index]]
   poly_fwd = @poly[EDGE2NODE1[edge_index]]
   
   n0 = edge_node
   n1 = @center
   n2 = @face_center[EDGE2FACE0[edge_index]]

   s0 = segment_finder.between(n1,n2)
   s1 = segment_finder.between(n0,n2)
   s2 = segment_finder.between(n0,n1)

   tri = Triangle.new(s0,s1,s2)

   triangle << tri
   poly_fwd.add_triangle tri
   poly_rev.add_reversed_triangle tri

   n0 = edge_node
   n1 = @face_center[EDGE2FACE1[edge_index]]
   n2 = @center

   s0 = segment_finder.between(n1,n2)
   s1 = segment_finder.between(n0,n2)
   s2 = segment_finder.between(n0,n1)

   tri = Triangle.new(s0,s1,s2)

   triangle << tri
   poly_fwd.add_triangle tri
   poly_rev.add_reversed_triangle tri

  end

  @boundary.each_with_index do |faceid, face_index|
   unless faceid.nil?

    face_nodes = face_index2face_nodes(face_index)
    n0 = @face_center[face_index]

    [[face_nodes[0], face_nodes[1]],
     [face_nodes[1], face_nodes[2]],
     [face_nodes[2], face_nodes[0]] ].each do |triangle_side|

     node0 = triangle_side[0]
     node1 = triangle_side[1]

     n1, dummy = node_finder.get(node0)
     n2 = triangle_side2edge_center(node0,node1)

     s0 = segment_finder.between(n1,n2)
     s1 = segment_finder.between(n0,n2)
     s2 = segment_finder.between(n0,n1)
     tri = Triangle.new(s0,s1,s2)

     triangle << tri
     @poly[node2index(node0)].add_triangle tri
    
     n1 = triangle_side2edge_center(node0,node1)
     n2, dummy = node_finder.get(node1)
     
     s0 = segment_finder.between(n1,n2)
     s1 = segment_finder.between(n0,n2)
     s2 = segment_finder.between(n0,n1)
     tri = Triangle.new(s0,s1,s2)

     triangle << tri
     @poly[node2index(node1)].add_triangle tri
    end # triangle side
   end # !faceid.nil?
  end

 end

 def node2index(node)
  index = @nodes.index(node)
  raise "cell node not found" if index.nil?
  index
 end

 def triangle_side2edge_center(node0, node1)
  i0 = node2index(node0)
  i1 = node2index(node1)
  index0=[i0,i1].min
  index1=[i0,i1].max
  6.times do |edge_index|
   if index0==EDGE2NODE0[edge_index] && index1==EDGE2NODE1[edge_index]
    return @edge_center [edge_index]
   end
  end
  raise "edge_node not found"
 end

 def all_poly_active?
  (@poly[0].active && @poly[1].active && @poly[2].active && @poly[3].active)
 end

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
  
  node_index = 0
  grid.nconn.times do |conn_index|
   conn_nodes = grid.conn2Node(conn_index)
   xyz0 = grid.nodeXYZ(conn_nodes[0])
   xyz1 = grid.nodeXYZ(conn_nodes[1])
   edge_center[conn_index] = Node.new( 0.5*(xyz0[0]+xyz1[0]),
                                       0.5*(xyz0[1]+xyz1[1]),
                                       0.5*(xyz0[2]+xyz1[2]), 
                                       node_index )
   node_index += 1
  end

  node_finder = NodeFinder.new(grid)

  tets = Array.new(grid.ncell)
  grid.ncell.times do |cell|
   nodes = grid.cell(cell)
   xyz0 = grid.nodeXYZ(nodes[0])
   xyz1 = grid.nodeXYZ(nodes[1])
   xyz2 = grid.nodeXYZ(nodes[2])
   xyz3 = grid.nodeXYZ(nodes[3])
   center = Node.new( 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]),
                      0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]),
                      0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]), 
                      node_index )
   node_index += 1
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

  tets.each_with_index do |t,cell|
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
     node_index = t.boundary_face(face_index,faceid,xyz,node_index,node_finder)
    else
     node_index = t.shares_face_with(face_index,tets[other_cell],xyz,node_index)
    end
   end
  end

  segment_finder = SegmentFinder.new(node_index)

  triangle = Array.new

  tets.each do |t|
   t.create_dual(segment_finder, node_finder, triangle)
  end

  Dual.new(poly,triangle,tets,grid)
 end

 def initialize(poly=Array.new, triangles=Array.new, tets=Array.new, grid=nil)
  @poly      = poly
  @cut_poly  = Array.new
  @triangles = triangles
  @tets      = tets
  @grid      = grid
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
  gather_cuts
  puts "the gather_cuts required #{Time.now-start_time} sec"

  start_time = Time.now
  trim_external
  puts "the trim_external required #{Time.now-start_time} sec"

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
   poly.gather_cutters
   if poly.cutters.size > 0
    @cut_poly << poly
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
  self
 end

 def dump_grid_for_fun3d

  File.open('postslice.nodes','w') do |f|
   nnode = 0
   @poly.each_with_index do |poly,node|
    if poly.active
     xyz = @grid.nodeXYZ(node)
     xyz = poly.centroid(xyz) if poly.cut?
     poly.primal_node = Node.new( xyz[0], xyz[1], xyz[2], nnode )
     nnode += 1
    end
   end
   
   f.puts nnode

   @poly.each do |poly|
    unless poly.primal_node.nil?
     f.puts poly.primal_node.join(' ')
    end 
   end

   @poly.each do |poly|
    unless poly.primal_node.nil?
     f.puts poly.volume(poly.primal_node)
    end 
   end
  end

  File.open('postslice.tets','w') do |f|
   ntet = 0
   @tets.each do |tet|
    if tet.all_poly_active?
     ntet += 1
    end
   end

   f.puts ntet
   @tets.each do |tet|
    if tet.all_poly_active?
     f.printf( "%d %d %d %d\n",
               tet.poly[0].primal_node.indx+1,
               tet.poly[1].primal_node.indx+1,
               tet.poly[2].primal_node.indx+1,
               tet.poly[3].primal_node.indx+1)
    end
   end
  end

  File.open('postslice.edges','w') do |f|

   nedge = 0
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    nedge +=1 if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
   end

   f.puts nedge
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
     node0 = @poly[conn_nodes[0]].primal_node.indx
     node1 = @poly[conn_nodes[1]].primal_node.indx
     if (node0<node1) # reverse edge to enforce node0 < node1
      f.printf( "%d %d\n",node0+1,node1+1)
     else
      f.printf( "%d %d\n",node1+1,node0+1)
     end
    end
   end
   
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
     poly0 = @poly[conn_nodes[0]]
     poly1 = @poly[conn_nodes[1]]
     node0 = poly0.primal_node.indx
     node1 = poly1.primal_node.indx
     if (node0<node1) # reverse edge to enforce node0 < node1
      f.puts poly0.directed_area_to(poly1).join(' ')
     else
      f.puts poly1.directed_area_to(poly0).join(' ')
     end
    end
   end

   norig = 0
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
     norig +=1 unless @poly[conn_nodes[0]].cut? || @poly[conn_nodes[1]].cut?
    end
   end

   f.puts norig
   nedge = 0
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
     f.puts(nedge+1) unless ( @poly[conn_nodes[0]].cut? || 
                              @poly[conn_nodes[1]].cut? )
     nedge += 1
    end
   end
  end

  File.open('postslice.faces','w') do |f|

   ncut = 0
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    if @poly[conn_nodes[0]].active && @poly[conn_nodes[1]].active
     ncut +=1 if @poly[conn_nodes[0]].cut? || @poly[conn_nodes[1]].cut?
    end
   end

   f.puts ncut
   @grid.nconn.times do |conn_index|
    conn_nodes = @grid.conn2Node(conn_index)
    poly0 = @poly[conn_nodes[0]]
    poly1 = @poly[conn_nodes[1]]
    if poly0.active && poly1.active
     if poly0.cut? || poly1.cut?
      node0 = poly0.primal_node.indx
      node1 = poly1.primal_node.indx
      geom = if (node0<node1) # reverse edge to enforce node0 < node1
              f.printf( "%d %d\n",node0+1,node1+1)
              poly0.face_geometry_to(poly1)
             else
              f.printf( "%d %d\n",node1+1,node0+1)
              poly1.face_geometry_to(poly0)
             end
      f.puts geom.size
      geom.each do |tri|
       f.puts tri.join(' ')
      end
     end
    end
   end

  end # postslice.faces
  
  File.open('postslice.bound','w') do |f|
   puts "NBOUND"

   puts "UNCUT IBNODE"

   puts "UNCUT FACE2NODE"
  end

  File.open('postslice.surf','w') do |f|
   puts "All the little faces for boundary integration"

  end

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

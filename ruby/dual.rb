
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

class Tet

 attr_reader :nodes
 attr_redaer :center

 def initialize(grid,nodes)
  @grid = grid
  @nodes = nodes
  create_center
 end

 def create_center
  xyz0 = @grid.nodeXYZ(@node[0])
  xyz1 = @grid.nodeXYZ(@node[1])
  xyz2 = @grid.nodeXYZ(@node[2])
  xyz3 = @grid.nodeXYZ(@node[3])
  @center = Node.new( 0.25*(xyz0[0]+xyz1[0]+xyz2[0]+xyz3[0]),
                      0.25*(xyz0[1]+xyz1[1]+xyz2[1]+xyz3[1]),
                      0.25*(xyz0[2]+xyz1[2]+xyz2[2]+xyz3[2]) )
 end

end

class Dual

 attr_reader :grid

 def Dual.from_grid(grid)
  poly = Array.new(grid.nnode)
  grid.nnode.times do |node|
   poly[node] = Polyhedron.new
  end

  tets = Array.new(grid.ncell)
  grid.ncell.times do |cell|
   tets[cell] = Tet.new(grid.cell(cell))
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

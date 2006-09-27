
require 'node'
require 'segment'
require 'triangle'
require 'polyhedron'
require 'cut'

class Dual

 attr_reader :grid

 def Dual.from_grid(volume_grid)
  Dual.new(Array.new,volume_grid)
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


class Dual

 EMPTY = -1

 def Dual.from_grid_near_surface(grid,cut_surface)

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

  self
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

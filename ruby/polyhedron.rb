
class Polyhedron

 attr_reader :triangles, :reversed
 attr_reader :cutters

 def initialize
  @triangles = Array.new
  @reversed  = Array.new
 end

 def add_triangle(triangle, reversed = false)
  @triangles << triangle
  @reversed << reversed
  self
 end

 def add_reversed_triangle(triangle)
  add_triangle(triangle, true)
 end

 def gather_cutters
  @cutters = Array.new
  @triangles.each do |triangle|
   @cutters += triangle.cut_by
  end
  @cutters.uniq!
 end

 def tecplot_header
  @triangles.first.tecplot_header
 end

 def tecplot_zone
  output = ""
  @triangles.each do |triangle|
   output += triangle.tecplot_zone
  end
  if @cutters
   @cutters.each do |triangle|
    output += triangle.tecplot_zone
   end
  end
  output
 end

end

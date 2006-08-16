
class Polyhedron

 attr_reader :triangles, :reversed

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

end

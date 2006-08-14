#  node of a triangle created from 

class TriangleNode

 attr_reader :u, :v, :w

 def initialize(triangle,index)
  @u = 0.0
  @v = 0.0
  @w = 0.0
  case index
  when 0
   @u = 1.0
  when 1
   @v = 1.0
  when 2
   @w = 1.0
  else
   raise "index #{index}"
  end
 end

end

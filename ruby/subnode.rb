#  node of a triangle created from 

class TriangleNode

 attr_reader :u, :v, :w
 attr_reader :parent

 def initialize(u,v,w,parent=nil)
  @u = u
  @v = v
  @w = w
  @parent = parent
 end

end

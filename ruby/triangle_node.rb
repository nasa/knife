#  node of a triangle created from 

class TriangleNode

 attr_reader :u, :v, :w
 attr_reader :intersection

 def initialize(u,v,w,intersection=nil)
  @u = u
  @v = v
  @w = w
  @intersection = intersection
 end

end

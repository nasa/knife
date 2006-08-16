#  node of a triangle

class Subnode

 attr_reader :u, :v, :w
 attr_reader :parent

 def initialize(u,v,w,parent=nil)
  @u = u
  @v = v
  @w = w
  @parent = parent
 end

 def x; parent.x; end
 def y; parent.y; end
 def z; parent.z; end

end

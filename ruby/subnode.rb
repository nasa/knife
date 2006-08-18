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

 def [](xyz)
  return x if 0==xyz
  return y if 1==xyz
  return z if 2==xyz
  nil
 end

end

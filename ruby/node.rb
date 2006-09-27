#  x-y-z triplet

class Node < Array

 attr_accessor :indx

 def initialize(x, y, z, indx = nil)
  self << x
  self << y
  self << z
  @indx = indx
 end

 def x; self[0]; end
 def y; self[1]; end
 def z; self[2]; end

end

#  x-y-z triplet

class Node < Array

 def initialize(x, y, z)
  self << x
  self << y
  self << z
 end

 def x; self[0]; end
 def y; self[1]; end
 def z; self[2]; end

end

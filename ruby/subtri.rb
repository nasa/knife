#  sub triangle defined by 3 triangle nodes

require 'intersection'
require 'cut'

class Subtri

 attr_accessor :n0, :n1, :n2
 attr_accessor :s0, :s1, :s2

 def Subtri.area(n0,n1,n2)
  a = n0.v-n2.v
  b = n0.w-n2.w
  c = n1.v-n2.v
  d = n1.w-n2.w
  0.5*(a*d-b*c)
 end

 def Subtri.mean_ratio(n0,n1,n2)
  dv0 = n2.v-n1.v
  dv1 = n0.v-n2.v
  dv2 = n1.v-n0.v
  dw0 = n2.w-n1.w
  dw1 = n0.w-n2.w
  dw2 = n1.w-n0.w

  l2 = dv0*dv0+dw0*dw0 + dv1*dv1+dw1*dw1 + dv2*dv2+dw2*dw2

  4.0*(1.73205080756888)*area(n0,n1,n2)/l2 # 4 times the sqrt(3) to normalize
 end

 def Subtri.right_handed?(n0,n1,n2)
  area(n0,n1,n2) > 1.0e-15
 end

 def initialize(n0, n1, n2, s0=nil, s1=nil, s2=nil)
  @n0 = n0
  @n1 = n1
  @n2 = n2

  @s0 = s0
  @s1 = s1
  @s2 = s2
 end

 def has?(node)
  (n0==node||n1==node||n2==node)
 end

 def set_side(node0, node1, new_side = nil)
  if ( (node0 == @n0 && node1 == @n1) || (node1 == @n0 && node0 == @n1) )
   @s2 = new_side
   return self
  end
  if ( (node0 == @n1 && node1 == @n2) || (node1 == @n1 && node0 == @n2) )
   @s0 = new_side
   return self
  end
  if ( (node0 == @n2 && node1 == @n0) || (node1 == @n2 && node0 == @n0) )
   @s1 = new_side
   return self
  end
  nil
 end

 def side_with_nodes(node0, node1)
  if ( (node0 == @n0 && node1 == @n1) || (node1 == @n0 && node0 == @n1) )
   return s2
  end
  if ( (node0 == @n1 && node1 == @n2) || (node1 == @n1 && node0 == @n2) )
   return s0
  end
  if ( (node0 == @n2 && node1 == @n0) || (node1 == @n2 && node0 == @n0) )
   return s1
  end
  nil
 end

 def split_side_with(newnode,node0,node1)
  if ( (node0 == n0 && node1 == n1) || (node1 == n0 && node0 == n1) )
   subtri = dup
   subtri.n1 = newnode
   @n0 = newnode
   set_side(newnode,@n2)
   subtri.set_side(newnode,subtri.n2)
   return subtri
  end
  if ( (node0 == n1 && node1 == n2) || (node1 == n1 && node0 == n2) )
   subtri = dup
   subtri.n2 = newnode
   @n1 = newnode
   set_side(newnode,@n0)
   subtri.set_side(newnode,subtri.n0)
   return subtri
  end
  if ( (node0 == n2 && node1 == n0) || (node1 == n2 && node0 == n0) )
   subtri = dup
   subtri.n0 = newnode
   @n2 = newnode
   set_side(newnode,@n1)
   subtri.set_side(newnode,subtri.n1)
   return subtri
  end
  return nil
 end

 def orient(node)
  return [n0, n1, n2] if node==n0
  return [n1, n2, n0] if node==n1
  return [n2, n0, n1] if node==n2
  raise "node #{node} not found for orient"
 end

 def barycentric(node)
  area0 = Subtri.area(node,n1,n2)
  area1 = Subtri.area(n0,node,n2)
  area2 = Subtri.area(n0,n1,node)
  total = area0+area1+area2
  [area0/total, area1/total, area2/total]
 end

 def area
  Subtri.area(n0,n1,n2)
 end

 def mean_ratio
  Subtri.mean_ratio(n0,n1,n2)
 end

 def above(other)
  node = n0
  node = n1 if ( node.parent == other.n0.parent || 
                 node.parent == other.n1.parent || 
                 node.parent == other.n2.parent )
  node = n2 if ( node.parent == other.n0.parent || 
                 node.parent == other.n1.parent || 
                 node.parent == other.n2.parent )
  (Intersection.volume6(other.n0,other.n1,other.n2,node) > 0.0)
 end

 def each_cut
  yield s0 if s0.is_a?(Cut)
  yield s1 if s1.is_a?(Cut)
  yield s2 if s2.is_a?(Cut)
 end

 def directed_area
  node0 = n0.parent
  node1 = n1.parent
  node2 = n2.parent
  edge1 = [node1.x-node0.x, node1.y-node0.y, node1.z-node0.z]
  edge2 = [node2.x-node0.x, node2.y-node0.y, node2.z-node0.z]
  norm = [ edge1[1]*edge2[2] - edge1[2]*edge2[1],
           edge1[2]*edge2[0] - edge1[0]*edge2[2],
           edge1[0]*edge2[1] - edge1[1]*edge2[0] ] 
 end

 def quadrature_rule
  norm = directed_area
  area = 0.5*Math.sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2])
  b0 = 1.0/3.0; b1 = 1.0/3.0; b2 = 1.0/3.0;
  [ [b0*n0.u+b1*n1.u+b2*n2.u, 
     b0*n0.v+b1*n1.v+b2*n2.v,
     b0*n0.w+b1*n1.w+b2*n2.w,
     area] ]
 end

 def physical_quadrature_rule
  norm = directed_area
  area2 = Math.sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2])
  b0 = 1.0/3.0; b1 = 1.0/3.0; b2 = 1.0/3.0;
  [ [b0*n0.x+b1*n1.x+b2*n2.x, 
     b0*n0.y+b1*n1.y+b2*n2.y,
     b0*n0.z+b1*n1.z+b2*n2.z,
     2.0,
     norm[0], norm[1], norm[2]] ]
 end

end

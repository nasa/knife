#  sub triangle defined by 3 triangle nodes

class Subtri

 attr_accessor :n0, :n1, :n2
 attr_accessor :s0, :s1, :s2

 def Subtri.area(n0,n1,n2)
  a = (n0.v-n2.v)
  b = (n0.w-n2.w)
  c = (n1.v-n2.v)
  d = (n1.w-n2.w)
  0.5*(a*d-b*c)
 end

 def Subtri.right_handed?(n0,n1,n2)
  area(n0,n1,n2) > 1.0e-15
 end

 def initialize(n0, n1, n2)
  @n0 = n0
  @n1 = n1
  @n2 = n2
 end

 def has?(node)
  (n0==node||n1==node||n2==node)
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

end

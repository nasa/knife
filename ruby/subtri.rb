#  sub triangle defined by 3 triangle nodes

class Subtri

 attr_accessor :n0, :n1, :n2

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
  u0 = n1.v-n0.v
  u1 = n1.w-n0.w
  v0 = n2.v-n0.v
  v1 = n2.w-n0.w
  w0 = node.v-n0.v
  w1 = node.w-n0.w

  uv = u0*v0+u1*v1
  wv = w0*v0+w1*v1
  wu = w0*u0+w1*u1

  uu = u0*u0+u1*u1
  vv = v0*v0+v1*v1

  denom = uv*uv - uu*vv
  raise "barycentric denom #{denom} small" if denom.abs < 1.0e-15
  s = (uv*wv-vv*wu)/denom;
  t = (uv*wu-uu*wv)/denom;

  bary = [1.0-s-t,s,t]
 end

end

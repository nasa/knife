#  polyhedron face defined by 3 parents

class Polytri

 attr_accessor :n0, :n1, :n2
 attr_accessor :s0, :s1, :s2

 def initialize(n0, n1, n2, s0=nil, s1=nil, s2=nil)
  @n0 = n0
  @n1 = n1
  @n2 = n2

  @s0 = s0
  @s1 = s1
  @s2 = s2
 end

end

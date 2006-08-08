
class Triangle

 attr_reader :nodes, :children

 def initialize(n0,n1,n2)
  @nodes = [n0, n1, n2]
  @children = [[0,1,2]]
 end

 def cut_with(n0,n1,n2)
  false
 end

end

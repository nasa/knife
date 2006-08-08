
class Triangle

 EMPTY = -1

 attr_reader :nodes, :children

 def initialize(n0,n1,n2)
  @nodes = [n0, n1, n2]
  @children = [[0,1,2]]
 end

 def add_unique_node(new_node)
  smallest_one_norm = one_norm(new_node,@nodes[0])
  smallest_index = 0
  @nodes.each_index do |existing_node_index|
   norm = one_norm(new_node,@nodes[existing_node_index])
   if (norm < smallest_one_norm)
    smallest_one_norm = norm
    smallest_index = existing_node_index
   end
  end
  if smallest_one_norm < 1.0e-14
   smallest_index
  else
   @nodes << new_node
   (@nodes.size-1)
  end
 end

 def cut_with(n0,n1,n2)
  false
 end

#extract to node or vertex class?

 def one_norm(n0,n1)
  (n0[0]-n1[0]).abs + (n0[1]-n1[1]).abs + (n0[2]-n1[2]).abs
 end


end

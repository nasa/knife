# defined between two nodes

class Segment

 attr_reader :cuts

 def initialize(node0, node1)
  @nodes = [node0, node1]
  @cuts = Array.new
 end

 def node(index)
  @nodes[index]
 end

end

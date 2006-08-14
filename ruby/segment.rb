# defined between two nodes

class Segment

 attr_reader :intersections

 def initialize(node0, node1)
  @nodes = [node0, node1]
  @intersections = Array.new
 end

 def node(index)
  @nodes[index]
 end

end

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

 def common_node(segment)
  return node(0) if node(0)==segment.node(0)
  return node(0) if node(0)==segment.node(1)
  return node(1) if node(1)==segment.node(0)
  return node(1) if node(1)==segment.node(1)
  nil
 end

end

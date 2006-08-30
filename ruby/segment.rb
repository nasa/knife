# defined between two nodes

class Segment

 attr_reader :node0, :node1
 attr_reader :intersections
 attr_reader :triangles

 def initialize(node0, node1)
  @node0 = node0
  @node1 = node1
  @intersections = Array.new
  @triangles = Array.new
 end

 def common_node(segment)
  return node0 if node0==segment.node0
  return node0 if node0==segment.node1
  return node1 if node1==segment.node0
  return node1 if node1==segment.node1
  nil
 end

end

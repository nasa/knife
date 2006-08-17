# intersection of two triangles

require "intersection"

class Cut

 attr_reader :intersection0, :intersection1

 def Cut.between(triangle0, triangle1)
  return nil if exclude?(triangle0, triangle1)
  intersections = Array.new
  triangle0.segments.each do |segment|
   intersection = Intersection.of(triangle1, segment)
   intersections << intersection if intersection
  end
  triangle1.segments.each do |segment|
   intersection = Intersection.of(triangle0, segment)
   intersections << intersection if intersection
  end
  return nil if 0 == intersections.size
  raise "improper cut #{intersections.size}" unless 2 == intersections.size
  Cut.new(intersections[0],intersections[1], triangle0, triangle1)
 end
 
 def Cut.exclude?(triangle0, triangle1)
  t0 = triangle0.node0
  t1 = triangle0.node1
  t2 = triangle0.node2

  s0 = triangle1.node0
  s1 = triangle1.node1
  s2 = triangle1.node2

  v0 = Intersection.volume6(t0,t1,t2,s0)
  v1 = Intersection.volume6(t0,t1,t2,s1)
  v2 = Intersection.volume6(t0,t1,t2,s2)
  return true if ( v0 > 0.0 && v1 > 0.0 && v2 > 0.0)
  return true if ( v0 < 0.0 && v1 < 0.0 && v2 < 0.0)

  v0 = Intersection.volume6(s0,s1,s2,t0)
  v1 = Intersection.volume6(s0,s1,s2,t1)
  v2 = Intersection.volume6(s0,s1,s2,t2)
  return true if ( v0 > 0.0 && v1 > 0.0 && v2 > 0.0)
  return true if ( v0 < 0.0 && v1 < 0.0 && v2 < 0.0)

  false
 end

 def initialize(intersection0, intersection1, triangle0, triangle1)
  @intersection0 = intersection0
  @intersection1 = intersection1
  @triangles = [triangle0, triangle1]
  triangle0.cuts << self
  triangle1.cuts << self
 end

end

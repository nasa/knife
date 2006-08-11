# intersection of two triangles

require "intersection"

class Cut

 def Cut.between(triangle0, triangle1)
  intersections = Array.new
  triangle0.each do |segment|
   intersection = Intersection.of(triangle1, segment)
   intersections << intersection if intersection
  end
  triangle1.each do |segment|
   intersection = Intersection.of(triangle0, segment)
   intersections << intersection if intersection
  end
  return nil if 0 == intersections.size
  raise "improper cut #{intersections.size}" unless 2 == intersections.size
  Cut.new(intersections[0],intersections[1])
 end
 
 def initialize(intersection0,intersection1)
  @intersection0 = intersection0
  @intersection1 = intersection1
 end

end

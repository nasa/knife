# intersection of two triangles

class Cut

 def Cut.between(triangle0, triangle1)
  intersections = Array.new
  triangle0.each do |segment|
   intersections << [triangle1, segment] if Cut.intersect(triangle1, segment)
  end
  triangle1.each do |segment|
   intersections << [triangle0, segment] if Cut.intersect(triangle0,segment)
  end
  return nil unless 2 == intersections.size
  Cut.new(intersections[0],intersections[1])
 end

 def Cut.intersect(triangle, segment)
  
  t0 = triangle.node(0)
  t1 = triangle.node(1)
  t2 = triangle.node(2)

  s0 = segment[0]
  s1 = segment[1]

  singular_tol = 1.0e-12

  # these tetradedral volumes are made of 3 triangle vertexes 
  # and segment endpoints
  top_volume    = Cut.volume6(t0, t1, t2, s0)
  bottom_volume = Cut.volume6(t0, t1, t2, s1)

  puts "top #{top_volume}" if top_volume.abs < singular_tol
  puts "bottom #{bottom_volume}" if bottom_volume.abs < singular_tol

  # raise exception if degeneracy detected
  raise 'degeneracy, top_volume zero'    if top_volume.zero?
  raise 'degeneracy, bottom_volume zero' if bottom_volume.zero?

  # if signs match, segment is entirely above or below triangle
  return nil if (top_volume > 0.0 && bottom_volume > 0.0 )
  return nil if (top_volume < 0.0 && bottom_volume < 0.0 )
  
  # see if segment passes through triangle
  volume_side2 = Cut.volume6(t0, t1, s0, s1)
  volume_side0 = Cut.volume6(t1, t2, s0, s1)
  volume_side1 = Cut.volume6(t2, t0, s0, s1)

  puts "side0 #{volume_side0}" if volume_side0.abs < singular_tol
  puts "side1 #{volume_side1}" if volume_side1.abs < singular_tol
  puts "side2 #{volume_side2}" if volume_side2.abs < singular_tol

  # raise exception if degeneracy detected
  raise 'degeneracy, volume_side0 zero' if volume_side0.zero?
  raise 'degeneracy, volume_side1 zero' if volume_side1.zero?
  raise 'degeneracy, volume_side2 zero' if volume_side2.zero?

  # if signs match segment is inside triangle
  unless ( (volume_side0 > 0.0 && volume_side1 > 0.0 && volume_side2 > 0.0 ) ||
           (volume_side0 < 0.0 && volume_side1 < 0.0 && volume_side2 < 0.0 ) )
   return nil
  end

  # find intersection point
  true
 end

 def Cut.volume6(a,b,c,d)
  m11 = (a[0]-d[0])*((b[1]-d[1])*(c[2]-d[2])-(c[1]-d[1])*(b[2]-d[2]))
  m12 = (a[1]-d[1])*((b[0]-d[0])*(c[2]-d[2])-(c[0]-d[0])*(b[2]-d[2]))
  m13 = (a[2]-d[2])*((b[0]-d[0])*(c[1]-d[1])-(c[0]-d[0])*(b[1]-d[1]))
  det = -( m11 - m12 + m13 )
 end

 def initialize(intersection0,intersection1)
  @intersection0 = intersection0
  @intersection1 = intersection1
 end

end

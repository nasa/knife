# point where a triangle and segment meet

class Intersection

 attr_reader :triangle, :segment
 attr_reader :t
 attr_reader :u, :v, :w

 def Intersection.of(triangle, segment)
  
  t0 = triangle.node(0)
  t1 = triangle.node(1)
  t2 = triangle.node(2)

  s0 = segment.node(0)
  s1 = segment.node(1)

  singular_tol = 1.0e-12

  # these tetradedral volumes are made of 3 triangle vertexes 
  # and segment endpoints
  top_volume    = Intersection.volume6(t0, t1, t2, s0)
  bottom_volume = Intersection.volume6(t0, t1, t2, s1)

  puts "top #{top_volume}" if top_volume.abs < singular_tol
  puts "bottom #{bottom_volume}" if bottom_volume.abs < singular_tol

  # raise exception if degeneracy detected
  raise 'degeneracy, top_volume zero'    if top_volume.zero?
  raise 'degeneracy, bottom_volume zero' if bottom_volume.zero?

  # if signs match, segment is entirely above or below triangle
  return nil if (top_volume > 0.0 && bottom_volume > 0.0 )
  return nil if (top_volume < 0.0 && bottom_volume < 0.0 )
  
  # see if segment passes through triangle
  volume_side2 = Intersection.volume6(t0, t1, s0, s1)
  volume_side0 = Intersection.volume6(t1, t2, s0, s1)
  volume_side1 = Intersection.volume6(t2, t0, s0, s1)

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

  new(triangle, segment)
 end

 def Intersection.volume6(a,b,c,d)
  m11 = (a[0]-d[0])*((b[1]-d[1])*(c[2]-d[2])-(c[1]-d[1])*(b[2]-d[2]))
  m12 = (a[1]-d[1])*((b[0]-d[0])*(c[2]-d[2])-(c[0]-d[0])*(b[2]-d[2]))
  m13 = (a[2]-d[2])*((b[0]-d[0])*(c[1]-d[1])-(c[0]-d[0])*(b[1]-d[1]))
  det = -( m11 - m12 + m13 )
 end

 def initialize(triangle, segment)
  @triangle = triangle
  @segment = segment

  # find intersection point
  t0 = triangle.node(0)
  t1 = triangle.node(1)
  t2 = triangle.node(2)

  s0 = segment.node(0)
  s1 = segment.node(1)

  top_volume    = Intersection.volume6(t0, t1, t2, s0)
  bottom_volume = Intersection.volume6(t0, t1, t2, s1)

  total_volume = top_volume - bottom_volume
  @t = top_volume/total_volume

  volume_side2 = Intersection.volume6(t0, t1, s0, s1)
  volume_side0 = Intersection.volume6(t1, t2, s0, s1)
  volume_side1 = Intersection.volume6(t2, t0, s0, s1)
  total_volume = volume_side0 + volume_side1 + volume_side2
  @u = volume_side0/total_volume
  @v = volume_side1/total_volume
  @w = volume_side2/total_volume
end

end

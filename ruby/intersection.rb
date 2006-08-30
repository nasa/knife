# point where a triangle and segment meet

class Intersection

 attr_reader :triangle, :segment
 attr_reader :t
 attr_reader :u, :v, :w

 def Intersection.of(triangle, segment)
  
  segment.intersections.each do |intersection|
   if (segment == intersection.segment) && (triangle == intersection.triangle)
    return intersection
   end
  end

  t0 = triangle.node0
  t1 = triangle.node1
  t2 = triangle.node2

  s0 = segment.node0
  s1 = segment.node1

  singular_tol = 1.0e-12

  # these tetradedral volumes are made of 3 triangle vertexes 
  # and segment endpoints
  top_volume    = Intersection.volume6(t0, t1, t2, s0)
  bottom_volume = Intersection.volume6(t0, t1, t2, s1)

  # raise exception if degeneracy detected
  raise "top #{top_volume}"    if top_volume.abs < singular_tol
  raise "bottom #{bottom_volume}" if bottom_volume.abs < singular_tol

  # if signs match, segment is entirely above or below triangle
  return nil if (top_volume > 0.0 && bottom_volume > 0.0 )
  return nil if (top_volume < 0.0 && bottom_volume < 0.0 )
  
  # see if segment passes through triangle
  volume_side2 = Intersection.volume6(t0, t1, s0, s1)
  volume_side0 = Intersection.volume6(t1, t2, s0, s1)
  volume_side1 = Intersection.volume6(t2, t0, s0, s1)

  # raise exception if degeneracy detected
  raise "side0 #{volume_side0}" if volume_side0.abs < singular_tol
  raise "side1 #{volume_side1}" if volume_side1.abs < singular_tol
  raise "side2 #{volume_side2}" if volume_side2.abs < singular_tol

  # if signs match segment is inside triangle
  unless ( (volume_side0 > 0.0 && volume_side1 > 0.0 && volume_side2 > 0.0 ) ||
           (volume_side0 < 0.0 && volume_side1 < 0.0 && volume_side2 < 0.0 ) )
   return nil
  end

  # compute intersection parameter, can be replaced with det ratios?
  total_volume = top_volume - bottom_volume
  t = top_volume/total_volume

  total_volume = volume_side0 + volume_side1 + volume_side2
  u = volume_side0/total_volume
  v = volume_side1/total_volume
  w = volume_side2/total_volume

  new(triangle, segment, t, u, v, w)
 end

 def Intersection.volume6(a,b,c,d)
  m11 = (a[0]-d[0])*((b[1]-d[1])*(c[2]-d[2])-(c[1]-d[1])*(b[2]-d[2]))
  m12 = (a[1]-d[1])*((b[0]-d[0])*(c[2]-d[2])-(c[0]-d[0])*(b[2]-d[2]))
  m13 = (a[2]-d[2])*((b[0]-d[0])*(c[1]-d[1])-(c[0]-d[0])*(b[1]-d[1]))
  det = -( m11 - m12 + m13 )
 end

 def initialize(triangle, segment, t, u, v, w)
  @triangle = triangle
  @segment = segment
  @t = t
  @u = u
  @v = v
  @w = w
  @segment.intersections << self
 end

 def uvw(triangle)
  if @triangle == triangle
   return @u,@v,@w
  end
  u=0.0;v=0.0;w=0.0
  u = (    @t) if @segment.node1 == triangle.node0
  v = (    @t) if @segment.node1 == triangle.node1
  w = (    @t) if @segment.node1 == triangle.node2
  u = (1.0-@t) if @segment.node0 == triangle.node0
  v = (1.0-@t) if @segment.node0 == triangle.node1
  w = (1.0-@t) if @segment.node0 == triangle.node2
  return u,v,w
 end

 def x; (1.0-t)*segment.node0.x + t*segment.node1.x; end
 def y; (1.0-t)*segment.node0.y + t*segment.node1.y; end
 def z; (1.0-t)*segment.node0.z + t*segment.node1.z; end

end

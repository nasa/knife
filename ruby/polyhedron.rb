
class Polyhedron

 attr_reader :triangles, :reversed
 attr_reader :cutters

 def initialize
  @triangles = Array.new
  @reversed  = Array.new
 end

 def add_triangle(triangle, reversed = false)
  @triangles << triangle
  @reversed << reversed
  self
 end

 def add_reversed_triangle(triangle)
  add_triangle(triangle, true)
 end

 def gather_cutters
  @cutters = Array.new
  @triangles.each do |triangle|
   @cutters += triangle.cut_by
  end
  @cutters.uniq!
  self
 end

 def copy_children
  @triangles.collect! { |triangle| triangle.dup }
  @cutters.collect! { |triangle| triangle.dup }
  self
 end

 def tecplot_header
  'title="cut cell geometry"'+"\n"+'variables="x","y","z"'+"\n"
 end

 def parent_nodes
  nodes = Array.new
  targets = @triangles
  targets += @cutters if @cutters
  targets.each do |triangle|
   triangle.subnodes.each do |subnode|
    nodes << subnode.parent
   end
  end
  nodes.uniq
 end
 
 def all_subtris
  subtris = Array.new
  targets = @triangles
  targets += @cutters if @cutters
  targets.each do |triangle|
   subtris += triangle.subtris
  end
  subtris
 end
 
 def tecplot_zone(title='surf')
  subnodes = parent_nodes
  subtris = all_subtris
  output = sprintf("zone t=%s, i=%d, j=%d, f=fepoint, et=triangle\n",
                   title, subnodes.size, subtris.size)
  subnodes.each do |subnode|
   output += sprintf("%25.15e%25.15e%25.15e\n",
                     subnode.x,subnode.y,subnode.z)
  end
  subtris.each do |subtri|
   output += sprintf(" %6d %6d %6d\n",
                     1+subnodes.index(subtri.n0.parent),
                     1+subnodes.index(subtri.n1.parent),
                     1+subnodes.index(subtri.n2.parent) )
  end
  output
 end

end

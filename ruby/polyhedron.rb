
require 'mask'

class Polyhedron

 attr_reader :triangles
 attr_reader :cutters

 def initialize
  @triangles = Array.new
  @cutters = Array.new
 end

 def add_triangle(triangle, reversed = false)
  @triangles << Mask.new(triangle,reversed)
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
  @cutters.collect! { |triangle| Mask.new(triangle) }
  self
 end

 def trim_external_subtri
  (@cutters).each do |triangle|
   triangle.deactivate_all_subtri
  end
  @cutters.each do |cutter|
   cutter.cuts.each do |cut|
    if ( @triangles.include?(cut.triangles[0]) ||
         @triangles.include?(cut.triangles[1]) )
     cutter.activate(cut.intersection0,cut.intersection1)
     cutter.activate(cut.intersection1,cut.intersection0)
    end
   end
  end
  self
 end

 def tecplot_header
  'title="cut cell geometry"'+"\n"+'variables="x","y","z"'+"\n"
 end

 def parent_nodes
  nodes = Array.new
  (@triangles+@cutters).each do |triangle|
   triangle.subnodes.each do |subnode|
    nodes << subnode.parent
   end
  end
  nodes.uniq
 end
 
 def all_subtris
  subtris = Array.new
  (@triangles+@cutters).each do |triangle|
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

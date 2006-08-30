
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
  (@triangles+@cutters).each do |triangle|
   triangle.deactivate_all_subtri
  end
  @cutters.each do |cutter|
   cutter.cuts.each do |cut|
    triang = nil
    triang = cut.triangles[0] if @triangles.include?(cut.triangles[0])
    triang = cut.triangles[1] if @triangles.include?(cut.triangles[1])
    if triang
     cutter_subtri01 = cutter.find_subtri_with_parents(cut.intersection0,
                                                       cut.intersection1)
     cutter_subtri10 = cutter.find_subtri_with_parents(cut.intersection1,
                                                       cut.intersection0)
     triang_subtri01 = triang.find_subtri_with_parents(cut.intersection0,
                                                       cut.intersection1)
     triang_subtri10 = triang.find_subtri_with_parents(cut.intersection1,
                                                       cut.intersection0)

     activate(triang_subtri01) if (triang_subtri01.above(cutter_subtri01))
     activate(triang_subtri10) if (triang_subtri10.above(cutter_subtri01))

     if reversed?(triang)
      activate(cutter_subtri01) if (!cutter_subtri01.above(triang_subtri01))
      activate(cutter_subtri10) if (!cutter_subtri10.above(triang_subtri01))
     else
      activate(cutter_subtri01) if (cutter_subtri01.above(triang_subtri01))
      activate(cutter_subtri10) if (cutter_subtri10.above(triang_subtri01))
     end
    end
   end
  end
  self
 end

 def paint
  (@triangles+@cutters).each do |triangle|
   triangle.paint
  end
  #only worry about cutters
  #redo =false
  #@cutters.each do |triangle|
  # triangle.segment.each do |segment|
  #  subtri = traingle.find_subtri_with_parents(segment.node(0),segment.node(1))
  #  if subtri.nil? # try other segment orientation
  #   subtri = traingle.find_subtri_with_parents(segment.node(1),segment.node(0))
  #  end
  #  if (!subtri.nil? && triangle.active(subtri))
  #   add nieghboring cutter, activate its subtri, paint it
  # redo = true
  #  end
  # end
  #end
  #paint if redo
  self
 end

 def reversed?(target)
  @triangles.each do |triangle|
   return true if triangle == target && triangle.reversed
  end
  false
 end

 def activate(subtri)
  (@triangles+@cutters).each do |triangle|
   triangle.activate(subtri)
  end
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


require 'mask'

class Polyhedron

 attr_reader :triangles
 attr_reader :cutters
 attr_reader :active

 def initialize
  @triangles = Array.new
  @cutters = Array.new
  @active = true
 end

 def add_triangle(triangle, reversed = false)
  mask = Mask.new(triangle,reversed)
  mask.polyhedra << self
  @triangles << mask
  self
 end

 def corner_indexes
  indx = Array.new
  @triangles.each do |tri|
   indx << tri.node0.indx+1
   indx << tri.node1.indx+1
   indx << tri.node2.indx+1
  end
  indx.uniq
 end

 def add_reversed_triangle(triangle)
  add_triangle(triangle, true)
 end

 def add_unique_cutter(cutter)
  return nil if cutter.nil?
  @cutters.each do |existing|
   return nil if existing.triangle == cutter
  end
  mask = Mask.new(cutter)
  mask.deactivate_all_subtri
  @cutters << mask
  mask
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
  # paint individuals
  (@triangles+@cutters).each do |triangle|
   triangle.paint
  end

  requires_another_coat_of_paint = false

  # activate triangles next to active subtris
  @triangles.each do |triangle|
   if 0 == triangle.cuts.size && !triangle.active?(triangle.subtris[0])
    @triangles.each do |other|
     subtri = nil
     subtri ||= other.find_subtri_with_parents(triangle.node0,triangle.node1)
     subtri ||= other.find_subtri_with_parents(triangle.node1,triangle.node0)
     subtri ||= other.find_subtri_with_parents(triangle.node1,triangle.node2)
     subtri ||= other.find_subtri_with_parents(triangle.node2,triangle.node1)
     subtri ||= other.find_subtri_with_parents(triangle.node2,triangle.node0)
     subtri ||= other.find_subtri_with_parents(triangle.node0,triangle.node2)
     if other.active?(subtri)
      triangle.activate_all_subtri
      requires_another_coat_of_paint = true
     end
    end
   end
  end
  
  # activate cutters next to active subtris
  @cutters.each do |triangle|
   triangle.segments.each do |segment|
    subtri = nil
    subtri ||= triangle.find_subtri_with_parents(segment.node0,segment.node1)
    subtri ||= triangle.find_subtri_with_parents(segment.node1,segment.node0)
    if (!subtri.nil? && triangle.active?(subtri))
     neighboring_cutter = triangle.neighboring(segment)
     neighboring_mask = add_unique_cutter neighboring_cutter
     unless neighboring_mask.nil?
      subtri = nil
      subtri ||= neighboring_mask.find_subtri_with_parents(segment.node0,
                                                           segment.node1)
      subtri ||= neighboring_mask.find_subtri_with_parents(segment.node1,
                                                           segment.node0)
      neighboring_mask.activate(subtri).paint
      requires_another_coat_of_paint = true
     end
    end
   end
  end

  return paint if requires_another_coat_of_paint

  self
 end

 def cut?
  (!@cutters.empty?)
 end

 def section
  return self unless cut?
  starting_index = 0
  (@triangles+@cutters).each do |triangle|
   starting_index = triangle.uniquely_mark(starting_index)
  end
  relax_mark
  self
 end

 def relax_mark
  # paint individuals
  (@triangles+@cutters).each do |triangle|
   triangle.relax_mark
  end

  keep_relaxing = false

  # relax across cuts
  (@triangles+@cutters).each do |mask0|
   mask0.subtris.each do |subtri0|
    subtri0.each_cut do |cut|
     triangle0 = mask0.triangle
     triangle1 = cut.other_triangle triangle0
     mask1 = triangle_mask triangle1
     subtri1 = mask1.find_subtri_with_parents(cut.intersection0,
                                              cut.intersection1)
     unless mask1.active?(subtri1)
      subtri1 = mask1.find_subtri_with_parents(cut.intersection1,
                                               cut.intersection0)
     end
     if mask1.active?(subtri1)
      indx0 = triangle0.subtris.index(subtri0)
      indx1 = triangle1.subtris.index(subtri1)
      mark0 = mask0.mark[indx0]
      mark1 = mask1.mark[indx1]
      mark = [mark0, mark1].min
      mask0.mark[indx0] = mark
      mask1.mark[indx1] = mark
      keep_relaxing = true if mark0 > mark
      keep_relaxing = true if mark1 > mark
     end
    end
   end
  end
  
  # relax across uncut segments
  (@triangles+@cutters).each do |mask0|
   if 0 == mask0.cuts.size && mask0.active?(mask0.subtris[0])
    subtri0 = mask0.subtris[0]
    (@triangles+@cutters).each do |mask1|
     subtri1 = nil
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node0,mask0.node1)
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node1,mask0.node0)
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node1,mask0.node2)
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node2,mask0.node1)
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node2,mask0.node0)
     subtri1 ||= mask1.find_subtri_with_parents(mask0.node0,mask0.node2)
     if mask1.active?(subtri1)
      indx0 = mask0.triangle.subtris.index(subtri0)
      indx1 = mask1.triangle.subtris.index(subtri1)
      mark0 = mask0.mark[indx0]
      mark1 = mask1.mark[indx1]
      mark = [mark0, mark1].min
      mask0.mark[indx0] = mark
      mask1.mark[indx1] = mark
      keep_relaxing = true if mark0 > mark
      keep_relaxing = true if mark1 > mark
     end
    end
   end
  end

  return relax_mark if keep_relaxing

  self
 end

 def triangle_mask(triangle)
  (@triangles+@cutters).each do |mask|
   return mask if triangle == mask.triangle
  end
  nil
 end

 def unique_marks
  marks = Array.new
  (@triangles+@cutters).each do |mask|
   marks += mask.mark
  end
  marks.compact.uniq
 end

 def mark_exterior
  return self unless @active
  @active = false unless cut?
  @triangles.each do |mask|
   if mask.triangle.subtris.size == 1 && mask.subtris.size == 0
    mask.mark_exterior 
   end
  end  
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

#  a triangle mash that deactivates subtris

class Mask
 
 attr_reader :triangle, :reversed

 def initialize(triangle,reversed=false)
  @triangle = triangle
  @reversed = reversed
  @trimmed = nil
 end

 def method_missing(method, *arguments)
  @triangle.send(method, *arguments)
 end

 def deactivate_all_subtri
  @trimmed = Array.new(triangle.subtris.size,true)
 end

 def activate_all_subtri
  if @trimmed.nil?
   @trimmed = Array.new(triangle.subtris.size,false)
  else
   @trimmed.collect! { false }
  end
 end

 def subtris
  if @trimmed
   out = Array.new
   triangle.subtris.size.times do |indx|
    out << triangle.subtris[indx] unless @trimmed[indx]
   end
   out
  else
   triangle.subtris
  end
 end

 def ==(other)
  (triangle == other)
 end

 def activate(subtri)
  indx = triangle.subtris.index(subtri)
  @trimmed[indx] = false if indx
  self
 end

 def active?(subtri)
  subtris.include? subtri
 end

 def paint
  subtris.each do |subtri|
   paint_subtri subtri
  end
  self
 end

 def paint_subtri(subtri)
  return self if subtri.nil?
  activate(subtri)
  paint_edge(subtri.n2,subtri.n1) if subtri.s0.nil?
  paint_edge(subtri.n0,subtri.n2) if subtri.s1.nil?
  paint_edge(subtri.n1,subtri.n0) if subtri.s2.nil?
  self
 end

 def paint_edge(n0,n1)
  subtri = find_subtri_with(n0,n1)
  return self if active?(subtri)
  paint_subtri(subtri)
 end

 def uniquely_mark(starting_index)
  @mark = Array.new(@trimmed.size)
  @trimmed.each_index do |indx|
   unless @trimmed[indx]
    @mark[indx] = starting_index
    starting_index += 1
   end
  end
  starting_index
 end

end

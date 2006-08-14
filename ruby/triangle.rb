# defined from 3 segments

class Triangle

 attr_reader :segments
 attr_reader :cuts

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
  @cuts = Array.new
 end
 
 def segment(index)
  @segments[index]
 end

 def node(index)
  case index
  when 0
   segment(2).node(0)
  when 1
   segment(0).node(0)
  when 2
   segment(1).node(0)
  else
   nil
  end
 end

end

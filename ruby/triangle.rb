# defined from 3 segments

class Triangle

 attr_reader :segments

 def initialize(segment0,segment1,segment2)
  @segments = [ segment0, segment1, segment2 ]
 end
 
 def segment(index)
  @segments[index]
 end

 def node(index)
  case index
  when 0
   @segments[2][0]
  when 1
   @segments[0][0]
  when 2
   @segments[1][0]
  else
   nil
  end
 end

end

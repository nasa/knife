# defined from 3 segments

class Triangle < Array

 def initialize(segment0,segment1,segment2)
  self << segment0
  self << segment1
  self << segment2
 end

 def node(index)
  case index
  when 0
   self[2][0]
  when 1
   self[0][0]
  when 2
   self[1][0]
  else
   nil
  end
 end

end

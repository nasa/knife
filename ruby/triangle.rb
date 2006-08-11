# defined from 3 segments

class Triangle < Array

 def initialize(segment0,segment1,segment2)
  self << segment0
  self << segment1
  self << segment2
 end

end

#  a triangle mash that deactivates subtris

class Mask
 
 attr_reader :triangle, :reversed

 def initialize(triangle,reversed=false)
  @triangle = triangle
  @reversed = reversed
 end

end

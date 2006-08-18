#  a triangle mash that deactivates subtris

class Mask
 
 attr_reader :triangle, :reversed

 def initialize(triangle,reversed=false)
  @triangle = triangle
  @reversed = reversed
 end

 def eql?(other)
  @triangle.eql?(other)
 end

 def method_missing(method, *arguments)
  @triangle.send(method, *arguments)
 end

end

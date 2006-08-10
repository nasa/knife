# ring of segments to be filled with triangles

require 'triangle'

class Ring < Array

 def Ring.from_children(children)
  ring = new
  children.each do |child|
   ring.insert_child(child)
  end
  ring
 end

 def insert_segment(segment)
  return self if include?(segment)
  if include?(segment.reverse)
   delete_if { |exisiting_segment| exisiting_segment==segment.reverse }
  else
   self << segment
  end
 end

 def insert_child(child)
  insert_segment([child[0],child[1]])
  insert_segment([child[1],child[2]])
  insert_segment([child[2],child[0]])
  self
 end

 def has?(node)
  each do |segment|
   return true if node==segment.first 
   return true if node==segment.last
  end
  false
 end

 def to_s
  out = ""
  each do |seg|
   out += "[#{seg.join(',')}]"
  end
  out
 end

 def why_no_split(node0,node1)
  puts to_s
  puts "no node0" unless has?(node0)
  puts "no node0"  unless has?(node1)
  puts "no order"  if order!(node0).nil?
 end

 def split!(node0,node1)
  return nil unless has?(node0)
  return nil unless has?(node1)
  return nil if order!(node0).nil?
  
  ring = Ring.new
  target = node1
  until target == node0
   segment = find_segment_starting_with(target)
   return nil if segment.nil?
   target = segment.last
   ring << segment
   insert_segment(segment.reverse)
  end
  insert_segment([node1,node0])
  ring.insert_segment([node0,node1])
  ring
 end

 def find_segment_starting_with(node)
  each do |segment|
   return segment if (segment.first == node)
  end
  nil
 end 

 def order!(node)
  target = node
  list = []
  until empty?
   segment = find_segment_starting_with(target)
   if segment.nil?
    list+=self
    self.clear
    list.each { |entry| self << entry }
    return nil
   end
   list << segment
   insert_segment(segment.reverse)
   target = segment.last
  end
  list+=self
  self.clear
  list.each { |entry| self << entry }
  self
 end

 def triangulate
  children = Array.new
  node0 = first.first
  order!(node0)
  self[1..(size-2)].each do |segment|
   children << [node0,segment.first,segment.last]
  end
  children
 end

end

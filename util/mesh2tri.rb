#!/usr/bin/env ruby

# use admesh to make a off file from an stl file
# http://www.varlog.com/products/admesh/ 

def vertices(mesh)
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Vertices/)
  nil
 end
 mesh[line_number+1].to_i
end

def dump_points(mesh,f)
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Vertices/)
  nil
 end
 n=mesh[line_number+1].to_i
 (line_number+2).upto(line_number+1+n) do |number|
  line = mesh[number]
  numbers = line.scan(/\S+/)
  f.puts numbers[0..2].join(' ')
 end
end

def dump_triangles(mesh,f)
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Triangles/)
  nil
 end
 n=mesh[line_number+1].to_i
 (line_number+2).upto(line_number+1+n) do |number|
  line = mesh[number]
  numbers = line.scan(/\S+/)
  f.puts numbers[0..2].join(' ')
 end
end

def dump_faceids(mesh,f)
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Triangles/)
  nil
 end
 n=mesh[line_number+1].to_i
 (line_number+2).upto(line_number+1+n) do |number|
  line = mesh[number]
  numbers = line.scan(/\S+/)
  f.puts numbers[3]
 end
end

def faceids(mesh)
 output = Array.new
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Triangles/)
  nil
 end
 n=mesh[line_number+1].to_i
 (line_number+2).upto(line_number+1+n) do |number|
  line = mesh[number]
  numbers = line.scan(/\S+/)
  output << numbers[3]
 end
 output.unique.sort
end

def triangles(mesh)
 line_number = mesh.each_with_index do |line, number|
  break(number) if (line =~ /Triangles/)
  nil
 end
 mesh[line_number+1].to_i
end

mesh = IO.readlines(ARGV[0])

nnode = vertices(mesh)
ntri = triangles(mesh)
puts "nodes "+nnode.to_s
puts "tris  "+ntri.to_s

output_file = ARGV[0]+'.tri'

File.open(output_file,'w') do |f|
 f.puts "#{nnode} #{ntri}"
 dump_points(mesh,f)
 dump_triangles(mesh,f)
 dump_faceids(mesh,f)
end

puts faceids(mesh)


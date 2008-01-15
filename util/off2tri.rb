#!/usr/bin/env ruby

# use admesh to make a off file from an stl file
# http://www.varlog.com/products/admesh/ 

off = IO.readlines(ARGV[0])

puts off[0]
size = off[1].scan(/\w+/)
nnode = size[0].to_i
ntri = size[1].to_i
puts "nodes "+nnode.to_s
puts "tris  "+ntri.to_s

File.open(ARGV[0]+'.tri','w') do |f|
 f.puts "#{nnode} #{ntri}"
 nnode.times do |n|
  f.puts off[2+n]
 end
 ntri.times do |t|
  nodes = off[2+nnode+t].scan(/\w+/)
  n1 = nodes[1].to_i+1
  n2 = nodes[2].to_i+1
  n3 = nodes[3].to_i+1
  f.puts "#{n1} #{n2} #{n3}"
 end
 ntri.times do |t|
  f.puts "1"
 end
end

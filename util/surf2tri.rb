#!/usr/bin/env ruby

off = IO.readlines(ARGV[0])

puts off[0]
size = off[0].scan(/\w+/)
nnode = size[2].to_i
ntri = size[0].to_i

puts "nodes "+nnode.to_s

File.open(ARGV[0]+'.tri','w') do |f|
 f.puts "#{nnode} #{ntri}"
 nnode.times do |n|
  values = off[1+n].scan(/\S+/)
    x = values[0]
    y = values[1]
    z = values[2]
  f.puts "#{x} #{y} #{z}"
 end

puts "tris  "+ntri.to_s

 ntri.times do |t|
  nodes = off[1+nnode+t].scan(/\w+/)
  n1 = nodes[1]
  n2 = nodes[2]
  n3 = nodes[3]
  f.puts "#{n1} #{n2} #{n3}"
 end
 ntri.times do |t|
  f.puts "1"
 end
end

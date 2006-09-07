#!/usr/bin/env ruby

require 'cut'
require 'cut_surface'
require 'domain'

initial_time = Time.now

# create cut surface from fast file
fast_filename = File.expand_path("~/GRIDEX/refine/test/om6_inv08.fgrid")
active_bcs = [1,2]
cut_surface = CutSurface.from_FAST( fast_filename, active_bcs )

# create domain from FAST file
fast_filename = File.expand_path("~/GRIDEX/refine/test/om6box.fgrid")
domain = Domain.from_FAST fast_filename

start_time = Time.now
domain.triangles.each do |triangle|
 center = triangle.center
 diameter = triangle.diameter
 probe = Near.new(-1,center[0],center[1],center[2],diameter)
 cut_surface.near_tree.first.touched(probe).each do |index|
  tool = cut_surface.triangles[index]
  begin
  Cut.between(triangle,tool)
  rescue RuntimeError
   puts "#{count} raised `#$!' at "+triangle.center.join(',')
  end
 end
end
puts "the cuts required #{Time.now-start_time} sec"

start_time = Time.now
cut_surface.triangulate
puts "the cut triangulation required #{Time.now-start_time} sec"

cut_surface.write_tecplot

start_time = Time.now
domain.triangulate
puts "the volume triangulation required #{Time.now-start_time} sec"

start_time = Time.now
domain.paint
puts "the painting required #{Time.now-start_time} sec"

puts "#{domain.cut_poly.size} of #{domain.poly.size} ployhedra cut"

start_time = Time.now
domain.section
puts "the sectioning required #{Time.now-start_time} sec"

domain.write_tecplot

puts "the entire process required #{Time.now-initial_time} sec"

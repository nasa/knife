#!/usr/bin/env ruby

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

domain.boolean_subtract cut_surface

cut_surface.write_tecplot
domain.write_tecplot

puts "the entire process required #{Time.now-initial_time} sec"

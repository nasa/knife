#!/usr/bin/env ruby

refine_path = File.expand_path("~/GRIDEX/refine/src")

$:.push refine_path

# for Grid...
require 'Adj/Adj'
require 'Line/Line'
require 'Sort/Sort'
require 'Grid/Grid'
require 'GridMath/GridMath'

require 'cut_surface'
require 'domain'

initial_time = Time.now

# create cut surface from FAST file
fast_filename = File.expand_path("~/GRIDEX/refine/test/om6_inv08.fgrid")
cut_surface_grid = Grid.from_FAST File.expand_path(fast_filename)
active_bcs = [1,2]
cut_surface = CutSurface.from_grid( cut_surface_grid, active_bcs )

# create domain from FAST file
fast_filename = File.expand_path("~/GRIDEX/refine/test/om6box.fgrid")
domain_grid = Grid.from_FAST File.expand_path(fast_filename)
domain = Domain.from_grid domain_grid

domain.boolean_subtract cut_surface

cut_surface.write_tecplot
domain.write_tecplot

puts "the entire process required #{Time.now-initial_time} sec"

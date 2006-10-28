#!/usr/bin/env ruby

files = '*.?'

file_modification_times = Hash.new
Dir[files].each do |file|
 file_modification_times[file] = File.mtime(file)
end

while true do
 run_make = false

 Dir[files].each do |file|
  modified = File.mtime(file)
  last_modified = file_modification_times[file]

  if last_modified.nil? || (modified>last_modified)
   file_modification_times[file] = modified
   run_make = true
  end
 end

 system('make '+ARGV.join(' ')) if (run_make)
 sleep 0.25
end


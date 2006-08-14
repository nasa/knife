#!/usr/bin/env ruby
# unit test runner

failed = Array.new
Dir['*_test.rb'].each do |test|
 failed << test unless system(File.expand_path(test))
end

if failed.empty?
 puts "All tests passed." 
else
 puts "Failed tests:" 
 puts failed
end

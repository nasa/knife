#!/usr/bin/env ruby
#
# Creates a ruby c extension from the args
#


unless extension_name = ARGV[0] 
 puts "ERROR: usage: ruby #{__FILE__} RubyExtensionName [extra_files.(c|h)]"
 exit 1
end

root_name = extension_name.downcase

source = [ root_name+".c", root_name+"_ruby.c" ]

ARGV[1..ARGV.size].each { |c| source.push c if c =~/\.c/ }

headers = source.collect do
 |c| h = c.sub(/\.c/, ".h") 
 h if File.exist? h
end
ARGV[1..ARGV.size].each { |h| headers.push h if h =~/\.h/ }

`chmod u+w . && mkdir -p #{extension_name}`
Dir.chdir extension_name
 unless FileTest.exists?('Makefile')
  require 'mkmf'
  $objs = source.collect{ |c| c.sub(/\.c/, ".o") }
  create_makefile(extension_name,'..')
 end
 exit 1 unless system "make --quiet --no-print-directory"
Dir.chdir '..'

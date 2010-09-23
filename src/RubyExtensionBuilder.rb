# Ruby C extension build for knife package
#


class RubyExtensionBuilder

 def RubyExtensionBuilder.actual_build_of extension
  system_command = ['ruby build_ruby_extension.rb',extension].join(' ')
  exit 1 unless system( system_command )
 end

 def RubyExtensionBuilder.build extension
  actual_build_of 'Adj' if 'Primal' == extension
  actual_build_of extension
 end

end

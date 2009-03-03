#!/usr/bin/env ruby

class FUN3D

 @@dsl_methods = %w[ 
     fun3d_directory
     root_project
     number_of_processors
     iteration
     iterations
     all_cl
     flo_cl
     adj_cl
     rad_cl
     flo_nl
     adj_nl
 ]

 @@dsl_methods.each do |dsl_method|
  eval "def #{dsl_method}(value=@#{dsl_method});@#{dsl_method}=value;end"
 end

 def initialize()
  fun3d_directory '~/FUN3D/faux'
  number_of_processors 4
  iteration 1
  init_cl_nl
  load_case_specifics
 end

 def init_cl_nl
  all_cl ' '
  flo_cl ' '
  adj_cl ' '
  rad_cl ' '
  flo_nl Hash.new
  adj_nl Hash.new  
 end

 def load_case_specifics(file_name = 'case_specifics')
  if File.exist?(file_name)
   init_cl_nl
   eval IO.readlines(file_name).join("\n") 
  end
 end

 def project(iter=@iteration)
  load_case_specifics
  (iter ? @root_project+sprintf('%02d',iter) : @root_project)
 end

 def input(hash = Hash.new, new_file = 'Flow/fun3d.nml')
  load_case_specifics
  hash['project_rootname'] = "'#{project}'"
  File.open(new_file,'w') do |f|
   IO.readlines('fun3d.nml').each do |line|
    hash.each_pair do |key,value|
     regex = /#{key}.*/
     replacement = "#{key} = #{value}"
     line.gsub!(regex,replacement)
    end
    f.puts line
   end
  end
  `cp namelist.input Flow` if File.exist?( 'namelist.input' )
  `cp perturb.input Flow` if File.exist?( 'perturb.input' )
  `cp user_vol_init.input Flow` if File.exist?( 'user_vol_init.input' )
 end

 def have_file?(suffix)
  if suffix =~ /adj/
   File.exist?( "Adjoint/#{project}_#{suffix}.1")
  else
   File.exist?( "Flow/#{project}_#{suffix}.1")
  end
 end

 def required_file(suffix)
  raise("#{project} #{suffix} files missing") unless have_file?(suffix)
 end

 def raise_if_nonzero_exit_code_in( directory )
  raise("nonzero exit code in #{directory}") if File.exist?( "#{directory}/.fun3d_nonzero_exit_code" )
 end

 def exec_prefix()
   if (number_of_processors > 1) then
     "mpiexec -np #{number_of_processors}"
   else
     " "
   end
 end

 def flo(  )
  required_file('part')
  load_case_specifics
  name_list = Hash.new.merge(flo_nl)
  name_list['restart_read'] = "'#{have_file?('flow') ? 'on':'off'}'"
  input(name_list)
  command = "( cd Flow && " +
            exec_prefix +
            " ./nodet_mpi " +
             all_cl+flo_cl +
            "< /dev/null | tee flow_out > #{project}_flow_out )"
  puts command
  system(command)
  raise_if_nonzero_exit_code_in('Flow')
  read_forces( )
 end

 def complex( )
  `mkdir Complex`
  `(cd Complex && ln -s #{fun3d_directory}/Complex/FUN3D_90/complex_nodet* complex_nodet_mpi)`
  Dir["Flow/fun3d.nml","Flow/namelist.input","Flow/#{project}_part.*"].each do |target|
   `( cd Complex && ln -s ../#{target} .)`
  end
  command = "( cd Complex && " +
            exec_prefix +
            " ./complex_nodet_mpi " +
             all_cl+flo_cl +
            "< /dev/null | tee flow_out > #{project}_flow_out )"
  puts command
  system(command)
  raise_if_nonzero_exit_code_in('Complex')
  
 end

 def read_forces( )
  IO.readlines("Flow/#{project}.forces").each do |line|
   line.scan(/C\S+\s*=\s*\S+/) do |force|
    eval("def #{force.gsub(/=/,';')};end".downcase)
   end
  end
 end

 def adj(  )
  required_file('part')
  required_file('flow')
  load_case_specifics
  name_list = Hash.new.merge(adj_nl)
  name_list['restart_read'] = "'#{ have_file?('adj') ? 'on':'off'}'"
  input(name_list)
  command = "( cd Adjoint && " +
            exec_prefix +
            " ./dual_mpi " +
            all_cl+adj_cl +
            "< /dev/null | tee dual_out > #{project}_dual_out " +
            ")"
  puts command
  system(command)
  raise_if_nonzero_exit_code_in('Adjoint')
  Dir['Adjoint/boom_surface.*'].each do |boom|
   `mv #{boom} #{boom.sub(/boom_surface/,project+"_boom_surface")}`
  end
  `cp Adjoint/pressure_signatures.tec Adjoint/#{project}_pressure_signatures.tec` if File.exist?( 'Adjoint/pressure_signatures.tec' )
 end

 def rad(  )
  required_file('part')
  required_file('flow')
  required_file('adj')
  input
  `cp faux_input Adjoint`
  command = "( cd Adjoint && " +
            exec_prefix +
            " ./dual_mpi " +
            "--adapt --embedrad --adapt_project #{project(@iteration+1)} " +
            all_cl+rad_cl  +
            "< /dev/null | tee rad_out > #{project}_rad_out " +
            ")"
  puts command
  system(command)
  raise_if_nonzero_exit_code_in('Adjoint')
 end

 def tec(type=20,format=5)
  load_case_specifics
  required_file('part')
  if (type == 20 )
   required_file('flow')
   command = "( cd Flow && " +
             "printf '#{type}\n#{project}\n#{number_of_processors}\n0\n0\n#{format}\n0\n' | " +
             "./party > party_vol_out " +
             ")"
  end
  if (type == 21 )
   required_file('adj')
   command = "( cd Flow && " +
             "printf '#{type}\n#{project}\n#{number_of_processors}\n#{format}\n' | " +
             "./party > party_vol_out " +
             ")"
  end
  puts command
  system command
 end

 def iterate
  iterations.each do |iter|
   iteration iter
   setup if ( 1 == iteration )
   flo
   tec(20,1);tec(20,2);tec(20,5)
   adj
   tec(21,2)
   rad
  end
 end

 def setup
  `rm -rf Flow Adjoint Complex`
  `mkdir Flow Adjoint`

  `(cd Flow && ln -s #{fun3d_directory}/Party/party .)`
  `(cd Flow && ln -s #{fun3d_directory}/FUN3D_90/nodet* nodet_mpi)`
  `(cd Adjoint && ln -s #{fun3d_directory}/Adjoint/dual* dual_mpi)`

  %w[ fgrid fastbc cogsg bc mapbc knife cutbc ].each do |ext| 
   file = "#{project}.#{ext}"
   if File.exist?(file)
    command = "cp #{file} Flow"
    puts command
    system command
   end
  end
  iwhere = ( File.exist?(project+".cogsg") ? 2 : 1 )
  command = "(cd Flow && "+
            "printf '#{iwhere}\n#{project}\n0\n#{number_of_processors}\n' | "+
            "./party > party_init_out)"
  puts command
  system command
 end

end

if ( __FILE__ == $0 )
 fun3d = FUN3D.new
 fun3d.iterate
end


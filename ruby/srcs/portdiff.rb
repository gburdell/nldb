#
#  The MIT License
# 
#  Copyright 2006-2010 Karl W. Pfalzer.
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.
#
require 'nldb'

#Return [[new ports in rev, ...] [rm'd ports in rev, ...]]
def portdiff(top, gold_gv, rev_gv, libs)
  libs = ('--lib '+libs.join(' --lib ')).split unless libs.empty?
  gold = Vmodule.new(top, [gold_gv]+libs).all_ports_by_name.keys
  rev  = Vmodule.new(top, [rev_gv]+libs).all_ports_by_name.keys
  diffs = []
  diffs << rev - gold
  diffs << gold - rev
  return diffs
end

if __FILE__ == $0
	#
	# B E G I N    M A I N
	#
	args = ARGV
	if 3 > ARGV.length
	  cmd = File.basename($0,".*")
	  indent = " "*(8+cmd.length)
	  STDERR << "\nUsage: #{cmd} topModule gold.v rev.v libs.v ...\n\n" \
	         << "Load gold.v and rev.v and any remaining libraries: libs.v.\n" \
	         << "Link each (gold and rev) at level topModule.\n" \
	         << "\n" \
	         << "Print ports which are not in the intersection set of ports\n" \
	         << "of gold and rev.\n\n"
		exit 1
	end
	
	top_name = args[0]
	gold_v = args[1]
	rev_v = args[2]
	libs = args - args[0..2]
	diffs = portdiff(top_name, gold_v, rev_v, libs)
  puts "Info: #{top_name}: "+diffs[0].length.to_s+" added, "+diffs[1].length.to_s+" removed in revised"
  type = ['added','removed']
  for i in 0..1
    diffs[i].each do |port|
      puts "Warn: #{top_name}: port \"#{port}\" #{type[i]} in revised"
    end
  end
end

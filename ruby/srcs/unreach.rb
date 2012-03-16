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

def unreachable(top, delete, lib_pins_arg, &is_vld_cell)
  unreach = []  #accumulate [name,cell] pair to delete after iterator
  lib_pins = lib_pins_arg
  top.foreach_cell do |cell, inst_name|
    if is_vld_cell.call(cell)
      pass_cnt = 0
      conns_by_pin = cell.get_conns
      unless lib_pins_arg
        lib_pins = []
        cell.get_ports(Port::OUT).each do |p|
          lib_pins << p.get_name
        end
      end
      lib_pins.each do |lib_pin|
        q_conn = conns_by_pin[lib_pin]
        if q_conn
          case q_conn.length
          when 1
            break if q_conn[0].wire.is_port?
            self_cnt = 0
            q_conn[0].on_conn.each do |on_net|
              break unless on_net.is_pin?
              break unless on_net.inst == cell
              self_cnt += 1
            end
            pass_cnt += 1 if self_cnt == q_conn[0].on_conn.length
          when 0
            pass_cnt += 1
          end
        else
          pass_cnt += 1
        end
      end
      if pass_cnt==lib_pins.length && cell.is_leaf? && (0 < lib_pins.length)
        cname = Name.get_name(inst_name)
        if delete
          unreach << [cname,cell]
        else
          Message.warn(cname+": unreachable")
        end
      end
    end
  end
  # We cannot delete cells from a hashmap during iteration,
  # so save em up and do afterwards
  rm_cnt = 0
  unreach.each do |e|
    cname = e[0]
    cell = e[1]
    ok = cell.delete?
    msg = ok ? " (deleted)" : ""
    Message.info(cname+": unreachable#{msg}")
    Message.error(cname+": could not delete") unless ok
    rm_cnt += 1 if ok
  end
  return rm_cnt
end

def delete_all_unreachable(top, delete, lib_pins, &is_vld_cell)
  cnts = []
  tl = 0
  while true
    i = cnts.length + 1
    Message.info("Unreachable pass #{i} ...")
    cnt = unreachable(top, delete, lib_pins) { |c| is_vld_cell.call(c)}
    break unless 0 < cnt
    cnts << cnt
    tl += cnt
  end
  cnts.each_index do |k|
    j = k + 1
    Message.info("After pass #{j}: #{cnts[k]} unreachable cell(s) deleted")
  end
  Message.info("#{tl} unreachable cell(s) deleted")
  return tl
end

if __FILE__ == $0
	#
	# B E G I N    M A I N
	#
	args = ARGV
	if 2 > ARGV.length
	  cmd = File.basename($0,".*")
	  indent = " "*(8+cmd.length)
	  STDERR << "\nUsage: #{cmd} topModule (--lib lib.v | design.v)+\n\n" \
	         << "    --lib lib.v : specifies \"lib.v\" as a library file.\n" \
	         << "                : A library file defines leaf level modules\n" \
	         << "                : which have no hierarchy or wires.  They are\n" \
	         << "                : simply empty modules with port definitions.\n" \
	         << "       design.v : specifies \"design.v\" as a Verilog structural netlist file.\n" \
	         << "                : At least one design file contains the definition\n" \
	         << "                : of the toplevel module \"topModule\".\n\n" \
	         << "Read one or more files (lib.v and/or design.v), link toplevel \"topModule\"\n" \
	         << "and proceed to delete any unreachable logic.\n" \
	         << "\n" \
	         << "Write modified (after removing unreachable logic)\n" \
	         << "netlist to <topModule>.save.gv\n" \
	         << "\n" \
	         << "Unreachable logic is defined as any leaf cell (or specific type or name)\n" \
	         << "which has no fanout from its outputs to any *other* cell/instance or port.\n" \
	         << "\n" \
	         << "The unreachable algorithm repeatedly removes any unreachable logic until there\n" \
	         << "is no more unreachable cell(s) remaining.  This *repeat* operation is required\n" \
	         << "since there can be \"deep\" logic which is ultimately unreachable.\n" \
	         << "\n" \
	         << "Details of deleted (unreachable) logic is given for each iteration.\n" \
	         << "\n"
		exit 1
	end
	
	top_name = args[0]
	top_module = Vmodule.new(top_name, (args - args[0..0]))
	
  delete_all_unreachable(top_module, true, nil) do |c|
	#Skip removal of any "d0nt_" cells
    c.get_name !~ /d0nt_/
  end
	
	top_module.save(top_module.to_s+".save.gv")
	
	exit 0
end

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

top_name = args[0]
top_module = Vmodule.new(top_name, (args - args[0..0]))
	
#Find important reg pins tied to 0
msg = ": pin tied 0"
top_module.foreach_on_const_net(0) do |conns, val, inst_name|
  conns.each do |conn|
    if conn.isPin
      pin = Pin.new(conn)
      Message.warn(pin.get_name(inst_name)+msg) if pin.get_name =~ /\/([CS]DN|CP)$/
    end
  end
end
	
#Print cell names
top_module.foreach_cell do |cell, inst_nm|
  Message.info("cell: "+Name.get_name(inst_nm))
end if false


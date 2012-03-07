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
require 'java'
require 'jinit'

module Nldb
  include_package "nldb"
end

def assert(cond)
  raise "assert failed" unless cond
rescue => detail
  STDERR << "#{detail.to_s}\n  " << detail.backtrace.join("\n  ") << "\n"
  exit 666
end

class Message
  private_class_method(:new)  #singleton object
  @@info_cnt = 0
  @@warn_cnt = 0
  @@err_cnt = 0
  def self.info(msg)
    STDOUT.puts "Info: #{msg}"
    @@info_cnt += 1
  end
  def self.warn(msg)
    STDOUT.puts "Warn: #{msg}"
    @@warn_cnt += 1
  end
  def self.error(msg, do_exit=false)
    STDERR.puts "Error: #{msg}"
    @@err_cnt += 1
    exit 1 if do_exit
  end
  def self.fatal(msg)
    error(msg, true)
  end
end

module Name
  def Name.get_name(inst, obj = '')
    nm = inst.join("/")
    nm += "/#{obj}" if obj
    if true
      #keep \\
      nm = nm.gsub(/(^\/|\/$)/, '')
    else
      nm = nm.gsub('\\','').gsub(/(^\/|\/$)/, '')
    end
  end
  def Name.get_bit_name(nm, bit = -1)
    nm += "[#{bit}]" if (0 <= bit)
  end
  protected
  def bit_name(base, ix = -1)
    return get_bit_name(base, ix)
  end
end

module BitConn
  def is_pin?
    false
  end
end

class Cell
  attr_reader :ref, :jcell, :ref_nm
  def initialize(jcell, enclosing_mod=nil)
    @jcell = jcell
    jref = @jcell.get_ref
    @ref = Vmodule.from_jmodule(jref) if jref
    @enclosing_mod = enclosing_mod
    @full_name = nil
  end
  
  #Add wire connection to pin.
  #  pin --- name of scalar pin on cell
  # wire --- Nldb::Wire (scalar)
  def add_connection(pin, wire)
    @jcell.add_wire_conn(pin, wire.name)
  end
  
  #remove connection to named port
  def rm_port_connection(port)
    @jcell.remove_port_conn(port)
  end

  #get Wire
  def get_net(name)
    jwire = @ref.get_net(name)
    nil || Wire.to_wire(jwire) if jwire
  end
  
  def ==(to)
    @jcell == to.jcell
  end
  def delete?
    @enclosing_mod.delete_cell_by_name?(get_name)
  end
  def get_name
    @jcell.getInstName
  end
  def get_ref_name
    (@ref.nil?) ? @jcell.get_ref_name : @ref.to_s
  end
  def set_full_name(nm)
    #save as array elements to reduce memory (and return as single string)
    #in get_full_name
    @full_name = nm unless @full_name
  end
  def get_full_name
    if @full_name
      Name.get_name(@full_name, nil) #@full_name.join('/').gsub(/\\/,'')
    else
      get_name
    end
  end
  def to_s
    get_name
  end
  def is_leaf?
    @jcell.isLeaf
  end
  def get_ports(dir=Port::OUT)
    @ref.get_ports(dir)
  end
  def get_conns
    conn_map = get_map_of_conns_by_pin_name
    conns_by_name = Hash.new
    conn_map.each do |pin_name, conns|
      conn_coll = []
      conns.each do |c| #"c" is a nldb.Wire.Lval
        conn_coll << Conn.new(c.getWire, c.getBit)
      end
      conns_by_name[pin_name] = conn_coll
    end if conn_map
    return conns_by_name
  end
  class Conn
    attr_reader :wire
    def initialize(jwire, bit)
      @wire = Wire.to_wire(jwire)
      @bit = bit
    end
    def ==(to)
      return (@wire == to.wire) && (@bit == to.bit)
    end
    def on_conn
      @wire.on_bit(@bit)
    end
  end
  private
  def get_map_of_conns_by_pin_name
    return nil unless @enclosing_mod
    nets = @enclosing_mod.get_nets_by_name
    return nil unless nets
    @jcell.getMapOfConnsByPinName(nets)
  end
end

class Pin
  include Name
  include BitConn
  def is_pin?
    true
  end
  attr_reader :inst, :jport, :bit
  def initialize(jpinconn)
    eles = jpinconn.getEles
    @inst = Cell.new(eles[0])
    @jport = eles[1]
    @bit = eles[2]
  end
  def cell_name
    @inst.to_s
  end
  def pin_name
    bit_name(@jport.getName, @bit)
  end
  def get_name(pfx = [])
    Name.get_name(pfx, @inst.to_s+"/"+pin_name)
  end
  def to_s
    get_name
  end
  def is_leaf?
    @inst.is_leaf?
  end
end

class Wire  #NOTE: cannot use "Net" as class name
  include Name
  attr_reader :is_scalar, :bounds, :name
  def initialize(jwire)
    @jwire = jwire
    @is_scalar = @jwire.isScalar
    @bounds = {:msb=>@jwire.msb, :lsb=>@jwire.lsb}
    @name = @jwire.getName
  end
  
  #Convert jwire to a Port or Wire
  def Wire.to_wire(jwire)
    jwire.isPort ? Port.new(jwire) : Wire.new(jwire)
  end
  
  def get_name(pfx = [])
    Name.get_name(pfx, @name)
  end
  def is_port?
    false
  end
  def is_scalar?
    return @is_scalar
  end
  def on_each_bit
    if @is_scalar
      yield(on_bit, -1)
    else
      incr = incr()
      @bounds[:msb].step(@bounds[:lsb], incr) do |ix|
        yield(on_bit(ix), ix)
      end
    end
  end
  def on_bit(ix = nil)
    on_net = @is_scalar ? @jwire.onNet : @jwire.onNet(ix)
    rval = []
    on_net.each { |v| rval << ((v.isPin) ? Pin.new(v) : Wire::Bit.new(v))}
    return rval
  end
  class Bit
    include BitConn
    attr_reader :jwire, :bit
    def initialize(jwireconn)
      eles = jwireconn.getEles
      @jwire = eles[0]
      @bit = eles[1]
    end
  end
  private
  def incr
    return (@bounds[:msb] > @bounds[:lsb]) ? -1 : 1
  end
end

class Port < Wire
  IN = 0; OUT = 1; INOUT = 2; #same ordinal values as nldb.Port
  attr_reader :dir
  def initialize(jwire)
    super(jwire)
    @dir = jwire.getDir.ordinal
  end
  def is_port?
    true
  end
  def to_s
    @name if is_scalar?
    #TODO: if bus
  end
end

class Vmodule
  def initialize(top, files, jmod=nil, link=2); #link=0 if no link
    if jmod
      @jmodule = jmod
    else
      files = files.to_java(:String)
      @jmodule = Nldb::Load.new(top, files, link).getMod()
      Message.fatal("#{top}: module did not link.  Cannot continue.") unless @jmodule
    end
  end
  def add_input(nm)
    @jmodule.add_port(nm, Java::Nldb::Port::EDir::eIn)
    Wire.to_wire(get_net(nm))
  end
  def add_net(nm)
    @jmodule.create_net(nm)
  end
  def delete_net(nm)
    @jmodule.remove_net(nm)
  end
  def self.from_jmodule(jmod)
    Vmodule.new(nil,nil,jmod)
  end
  def get_nets_by_name
    return @jmodule.getNetsByName
  end
  def get_net(name) 
    @jmodule.get_net(name)
  end
  def self.get_module(mod_name)
    jmod = Nldb::Module.getRef(mod_name)
    from_jmodule(jmod)
  end
  def save(fname)
    Nldb::Save.new(@jmodule, fname)
  end
  def delete_cell_by_name?(cell_name)
    @jmodule.deleteCell(cell_name)
  end
  def to_s
    @jmodule.getName if @jmodule
  end
  def get_jcells
  	@jmodule.getCellsByName
  end
  def foreach_cell(max_depth = 0, inst_name = [], set_full_name = true)
    jcells = get_jcells
    jcells.values.each do |jcell|
      cell = Cell.new(jcell, self)
      ref = cell.ref
      new_inst = inst_name + [cell.to_s]
      cell.set_full_name(new_inst) if set_full_name
      yield(cell, new_inst) #new_inst is full name of cell
      ref.foreach_cell(max_depth, new_inst) do |cell2, inst_name2|
        cell2.set_full_name(inst_name2) if set_full_name
        yield(cell2, inst_name2)
      end if ((max_depth > new_inst.length) || (0 >= max_depth)) if ref
    end if jcells
  end
  def get_cell(name)
    jcell = @jmodule.get_cell(name)
    return nil unless jcell
    return Cell.new(jcell,self)
  end
  
  #Return Cell for hierarchical cell reference.
  #pfx --- array of instance path elements.
  #name --- tail name of /pfx/name
  def get_hier_cell(pfx, name)
    return get_cell(name) if pfx.empty?
    cell = get_cell(pfx[0])
    ref = cell.ref
    npfx = pfx - pfx[0..0]
    ref.get_hier_cell(npfx, name)
  end
  
  def get_cell_refname(name)
    jcell = @jmodule.getCell(name)
    return nil unless jcell
    return jcell.getRefName
  end
  def get_ports(dir=Port::OUT)
    ports = []
    jports = @jmodule.getPorts
    jports.each do |jport|
      port = Port.new(jport)
      if port.dir == dir
        #TODO: only returns scalar ports
        return [] unless port.is_scalar?
        ports << port
      end
    end
    return ports
  end
  def all_ports_by_name
    ports_by_name = Hash.new
    jports = @jmodule.getPorts
    jports.each do |jport|
      port = Port.new(jport)
      ports_by_name[port.to_s] = port
    end
    return ports_by_name
  end
  def foreach_net(inst_name = [], do_one_level = false)
    all_nets = get_nets_by_name
    return unless all_nets
    all_nets = all_nets.values
    all_nets.each do |jwire|
      net = Wire.to_wire(jwire)
      yield(net, inst_name) if net
    end
    foreach_cell(1, inst_name) do |cell, new_inst|
      cell.ref.foreach_net(new_inst) do |net2, inst_name2|
        yield(net2, inst_name2)
      end if cell.ref if cell
    end unless do_one_level
  end
  def foreach_on_const_net(val, inst_name=[])
    sval = "*logic#{val}*"
    net = @jmodule.getNet(sval)
    on_net = net.onNet if net
    yield(on_net, val, inst_name) if on_net
    foreach_cell(1, inst_name) do |cell, new_inst|
      cell.ref.foreach_on_const_net(val, new_inst) do |on_net2, val, inst_name2|
        yield(on_net2, val, inst_name2)
      end
    end
  end

  #Iterate over each net and return flattened/leaf objects.
  #NOTE: currently this only flattens nets at this module level (into children).
  #
  def foreach_on_flat_net
    #Build up a map for the entire module, since need to flatten bitconns
    #which represent assign stmts, which could be cascaded, as in:
    #      assign w1 = w2; assign w2 = w3; assign w3 = w4; ...
    map_of_bitconn = Hash.new
    foreach_net([],true) do |net,name|
      net.on_each_bit do |eles,n|  #n is net[n] if net is a bus, else n<0
        bitconns = []
        eles.each do |ele|
          bitconns << ele unless ele.is_pin?
        end
        if bitconns.length > 0
          net_name = Name.get_bit_name(name, n)
          map_of_bitconn[net_name] = bitconns
        end
      end
    end
  end
end

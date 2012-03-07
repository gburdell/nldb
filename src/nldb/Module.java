/*
#  The MIT License
# 
#  Copyright 2006-2010 Karl W. Pfalzer.
#  Copyright 2012-     George P. Burdell.
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
*/
package nldb;
import  java.util.*;
import  java.io.*;
import static nldb.Util.invariant;
import static nldb.Util.error;
import static nldb.Util.info;

/**
 *
 * @author karl
 */
public class Module {
    public Module(String name, List<String> ports) {
        m_name = name;
        m_moduleByName.put(m_name, this);
        addPorts(ports);
    }
    public String getName() {
        return m_name;
    }
    /**
     * Reset so can read in another netlist hierarchy.
     */
    static void reset() {
        m_moduleByName = new HashMap<String,Module>();
        m_allInsts = new LinkedList<Cell>();
    }
    private void addPorts(List<String> ports) {
        invariant(null==m_portsByOrder);
        m_portsByOrder = ports;
    }
    public void addPort(String nm, Port.EDir dir) {
        Port p = new Port(dir, nm);
        m_portsByOrder.add(nm);
        invariant(null == m_netsByName.put(nm, p));
    }
    private void addNets(List<? extends Wire> nets) {
        initNets();
        String nm = null;
        for (Wire ele : nets) {
            nm = ele.getName();
            invariant(null==getNet(nm));
            m_netsByName.put(nm, ele);
        }
    }
    public boolean removeNet(String nm) {
        return (null != m_netsByName.remove(nm));
    }
    public Wire createNet(String name) {
        invariant(null == getNet(name));
        Wire w = new Wire(name);
        m_netsByName.put(name, w);
        return w;
    }
    public Port getPortAt(int ix) {
        String portNm = m_portsByOrder.get(ix);
        Wire asNet = m_netsByName.get(portNm);
        return (Port)asNet;
    }
    public void addAssign(Assign asgn) {
        if (null == m_assigns) {
            m_assigns = new LinkedList<Assign>();
        }
        m_assigns.add(asgn);
    }
    List<Assign> getAssigns() {
        return m_assigns;
    }
    public Cell createCell(String ref, String inst) {
        initCells();
        Cell cell = new Cell(inst, ref);
        m_cellsByName.put(inst, cell);
        m_allInsts.add(cell);
        return cell;
    }
    public Map<String,Wire> getNetsByName() {
        return m_netsByName;
    }
    public Map<String,Cell> getCellsByName() {
        return m_cellsByName;
    }
    public boolean isLeaf() {
        return (null == m_cellsByName) || (1 > m_cellsByName.size());
    }
    public Wire getNet(String name) {
        return (null == m_netsByName) ? null : m_netsByName.get(name);
    }
    public Port getPort(String name) {
        return (Port)getNet(name);
    }
    public Cell getCell(String name) {
        return (null == m_cellsByName) ? null : m_cellsByName.get(name);
    }
    public int getNumPorts() {
        return (null == m_portsByOrder) ? 0 : m_portsByOrder.size();
    }
    public int getNumNets() {
        return (null == m_netsByName) ? 0 : m_netsByName.size();
    }
    public void portDecl(Port.EDir dir, Range rng, List<String> ports) {
        initNets();
        Port port = null;
        for (String p : ports) {
            invariant(false == m_netsByName.containsKey(p));
            port = new Port(dir, p, rng);
            m_netsByName.put(p, port);
        }
    }
    public void netDecl(Range rng, List<String> nets) {
        initNets();
        Wire wire = null;
        for (String n : nets) {
            if (false == m_netsByName.containsKey(n)) { //assume port if exists
                wire = (rng == null) ? new Wire(n) : new Wire(n, rng);
                m_netsByName.put(n, wire);
            }
        }
    }
    /**
     * Get ports in declaration order.
     * @return ports in declaration order.
     */
    public Port[] getPorts() {
        Port[] rval = null;
        if (null != m_portsByOrder) {
            rval = new Port[m_portsByOrder.size()];
            int i = 0;
            for (String p : m_portsByOrder) {
                rval[i++] = getPort(p);
            }
        }
        return rval;
    }
    public void print(PrintStream os) {
        print(os, new HashMap<String,Boolean>());
    }
    public boolean isLibcell() {
        invariant(isLeaf() || !m_isLibcell);    //libcells must be leaf
        return m_isLibcell;
    }
    /**
     * Delete specified cell.
     * @param nm name of cell to delete.
     * @return true if cell is deleted, else false.
     */
    public boolean deleteCell(String nm) {
        Cell was = getCellsByName().remove(nm);
        boolean ok = (null != was);
        if (ok) {
            ok &= was.delete(getNetsByName());
        }
        return ok;
    }
    private void print(PrintStream os, Map<String,Boolean> modsDone) {
        if (true == modsDone.containsKey(getName())) {
            return;
        }
        os.println("module "+m_name);
        if (null != m_cellsByName) {
            for (Cell c : m_cellsByName.values()) {
                os.println("cell "+c.getInstName()+"("+c.getRef().getName()+")");
            }
        }
        if (null != m_netsByName) {
            for (Wire w : m_netsByName.values()) {
                w.print(os);
            }
        }
        os.println("endmodule\n");
        modsDone.put(getName(), true);
        if (null != m_cellsByName) {
            for (Cell c : m_cellsByName.values()) {
                Module ref = c.getRef();
                ref.print(os, modsDone);
            }
        }
    }
    static boolean setMarkAsLibcell(boolean val) {
        boolean was = m_markAsLibcell;
        m_markAsLibcell = val;
        return was;
    }
    static boolean getMarkAsLibcell() {
        return m_markAsLibcell;
    }
    public static Module link(String top, final int maxErr) {
		return link(top, maxErr, 2);
	}
    public static Module link(String top, final int maxErr, final int strength) {
        info("linking references ...");
        long time = System.currentTimeMillis();
        boolean ok = true;
        int errCnt = 0;
        Module ref;
        String refNm;
		if (0 < strength) {
	        for (Cell inst : m_allInsts) {
	            refNm = inst.getRefName();
	            ref = m_moduleByName.get(refNm);
	            if (null == ref) {
                    if (1 < strength) {
                        error(refNm+": undefined.");
                        if (maxErr <= errCnt++ ) {
                            break; //for
                        }
                    }
                    //For strength==1, no errors are shown
                    ok = false;
	            } else {
	                inst.link(ref);
	            }
	        }
		}
        //free up inst list
        m_allInsts = null;
        if (ok || (1 == strength)) {
            ref = m_moduleByName.get(top);
        } else {
            ref = null;
        }
        if (0 != strength) {
            //free up modules
            m_moduleByName = null;
        }
        time = System.currentTimeMillis() - time;
        time /= 1000;
        info("link (references) time: "+time+" (secs).");
        if (ok && (1 < strength)) {
            info("linking connections ...");
            time = System.currentTimeMillis();
            ok = ref.linkConns();
            time = System.currentTimeMillis() - time;
            time /= 1000;
            info("link (connections) time: "+time+" (secs).");
        }
        return ref;
    }
    private boolean linkConns() {
        boolean rval = true;
        if (null != m_assigns) {
            for (Assign asn : m_assigns) {
                asn.connect(m_netsByName);
            }
        }
        if (null != m_cellsByName) {
            for (Cell cell : m_cellsByName.values()) {
                if (false == cell.isLinked()) {
                    initNets();
                    rval &= cell.linkConns(m_netsByName);
                    rval &= cell.getRef().linkConns();
                }
            }
        }
        return rval;
    }
    private void initNets() {
        if (null == m_netsByName) {
            m_netsByName = new HashMap<String,Wire>();
        }
    }
    private void initCells() {
        if (null == m_cellsByName) {
            m_cellsByName = new HashMap<String,Cell>();
        }
    }
    public static Module getRef(String nm) {
        return (null == m_moduleByName) ? null : m_moduleByName.get(nm);
    }
    private final String        m_name;
    private final boolean       m_isLibcell = m_markAsLibcell;
    private List<String>        m_portsByOrder = null;
    private List<Assign>        m_assigns;
    private Map<String,Wire>    m_netsByName;
    private Map<String,Cell>    m_cellsByName;
    private static Map<String,Module>   m_moduleByName;
    private static List<Cell>   m_allInsts;

    static {
        reset();
    }

    /**
     * Use to mark subsequent module definitions inherit value which
     * indicates if module is a libcell.
     */
    private static boolean m_markAsLibcell = false;
}

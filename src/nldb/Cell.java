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
import static nldb.Util.invariant;
import static nldb.Util.error;
import static nldb.Util.vnm;
/**
 *
 * @author karl
 */
public class Cell {
    public Cell(String inst, String ref) {
        m_instName = inst;
        m_refNm = ref;
    }
    public void addConn(PortConnection pc) {
        if (null == m_conns) {
            m_conns = new LinkedList<PortConnection>();
        }
        m_conns.add(pc);
    }
    /**
     * Add scalar wire connection to scalar port.
     * @param port scalar port.
     * @param wire scalar wire.
     */
    public void addWireConn(String port, String wire) {
        Expression expr = new Expression(new Primary(wire, null));
        PortConnection pc = new PortConnection(port, expr);
        addConn(pc);
    }
    /**
     * Remove connection to port.
     * @param port port to find and remove connection.
     */
    public boolean removePortConn(String port) {
        int ix = 0;
        boolean ok = false;
        for (; ix < m_conns.size(); ix++) {
            if (ok = m_conns.get(ix).getPortId().equals(port)) {
                m_conns.remove(ix);
                break;
            }
        }
        return ok;
    }
    public String getRefName() {
        return m_refNm;
    }
    public Module getRef() {
        return isLinked() ? m_ref : Module.getRef(getRefName());
    }
    public String getInstName() {
        return m_instName;
    }
    public boolean isLeaf() {
        return m_ref.isLeaf();
    }
    void link(Module mod) {
        m_ref = mod;
        m_refNm = null;
    }
    /**
     * Transform PortConnection to net-based connections.
     * 
     * @param mapOfNets nets from (enclosing) module context.
     * @return true on successful link; else return false.
     */
    boolean linkConns(Map<String,Wire> mapOfNets) {
        boolean ok = true;
        if (null != m_conns) {
            Port instPin = null;
            List<Wire.Lval> netsConnected = null;
            Map<String, List<Wire.Lval>> mapOfConnsByPinName =
                    getMapOfConnsByPinName(mapOfNets);
            for (String portName : mapOfConnsByPinName.keySet()) {
                instPin = m_ref.getPort(portName);
                if (null == instPin) {
                    ok = false;
                    error("LINK-3", m_ref.getName(), portName);
                } else {
                    netsConnected = mapOfConnsByPinName.get(portName);
                    ok &= addPinConn(netsConnected, instPin);
                }
            }
            if (Config.m_freeConnsAfterLink) {
                m_conns = null;
            }
        }
        m_isLinked = (null != m_ref) && ok;
        return m_isLinked;
    }
    /**
     * Get map of connections by pin name.
     * 
     * @param mapOfNets nets from (enclosing) module context.
     * @return map of connections by pin name.
     */
    public Map<String, List<Wire.Lval>> getMapOfConnsByPinName(Map<String,Wire> mapOfNets) {
       Map<String, List<Wire.Lval>> rval = new HashMap<String, List<Wire.Lval>>();
       int portPos = 0;    //port pos for unnamed
       String portName = null;
       Port instPin = null;
       Expression portExpr = null;
       List<Wire.Lval> netsConnected = null;
       for (PortConnection pc : m_conns) {
           if (pc.isNamed()) {
               portName = pc.getPortId();
               instPin = m_ref.getPort(portName);
           } else {
               instPin = m_ref.getPortAt(portPos);
               portName = instPin.getName();
           }
           portExpr = pc.getConns();
           if (null != portExpr) {
               netsConnected = portExpr.expand(mapOfNets);
               rval.put(portName, netsConnected);
           }
           portPos++;
       }
       return rval;
    }
    boolean isLinked() {
        return m_isLinked;
    }
    private boolean addPinConn(List<Wire.Lval> netsConnected, Port instPin) {
        boolean rval = true;
        final int n = instPin.getLength();
        invariant(n == netsConnected.size());
        if (instPin.isScalar()) {
            invariant(1 == n);
            netsConnected.get(0).connect(this, instPin);
        } else {
            int ix = instPin.msb();
            final int incr = instPin.getIncr();
            for (Wire.Lval lval : netsConnected) {
                lval.connect(this, instPin, ix);
                ix += incr;
            }
        }
        return rval;
    }
    /**
     * Delete pin connections from net.
     * TODO: currently assumes cell pins scalar.
     */
    boolean delete(Map<String,Wire> mapOfNets) {
        invariant(isLinked());
        //TODO: need netsByName of enclosing module NOT reference!
        Map<String, List<Wire.Lval>> conns = getMapOfConnsByPinName(mapOfNets);
        final int n = conns.size();
        int cnt = 0;
        Port instPin = null;
        for (String pin : conns.keySet()) {
            boolean ok = true;
            for (Wire.Lval wire : conns.get(pin)) {
                instPin = m_ref.getPort(pin);
                ok &= wire.delete(this, instPin);
            }
            cnt += ((ok) ? 1 : 0);
        }
        return (cnt >= n);
    }
    /**
     * Write out literal instance for unlinked module.
     * @param fww writer.
     * @return writer.
     */
    public FixedWidthWriter print(FixedWidthWriter fww) {
        assert(!isLinked());
        fww.append(vnm(m_refNm)).append(' ').append(vnm(m_instName)).append(" (");
        fww.incrIndent(stIncrIndent);
        boolean comma = false;
        for (PortConnection pc : m_conns) {
            if (comma) {
                fww.append(',');
            }
            pc.print(fww);
            comma = true;
        }
        fww.append(");");
        fww.incrIndent(-stIncrIndent);
        return fww;
    }
    
    private static final int stIncrIndent = 2;
    private final String            m_instName;
    private String                  m_refNm;
    private Module                  m_ref;
    private List<PortConnection>    m_conns;
    private boolean                 m_isLinked = false;
}

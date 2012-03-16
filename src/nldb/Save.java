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
import  java.io.*;
import  java.util.*;
import static nldb.Util.info;
import static nldb.Util.error;
import static nldb.Util.vnm;
/**
 *
 * @author karl
 */
public class Save {
    public Save(Module top, String fname) {
        m_top = top;
        info("FILE-1", fname);
        init(fname);
        save();
    }
    private void init(String fname) {
        try {
            m_os = new PrintWriter(fname);
            m_buf = new StringBuffer();
            m_modsDone = new HashMap<String,Module>();
        }
        catch (Exception ex) {
            error(ex);
        }
    }
    private void save() {
        save(m_top);
        m_os.close();
    }
    private void save(final Module top) {
        boolean nope = Config.m_doNotSaveEmptyModuleDefns && top.isLeaf();
        nope |= m_modsDone.containsKey(top.getName());
        nope |= top.isLibcell();
        if (nope) {
            return;
        }
        m_curr = top;
        m_modsDone.put(top.getName(), top);
        append("module ");
        int indent = 2;//m_buf.length();
        append(vnm(top.getName())).append("(");
        savePorts(indent);
        append(");").eoln();
        savePorts();
        saveWires();
        saveAssigns();
        saveCells();
        eoln(0).append("endmodule",false,0).eoln(0).eoln(0);
        if (false == top.isLeaf()) {
            //and references
            Module ref = null;
            for (Cell inst : top.getCellsByName().values()) {
                if (inst.isLinked()) {
                    ref = inst.getRef();
                } else {
                    ref = Module.getRef(inst.getRefName());
                }
                if (null != ref) {
                    save(ref);
                }
            }
        }
    }
    private void savePorts(int indent) {
        final Port ports[] = m_curr.getPorts();
        boolean doComma = false;
        for (final Port port : ports) {
            append(vnm(port.getName()), doComma, indent);
            doComma = true;
        }
    }
    /**
     * Write port definitions.
     * @param ports ports to define, in order.
     */
    private void savePorts() {
        final Port ports[] = m_curr.getPorts();
        /*
        append("//").eoln()
            .append("//Port definitions (in declaration order)").eoln()
            .append("//").eoln();
         */
        for (final Port port : ports) {
            port.getDefn(m_buf).append(';');
            eoln();
        }
    }
    /**
     * Write wire definitions. (written in alphabetical order.)
     * @param top module whose wire decls to write.
     */
    private void saveWires() {
        Map<String,Wire> wiresByName = m_curr.getNetsByName();
        if (null == wiresByName) {
            return;
        }
        /*
        append("//").eoln()
            .append("//Wire definitions (sorted by name)").eoln()
            .append("//").eoln();
         */
        List<String> names = new ArrayList<String>(wiresByName.keySet());
        Collections.sort(names);
        Wire wire = null;
        for (String nm : names) {
            wire = wiresByName.get(nm);
            if (false == (wire.isPort() || wire.isConst())) {
                wire.getDefn(m_buf).append(';');
                eoln();
            }
        }
    }
    /**
     * Write cell instances. (written in alphabetical order by instance name.)
     */
    private void saveCells() {
        Map<String,Cell> instsByName = m_curr.getCellsByName();
        if (null == instsByName) {
            return;
        }
        /*
        append("//").eoln()
            .append("//Instances (sorted by instance name)").eoln()
            .append("//").eoln();
         */
        List<String> names = new ArrayList<String>(instsByName.keySet());
        Collections.sort(names);
        Cell inst = null;
        for (String nm : names) {
            inst = instsByName.get(nm);
            if (inst.isLinked()) {
                String refNm = inst.getRefName();
                append(vnm(refNm)).append(" ").append(vnm(nm))
                    .append(" (");
                save(inst.getMapOfConnsByPinName(m_curr.getNetsByName()));
                append(");").eoln();
            } else {
                FixedWidthWriter fww = new FixedWidthWriter(m_os);
                inst.print(fww);
                fww.eoln();
            }
        }
    }
    /**
     * Write cell instance connections. (written in alphabetical order by pin name.)
     * @param conns map of connections by pin name.
     */
    private void save(Map<String, List<Wire.Lval>> conns) {
        List<String> pins = new ArrayList<String>(conns.keySet());
        Collections.sort(pins);
        List<Wire.Lval> pinConns = null;
        boolean doComma = false;
        for (String pinName : pins) {
            pinConns = conns.get(pinName);
            append("."+vnm(pinName)+"(",doComma,m_indent2);
            save(pinConns);
            append(")");
            doComma = true;
        }
    }
    private void saveAssigns() {
        final List<Assign> asgns = m_curr.getAssigns();
        if (null == asgns) {
            return;
        }
        append("//").eoln()
            .append("//Continuous assignments").eoln()
            .append("//").eoln();
        List<Wire.Lval> lhs = null, rhs = null;
        for (Assign asgn : asgns) {
            lhs = asgn.getLhs().expand(m_curr.getNetsByName());
            rhs = asgn.getRhs().expand(m_curr.getNetsByName());
            append("assign ");
            save(lhs, false);
            append(" = ");
            save(rhs);
            append(" ;").eoln();
        }
    }
    private void save(final List<Wire.Lval> conns, boolean useCurly) {
        Ele compress = new Ele(conns.size());
        if (1 == conns.size()) {
            compress.next(conns.get(0));
            append(compress.toString(),false,m_indent2);
        } else if (1 < conns.size()) {
            if (useCurly) {
                append("{",false,m_indent2);
            }
            boolean doComma = false;
            String s;
            for (Wire.Lval wire : conns) {
                s = compress.next(wire);
                if (null != s) {
                    append(s, doComma, m_indent2);
                    doComma = true;
                }
            }
            append(compress.toString(), doComma, m_indent2);
            if (useCurly) {
                append("}");
            }
        }
    }
    private void save(final List<Wire.Lval> conns) {
        save(conns, true);
    }
    private Save append(String s, boolean doComma, int indent) {
        int n = s.length() + ((doComma) ? 2 : 0);
        if (m_maxPerLn < (m_buf.length() + n)) {
            if (doComma) {
                m_os.println(m_buf.append(','));
            } else {
                m_os.println(m_buf);
            }
            indent(indent).append(s);
        } else {
            if (doComma) {
                m_buf.append(", ").append(s);
            } else {
                m_buf.append(s);
            }
        }
        return this;
    }
    private Save append(String s, boolean doComma) {
        return append(s, doComma, m_indent);
    }
    private Save append(String s) {
        return append(s, false, 0);
    }
    private Save eoln(int indent) {
        m_os.println(m_buf);
        m_os.flush();
        indent(indent);
        return this;
    }
    private Save eoln() {
        return eoln(m_indent);
    }
    private StringBuffer indent(int n) {
        m_buf = new StringBuffer();
        for (int i = 0; i < n; i++) {
            m_buf.append(' ');
        }
        return m_buf;
    }
    /**
     * A helper class for compressing connections.
     */
    private static class Ele {
        Ele(int n) {
            m_bits = new boolean[n];
            init();
        }
        void init() {
            for (int i = 0; i < m_bits.length; i++) {
                m_bits[i] = false;
            }
            m_isBit = false;
            m_incr = 0;
            m_msb = m_lsb = -1;
            m_nm = null;
            m_bitCnt = 0;
        }
        @Override
        public String toString() {
            StringBuffer buf = new StringBuffer();
            if (m_isBit) {
                buf.append(m_bitCnt).append("'b");
                for (int i = 0; i < m_bitCnt; i++) {
                    buf.append((m_bits[i]) ? '1' : '0');
                }
            } else {
                buf.append(vnm(m_nm));
                if (0 <= m_msb) {
                    buf.append('[').append(m_msb);
                    if (0 <= m_lsb) {
                        buf.append(':').append(m_lsb);
                    }
                    buf.append(']');
                }
            }
            return buf.toString();
        }
        String next(Wire.Lval ele) {
            String rval = null;
            Wire w = ele.getWire();
            int bit = ele.getBit();
            //Check if we're different
            boolean different = false;
            if (0 < m_bitCnt) {
                if (m_isBit && !w.isConst()) {
                    different = true;   //was bits, now name
                } else if (!m_isBit && !m_nm.equals(w.getName())) {
                    different = true;   //was name, now different
                } else if (!m_isBit) {
                    if (0 > bit) {
                        different = true;   //was name[] now name
                    } else if (1 < m_bitCnt) {
                        if (bit != (m_lsb + m_incr)) {
                            different = true;   //not consecutive bit
                        }
                    } else if (1 == m_bitCnt) {
                        if (bit != (m_msb + m_incr)) {
                            different = true;   //not consecutive
                        }
                    }
                }
                if (different) {
                    rval = toString();
                    init();
                }
            }
            if (w.isConst()) {
                m_isBit = true;
                m_bits[m_bitCnt++] = ele.asConst();
            } else {
                m_isBit = false;
                if (0 == m_bitCnt) {
                    m_nm = w.getName();
                    m_msb = bit;
                    if (w.isBus()) {
                        m_incr = w.getIncr();
                    }
                } else {
                    m_lsb = bit;
                }
                m_bitCnt++;
            }
            return rval;
        }
        boolean m_isBit;    //else name
        int     m_incr;     //case of bus: 0 is not known, 1 is up, -1 is down
        int     m_msb, m_lsb;
        String  m_nm;
        boolean  m_bits[];
        int m_bitCnt;
    }
    private final Module    m_top;
    private Module          m_curr;  //current module
    private PrintWriter     m_os;
    private StringBuffer    m_buf;
    final static int m_maxPerLn = 80-1; //acct for nl
    final static int m_indent = 2;
    private final static int m_indent2 = 2 * m_indent;
    /**
     * Keep a map of modules done.
     */
    private Map<String,Module>  m_modsDone;
}

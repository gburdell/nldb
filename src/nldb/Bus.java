/*#
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
#*/
package nldb;
import  java.util.*;
import  java.io.PrintStream;
import static nldb.Util.invariant;

/**
 *
 * @author karl
 */
public class Bus {
    protected Bus(String name) {
        this(name, -1, -1);
    }
    protected Bus(String name, int lb, int rb) {
        m_name = name;
        m_lb = lb;
        m_rb = rb;
        m_conns = new ConnList[size()];
    }
    public String getName() {
        return m_name;
    }
    public boolean isScalar() {
        return (0 > m_lb);
    }
    public boolean isBus() {
        return !isScalar();
    }
    public int getLength() {
        return isScalar() ? 1 : (1 + Math.abs(m_lb - m_rb));
    }
    /**
     * Get increment for iterating rb() to lb() (MSB to LSB).
     * @return increment for iterating MSB towards LSB.
     */
    public int getIncr() {
        return (lsb() > msb()) ? 1 : -1;
    }
    public int size() {
        return getLength();
    }
    public int msb() {
        return m_lb;
    }
    public int lsb() {
        return m_rb;
    }
    public int[] getBounds() {
        return new int[]{msb(), lsb()};
    }
    /**
     * Add pin[n] inst.pin to [ix] bit of this bus.
     * @param ix bit index of this bus to add inst.pin connection.
     * @param inst instance to connect to.
     * @param pin (scalar) port name of instance to connect to.
     * @param n valid pin[n] or -1 for scalar pin.
     */
    public void addConn(int ix, Cell inst, Port pin, int n) {
        ConnList cons = getConns(ix);
        PinConn pc = new PinConn(inst, pin, n);
        cons.add(pc);
    }
    /**
     * Add scalar inst.pin to [ix] bit of this bus.
     * @param ix bit index of this bus to add inst.pin connection.
     * @param inst instance to connect to.
     * @param pin (scalar) port name of instance to connect to.
     */
    public void addConn(int ix, Cell inst, Port pin) {
        addConn(ix, inst, pin, -1);
    }
    /**
     * Add conn[n] wire to [ix] bit of this bus.
     * @param ix bit index of this bus.
     * @param conn connection to add.
     * @param n conn[n] bit.
     */
    public void addConn(int ix, Wire conn, int n) {
        ConnList cons = getConns(ix);
        WireConn wc = new WireConn(conn, n);
        cons.add(wc);
    }
    /**
     * Add scalar conn wire to [ix] bit of this bus.
     * @param ix bit index of this bus.
     * @param conn connection to add.
     */
    public void addConn(int ix, Wire conn) {
        addConn(ix, conn, -1);
    }
    boolean deleteConn(int ix, Cell inst, Port pin) {
        ConnList cons = getConns(ix);
        PinConn torm = new PinConn(inst, pin);
        /**int i = 0;
        for (Conn c : cons) {
            if (c.isPin()) {
                if (c.equals(torm)) {

                }
            }
        }**/
        return cons.remove(torm);
    }
    public void print(PrintStream os) {
        if (isScalar()) {
            os.print(getName()+": ");
            print(os, m_conns[0]).println(" ;");
        } else {
            int ix = msb();
            final int incr = getIncr();
            for (ConnList conn : m_conns) {
                os.print(getName()+"["+ix+"]: ");
                print(os, conn).println(" ;");
                ix += incr;
            }
        }
    }
    private PrintStream print(PrintStream os, ConnList conns) {
        if (null != conns) {
            boolean comma = false;
            for (Conn conn : conns) {
                if (comma) {
                    os.print(", ");
                } else {
                    comma = true;
                }
                os.print(conn.toString());
            }
        }
        return os;
    }
    public Conn[] onNet() {
        invariant(isScalar());
        return onNet(msb());
    }
    public Conn[] onNet(int ix) {
        Conn[] rval = null;
        ConnList list = getConns(ix);
        if (null != list) {
            rval = list.toArray(new Conn[0]);
        }
        return rval;
    }
    private ConnList getConns(int ix) {
        int i = offset(ix);
        ConnList cons = m_conns[i];
        if (null == cons) {
            cons = m_conns[i] = new ConnList();
        }
        return cons;
    }
    protected int offset(int ix) {
        return Math.abs(ix - msb());
    }
    protected final String  m_name;
    protected final int     m_lb, m_rb;

    public static abstract class Conn {
        public abstract boolean isPin();
        @Override
        public abstract String toString();
        /**
         * Object name elements.
         * @return object name elements.
         */
        public abstract String[] getNameEles();
        /**
         * Get objects of connnection point.
         * @return objects comprising connection point.
         *           See implementation for details.
         */
        public abstract Object[] getEles();
        public boolean equals(Object o) {
            Conn ac = (Conn)o;
            if (isPin()) {
                return (ac.isPin()) ? equals(ac) : false;
            } else {
                return (ac.isPin()) ? false : equals(ac);
            }
        }
        protected abstract boolean equals(Conn ac);
        protected String bitName(String nm, int bit) {
            StringBuffer rval = new StringBuffer(nm);
            if (0 <= bit) {
                rval.append('[').append(bit).append(']');
            }
            return rval.toString();
        }
    }
    private static class WireConn extends Conn {
        @Override
        public boolean isPin() {
            return false;
        }
        @Override
        public String[] getNameEles() {
            return new String[]{null, bitName(m_conn.getName(), m_bit)};
        }
        @Override
        public String toString() {
            return getNameEles()[1];
        }
        @Override
        public Object[] getEles() {
            return new Object[]{m_conn, m_bit};
        }
        protected boolean equals(Conn ac) {
            WireConn ref = (WireConn)ac;
            return (m_conn == ref.m_conn) && (m_bit == ref.m_bit);
        }
        /**
         * Connection to a wire[bit], as through an assign statement.
         * @param conn wire connected to.
         * @param connBit (absolute) bit index of conn.
         */
        private WireConn(Wire conn, int connBit) {
            m_conn = conn;
            m_bit = connBit;
        }
        private final Wire  m_conn;
        private final int   m_bit;
    }
    private class PinConn extends Conn {
        @Override
        public boolean isPin() {
            return true;
        }
        @Override
        public String[] getNameEles() {
            return new String[]{m_inst.getInstName(), bitName(m_pin.getName(), m_bit)};
        }
        @Override
        public String toString() {
            String[] eles = getNameEles();
            return (new StringBuffer(eles[0]).append('.').append(eles[1])).toString();
        }
        @Override
        public Object[] getEles() {
            return new Object[]{m_inst, m_pin, m_bit};
        }
        private PinConn(Cell inst, Port port, int bit) {
            m_inst = inst;
            m_pin = port;
            invariant (((0 > bit) && port.isScalar()) || port.isBus());
            m_bit = bit;
        }
        private PinConn(Cell inst, Port port) {
            this(inst, port, -1);
        }
        protected boolean equals(Conn ac) {
            PinConn ref = (PinConn)ac;
            return (m_inst == ref.m_inst)
                    && (m_pin == ref.m_pin)
                    && (m_bit == ref.m_bit);
        }
        private final Cell    m_inst;
        private final Port    m_pin;
        /**
         * Bit index of m_pin.
         */
        private final int     m_bit;
    }
    public static class ConnList extends LinkedList<Conn> {

    }
    protected ConnList  m_conns[];
}

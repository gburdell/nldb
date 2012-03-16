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
import static nldb.Util.vnm;

/**
 *
 * @author karl
 */
public class Wire extends Bus {
    public static enum EConst {eLogic0, eLogic1};

    public Wire(String name) {
        super(name);
    }
    public Wire(String name, int lb, int rb) {
        super(name, lb, rb);
    }
    public Wire(String name, Range rng) {
        super(name, rng.msb(), rng.lsb());
    }
    protected Wire(Wire r) {
        super(r.m_name, r.m_lb, r.m_rb);
    }
    private static Wire createConst(EConst val) {
        if (EConst.eLogic0 == val) {
            return new Wire(m_consts[0]);
        } else {
            return new Wire(m_consts[1]);
        }
    }
    /**
     * Find constant value in wire map or add if it does not exist.
     * @param val name of constant value.
     * @param mapOfWires wire set to query or add to.
     * @return constant value wire.
     */
    public static Wire getConst(String val, Map<String,Wire> mapOfWires) {
        Wire rval = mapOfWires.get(val);
        if (null == rval) {
            EConst c = (val.equals(LOGIC0)) ? EConst.eLogic0 : EConst.eLogic1;
            rval = createConst(c);
            mapOfWires.put(val, rval);
        }
        return rval;
    }
    public boolean isConst() {
        return m_name.equals(m_consts[0]) || m_name.equals(m_consts[1]);
    }
    public boolean isConst(EConst val) {
        return m_name.equals(m_consts[val.ordinal()]);
    }
    public boolean isPort() {
        return false;
    }
    protected StringBuffer getDefn(StringBuffer buf, boolean asWire) {
        if (null == buf) {
            buf = new StringBuffer();
        }
        if (asWire) {
            buf.append("wire ");
        }
        if (isBus()) {
            buf.append("[").append(msb()).append(':').append(lsb()).append(']');
        }
        buf.append(' ').append(vnm(getName()));
        return buf;
    }
    StringBuffer getDefn(StringBuffer buf) {
        return getDefn(buf, true);
    }

    /**
     * Connect to this[bit].
     * @param bit add connection to (absolute) [bit].
     * @param conn connection to add.
     */
    void connect(int bit, Wire.Lval conn) {
        super.addConn(bit, conn.m_ref, conn.m_bit);
    }
    void connect(Wire.Lval conn) {
        super.addConn(-1, conn.m_ref, conn.m_bit);
    }
    /**
     * Expand range into bit-blasted references: MSB is head, LSB is tail.
     * @param rng selected range.
     * @return bit-blasted references: head is MSB, tail is LSB.
     */
    List<Wire.Lval> expand(Range rng) {
       List<Wire.Lval> rval = new LinkedList<Wire.Lval>();
       if (isScalar()) {
           rval.add(new Lval(this));
       } else {
           final int lsb = (null != rng) ? rng.lsb() : lsb();
           final int incr = getIncr();
           int ix = (null != rng) ? rng.msb() : msb();
           while (true) {
               rval.add(new Lval(this, ix));
               if (ix == lsb) {
                   break;
               }
               ix += incr;
           }
       }
       return rval;
    }
    /**
     * Used to pass reference to a wire during xformation of instance-based
     * connections to net based.
     */
    public static class Lval {
        Lval(Wire w, int bit) {
            m_ref = w;
            m_bit = bit;
        }
        Lval(Wire w) {
            m_ref = w;
            m_bit = w.msb();
        }
        void connect(Cell inst, Port pin, int pinBit) {
            m_ref.addConn(m_bit, inst, pin, pinBit);
        }
        void connect(Cell inst, Port pin) {
            invariant(pin.isScalar());
            m_ref.addConn(m_bit, inst, pin);
        }
        void connect(Lval conn) {
            m_ref.addConn(m_bit, conn.m_ref, conn.m_bit);
        }
        boolean delete(Cell inst, Port pin) {
            if (pin.isScalar()) {
            	return m_ref.deleteConn(m_bit, inst, pin);
			}
			return false;
        }
        public Wire getWire() {
            return m_ref;
        }
        public int getBit() {
            return m_bit;
        }
        public boolean asConst() {
            invariant(getWire().isConst());
            return getWire().getName().equals(LOGIC1);
        }
        final Wire  m_ref;
        /**
         * Absolute (as opposed to offset) bit of reference.
         */
        final int   m_bit;
    }

    public static final String LOGIC0 = "*logic0*";
    public static final String LOGIC1 = "*logic1*";

    private static final String[] m_consts = new String[2];
    static {
        m_consts[EConst.eLogic0.ordinal()] = LOGIC0;
        m_consts[EConst.eLogic1.ordinal()] = LOGIC1;
    }
}

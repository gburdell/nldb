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

/**
 *
 * @author karl
 */
public class Primary {
    public Primary(Vnumber ele) {
        m_item = new ANum(ele);
    }
    public Primary(String id, Range ele) {
        m_item = new APartSel(id,ele);
    }
    public Primary(MultipleConcat ele) {
        m_item = new AMultConcat(ele);
    }
    public Primary(Concat ele) {
        m_item = new AConcat(ele);
    }
    public Primary(Expression ele) {
        m_item = new AExpr(ele);
    }
    public FixedWidthWriter print(FixedWidthWriter sb) {
        return m_item.print(sb);
    }
    public int asInt() {
        return ((ANum)m_item).asInt();
    }
    /**
     * Expand primary value into scalar Wire reference(s).
     * @param netsByName set of nets to query.
     * @return bit-blasted: MSB is head, LSB is tail.
     */
    List<Wire.Lval> expand(Map<String,Wire> netsByName) {
        return m_item.expand(netsByName);
    }
    protected Primary(Primary ref) {
        m_item = ref.m_item;
    }
    private enum EType {
        eNum,
        ePartSel,
        eMultConcat,
        eConcat,
        eExpr
    }

    private A   m_item;

    private static abstract class A {
        protected A(EType e) {
            m_type = e;
        }
        abstract List<Wire.Lval> expand(Map<String,Wire> netsByName);
        abstract FixedWidthWriter print(FixedWidthWriter sb);
        final EType m_type;
    }


    private static class ANum extends A {
        private ANum(Vnumber ele) {
            super(EType.eNum);
            m_ele = ele;
        }
        
        @Override
        public String toString() {
            return m_ele.toString();
        }
        List<Wire.Lval> expand(Map<String,Wire> netsByName) {
            List<Wire.Lval> rval = new LinkedList<Wire.Lval>();
            String bitVal = m_ele.getVal().toString(2);
            Wire b0 = null, b1 = null, w = null;
            for (int i = 0; i < bitVal.length(); i++) {  //MSB to LSB
                if ('0' == bitVal.charAt(i)) {
                    if (null == b0) {
                        b0 = Wire.getConst(Wire.LOGIC0, netsByName);
                    }
                    w = b0;
                } else if (null == b1) {
                    w = b1 = Wire.getConst(Wire.LOGIC1, netsByName);
                } else {
                    invariant('1' == bitVal.charAt(i));
                    w = b1;
                }
                rval.add(new Wire.Lval(w));
            }
            //Extend to correct number of bits?
            int diff = m_ele.getWidth() - bitVal.length();
            invariant(0 <= diff);
            if (0 < diff) {
                if (null == b0) {
                    b0 = Wire.getConst(Wire.LOGIC0, netsByName);
                }
                for (int i = 0; i < diff; i++) {
                    rval.add(0, new Wire.Lval(b0));
                }
            }
            return rval;
        }
        int asInt() {
            return m_ele.getVal().intValue();
        }
        private Vnumber m_ele;

        @Override
        FixedWidthWriter print(FixedWidthWriter sb) {
            return sb.append(m_ele.toString());
        }
    }
    /**
     * Add scalar wire.
     */
    private static Wire addWire(Map<String,Wire> netsByName, String wire) {
        Wire w = new Wire(wire);
        Wire was = netsByName.put(wire, w);
        invariant(null == was);   //should not have existed.
        return w;
    }
    private static class APartSel extends A {
        private APartSel(String id, Range rng) {
            super(EType.ePartSel);
            m_id  = id;
            m_rng = rng;
        }
        List<Wire.Lval> expand(Map<String,Wire> netsByName) {
            Wire w = netsByName.get(m_id);
            if (null == w) {
                w = addWire(netsByName, m_id);
            }
            return w.expand(m_rng);
        }
        private String m_id;
        private Range m_rng;

        @Override
        FixedWidthWriter print(FixedWidthWriter sb) {
            sb.append(m_id);
            if (null != m_rng) {
                sb.append(m_rng.toString());
            }
            return sb;
        }
        
    }

    private static class AMultConcat extends A {
        private AMultConcat(MultipleConcat ele) {
            super(EType.eMultConcat);
            m_ele = ele;
        }
        List<Wire.Lval> expand(Map<String,Wire> netsByName) {
            return m_ele.expand(netsByName);
        }
        private MultipleConcat m_ele;

        @Override
        FixedWidthWriter print(FixedWidthWriter sb) {
            return m_ele.print(sb);
        }
        
    }

    private static class AConcat extends A {
        private AConcat(Concat ele) {
            super(EType.eConcat);
            m_ele = ele;
        }
        List<Wire.Lval> expand(Map<String,Wire> netsByName) {
            return m_ele.expand(netsByName);
        }
        private Concat m_ele;

        @Override
        FixedWidthWriter print(FixedWidthWriter sb) {
            return m_ele.print(sb);
        }
        
    }

    private static class AExpr extends A {
        private AExpr(Expression ele) {
            super(EType.eExpr);
            m_ele = ele;
        }
        List<Wire.Lval> expand(Map<String,Wire> netsByName) {
            return m_ele.expand(netsByName);
        }
        private Expression m_ele;

        @Override
        FixedWidthWriter print(FixedWidthWriter sb) {
            return m_ele.print(sb);
        }
    }
}

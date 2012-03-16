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

/**
 *
 * @author karl
 */
public class MultipleConcat {

    public MultipleConcat(Expression expr, Concat concat) {
        m_expr = expr;
        m_concat = concat;
    }
    /**
     * Expand to wire references.
     * @param netsByName wire definitions.
     * @return bit-blasted: MSB head to LSB tail references.
     */
    List<Wire.Lval> expand(Map<String,Wire> netsByName) {
        List<Wire.Lval> rval = new LinkedList<Wire.Lval>();
        final int repl = m_expr.asInt();
        List<Wire.Lval> concat = m_concat.expand(netsByName);
        for (int i = 0; i < repl; i++) {
            rval.addAll(concat);
        }
        return rval;
    }
    
    public FixedWidthWriter print(FixedWidthWriter fb) {
        fb.append('{');
        m_expr.print(fb);
        m_concat.print(fb);
        return fb.append('}');
    }

    private Expression  m_expr;
    private Concat      m_concat;
}

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
import  nldb.parser.VnlParserTokenTypes.*;
import static nldb.Util.invariant;
/**
 *
 * @author karl
 */
public class Port extends Wire {
    public static enum EDir {eIn, eOut, eInout};

    public Port(EDir dir, String name) {
        super(name);
        m_direction = dir;
    }
    public Port(EDir dir, String name, Range rng) {
        super((null != rng) ? new Wire(name, rng.msb(), rng.lsb())
                            : new Wire(name));
        m_direction = dir;
    }
    public EDir getDir() {
        return m_direction;
    }
    @Override
    StringBuffer getDefn(StringBuffer buf) {
        buf.append(m_dirs[m_direction.ordinal()]).append(' ');
        super.getDefn(buf, false);
        return buf;
    }

    @Override
    public boolean isPort() {
        return true;
    }
    private final EDir  m_direction;

    static private final String m_dirs[] = new String[] {
      "input", "output", "inout"
    };
}

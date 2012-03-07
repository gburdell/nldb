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
import  java.io.PrintWriter;

/**
 * StringBuilder type interface, but controlled line width and indent.
 * @author kpfalzer
 */
public class FixedWidthWriter {
    public FixedWidthWriter(PrintWriter os) {
        this(os, stIndent, stMaxPerLn);
    }
    public FixedWidthWriter(PrintWriter os, int indent, int maxPerLn) {
        m_os = os;
        m_buf = new StringBuilder();
        m_indent = indent;
        m_maxPerLn = maxPerLn;
        indent(m_indent);
    }
    public FixedWidthWriter append(final CharSequence s) {
        int n = s.length();
        if (m_maxPerLn < (m_buf.length() + n)) {
            eoln();
        } 
        m_buf.append(s);
        return this;
    }
    public FixedWidthWriter append(char c) {
        return append(String.valueOf(c));
    }
    public FixedWidthWriter eoln() {
        return eoln(m_indent);
    }
    public int incrIndent(int incr) {
        m_indent += incr;
        return m_indent;
    }
    private FixedWidthWriter eoln(int indent) {
        m_os.println(m_buf);
        m_os.flush();
        indent(indent);
        return this;
    }
    private FixedWidthWriter indent(int n) {
        m_buf = new StringBuilder();
        for (int i = 0; i < n; i++) {
            m_buf.append(' ');
        }
        return this;
    }
    private StringBuilder   m_buf;
    private int       m_indent;
    private final int       m_maxPerLn;
    private PrintWriter     m_os;
    
    private final static int stMaxPerLn = Save.m_maxPerLn;
    private final static int stIndent = Save.m_indent;
}

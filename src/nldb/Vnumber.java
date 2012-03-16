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
import java.math.BigInteger;
import  java.util.regex.*;
import static nldb.Util.invariant;

/**
 *
 * @author kpfalzer
 */
public class Vnumber {
    public Vnumber(String val) {
        m_origVal = val;
        init(val);
    }
    private void init(String val) {
        //remove _
        String nval = "";
        char c;
        for (int i = 0; i < val.length(); i++) {
            c = val.charAt(i);
            if (c != '_') {
                nval += c;
            }
        }
        Matcher m = m_regex.matcher(nval);
        boolean ok = m.matches();
        invariant(ok);
        int gcnt = 1 + m.groupCount();
        invariant(5==gcnt);
        String fld[] = new String[gcnt];
        for (int i = 0; i < gcnt; i++) {
            fld[i] = m.group(i);
        }
        if ((null != fld[2]) && (0 < fld[2].length())) {
            m_width = Integer.parseInt(fld[2]);
        }
        int radix = 10;
        if ((null != fld[3]) && (0 < fld[3].length())) {
            switch (fld[3].charAt(0)) {
                case 'd': case 'D': radix = 10; break;
                case 'o': case 'O': radix = 8;  break;
                case 'h': case 'H': radix = 16; break;
                case 'b': case 'B': radix = 2;  break;
                default:
                    invariant(false);
            }
        }
        try {
            m_val = new BigInteger(fld[4], radix);
        }
        catch (Exception ex) {
            ex.printStackTrace();
            System.exit(1);
        }
    }
    private boolean isSized() {
        return (0 < m_width);
    }
    public BigInteger getVal() {
        return m_val;
    }
    public int getWidth() {
        return (isSized()) ? m_width : m_val.bitLength();
    }
    @Override
    public String toString() {
        return m_origVal;
    }
    private int m_width = 0;
    private BigInteger m_val = null;
    private final String m_origVal;

    private static final Pattern m_regex =
        Pattern.compile("(([0-9]*)'([bBoOdDhH]))?([0-9a-fA-F_]+)");

    public static void main(String argv[]) {
        for (String i : new String[]{"10","4'b0","'hfeed","16'hbeef","23","49'b0001"}) {
            Vnumber n = new Vnumber(i);
        }
    }
}

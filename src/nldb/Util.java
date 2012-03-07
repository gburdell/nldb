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

/**
 *
 * @author kpfalzer
 */
public final class Util {
    public static void error(Exception ex) {
        ex.printStackTrace();
        System.exit(1);
    }
    public static void invariant(boolean cond) {
        if (false == cond) {
            error(new Exception("invariant failed"));
        }
    }
    public static void error(String msg) {
        System.err.println("Error: "+msg);
        m_errCnt++;
    }
    public static int getErrCnt() {
        return m_errCnt;
    }
    public static void info(String msg) {
        System.out.println("Info: "+msg);
        m_infoCnt++;
    }
    public static int getInfoCnt() {
        return m_infoCnt;
    }
    /**
     * Generate Verilog style name
     * @param nm name to Verilog-ize.
     * @return Verilog style name.
     */
    static String vnm(String nm) {
        return (nm.startsWith("\\")) ? (nm+" ") : nm;
    }
    private static int m_infoCnt = 0;
    private static int m_errCnt = 0;
}

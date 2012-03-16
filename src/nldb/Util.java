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
import java.io.PrintStream;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import static nldb.MessageMgr.message;
import static nldb.MessageMgr.getErrorCnt;
/**
 *
 * @author kpfalzer
 */
public final class Util {
    /**
     * Generate Verilog style name
     * @param nm name to Verilog-ize.
     * @return Verilog style name.
     */
    public static String vnm(String nm) {
        return (nm.startsWith("\\")) ? (nm+" ") : nm;
    }

    public static void info(String code, Object... args) {
        message('I', code, args);
    }
    
    public static void warn(String code, Object... args) {
        message('W', code, args);
    }
    
    public static void error(String code, Object... args) {
        message('E', code, args);
    }
    
    public static void error(Exception ex) {
        fatal(ex);
    }
    
    public static int getErrorMsgCnt() {
        return getErrorCnt();
    }
    
    /** Creates a new instance of Utils */
    public static String stripDoubleQuotes (final String s) {
        int len = s.length();
        String ns = s.substring(1, len-1);
        return ns;
    }

    @SuppressWarnings("CallToPrintStackTrace")
    public static void invariant(boolean c) {
        if (false == c) {
            Thread.dumpStack();
            System.exit(1);
        }
    }

    private final static String stDOT = ".";

    public static String getToolRoot() {
        String root = System.getProperty("tool.root");
        if (null == root) {
            root = stDOT;
        }
        return root;
    }

    public static void fatal(Exception ex) {
        PrintStream err = System.err;
        err.print(ex.getMessage());
        ex.printStackTrace(err);
        System.exit(1);
    }

    /**Lookup property value.
     *
     * @param prop  property name
     * @return true if property exists and set to "true" or else false.
     */
    public static boolean getPropertyAsBool(String prop) {
        String pv = System.getProperty(prop);
        boolean v = (null == pv) ? false : Boolean.parseBoolean(pv);
        return v;
    }
    public static int getPropertyAsInt(String nm) {
        int rval = Integer.MIN_VALUE;
        String str = System.getProperty(nm);
        if (null != str) {
            rval = Integer.parseInt(str);
        }
        return rval;
    }
    public static void abnormalExit(Exception ex) {
        System.err.println(ex.getMessage());
        ex.printStackTrace(System.err);
        System.exit(1);
    }

    public static List<String> arrayToList(String s[]) {
        return Arrays.asList(s);
    }

    public static String[] toStringArray(Object args[]) {
        final int n = args.length;
        String rval[] = new String[n];
        for (int i = 0; i < n; i++) {
            rval[i] = downCast(args[i]);
        }
        return rval;
    }
    
    public static <T> List<T> addTo(List<T> list, T ele) {
        if (null == list) {
            list = new LinkedList<T>();
        }
        list.add(ele);
        return list;
    }

    public static String nl() {
        return NL;
    }

    /** Return a null x as an empty collection. */
    public static <T> T asEmpty(T x, T empty) {
        return (null != x) ? x : empty;
    }

    @SuppressWarnings("unchecked")
	public static <T> T downCast(Object o) {
		return (T)o;
	}

    public static String toCamelCase(String s) {
        StringBuilder rval = new StringBuilder();
        char c = 0;
        for (int i = 0; i < s.length(); i++) {
            c = s.charAt(i);
            if (0 == i) {
                rval.append(Character.toUpperCase(c));
            } else if (c != '_') {
                if (s.charAt(i-1) == '_') {
                    c = Character.toUpperCase(c);
                }
                rval.append(c);
            }
        }
        return rval.toString();
    }

    public static void exit(int status) {
        System.exit(status);
    }

    public final static String NL = System.getProperty("line.separator");
}

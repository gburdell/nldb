/*#
 #  The MIT License
 # 
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
package nldb.jni;

/**
 * Encapsulate methods to interact with Tcl side.
 * 
 * @author gburdell
 */
public class TclBridge {

    /**
     * The execute command returns this object to the Tcl side (as jobject).
     * The Tcl side, in turn, can query the members using query methods.
     */
    public static class Rval {
        public Rval(Object obj) {
            m_type = cObject;
            m_obj = obj;
        }
        
        public Rval(int i) {
            m_type = cInt;
            m_obj = i;
        }
        
        public Rval(boolean b) {
            m_type = cBool;
            m_obj = b;
        }
        
        public Rval(double d) {
            m_type = cDouble;
            m_obj = d;
        }
        
        public Rval(String s) {
            m_type = cString;
            m_obj = s;
        }
        
        //Use consts here, since easier to get from jni.
        public final static int cBool       = 1;   //jbool
        public final static int cDouble     = 2;   //jdouble
        public final static int cInt        = 3;   //jint
        public final static int cObject     = 4;   //jobject
        public final static int cString     = 5;   //jstring
        
        public int getType() {
            return m_type;
        }
        
        public boolean asBool() {
            return ((Boolean)m_obj).booleanValue();
        }
        
        public int asInt() {
            return ((Integer)m_obj).intValue();
        }
        
        public String asString() {
            return (String)m_obj;
        }
        
        public double asDouble() {
            return ((Double)m_obj).doubleValue();
        }
        
        /**
         * One of cXXX values: cObject, ..., cString //cCollection.
         */
        public final int    m_type;
        public final Object m_obj;
    }
    
    public static Object execute(final Object argv[]) {
        final String cmd = (String)argv[0];
        //TODO: emit as string for replay/debug (see notes.txt)
        System.out.print("TclBridge.execute: ");
        for (int i = 0; i < argv.length; i++) {
            System.out.print(argv[i].toString() + " ");
        }
        System.out.println();
        Rval r = Command.runCommand(cmd, argv);
        return r;
    }
}

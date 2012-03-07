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
 *
 * @author gburdell
 */
import java.util.Iterator;
import nldb.Load;

public class Module {

    /**
     * Load netlist.
     *
     * @param top toplevel module
     * @param fname filename
     * @return hid (nldb.Module).
     */
    public static int load(String top, String fname) {
        Load loader = new Load(top, new String[]{fname}, 0);
        nldb.Module mod = loader.getMod();
        int hid = addToMap(mod);
        return hid;
    }

    /**
     * Get names of cells.
     *
     * @param module (hid).
     * @return hid (Iter<String>).
     */
    public static int getCells(int module) {
        nldb.Module mod = (nldb.Module) getObjById(module);
        Iterator<String> names = mod.getCellsByName().keySet().iterator();
        int hid = addToMap(names);
        return hid;
    }

    /**
     * Check if iterator has more items.
     *
     * @param iterId iterator.
     * @return true if has more items.
     */
    public static boolean hasNext(int iterId) {
        Iterator iter = (Iterator) getObjById(iterId);
        return iter.hasNext();
    }

    public static String nextAsString(int iterId) {
        Iterator<String> iter = (Iterator<String>) getObjById(iterId);
        String s = iter.next();
        return s;
    }

    private static int addToMap(Object obj) {
        return ObjMap.getTheOne().add(obj);
    }

    private static Object getObjById(int id) {
        return ObjMap.getTheOne().getById(id);
    }
}

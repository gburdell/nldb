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
 * Map of java Object to native side (int)
 * @author gburdell
 */
import java.util.HashMap;

public class ObjMap {
    static ObjMap getTheOne() {
        return stTheOne;
    }
    
    private ObjMap() {}
    
    public int add(Object r) {
        m_objById.put(m_nextId, r);
        return m_nextId++;
    }
    
    public Object getById(int id) {
        return m_objById.get(id);
    }
    
    public void remove(int id) {
        m_objById.remove(id);
    }
    
    private HashMap<Integer,Object> m_objById   = new HashMap<Integer,Object>();
    private int m_nextId = 1;
    private static ObjMap stTheOne = new ObjMap();
}

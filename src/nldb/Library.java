/*
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
*/
package nldb;
import java.util.HashMap;

/**
 * Library is module container.  Contains singleton named "work"
 * which contains modules defined in netlists (ala Load).
 * @author gburdell
 */
public class Library {
    public static Library getWorkLib() {
        return m_workLib;
    }
    public Library(String name) {
        m_name =  name;
    }
    public Library() {
        this("work");
    }
    /**
     * Add module to library.
     * @param mod module to add.
     * @return true on success; else false on duplicate).
     */
    public boolean add(Module mod) {
        String nm = mod.getName();
        boolean ok = !m_modulesByName.containsKey(nm);
        if (ok) {
            m_modulesByName.put(nm, mod);
        }
        return ok;
    }
    public boolean hasModule(String nm) {
        return m_modulesByName.containsKey(nm);
    }
    public Module getModule(String nm) {
        assert(hasModule(nm));
        return m_modulesByName.get(nm);
    }
    private HashMap<String,Module>  m_modulesByName = new HashMap<String, Module>();
    private final String    m_name;
    private static Library m_workLib = new Library();
}

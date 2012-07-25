/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
 * Copyright (c) 2012-      George P. Burdell
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#if !defined(_vnl_module_hxx_)
#define  _vnl_module_hxx_

#include <map>
#include <list>
#include "vnl/vnl.hxx"
#include "vnl/portbus.hxx"
#include "vnl/cell.hxx"

namespace vnl {
    using std::map;
    using std::list;

    class Module : virtual public Object {
    public:
        typedef map<string, TRcWire> t_wiresByName;
        typedef list<string> t_portDeclOrder;
        typedef list<TRcPort> t_ports; //in decl order
        typedef map<string, TRcCell> t_cellsByName;
        typedef map<string, unsigned> t_unresolvedCntByName;
        typedef PTRcPtr<t_unresolvedCntByName> trc_unresolvedCntByName;
        typedef list<TRcObject> t_libObjs;  //TRcObject base of TRcLibrary

        explicit Module(string nm);

        string getName(bool unused = false) const {
            return m_name;
        }

        /**
         * Link module by resolving all module references.
         * @param libs reference to list of libs to link against (in order).
         * @param strength link strength: 1 indicates to only resolve references.
         *                 >1 to check pin reference/widths too.
         * @return map of unresolved module counts.
         */
        trc_unresolvedCntByName link(const t_libObjs &libs, unsigned strength = 1) ;

        /**
         * Link module by resolving all module references.
         * @param lib reference to lib to link against.
         * @param strength link strength: 1 indicates to only resolve references.
         *                 >1 to check pin reference/widths too.
         * @return map of unresolved module counts.
         */
        trc_unresolvedCntByName link(const TRcObject &lib, unsigned strength = 1) ;

        /**
         * Port is declared, but not defined.
         * @param nm name of port.
         */
        void appendPortDecl(string nm);

        /**
         * Add port as bus (in declaration order).
         * @param bus port as bus.
         */
        void appendPortDecl(TRcPortBus &bus);

        /**
         * Add port (in declaration order).
         * @param bus port as scalar.
         */
        void appendPortDecl(TRcPort &port);

        void addWire(TRcWire &wire);

        /**
         * Check if port is declared.
         * @param nm name of port.
         * @param pport pass back result if true (and pport != 0).
         * @return true if port is declared.
         */
        bool hasPort(string nm, TRcPort *pport = 0);
        bool hasPort(string nm, const TRcPort *pport = 0) const;

        /**
         * Check if wire is declared (not as a port).
         * @param nm name of wire.
         * @param pwire pass back result if true (and pwire != 0).
         * @return true if pwire is declared.
         */
        bool hasWire(string nm, const TRcWire *pwire = 0) const;
        bool hasWire(string nm, TRcWire *pwire = 0);

        /**
         * Check if instance exists.
         * @param nm instance name.
         * @return true if instance exists.
         */
        bool hasCell(string nm) const;

        void addCell(TRcCell &cell);

        /**
         * Get wire by name.
         * @param nm name of wire (or port).
         * @return wire or isNull() if not found.
         */
        TRcWire getWire(string nm);
        const TRcWire getWire(string nm) const;

        const TRcWire getConstWire(bool v) const {
            return getWire(Wire::stConstNm[v ? 1 : 0]);
        }

        TRcWire getConstWire(bool v) {
            return getWire(Wire::stConstNm[v ? 1 : 0]);
        }

        bool isHierarchical() const {
            return !m_cellsByName.empty();
        }

        /**
         * Get port by name.
         * @param nm name of port.
         * @return port by name (or .isNull() if not found).
         */
        const TRcPort getPort(string nm) const;
        TRcPort getPort(string nm);

        t_cellsByName& getCellsByName() {
            return m_cellsByName;
        }

        const t_cellsByName& getCellsByName() const {
            return m_cellsByName;
        }

        t_wiresByName& getWiresByName() {
            return m_wiresByName;
        }

        const t_wiresByName& getWiresByName() const {
            return m_wiresByName;
        }

        /**
         * Get ports in declaration order.
         * @param ports empty list to add ports.
         */
        void getPortsInDeclOrder(t_ports &ports) const;

        static const TRcModule downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Module > (r);
        }

        static TRcModule downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Module > (r);
        }

        virtual bool isLibCell() const {
            return false;
        }

        virtual ~Module();

    private:
        t_wiresByName m_wiresByName;
        t_portDeclOrder m_portDeclOrder;
        t_cellsByName m_cellsByName;

        const string m_name;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Module);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        /**
         * Replace port with new port (as wire).
         * @param from port to replace.
         * @param to replace with this port (as wire).
         */
        void replace(TRcPort &from, TRcWire &to);
        friend class Parser; //allow Parser to replace

        static const unsigned stTypeId;

    };

    inline const TRcObject upcast(const TRcModule &p) {
        return xyzzy::upcast<Object, Module > (p);
    }

    inline TRcObject upcast(TRcModule &p) {
        return xyzzy::upcast<Object, Module > (p);
    }
}

#endif

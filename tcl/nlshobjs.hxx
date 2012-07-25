//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
//Copyright (c) 2012-      George P. Burdell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
#ifndef VNLTCL_NLSHOBJS_HXX
#define	VNLTCL_NLSHOBJS_HXX

#include <list>
#include "xyzzy/refcnt.hxx"
#include "xyzzy/array.hxx"
#include "xyzzy/slist.hxx"
#include "tcl/attribute.hxx"
#include "vnl/module.hxx"
#include "vnl/port.hxx"
#include "vnl/cell.hxx"
#include "vnl/pinref.hxx"

namespace vnltcl {
    using std::list;
    using xyzzy::PTRcObjPtr;
    using xyzzy::PTRcPtr;
    using xyzzy::PTArray;
    using xyzzy::PTSlist;
    using vnl::Object;
    using vnl::TRcObject;
    using vnl::Module;
    using vnl::Port;
    using vnl::TRcWire;
    using vnl::Cell;
    using vnl::TRcCell;
    using vnl::PinRef;
    using vnl::TRcPinRef;

    DECL_CLASS(NlshObject);
    DECL_CLASS(NlshDesign);
    DECL_CLASS(NlshPort);
    DECL_CLASS(NlshCell);
    DECL_CLASS(NlshPin);
    DECL_CLASS(NlshWire); //a different object than Port (it has scope)

    typedef PTSlist<TRcNlshObject> t_nlshObjList;

    /**
     * Interface to be implemented by object types manipulated by tcl side.
     */
    class IAttribute {
    public:
        virtual TRcAttrVal getAttrVal(const string &name) const throw (AttrException) = 0;

    protected:

        explicit IAttribute() {
        }

        virtual ~IAttribute() {
        }

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(IAttribute);
    };

    /**
     * NlshObject is base of proxy objects manipulated by tcl side.
     */
    class NlshObject : public Object, public IAttribute {
    protected:

        explicit NlshObject() {
        }

        virtual ~NlshObject() {
        }

    public:

        bool isA(const TRcNlshObject &o) const {
            return Object::isA(upcast(o));
        }

        static const TRcNlshObject downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, NlshObject > (r);
        }

        static TRcNlshObject downcast(TRcObject &r) {
            return xyzzy::downcast<Object, NlshObject > (r);
        }

        static const TRcObject upcast(const TRcNlshObject &p) {
            return xyzzy::upcast<Object, NlshObject > (p);
        }

        static TRcObject upcast(TRcNlshObject &p) {
            return xyzzy::upcast<Object, NlshObject > (p);
        }
        
        virtual string getName(bool full = false) const;

        virtual bool operator==(const TRcNlshObject &c) const;
        
        virtual bool operator!=(const TRcNlshObject &c) const {
            return !operator==(c);
        }
    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshObject);
    };

    /**
     * Useful template class for creating tcl side objects
     * which are simply proxy to vnl/object types.
     * T is a reference counted type PTRcObjPtr<xxx>.
     */
    template<typename T>
    class PTNlshObject : public NlshObject {
    public:
        typedef PTRcObjPtr<T> t_rcObj;

        unsigned getTypeId() const {
            return stTypeId;
        }

        typedef PTRcObjPtr<PTNlshObject<T> > t_rc;
        
        static const t_rc downcast(const TRcNlshObject &r) {
            return xyzzy::downcast<NlshObject, PTNlshObject<T> > (r);
        }

    protected:

        explicit PTNlshObject(t_rcObj &rcobj) : m_obj(rcobj) {
        }

        virtual ~PTNlshObject() {
        }

        t_rcObj m_obj;

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(PTNlshObject);

        static const unsigned stTypeId;
    };

    class NlshDesign : public PTNlshObject<Module> {
    public:

        explicit NlshDesign(t_rcObj &mod)
        : PTNlshObject<Module>(mod) {
        }

        //Specialization needs to implement.
        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        t_rcObj& asModule() {
            return m_obj;
        }

        const t_rcObj& asModule() const {
            return m_obj;
        }

        /**
         * Get list of cells.
         * @param coll add to this collection.
         * @param tailRex regular expression to qualify tail of cell name.
         *                Matches are added.
         * @param hier true if breadth-first hierarchical; else just children.
         * @return list of cells.
         */
        void getCells(PTSlist<TRcNlshObject> &coll, const string &tailRex, bool hier = false);

        virtual ~NlshDesign() {
        }

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshDesign);
    };

    class NlshPort : public PTNlshObject<Port> {
    public:

        explicit NlshPort(t_rcObj &obj)
        : PTNlshObject<Port>(obj) {
        }

        //Specialization needs to implement.
        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        t_rcObj& asPort() {
            return m_obj;
        }

        const t_rcObj& asPort() const {
            return m_obj;
        }

        virtual ~NlshPort() {
        }

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshPort);
    };

    /*
     * Cell, PinRef and Wire are instanced based; so we need scoping mechanism.
     */
    class NlshCell : public NlshObject {
    public:
        typedef list<TRcCell> t_cells;

         explicit NlshCell(const t_cells &hier)
        : m_hier(hier) {
        }

        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        bool isHierarchical() const {
            return (0 < m_hier.length());
        }

        ~NlshCell() {
        }

    private:
        typedef PTArray<TRcCell> t_hier;

        t_hier m_hier;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshCell);

        ///Attribute handler is our friend.
        friend class CellHandler;

        static const unsigned stTypeId;
    };

    class NlshPin : public NlshObject {
    public:

        explicit NlshPin(TRcNlshCell &hier, TRcPinRef &pin)
        : m_hier(hier), m_pin(pin) {
        }

        explicit NlshPin(NlshCell::t_cells &hier, TRcPinRef &pin)
        : m_hier(new NlshCell(hier)), m_pin(pin) {
        }

        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        bool isHierarchical() const {
            return (m_hier.isValid() && m_hier->isHierarchical());
        }

        ~NlshPin() {
        }

    private:
        TRcNlshCell m_hier;
        TRcPinRef m_pin;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshPin);

        static const unsigned stTypeId;
    };

    class NlshWire : public NlshObject {
    public:

        explicit NlshWire(TRcWire &wire)
        : m_wire(wire) {
        }

        explicit NlshWire(TRcNlshCell &hier, TRcWire &wire)
        : m_hier(hier), m_wire(wire) {
        }

        explicit NlshWire(NlshCell::t_cells &hier, TRcWire &wire)
        : m_hier(new NlshCell(hier)), m_wire(wire) {
        }

        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        bool isHierarchical() const {
            return (m_hier.isValid() && m_hier->isHierarchical());
        }

        ~NlshWire() {
        }

    private:
        TRcNlshCell m_hier;
        TRcWire m_wire;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshWire);

        static const unsigned stTypeId;
    };

    inline const TRcNlshObject upcast(const TRcNlshPort &p) {
        return xyzzy::upcast<NlshObject, NlshPort > (p);
    }

    inline TRcNlshObject upcast(TRcNlshPort &p) {
        return xyzzy::upcast<NlshObject, NlshPort > (p);
    }

    inline const TRcNlshObject upcast(const TRcNlshCell &p) {
        return xyzzy::upcast<NlshObject, NlshCell > (p);
    }

    inline TRcNlshObject upcast(TRcNlshCell &p) {
        return xyzzy::upcast<NlshObject, NlshCell > (p);
    }

    inline bool operator==(const TRcNlshObject &a, const TRcNlshObject &b) {
        return a->operator ==(b);
    }
    
    inline bool operator!=(const TRcNlshObject &a, const TRcNlshObject &b) {
        return a->operator !=(b);
    }
}

#endif	/* VNLTCL_NLSHOBJS_HXX */


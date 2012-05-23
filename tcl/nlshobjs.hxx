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

#include "xyzzy/refcnt.hxx"
#include "tcl/attribute.hxx"
#include "vnl/module.hxx"
#include "vnl/port.hxx"
#include "vnl/cell.hxx"
#include "vnl/pinref.hxx"

namespace vnltcl {
    using xyzzy::PTRcObjPtr;
    using vnl::Object;
    using vnl::TRcObject;
    using vnl::Module;
    using vnl::Port;
    using vnl::Cell;
    using vnl::PinRef;

    DECL_CLASS(NlshObject);
    DECL_CLASS(NlshDesign);
    DECL_CLASS(NlshPort);
    DECL_CLASS(NlshCell);
    DECL_CLASS(NlshPin);

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

    protected:

        explicit NlshObject() {
        }

        virtual ~NlshObject() {
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

        static const unsigned stTypeId;

    protected:

        explicit PTNlshObject(t_rcObj &rcobj) : m_obj(rcobj) {
        }

        virtual ~PTNlshObject() {
        }

        t_rcObj m_obj;

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(PTNlshObject);
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

    class NlshCell : public PTNlshObject<Cell> {
    public:

        explicit NlshCell(t_rcObj &obj)
        : PTNlshObject<Cell>(obj) {
        }

        //Specialization needs to implement.
        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        t_rcObj& asCell() {
            return m_obj;
        }

        const t_rcObj& asCell() const {
            return m_obj;
        }

        virtual ~NlshCell() {
        }

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshCell);
    };

    class NlshPin : public PTNlshObject<PinRef> {
    public:

        explicit NlshPin(t_rcObj &obj)
        : PTNlshObject<PinRef>(obj) {
        }

        //Specialization needs to implement.
        TRcAttrVal getAttrVal(const string &name) const throw (AttrException);

        t_rcObj& asPinRef() {
            return m_obj;
        }

        const t_rcObj& asPinRef() const {
            return m_obj;
        }

        virtual ~NlshPin() {
        }

    private:
        //Cannot copy
        DECL_COPY_CONSTRUCTORS(NlshPin);
    };
}

#endif	/* VNLTCL_NLSHOBJS_HXX */


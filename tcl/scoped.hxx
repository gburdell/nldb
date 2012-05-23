/* The MIT License
 * 
 * vnl - verilog netlist
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
#if !defined(_vnltcl_scoped_hxx_)
#define  _vnltcl_scoped_hxx_

#include "xyzzy/refcnt.hxx"
#include "vnl/vnl.hxx"
#include "vnl/cell.hxx"
#include "vnl/wire.hxx"
#include "vnl/pinref.hxx"
#include "tcl/vnltcl.hxx"
#include "tcl/nlshobjs.hxx"

/**
 * Scoped element objects: Cell, PinRef and Wire.
 */
namespace vnltcl {
    using vnl::Cell;
    using vnl::TRcCell;
    using vnl::TRcWire;
    using vnl::TRcPinRef;

    /**
     * ScopeLink maintains a back-pointer to this elements parent.
     */
    class ScopeLink;

    typedef PTRcObjPtr<ScopeLink> TRcScopeLink;

    class ScopeLink : virtual public NlshObject {
    public:
        explicit ScopeLink();

        explicit ScopeLink(TRcScopeLink &parent);

        bool hasParent() const {
            return m_parent.isValid();
        }

        TRcScopeLink& getParent() {
            return m_parent;
        }

        const TRcScopeLink& getParent() const {
            return m_parent;
        }

        virtual ~ScopeLink() {
        }

    private:
        TRcScopeLink m_parent;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(ScopeLink);

        static const unsigned stTypeId;
    };

    template<class T>
    class ScopeEle : virtual public NlshObject, public ScopeLink {
    public:

        explicit ScopeEle(TRcScopeLink &parent, T& ele)
        : ScopeLink(parent), m_ele(ele) {
        }

        explicit ScopeEle(T& ele)
        : m_ele(ele) {
        }

        T& getEle() {
            return m_ele;
        }

        const T& getEle() const {
            return m_ele;
        }

        virtual ~ScopeEle() {
        }

    private:
        T m_ele;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(ScopeEle);

        static const unsigned stTypeId;
    };

    /* Scoped elements.
     */
    //ScopedCell is a list of ScopedCell: i.e., (ScopedCell)+
    typedef ScopeEle<TRcCell> ScopedCell;
    //ScopedWire is (ScopedCell)* -> ScopedWire
    typedef ScopeEle<TRcWire> ScopedWire;
    //ScopedPinRef is (ScopedCell)* -> ScopedPinRef
    typedef ScopeEle<TRcPinRef> ScopedPinRef;

    typedef PTRcObjPtr<ScopedCell> TRcScopedCell;
    typedef PTRcObjPtr<ScopedWire> TRcScopedWire;
    typedef PTRcObjPtr<ScopedPinRef> TRcScopedPinRef;

    //Convert a parent link to its actual ScopedCell.

    inline
    TRcScopedCell asScopedCell(const TRcScopeLink &link) {
        return xyzzy::downcast<ScopeLink, ScopedCell > (link);
    }
}

#endif

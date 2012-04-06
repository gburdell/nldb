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
#if !defined(_vnl_vnl_hxx_)
#define  _vnl_vnl_hxx_

#include <string>
#include <list>
#include <map>
#include "xyzzy/refcnt.hxx"
#include "xyzzy/array.hxx"
#include "xyzzy/slist.hxx"
#include "xyzzy/exception.hxx"

///All things related to verilog netlist
namespace vnl {
    using std::string;
    using std::list;
    using std::map;
    using xyzzy::TRcObj;
    using xyzzy::PTRcObjPtr;
    using xyzzy::PTRcPtr;
    using xyzzy::PTArray;
    using xyzzy::TException;
    using xyzzy::PTSlist;

    class Location;
    class Lexer;
    class Token;
    class Parser;
    class ConstRef;
    class Writer;

    typedef PTRcPtr<Token> TRcToken;
    typedef PTRcPtr<Location> TRcLocation;
    typedef PTRcPtr<Lexer> TRcLexer;
    typedef PTRcPtr<Parser> TRcParser;
    typedef PTRcPtr<ConstRef> TRcConstRef;
    typedef PTRcPtr<Writer> TRcWriter;

#define DECL_CLASS(_cls)                \
    class _cls ;                        \
    typedef PTRcObjPtr<_cls> TRc##_cls

    DECL_CLASS(Object);
    DECL_CLASS(PinRef);
    DECL_CLASS(WireBitRef);
    DECL_CLASS(Library);
    DECL_CLASS(Module);
    DECL_CLASS(LibCell);
    DECL_CLASS(Port);
    DECL_CLASS(Wire);
    DECL_CLASS(Cell);
    DECL_CLASS(Bus);
    DECL_CLASS(PortBus);
    DECL_CLASS(WireBus);
    //#undef DECL_CLASS

    /**
     * List of connection items to a pin:  Wire/Port | WireBitRef
     *                          to a wire: Wire/Port | WireBitRef | PinRef
     */
    typedef list<TRcObject> TConnList;
    typedef PTRcPtr<TConnList> TRcConnList;

    /**
     * Cell contains map of connections to pin (by port name).
     */
    /// Per-bit connections to a pin (which could be bus; hence, array).
    typedef PTArray<TRcObject> TBitConns;
    typedef map<string, TBitConns> TConnsByPortName;
    typedef PTRcPtr<TConnsByPortName> TRcConnsByPortName;

#define COPY_CONSTRUCTOR_DECL(_cls)   \
	_cls(const _cls &);               \
	_cls& operator=(const _cls &)

    unsigned length(unsigned lb, unsigned rb);

    class Object : public virtual TRcObj {
    public:
        /**
         * Check for class/type equivalence.
         * @param o an object of type to compare.
         * @return true if this and other object are same type.
         */
        bool isA(const TRcObject &o) const;

        /**
         * Return object's typeName.
         * @return typeName.
         */
        string getTypeName() const;

        bool isType(unsigned typeId) const {
            return (typeId == getTypeId());
        }

        /**
         * (Almost) every object (instance) should have a name.
         * @return name of (instance) object.
         */
        virtual string getName() const;
        
        /**
         * Get typeId for this class.
         * @return typeId for this class.
         */
        virtual unsigned getTypeId() const;

    protected:

        virtual ~Object() {
        }

        explicit Object() {
        }

        /**
         * Subclass will call this static method (during static init) to
         * obtain a unique id which becomes the RTTI.
         *
         * @param typeName name of type.
         * @return next typeId (which subclass should record in static member).
         */
        static unsigned getNextTypeId(string typeName);

    private:
        COPY_CONSTRUCTOR_DECL(Object);

        static unsigned stTypeId;
    };

}

#endif

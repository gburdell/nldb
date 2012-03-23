//The MIT License
//
//slf - Synopsys Liberty File
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
#if !defined(_slf_slf_hxx_)
#define  _slf_slf_hxx_

#include <string>
#include <list>
#include <map>
#include "xyzzy/refcnt.hxx"
#include "xyzzy/array.hxx"
#include "xyzzy/slist.hxx"
#include "xyzzy/exception.hxx"

#if !defined(DECL_CLASS)
#define DECL_CLASS(_cls)                \
    class _cls ;                        \
    typedef PTRcObjPtr<_cls> TRc##_cls
#endif

//Simpler reference counting w/ no (easy) base<-->derived
#define DECL_CLASS_SIMPLE_RC(_cls)      \
    class _cls ;                        \
    typedef PTRcPtr<_cls> TRc##_cls

#if !defined(COPY_CONSTRUCTOR_DECL)
#define COPY_CONSTRUCTOR_DECL(_cls)   \
	_cls(const _cls &);               \
	_cls& operator=(const _cls &)
#endif

namespace slf {
    using std::string;
    using std::list;
    using std::map;
    using xyzzy::TRcObj;
    using xyzzy::PTRcObjPtr;
    using xyzzy::PTRcPtr;
    using xyzzy::PTArray;
    using xyzzy::TException;
    using xyzzy::PTSlist;

    DECL_CLASS_SIMPLE_RC(Lexer);
    DECL_CLASS_SIMPLE_RC(Token);
    DECL_CLASS_SIMPLE_RC(Parser);

	//The following used to build up AST.
	DECL_CLASS(LibraryEle);
	DECL_CLASS(KeyValue);	//subclass of LibraryEle;
	DECL_CLASS(LibCell);	//subclass of LibraryEle;

	DECL_CLASS_SIMPLE_RC(ValueSet);
	DECL_CLASS_SIMPLE_RC(ValueType);
	DECL_CLASS_SIMPLE_RC(Expr);
	DECL_CLASS_SIMPLE_RC(ExprVal);
	DECL_CLASS_SIMPLE_RC(ExprOp);
}

#endif

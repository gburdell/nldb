//The MIT License
//
//xyzzy - c++ library
//Copyright (c) 2007-2010  Karl W. Pfalzer
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

#if	!defined(_xyzzy_exception_hxx_)
#define  _xyzzy_exception_hxx_

#include <string>

namespace xyzzy {
    class TException;

    /** Base class for all exceptions.
     */
    class TException {
    public:

        /** Default constructor.
         */
        explicit TException()
        : m_reason(stNoReason) {
        }

        /** More specific constructor.
         *	@param reason detail of exception.
         */
        explicit TException(const char* reason)
        : m_reason(reason) {
        }

        /** More specific constructor.
         *	@param reason detail of exception.
         */
        explicit TException(const std::string reason)
        : m_reason(reason.c_str()) {
        }

        const char* getReason() const {
            return m_reason;
        }

        /**
         * Print getReason() to cerr.
         */
        void print() const;

        virtual ~TException();

    protected:
        /**	Detail of exception.
         */
        const char* m_reason;

    private:
        static const char* const stNoReason;
    };

    void invariant(bool cond) throw (TException);

    /// Null pointer exception class.

    class TNullPointerException : public TException {
    public:
        explicit TNullPointerException();
    };

    /// Invalid object cast.

    class TDynamicCastException : public TException {
    public:
        explicit TDynamicCastException();
    };
};

#endif	//_xyzzy_exception_hxx_

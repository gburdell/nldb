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

#if	!defined(_xyzzy_array_hxx_)
#define  _xyzzy_array_hxx_

#include <vector>
#include <list>
#include "xyzzy/exception.hxx"
#include "xyzzy/refcnt.hxx"
#include "xyzzy/slist.hxx"

///All things related to portable library.
namespace xyzzy {

    /** Base class for PTArray template.
     *  @see PTArray
     */
    class TArray {
    public:
        typedef signed long len_t;

        class IndexOutOfBoundsException : public TException {
        public:
            explicit IndexOutOfBoundsException(len_t max, len_t ix);

        private:
            len_t m_max, m_ix;
        };

        len_t length() const {
            return m_len;
        }

    protected:
        explicit TArray();

        explicit TArray(len_t len);

        len_t checkIx(len_t ix) const throw (IndexOutOfBoundsException);

        len_t m_len;
    };

    /// Type-specific array with bounds checking.

    template<class T>
    class PTArray : public TArray {
    public:

        explicit PTArray()
        : TArray(0), mp_ar(0) {
        }

        explicit PTArray(len_t len)
        : mp_ar(0) {
            resize(len);
        }

        PTArray(const PTArray &r)
        : mp_ar(0) {
            resize(r.length());
            copy(r);
        }

        explicit PTArray(const T * const p, const T stopAt)
        : mp_ar(0) {
            int n = 0;
            for (; stopAt != p[n]; n++)
                ;
            resize(n);
            for (n = 0; n < length(); n++)
                this->operator[](n) = p[n];
        }

        explicit PTArray(const std::vector<T> &r)
        : mp_ar(0) {
            resize(r.size());
            for (len_t i = 0; i < length(); i++) {
                (*this)[i] = r[i];
            }
        }

        explicit PTArray(const std::list<T> &r) 
        : mp_ar(0) {
            resize(r.size());
            typename std::list<T>::const_iterator iter(r.begin());
            for (unsigned i = 0; iter != r.end(); ++iter) {
                (*this)[i++] = (*iter);
            }
        }

        typedef PTRcPtr<std::vector<T> > TRcVector;
        
        TRcVector
        toVector() const {
            TRcVector vec = new std::vector<T>(length());
            for (len_t i = 0; i < length(); i++) {
                vec.asT()[i] = (*this)[i];
            }
            return vec;
        }
        
        void reset(const T val) {
            for (len_t n = 0; n < length(); n++)
                this->operator[](n) = val;
        }

        PTArray& operator=(const PTArray &r) {
            resize(r.length());
            copy(r);
            return *this;
        }

        void resize(len_t len) {
            delete [] mp_ar;
            m_len = len;
            mp_ar = new T[m_len];
        }

        T& operator[](len_t ix) {
            return mp_ar[checkIx(ix)];
        }

        const T& operator[](len_t ix) const {
            return mp_ar[checkIx(ix)];
        }

        ///Return last element.
        T& end() {
            return mp_ar[length()-1];
        }

        const T& end() const {
            return mp_ar[length()-1];
        }

        ~PTArray() {
            delete [] mp_ar;
        }

        static void copy(const PTArray &from, PTArray &to) {
            invariant(from.length() <= to.length());
            for (int i = 0; i < from.length(); i++) {
                to[i] = from[i];
            }
        }

        static void copy(PTSlist<T> &from, PTArray &to) {
            invariant(from.length() <= (unsigned) to.length());
            typename PTSlist<T>::Iterator iter(from);
            for (int i = 0; iter.hasMore(); ++iter) {
                to[i++] = *iter;
            }
        }

        static void copy(PTSlist<T> *from, PTArray *to) {
            copy(*from, *to);
        }

    private:
        T *mp_ar;

        void copy(const PTArray &from) {
            copy(from, *this);
        }
    };

    template<class T>
    class PTRcArray {
    public:
        typedef TArray::len_t len_t;

        explicit PTRcArray()
        : m_ar(0) {
        }

        PTRcArray(PTArray<T> *p)
        : m_ar(p) {
        }

        PTRcArray(const PTRcArray &r)
        : m_ar(r.m_ar) {
        }

        const PTRcArray& operator=(const PTRcArray &r) {
            if (this != &r) {
                m_ar = r.m_ar;
            }
            return *this;
        }

        len_t length() const {
            return m_ar->length();
        }

        T& operator[](len_t ix) {
            return m_ar->operator[](ix);
        }

        const T& operator[](len_t ix) const {
            return m_ar->operator[](ix);
        }

        bool isNull() const {
            return m_ar.isNull();
        }

        bool isValid() const {
            return !m_ar.isNull();
        }

    private:
        PTRcPtr<PTArray<T> > m_ar;
    };
};

#endif	//_xyzzy_array_hxx_

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

#if	!defined(_xyzzy_refcnt_hxx_)
#define  _xyzzy_refcnt_hxx_

#include "xyzzy/exception.hxx"

namespace xyzzy {
    // Any reference counted object must derive from TRcObj.

    class TRcObj {
    public:

        virtual ~TRcObj() {
        }
    };

    //	A class used by PTRcPtr.

    struct _TRcPtr {

        _TRcPtr()
        : mp_dat(0), m_cnt(1) {
        }

        _TRcPtr(TRcObj * p)
        : mp_dat(p), m_cnt(1) {
        }

        bool decr() {
            if (1 > --m_cnt) {
                delete mp_dat;
            }
            return (1 > m_cnt) ? true : false;
        }

        ~_TRcPtr() {
        }

        TRcObj *mp_dat;
        unsigned m_cnt;
    };

    template<class T> class PTRcObjPtr;

    template<class T> _TRcPtr* getBasePtr(const PTRcObjPtr<T> &r);

    // A class derived from TRcObj uses PTRcObjPtr for refence counting.
    // Casting between Base <--> Derived can be done using:
    //		downcast (base to derived)
    //      and upcast (derived to base) functions.

    template<class T>
    class PTRcObjPtr {
    public:
        explicit PTRcObjPtr();

        PTRcObjPtr(T *p);

        PTRcObjPtr(const PTRcObjPtr &r);

        explicit PTRcObjPtr(_TRcPtr *p);

        const PTRcObjPtr& operator=(const PTRcObjPtr &r);

        const PTRcObjPtr& operator=(T *p);

        bool isNull() const;

        bool isValid() const {
            return !isNull();
        }

        T* operator->();

        const T* operator->() const;

        T& operator*() {
            return *this->operator->();
        }

        const T& operator*() const {
            return *this->operator->();
        }

        T* getPtr() //also see asT()
        {
            return this->operator->();
        }

        const T* getPtr() const //also see asT()
        {
            return this->operator->();
        }

        T& asT() {
            return *getPtr();
        }

        const T& asT() const {
            return *getPtr();
        }

        ~PTRcObjPtr();

    private:
        void decr();

        _TRcPtr *m_p;

        friend _TRcPtr* getBasePtr<T>(const PTRcObjPtr<T> &r);

#if defined(DEBUG)
        const T *pDBG_t;

#define SET_T pDBG_t = dynamic_cast<const T*>(m_p->mp_dat)
#else
#define SET_T
#endif
    };

    template<class T>
    _TRcPtr*
    getBasePtr(const PTRcObjPtr<T> &r) {
        return r.m_p;
    }

    template<class Base, class Derived>
    PTRcObjPtr<Derived>
    downcast(const PTRcObjPtr<Base> &p) {
        _TRcPtr *rp = getBasePtr(p);
        return PTRcObjPtr<Derived > (rp);
    }

    template<class Base, class Derived>
    PTRcObjPtr<Base>
    upcast(const PTRcObjPtr<Derived> &p) {
        _TRcPtr *rp = getBasePtr(p);
        return PTRcObjPtr<Base > (rp);
    }

    // A class *not* derived from TRcObj uses PTRcPtr for reference counting.
    // NOTE: Casting between Base <--> Derived cannot (easily!) be done
    // (except using getPtr(), but not recommended.)

    template<class T>
    class PTRcPtr {
    public:
        explicit PTRcPtr();

        PTRcPtr(T *p);

        PTRcPtr(const PTRcPtr &r);

        const PTRcPtr& operator=(const PTRcPtr &r);

        const PTRcPtr& operator=(T *p);

        bool isNull() const;

        bool isValid() const {
            return !isNull();
        }

        T* operator->();

        const T* operator->() const;

        T* getPtr() //also see asT()
        {
            return this->operator->();
        }

        const T* getPtr() const //also see asT()
        {
            return this->operator->();
        }

        T& asT() {
            return *getPtr();
        }

        const T& asT() const {
            return *getPtr();
        }

        ~PTRcPtr();

    private:
        void decr();

        struct TRcPtr2 {

            TRcPtr2()
            : mp_dat(0), m_cnt(1) {
            }

            TRcPtr2(T * p)
            : mp_dat(p), m_cnt(1) {
            }

            bool decr() {
                if (1 > --m_cnt) {
                    delete mp_dat;
                }
                return (1 > m_cnt) ? true : false;
            }

            ~TRcPtr2() {
            }

            T *mp_dat;
            unsigned m_cnt;
        };

        TRcPtr2 *m_p;
    };

    //
    // Class definitions
    //

    template<class T>
    PTRcObjPtr<T>::PTRcObjPtr() {
        m_p = new _TRcPtr();
        SET_T;
    }

    template<class T>
    PTRcObjPtr<T>::PTRcObjPtr(T *p) {
        m_p = new _TRcPtr(p);
        SET_T;
    }

    template<class T>
    PTRcObjPtr<T>::PTRcObjPtr(const PTRcObjPtr &r) {
        m_p = r.m_p;
        m_p->m_cnt++;
        SET_T;
    }

    template<class T>
    PTRcObjPtr<T>::PTRcObjPtr(_TRcPtr *p) {
        m_p = p;
        m_p->m_cnt++;
        SET_T;
    }

    template<class T>
    const PTRcObjPtr<T>&
    PTRcObjPtr<T>::operator=(const PTRcObjPtr &r) {
        if (this != &r) {
            decr();
            m_p = r.m_p;
            m_p->m_cnt++;
            SET_T;
        }
        return *this;
    }

    template<class T>
    const PTRcObjPtr<T>&
    PTRcObjPtr<T>::operator=(T *p) {
        decr();
        m_p = new _TRcPtr(p);
        SET_T;
        return *this;
    }

    template<class T>
    bool
    PTRcObjPtr<T>::isNull() const {
        return (0 != m_p->mp_dat) ? false : true;
    }

    template<class T>
    T*
    PTRcObjPtr<T>::operator->() {
        if (isNull()) {
            throw TNullPointerException();
        }
        T* p = dynamic_cast<T*> (m_p->mp_dat);
        if (0 == p) {
            throw TDynamicCastException();
        }
        return p;
    }

    template<class T>
    const T*
    PTRcObjPtr<T>::operator->() const {
        if (isNull()) {
            throw TNullPointerException();
        }
        const T* p = dynamic_cast<const T*> (m_p->mp_dat);
        if (0 == p) {
            throw TDynamicCastException();
        }
        return p;
    }

    template<class T>
    PTRcObjPtr<T>::~PTRcObjPtr() {
        decr();
    }

    template<class T>
    void
    PTRcObjPtr<T>::decr() {
        if (true == m_p->decr()) {
            delete m_p;
        }
    }

    template<class T>
    PTRcPtr<T>::PTRcPtr() {
        m_p = new TRcPtr2();
    }

    template<class T>
    PTRcPtr<T>::PTRcPtr(T *p) {
        m_p = new TRcPtr2(p);
    }

    template<class T>
    PTRcPtr<T>::PTRcPtr(const PTRcPtr &r) {
        m_p = r.m_p;
        m_p->m_cnt++;
    }

    template<class T>
    const PTRcPtr<T>&
    PTRcPtr<T>::operator=(const PTRcPtr &r) {
        if (this != &r) {
            decr();
            m_p = r.m_p;
            m_p->m_cnt++;
        }
        return *this;
    }

    template<class T>
    const PTRcPtr<T>&
    PTRcPtr<T>::operator=(T *p) {
        decr();
        m_p = new TRcPtr2(p);
        return *this;
    }

    template<class T>
    bool
    PTRcPtr<T>::isNull() const {
        return (0 != m_p->mp_dat) ? false : true;
    }

    template<class T>
    T*
    PTRcPtr<T>::operator->() {
        if (isNull()) {
            throw TNullPointerException();
        }
        return m_p->mp_dat;
    }

    template<class T>
    const T*
    PTRcPtr<T>::operator->() const {
        if (isNull()) {
            throw TNullPointerException();
        }
        return m_p->mp_dat;
    }

    template<class T>
    PTRcPtr<T>::~PTRcPtr() {
        decr();
    }

    template<class T>
    void
    PTRcPtr<T>::decr() {
        if (true == m_p->decr()) {
            delete m_p;
        }
    }
}

#undef SET_T

#endif	//_xyzzy_refcnt_hxx_

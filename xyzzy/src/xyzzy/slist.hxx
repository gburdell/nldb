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

#if	!defined(_xyzzy_slist_hxx_)
#define  _xyzzy_slist_hxx_

#include <list>

namespace xyzzy {
    class TSlist {
    public:

        unsigned length() const {
            return m_len;
        }

        bool isEmpty() const {
            return (0 == m_len);
        }

        bool hasMore() const {
            return !isEmpty();
        }

    protected:

        TSlist()
        : m_len(0) {
        }

        virtual ~TSlist() {
        }

        unsigned m_len;
    };

    template<typename T>
    class PTSlist : public TSlist {
    public:
        class Iterator;

        explicit PTSlist()
        : TSlist(),
        mp_head(0),
        mp_tail(0) {
        }

    private:

        struct Ele {

            Ele(T dat, Ele *pnext = 0)
            : m_dat(dat), mp_next(pnext) {
            }

            T m_dat;
            Ele *mp_next;
        };

    public:

        ///copy constructor
        PTSlist(const PTSlist<T> &from) {
            copy(from);
        }
        
        PTSlist(const std::list<T> &from) {
            copy(from);
        }
        
        PTSlist<T>& operator=(const PTSlist<T> &from) {
            erase();
            copy(from);
            return *this;
        }

        PTSlist<T>& operator=(const std::list<T> &from) {
            erase();
            copy(from);
            return *this;
        }

        T peek() const {
            return mp_head->m_dat;
        }

        T pop() {
            T rval = peek();
            Ele *p = mp_head;
            mp_head = mp_head->mp_next;
            if (0 == --m_len) {
                mp_tail = 0;
            }
            delete p;
            return rval;
        }

        void append(T dat) {
            Ele *p = new Ele(dat);
            if (0 != mp_tail) {
                mp_tail->mp_next = p;
                mp_tail = p;
            } else {
                mp_head = mp_tail = p;
            }
            m_len++;
        }

        void prepend(T dat) {
            Ele *p = new Ele(dat, mp_head);
            mp_head = p;
            if (0 == mp_tail) {
                mp_tail = p;
            }
            m_len++;
        }

        void push(T dat) {
            prepend(dat);
        }

        /// Erase all elements.
        void erase() {
            erase(mp_head);
            delete mp_head;
        }

        ~PTSlist() {
            erase();
        }

    private:
        Ele *mp_head;
        Ele *mp_tail;

        void copy(const PTSlist<T> &from) {
            for (Iterator i = from; i.hasMore(); ++i) {
                append(*i);
            }
        }

        void copy(const std::list<T> &from) {
            for (typename std::list<T>::const_iterator i = from.begin(); i != from.end(); ++i) {
                append(const_cast<T&>(*i));
            }
        }
        
        /// Erase elements starting AFTER start.

        void erase(Ele *start) {
            if (0 == start) {
                return;
            }
            Ele *p, *q = start->mp_next;
            while (q != 0) {
                p = q;
                q = q->mp_next;
                delete p;
                m_len--;
            }
            mp_tail = start;
            mp_tail->mp_next = 0;
        }

        friend class Iterator;

    public:

        class Iterator {
        public:

            explicit Iterator(PTSlist &list)
            : mp_curr(list.mp_head) {
            }

            explicit Iterator(const Iterator &iter)
            : mp_curr(iter.mp_curr) {
            }

            const Iterator& operator=(const Iterator &iter) {
                mp_curr = iter.mp_curr;
                return *this;
            }

            T& operator*() const {
                return mp_curr->m_dat;
            }

            T* operator->() const {
                return &(operator*());
            }

            const Ele* operator()() const {
                return mp_curr;
            }

            bool hasMore() const {
                return (0 != mp_curr);
            }

            // pre-incr

            Iterator& operator++() {
                incr();
                return *this;
            }

            // NOTE: post-incr requires copy constructor.

            Iterator operator++(int) {
                Iterator rval(*this);
                incr();
                return rval;
            }

        private:

            void incr() {
                if (0 != mp_curr) {
                    mp_curr = mp_curr->mp_next;
                }
            }

            /// Only PTSlist can use this to generate specific/start points.

            Iterator(Ele *where)
            : mp_curr(where) {
            }

            Ele *mp_curr;

            friend class PTSlist;
        };

        /// Erase: (iter, end...]; i.e., start erase w/ element AFTER "start".

        void erase(const Iterator &start) {
            erase(start.mp_curr);
        }

        Iterator last() {
            Iterator rval(mp_tail);
            return rval;
        }

        /// Insert element after iterator.

        void insert(Iterator &after, T dat) {
            Ele *q = const_cast<Ele*> (after());
            if (mp_tail == q) {
                append(dat);
            } else {
                Ele *p = new Ele(dat, q->mp_next);
                q->mp_next = p;
                m_len++;
            }
        }
    };
};

#endif	//_xyzzy_slist_hxx_

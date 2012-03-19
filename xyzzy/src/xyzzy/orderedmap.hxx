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

#if	!defined(_xyzzy_orderedmap_hxx_)
#define  _xyzzy_orderedmap_hxx_

#include <list>
#include <map>
#include "xyzzy/assert.hxx"

namespace xyzzy {
    using namespace std;

    template<typename K, typename T>
    class PTOrderedMap {
    public:
        typedef list<T>         t_list;
        typedef map<K, T>       t_map;

        explicit PTOrderedMap() {}

        void append(K key, T item) {
            ASSERT_TRUE(!hasKey(key));
            m_list.push_back(item);
            m_map[key] = item;
        }

        bool hasKey(K key) const {
            return (m_map.end() != m_map.find(key));
        }

        const T& operator [](K key) const {
            ASSERT_TRUE(hasKey(key));
            return m_map[key];
        }

        T& operator [](K key) {
            ASSERT_TRUE(hasKey(key));
            return m_map[key];
        }
        
        typename t_list::const_iterator iter() const {
            return m_list.begin();
        }

        typename t_list::iterator iter() {
            return m_list.begin();
        }

    private:
        t_list m_list;
        t_map m_map;

        //copy not allowed
        PTOrderedMap(const PTOrderedMap&);
        PTOrderedMap& operator=(const PTOrderedMap&);

    };

}

#endif	/* ORDEREDMAP_HXX */


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

#if	!defined(_xyzzy_util_hxx_)
#define  _xyzzy_util_hxx_

#include <iostream>
#include <map>
#include "xyzzy/refcnt.hxx"

namespace xyzzy {
    unsigned int toUnsignedInt(const char* s);

    int toInt(const char* s);

    unsigned long int toUnsignedLongInt(const char* s);

    long int toLongInt(const char* s);

    /// Convert double to little endian hex.
    /// Return number of chars used in buf.
    /// buf should be sized to accomodate terminating nul.
    int toLittleEndianHex(double d, char *buf);

    class TTicker {
    public:
        explicit TTicker(unsigned tickIncr,
                unsigned tickPcnt,
                std::ostream &os = std::cout);

        void tick();

    private:
        const unsigned m_tickIncr;
        const unsigned m_tickPcnt;
        std::ostream &m_os;
        unsigned m_pcnt;
        unsigned m_tick;
    };

    template<typename K, typename T>
    inline
    bool
    mapHasKey(const std::map<K, T> &m, const K &key) {
        return (m.find(key) != m.end());
    }

    /**
     * Get map value of key.
     * @param m map.
     * @param key key value.
     * @param val return value (if key found).
     * @return true if key found; else false.
     */
    template<typename K, typename T>
    inline
    bool
    mapGetVal(const std::map<K, T> &m, const K &key, T &val) {
        typename std::map<K, T>::const_iterator i = m.find(key);
        bool ok = (i != m.end());
        if (ok) {
            val = i->second;
        }
        return ok;
    }

    /**
     * Get map value of key.
     * @param rcm (reference counted) map.
     * @param key key value.
     * @param val return value (if key found).
     * @return true if key found; else false.
     */
    template<typename K, typename T>
    inline
    bool
    mapGetVal(const PTRcPtr<std::map<K, T> > &rcm, const K &key, T &val) {
        return mapGetVal(rcm.asT(), key, val);
    }
};

#endif	//_xyzzy_util_hxx_

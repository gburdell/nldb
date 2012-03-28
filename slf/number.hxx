/* The MIT License
 * 
 * slf - verilog netlist
 * Copyright (c) 2012       George P. Burdell
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
#if !defined(_slf_number_hxx_)
#define  _slf_number_hxx_

#include "slf/slf.hxx"

namespace slf {

    class Number {
    public:

        enum EType {
            eInteger, eFloat
        };

        explicit Number(const string &txt, EType type);

        EType getType() const {
            return m_type;
        }

        double asDouble() const {
            return m_val.m_asDbl;
        }

        long int asInt() const {
            return m_val.m_asLong;
        }

        virtual ~Number();

    private:
        const EType m_type;

        union {
            double m_asDbl;
            long int m_asLong;
        } m_val;
        //Not allowed
        COPY_CONSTRUCTOR_DECL(Number);
    };

    DebugOstream& operator<<(DebugOstream &dos, const TRcNumber &num);

}

#endif
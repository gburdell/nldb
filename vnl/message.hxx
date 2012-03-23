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
#if !defined(_vnl_message_hxx_)
#define  _vnl_message_hxx_

#include <map>
#include "vnl/vnl.hxx"

namespace vnl {
    using std::map;

    /**
     * Singleton class used for messages.
     * Greatly inspired by xyzzy/message.hxx
     */
    class Message {
    public:

        enum EType {
            eInfo = 0, eWarn, eError, eFatal, eLast
        };

        explicit Message();

        void message(EType type, string code,
                const TRcLocation &loc, string s1, string s2 = "", string s3 = "");

        unsigned getMsgCnt(EType type) const {
            return m_msgCnts[type];
        }

        static Message& getTheOne() {
            return stTheOne;
        }

        /**
         * Get formatted message.
         * @param type type of message.
         * @param code the message hash code.
         * @param s1...sn string arguments.
         * @return formatted message.
         */
        string fmessage(EType type, string code,
                string s1, string s2 = "", string s3 = "", string s4 = "");

        void message(EType type, string msg);

		void addFmtByCode(string code, string fmt);

    private:
        typedef map<string, string> t_fmtByCode;

        t_fmtByCode m_fmtByCode;
        unsigned m_msgCnts[eLast];
        static const string stPfxs[];
        static Message stTheOne;

        COPY_CONSTRUCTOR_DECL(Message);
    };

    void error(string code,
            const TRcLocation &loc, string s1, string s2 = "", string s3 = "");

    void info(string code, string s1, string s2 = "", string s3 = "", string s4 = "");
}

#endif


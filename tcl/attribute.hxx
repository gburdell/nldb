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
#ifndef VNLTCL_ATTRIBUTE_HXX
#define VNLTCL_ATTRIBUTE_HXX

#include <string>
#include <map>
#include <utility>
#include <list>
#include "tcl/vnltcl.hxx"

namespace vnltcl {
    using std::string;
    using std::map;
    using std::pair;
    using std::list;
    using xyzzy::TRcObj;
    using xyzzy::PTRcObjPtr;

    class AttrVal;

    /**
     * A reference counted AttrVal type.
     */
    typedef PTRcObjPtr<AttrVal> TRcAttrVal;

    class AttrVal : public TRcObj {
    public:

        enum EType {
            eInt = 0, eString, eDouble, eBool
        };

        /**
         * Names align w/ types above (for direct index by EType).
         */
        static const string stTypeName[4];

        explicit AttrVal(int v);

        explicit AttrVal(string v);

        explicit AttrVal(double v);

        explicit AttrVal(bool b);

        EType getType() const {
            return m_type;
        }

        string getTypeName() const {
            return stTypeName[m_type];
        }

        //Allow default copy constructors.

        virtual ~AttrVal();

    private:
        const EType m_type;

        union Val {
            int m_int;
            string *mp_string;
            double m_double;
            bool m_bool;
            //

            Val(int v) {
                m_int = v;
            }

            //Unions cant have members w/ constructors (i.e., string).

            Val(string v) {
                mp_string = new string(v);
            }

            Val(double v) {
                m_double = v;
            }

            Val(bool v) {
                m_bool = v;
            }
        };

        Val m_val;
    };

    /**
     * Attribute exception.
     */
    class AttrException {
    public:
        static const string BAD_ATTR;
        static const string DUP_ATTR;

        explicit AttrException(string nm, string reason)
        : m_name(nm), m_reason(reason) {
        }

        const string m_name;
        const string m_reason;
    };

    /**
     * Object types with attributes "has-a" AttrHandler.
     */
    class AttrHandler;

    /**
     * A reference counted AttrVal type.
     */
    typedef PTRcObjPtr<AttrHandler> TRcAttrHandler;

    class AttrHandler : public TRcObj {
    public:
        class IProvider;

        explicit AttrHandler();

        /**
         * Return true if attribute name defined.
         * @param nm attribute name.
         * @return true if attribute name defined.
         */
        bool defined(string nm) const;

        /**
         * Get attribute value.
         * @param nm attribute name.
         * @param obj get attribute of this object.
         * @return value or throw AttrException.
         */
        TRcAttrVal getVal(string nm, TRcObject obj) const throw (AttrException);

        AttrVal::EType getType(string nm) const throw (AttrException);

        /**
         * Add attribute provider.
         * @param nm attribute name to add.
         * @param type
         * @param provider
         * @throw AttrException if nm is duplicate.
         */
        void addAttr(string nm, AttrVal::EType type, IProvider *provider) throw (AttrException);

        typedef pair<string, string> NameType;
        typedef list<NameType> NameTypeList;

        /**
         * Get list of attribute info: name, type.
         * @param info list to add info.
         * @return number of elements added.
         */
        int getAttrs(NameTypeList &info);

        /**
         * Interface of an attribute provider.
         */
        class IProvider {
        private:
            /**
             * Get attribute value of object.
             * @param nm attribute name.
             * @param obj get attribute of this object.
             * @return value.
             */
            virtual TRcAttrVal getVal(string nm, TRcObject obj) const;

            explicit IProvider();

            virtual ~IProvider() = 0;

            //AttrHandler calls getVal().
            friend class AttrHandler;
        };

    private:
        typedef pair<AttrVal::EType, IProvider*> Info;
        typedef map<string, Info> ProviderMap;

        ProviderMap m_infoByName;

        Info getInfo(string nm) const throw (AttrException);

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(AttrHandler);
    };
}

#endif  /* VNLTCL_ATTRIBUTE_HXX */


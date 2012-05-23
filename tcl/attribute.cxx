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
#include "xyzzy/util.hxx"
#include "tcl/attribute.hxx"

namespace vnltcl {
    using xyzzy::mapGetVal;
    using xyzzy::mapHasKey;

    const string AttrVal::stTypeName[4] = {
        "int", "string", "double", "bool"
    };

    const string AttrException::BAD_ATTR = "invalid attribute";
    const string AttrException::DUP_ATTR = "duplicate attribute";

    AttrVal::AttrVal(int v)
    : m_type(eInt),
    m_val(v) {
    }

    AttrVal::AttrVal(const string &v)
    : m_type(eString),
    m_val(v) {
    }

    AttrVal::AttrVal(double v)
    : m_type(eDouble),
    m_val(v) {
    }

    AttrVal::AttrVal(bool v)
    : m_type(eBool),
    m_val(v) {
    }

    string
    AttrVal::asString() const {
        ASSERT_TRUE(eString == getType());
        return *(m_val.mp_string);
    }

    AttrVal::~AttrVal() {
        if (eString == m_type) {
            delete m_val.mp_string;
        }
    }

    AttrHandler::TRcNameTypeList
    AttrHandler::getAttrs() const {
        TRcNameTypeList linfo = new NameTypeList();
        HandlerMap::const_iterator iter = m_infoByName.begin();
        const HandlerMap::const_iterator end = m_infoByName.end();
        string name;
        Info info;
        NameType nameType;
        for (; iter != end; ++iter) {
            name = iter->first;
            info = iter->second;
            nameType = std::make_pair(name, AttrVal::stTypeName[info.first]);
            linfo->push_back(nameType);
        }
        return linfo;
    }

    AttrHandler::AttrHandler() {
    }

    const AttrHandler::Info&
    AttrHandler::getInfo(string nm) const throw (AttrException) {
        if (!defined(nm)) {
            throw AttrException(nm, AttrException::BAD_ATTR);
        }
        return const_cast<AttrHandler*>(this)->m_infoByName[nm];
    }

    bool
    AttrHandler::defined(const string &nm) const {
        return mapHasKey(m_infoByName, nm);
    }

    TRcAttrVal
    AttrHandler::getVal(const string &nm) const throw (AttrException) {
        TRcAttrVal val;
        HandlerPtr fptr = getInfo(nm).second;
        if (0 != fptr) {
            val = (this->*fptr)(nm);
        } else {
            val = defaultGetVal(nm);
        }
        //verify we get type expected
        ASSERT_TRUE(val->getType() == getType(nm));
        return val;
    }

    AttrVal::EType
    AttrHandler::getType(const string &nm) const throw (AttrException) {
        Info info = getInfo(nm);
        return info.first;
    }

    void
    AttrHandler::addAttr(const string &nm, AttrVal::EType type, HandlerPtr func) throw (AttrException) {
        if (defined(nm)) {
            throw AttrException(nm, AttrException::DUP_ATTR);
        }
        Info info = std::make_pair(type, func);
        m_infoByName[nm] = info;
    }

    TRcAttrVal
    AttrHandler::defaultGetVal(const string &nm) const throw (AttrException) {
        ASSERT_NEVER;
    }
}


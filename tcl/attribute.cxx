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
#include "attribute.hxx"

namespace vnltcl {
    
    const string AttrVal::stTypeName[4] = {
        "int", "string", "double", "bool"
    };

    const string AttrException::BAD_ATTR = "invalid attribute";
    const string AttrException::DUP_ATTR = "duplicate attribute";

    AttrVal::AttrVal(int v)
    :   m_type(eInt),
        m_val(v)
    {}

    AttrVal::AttrVal(string v)
    :   m_type(eString),
        m_val(v)
    {}

    AttrVal::AttrVal(double v)
    :   m_type(eDouble),
        m_val(v)
    {}

    AttrVal::AttrVal(bool v)
    :   m_type(eBool),
        m_val(v)
    {}

    AttrVal::~AttrVal() {
        if (eString == m_type) {
            delete m_val.mp_string;
        }
    }
    
    int
    AttrHandler::getAttrs(NameTypeList &linfo) {
        int cnt = 0;
        ProviderMap::const_iterator iter = m_infoByName.begin();
        const ProviderMap::const_iterator end = m_infoByName.end();
        string name;
        Info info;
        NameType nameType;
        for (; iter != end; ++iter) {
            name = iter->first;
            info = iter->second;
            nameType = std::make_pair(name, AttrVal::stTypeName[info.first]);
            linfo.push_back(nameType);
            cnt++;
        }
        return cnt;
    }

    AttrHandler::AttrHandler()
    {}

    AttrHandler::Info
    AttrHandler::getInfo(string nm) const throw (AttrException) {
        if (! defined(nm)) {
            throw AttrException(nm, AttrException::BAD_ATTR);
        }
        Info info = const_cast<AttrHandler*>(this)->m_infoByName[nm];
        return info;
    }
    
    bool 
    AttrHandler::defined(string nm) const {
        return (m_infoByName.end() != m_infoByName.find(nm));
    }

    TRcAttrVal 
    AttrHandler::getVal(string nm, TRcObject obj) const throw (AttrException) {
        Info info = getInfo(nm);
        TRcAttrVal val = info.second->getVal(nm, obj);
        return val;
    }

    AttrVal::EType 
    AttrHandler::getType(string nm) const throw (AttrException) {
        Info info = getInfo(nm);
        return info.first;
    }

    void 
    AttrHandler::addAttr(string nm, AttrVal::EType type, IProvider *provider) throw (AttrException) {
        if (defined(nm)) {
            throw AttrException(nm, AttrException::DUP_ATTR);
        }
        Info info = std::make_pair(type, provider);
        m_infoByName[nm] = info;
    }
 
}


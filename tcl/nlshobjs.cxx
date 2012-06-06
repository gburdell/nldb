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
#include "tcl/nlshobjs.hxx"

namespace vnltcl {
    template<> const unsigned PTNlshObject<Module>::stTypeId = Object::getNextTypeId("design");
    template<> const unsigned PTNlshObject<Port>::stTypeId = Object::getNextTypeId("port");
    template<> const unsigned PTNlshObject<Cell>::stTypeId = Object::getNextTypeId("cell");
    template<> const unsigned PTNlshObject<PinRef>::stTypeId = Object::getNextTypeId("pin");

    //attribute names
    static const string cName = "name";
    static const string cIsHierarchical = "is_hierarchical";

    ///Provide an implementation of attribute handlers for each type.

    class DesignHandler : public AttrHandler {
    public:

        explicit DesignHandler() : mp_mod(0) {
            addAttr(cName, AttrVal::eString, static_cast<HandlerPtr> (&DesignHandler::name));
            addAttr(cIsHierarchical, AttrVal::eBool, static_cast<HandlerPtr> (&DesignHandler::isHierarchical));
            //add more attributes ...
        }

        TRcAttrVal name(const string &) const {
            return new AttrVal(mp_mod->asModule()->getName());
        }

        TRcAttrVal isHierarchical(const string &) const {
            return new AttrVal(mp_mod->asModule()->isHierarchical());
        }

        TRcAttrVal getVal(const string &nm, const NlshDesign *p) throw (AttrException) {
            mp_mod = p;
            return AttrHandler::getVal(nm);
        }

        ~DesignHandler() {}
        
    private:
        const NlshDesign *mp_mod;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(DesignHandler);
    };

    class PortHandler : public AttrHandler {
    public:

        explicit PortHandler() : mp_port(0) {
            addAttr(cName, AttrVal::eString, static_cast<HandlerPtr> (&PortHandler::name));
            //add more attributes ...
        }

        TRcAttrVal name(const string &) const {
            return new AttrVal(mp_port->asPort()->getName());
        }

        TRcAttrVal getVal(const string &nm, const NlshPort *p) throw (AttrException) {
            mp_port = p;
            return AttrHandler::getVal(nm);
        }

        ~PortHandler() {}
        
    private:
        const NlshPort *mp_port;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(PortHandler);
    };

    TRcAttrVal
    NlshDesign::getAttrVal(const string &nm) const throw (AttrException) {
        static DesignHandler *stHandler = new DesignHandler();
        return stHandler->getVal(nm, this);
    }

    TRcAttrVal
    NlshPort::getAttrVal(const string &nm) const throw (AttrException) {
        static PortHandler *stHandler = new PortHandler();
        return stHandler->getVal(nm, this);
    }
}

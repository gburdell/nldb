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
#include <sstream>
#include "xyzzy/regexp.hxx"
#include "tcl/nlshobjs.hxx"

namespace vnltcl {
    using std::ostringstream;
    using xyzzy::TRegExp;

    template<> const unsigned PTNlshObject<Module>::stTypeId = Object::getNextTypeId("design");
    template<> const unsigned PTNlshObject<Port>::stTypeId = Object::getNextTypeId("port");
    const unsigned NlshCell::stTypeId = Object::getNextTypeId("cell");
    const unsigned NlshPin::stTypeId = Object::getNextTypeId("pin");

    //TODO: keep hierSep in sync w/ tcl side
    static const string cHierSep = "/";

    //attribute names
    static const string cFullName = "full_name";
    static const string cIsHierarchical = "is_hierarchical";
    static const string cIsLibCell = "is_libcell";
    static const string cIsResolved = "is_resolved";
    static const string cName = "name";
    static const string cRefName = "ref_name";

    bool
    NlshObject::operator ==(const TRcNlshObject& c) const {
        return getName(true) == c->getName(true);
    }
    
    string 
    NlshObject::getName(bool full) const {
        TRcAttrVal aval;
        if (full) {
            aval = getAttrVal(cFullName);
        } 
        if (!full || aval.isNull()) {
            aval = getAttrVal(cName);
        }
        return aval->asString();
    }
    
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

        ~DesignHandler() {
        }

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

        ~PortHandler() {
        }

    private:
        const NlshPort *mp_port;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(PortHandler);
    };

    class CellHandler : public AttrHandler {
    public:

        explicit CellHandler() : mp_cell(0) {
            addAttr(cFullName, AttrVal::eString, static_cast<HandlerPtr> (&CellHandler::fullName));
            addAttr(cIsHierarchical, AttrVal::eBool, static_cast<HandlerPtr> (&CellHandler::isHierarchical));
            addAttr(cIsLibCell, AttrVal::eBool, static_cast<HandlerPtr> (&CellHandler::isLibCell));
            addAttr(cIsResolved, AttrVal::eBool, static_cast<HandlerPtr> (&CellHandler::isResolved));
            addAttr(cName, AttrVal::eString, static_cast<HandlerPtr> (&CellHandler::name));
            addAttr(cRefName, AttrVal::eString, static_cast<HandlerPtr> (&CellHandler::refName));
             //add more attributes ...
        }

        TRcAttrVal fullName(const string &) const {
            ostringstream oss;
            const NlshCell::t_hier &hier = mp_cell->m_hier;
            for (unsigned i = 0; i < hier.length(); i++) {
                if (0 < i) oss << cHierSep;
                oss << hier[i]->getName();
            }
            return new AttrVal(oss.str());
        }

        TRcAttrVal isHierarchical(const string &) const {
            const TRcCell &cell = mp_cell->m_hier.end();
            TRcAttrVal isHier = new AttrVal(cell->isResolved() && cell->isHierarchical());
            return isHier;
        }

        TRcAttrVal isLibCell(const string &) const {
            return new AttrVal(mp_cell->m_hier.end()->isLibCell());
        }

        TRcAttrVal isResolved(const string &) const {
            return new AttrVal(mp_cell->m_hier.end()->isResolved());
        }

        TRcAttrVal name(const string &) const {
            return new AttrVal(mp_cell->m_hier.end()->getName());
        }

        TRcAttrVal refName(const string &) const {
            return new AttrVal(mp_cell->m_hier.end()->getRefName());
        }

        TRcAttrVal getVal(const string &nm, const NlshCell *p) throw (AttrException) {
            mp_cell = p;
            return AttrHandler::getVal(nm);
        }

        ~CellHandler() {
        }

    private:
        const NlshCell *mp_cell;

        //Cannot copy
        DECL_COPY_CONSTRUCTORS(CellHandler);
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

    TRcAttrVal
    NlshCell::getAttrVal(const string &nm) const throw (AttrException) {
        static CellHandler *stHandler = new CellHandler();
        return stHandler->getVal(nm, this);
    }

    static
    void depthFirst(const vnl::TRcModule &vmod, const NlshCell::t_cells &pfx,
            t_nlshObjList &coll, bool hier, const TRegExp &trex) {
        typedef vnl::Module::t_cellsByName::iterator t_iter;
        vnl::Module::t_cellsByName cellsByName = vmod->getCellsByName();
        //create full path here and push/pop as we iterate
        NlshCell::t_cells path(pfx);
        TRcNlshCell cell;
        for (t_iter i = cellsByName.begin(); i != cellsByName.end(); ++i) {
            vnl::TRcCell &vcell = i->second;
            path.push_back(vcell);
            //keep cell if tail matches
            if (trex.match(vcell->getName())) {
                cell = new NlshCell(path);
                coll.append(vnltcl::upcast(cell));
            }
            if (hier) {
                if (vcell->isHierarchical() && vcell->getRef().isValid()) {
                    depthFirst(vcell->getRef(), path, coll, hier, trex);
                }
            }
            path.pop_back();
        }
    }

    void
    NlshDesign::getCells(t_nlshObjList &coll, const string &tailRex, bool hier) {
        NlshCell::t_cells path;
        TRegExp trex(tailRex);
        depthFirst(asModule(), path, coll, hier, trex);
    }
}


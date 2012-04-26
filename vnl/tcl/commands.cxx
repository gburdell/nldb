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

#include <map>
#include <cstring>
#include <tcl.h>
#include <tclDecls.h>
//
#include "xyzzy/assert.hxx"
#include "xyzzy/array.hxx"
#include "vnl/parser.hxx"
#include "vnl/library.hxx"
#include "vnl/module.hxx"
#include "slf/parser.hxx"
//#include "cell.hxx"
//#include "net.hxx"
//#include "pin.hxx"
//#include "writer.hxx"
//#include "hiername.hxx"
//
#include "vnl/tcl/vnltcl.hxx"
#include "vnl/tcl/commands.hxx"
#include "vnl/tcl/collection.hxx"
#include "vnl/tcl/iterator.hxx"
#include "vnl/tcl/message.hxx"

namespace vnltcl {

    using std::map;
    using xyzzy::PTArray;
    using xyzzy::PTRcArray;
    using vnl::TRcLibrary;
    using vnl::TRcModule;
    using vnltcl::TRcCollection;
    using vnltcl::TRcIterator;
    using vnltcl::Message;

    typedef PTArray<string> TStringAr;
    typedef PTRcArray<string> TRcStringAr;

    static const string stToolVersion = "nl_shell v2012-04-26_15.41.33";
    static const char *stPtrPfx = "_obj";
    static const string stNilArg = "_";
    // add 2 for 0x
    static const size_t stPtrSz = (2 * sizeof (const TRcObject*)) + strlen(stPtrPfx) + 2;

    //Used many times
    static const string stAllOpt = "-all";
    static const string stFilterOpt = "-filter";
    static const string stHierarchicalOpt = "-hierarchical";
    static const string stOfObjectsOpt = "-of_objects";

    /**
     * {{
     * Useful local/static methods
     */

#ifdef TODO
    namespace config {

        static bool toBool(Tcl_Interp *interp, const char *varnm) {
            const char *v = Tcl_GetVar(interp, varnm, TCL_GLOBAL_ONLY);
            return (string(v) == "true");
        }

        static int toInt(Tcl_Interp *interp, const char *varnm) {
            const char *v = Tcl_GetVar(interp, varnm, TCL_GLOBAL_ONLY);
            int rv;
            sscanf(v, "%d", &rv);
            return rv;
        }

#define WRITER_CONFIG(_nm, _type) \
        Writer::stConfig.m_##_nm = to##_type (interp, "config::vlogo("#_nm")")

        static void getConfigSettings(Tcl_Interp *interp) {
            //Transfer Tcl to here
            WRITER_CONFIG(ansiPortDecl, Bool);
            WRITER_CONFIG(compressBusConnExpr, Int);
            WRITER_CONFIG(instDeclBusConnIndent, Int);
            WRITER_CONFIG(instDeclIndent, Int);
            WRITER_CONFIG(instTopological, Bool);
            WRITER_CONFIG(portDeclIndent, Int);
            WRITER_CONFIG(topModuleLast, Bool);
            WRITER_CONFIG(wireDefnIndent, Int);
        }

#undef WRITER_CONFIG
    }; //namespace config
#endif//TODO

    static inline
    bool
    isEq(const Tcl_Obj *obj, string s) {
        return to_s(obj) == s;
    }

    static
    TRcModule
    getReference(Tcl_Interp *interp, TRcLibrary &lib, Tcl_Obj *CONST obj) {
        string refnm = to_s(obj);
        //1st try designs
        TRcModule ref = lib->getModule(refnm);
        if (ref.isNull()) {
            lib = getSlfLib();
            if (lib.isValid()) {
                ref = lib->getModule(refnm);
            }
            if (ref.isNull()) {
                throw TclError(interp, "OBJ-1", "reference", refnm);
            }
        }
        return ref;
    }

#ifdef TODO

    static
    TRcCollection
    filter(TRcCollection &coll, const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(3 == argc);
        string attrNm = to_s(argv[0]);
        bool ifEq = (to_s(argv[1]) == "=~");
        Tcl_RegExp re = Tcl_GetRegExpFromObj(getInterp(), argv[2], TCL_REG_ADVANCED);
        if (NULL == re) {
            throw TclError(); //already in interp result
        }
        TRcCollection rval = new Collection();
        const char *pnm;
        Tcl_Obj *tobj;
        int match;
        for (CollectionIter iter(*coll); iter.hasMore(); ++iter) {
            tobj = (*iter)->getAttribute(attrNm);
            pnm = to_s(tobj).c_str();
            match = Tcl_RegExpExec(getInterp(), re, pnm, pnm);
            if (0 > match) {
                throw TclError();
            }
            if ((ifEq && (0 < match)) || (!ifEq && (0 == match))) {
                rval->add(*iter);
            }
        }
        return rval;
    }
#endif

    TRcLibrary&
    getDesignLib(bool createIfNull) {
        TRcLibrary &lib = Commands::getTheOne().getState().m_designLib;
        if (lib.isNull() && createIfNull) {
            lib = new vnl::Library("design");
        }
        return lib;
    }

    TRcLibrary&
    getSlfLib(bool createIfNull) {
        TRcLibrary &lib = Commands::getTheOne().getState().m_slfLib;
        if (lib.isNull() && createIfNull) {
            lib = new vnl::Library("slf");
        }
        return lib;
    }

    TRcModule&
    setCurrentDesign(TRcModule &mod) {
        TRcModule &r = Commands::getTheOne().getState().m_currDesn;
        r = mod;
        return r;
    }

    /**
     * }}
     * Useful local/static methods
     */

    Commands Commands::stTheOne;

    Commands::Commands()
    : mp_objType(0), mp_interp(0) {
        //these are in alphabetical order
        m_cmdByName["current_design"] = &Commands::currentDesign;
        m_cmdByName["get_tool_version"] = &Commands::getToolVersion;
        m_cmdByName["read_slf"] = &Commands::readSlf;
        m_cmdByName["read_verilog"] = &Commands::readVerilog;
        //some hidden commands
        m_cmdByName["_is_object"] = &Commands::isObject;
#ifdef TODO
        m_cmdByName["add_to_collection"] = &Commands::addToCollection;
        m_cmdByName["append_to_collection"] = &Commands::appendToCollection;
        m_cmdByName["all_connected"] = &Commands::allConnected;
        m_cmdByName["change_link"] = &Commands::changeLink;
        m_cmdByName["connect_net"] = &Commands::connectNet;
        m_cmdByName["create_cell"] = &Commands::createCell;
        m_cmdByName["create_iterator"] = &Commands::createIterator;
        m_cmdByName["create_net"] = &Commands::createNet;
        m_cmdByName["create_port"] = &Commands::createPort;
        m_cmdByName["disconnect_net"] = &Commands::disconnectNet;
        m_cmdByName["filter_collection"] = &Commands::filterCollection;
        m_cmdByName["get_attribute"] = &Commands::getAttribute;
        m_cmdByName["get_cells"] = &Commands::getCells;
        m_cmdByName["get_designs"] = &Commands::getDesigns;
        m_cmdByName["get_name_eles"] = &Commands::getHierNameEles;
        m_cmdByName["get_nets"] = &Commands::getNets;
        m_cmdByName["get_pins"] = &Commands::getPins;
        m_cmdByName["get_ports"] = &Commands::getPorts;
        m_cmdByName["index_collection"] = &Commands::indexCollection;
        m_cmdByName["iterator_get_next"] = &Commands::iteratorGetNext;
        m_cmdByName["iterator_has_next"] = &Commands::iteratorHasNext;
        m_cmdByName["query_objects"] = &Commands::queryObjects;
        m_cmdByName["remove_cell"] = &Commands::removeCell;
        m_cmdByName["remove_from_collection"] = &Commands::removeFromCollection;
        m_cmdByName["remove_net"] = &Commands::removeNet;
        m_cmdByName["sizeof_collection"] = &Commands::sizeofCollection;
        m_cmdByName["write_verilog"] = &Commands::writeVerilog;
 #endif
    }

    void Commands::info(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eInfo, 0, code, s1, s2, s3, s4);
    }

    void Commands::warn(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eWarn, 0, code, s1, s2, s3, s4);
    }

    void Commands::error(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eError, 0, code, s1, s2, s3, s4);
    }

    template<typename D>
    PTRcObjPtr<D>
    Commands::downcast(Tcl_Obj *CONST tobj) throw (TclError) {
        TRcObject obj = getObject(tobj);
        if (!D::isA(obj)) {
            throw TclError(getInterp(), "OBJ-3",
                    to_s(tobj), D::getTypeName());
        }
        PTRcObjPtr<D> refobj = xyzzy::downcast<Object, D > (obj);
        return refobj;
    }

    char*
    Commands::ptrToString(const TRcObject *p) {
        char *buf = ckalloc(stPtrSz + 1); //+1 for null
        int n = sprintf(buf, "%s%p", stPtrPfx, p);
        ASSERT_TRUE(n <= stPtrSz);
        return buf;
    }

    Tcl_Obj*
    Commands::createTclObj(TRcObject ref) {
        Tcl_Obj *pobj = (Tcl_Obj*) ckalloc(sizeof (Tcl_Obj));
        TRcObject *pref = new TRcObject(ref);
        pobj->refCount = 0;
        pobj->typePtr = const_cast<Tcl_ObjType*> (mp_objType);
        pobj->internalRep.otherValuePtr = pref;
        char *p = ptrToString(pref);
        pobj->bytes = p;
        pobj->length = strlen(p) + 1;
        return pobj;
    }

    void
    Commands::setObjType(const Tcl_ObjType &r) {
        ASSERT_TRUE(0 == mp_objType);
        mp_objType = &r;
    }

    TRcObject
    Commands::getObject(Tcl_Obj *p) throw (TclError) {
        if (mp_objType != p->typePtr) {
            throw TclError(getInterp(), "OBJ-2", to_s(p));
        }
        TRcObject *pref = static_cast<TRcObject*> (p->internalRep.otherValuePtr);
        return *pref;
    }

    Tcl_Obj*
    Commands::dispatch(Tcl_Interp *interp, const int argc, Tcl_Obj *CONST argv[])
    throw (TclError) {
        mp_interp = interp;
        Tcl_Obj *pobj = 0;
        string subcmd = to_s(argv[1]);
        std::map<string, CommandPtr>::iterator iter = m_cmdByName.find(subcmd);
        if (m_cmdByName.end() == iter) {
            throw TclError(getInterp(), "invalid nlsh subcommand: " + to_s(argv[1]));
        } else {
            CommandPtr funcPtr = iter->second;
            pobj = (getTheOne().*funcPtr)(argc - 2, argv + 2); //skip vnltcl subcmd
        }
        return pobj;
    }

    TRcCollection
    Commands::asCollection(Tcl_Obj *CONST obj) throw (TclError) {
        TRcObject scalar = getObject(obj);
        //if we're not a collection; make one
        if (!Collection::isA(scalar)) {
            return new Collection(scalar);
        }
        return downcast<Collection > (obj);
    }

    TRcIterator
    Commands::asIterator(Tcl_Obj *CONST obj) throw (TclError) {
        return downcast<Iterator > (obj);
    }

#ifdef TODO

    TRcCollection
    Commands::matchByNameColl(TRcCollection coll, Tcl_Obj *CONST rex, bool useFullNm) throw (TclError) {
        {//An optimization if match all
            string re = to_s(rex);
            if (re == ".*" || re == ".+") {
                return coll;
            }
        }
        Tcl_RegExp re = Tcl_GetRegExpFromObj(getInterp(), rex, TCL_REG_ADVANCED);
        if (NULL == re) {
            throw TclError(); //already in interp result
        }
        TRcCollection rval = new Collection();
        const char *pnm;
        int match;
        for (CollectionIter iter(*coll); iter.hasMore(); ++iter) {
            //TODO: we will always use full name for now.
            //cleanup patterns -vs- -exact/-regexp as per DC?
            if (true || useFullNm) {
                pnm = (*iter)->getFullName().c_str();
            } else {
                pnm = (*iter)->getName().c_str();
            }
            match = Tcl_RegExpExec(getInterp(), re, pnm, pnm);
            if (0 > match) {
                throw TclError();
            }
            if (0 < match) {
                rval->add(*iter);
            }
        }
        return rval;
    }

    Tcl_Obj*
    Commands::matchByName(TRcCollection coll, Tcl_Obj *CONST rex, bool useFullNm) throw (TclError) {
        return createTclObj(matchByNameColl(coll, rex, useFullNm));
    }
#endif

    //BEGIN{ commands

    /*
     * Commands are implemented with consistent interface, so can build
     * a dispatch table/map.
     */

    Tcl_Obj*
    Commands::readVerilog(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcLibrary &lib = getDesignLib();
        TRcStringAr fnames = listAsStringAr(getInterp(), argv[0]);
        bool ok = true;
        for (int i = 0; ok && (i < fnames.length()); i++) {
            if (isFileReadable(fnames[i])) {
                vnl::TRcLexer lexer = new vnl::Lexer(fnames[i]);
                vnl::Parser parser(lexer);
                parser.start(lib);
                ok &= (0 == parser.getErrorCnt());
            } else {
                error("FILE-1", fnames[i], "read");
                ok = false;
            }
        }
        if (!ok) {
            throw TclError(getInterp(), "PARSE-1", "");
        }
        Tcl_Obj* pobj = Tcl_NewIntObj(ok ? 1 : 0);
        return pobj;
    }

    Tcl_Obj*
    Commands::readSlf(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcLibrary &lib = getSlfLib();
        TRcStringAr fnames = listAsStringAr(getInterp(), argv[0]);
        bool ok = true;
        for (int i = 0; ok && (i < fnames.length()); i++) {
            if (isFileReadable(fnames[i])) {
                slf::TRcLexer lexer = new slf::Lexer(fnames[i]);
                slf::Parser parser(lexer);
                parser.start(lib);
                ok &= (0 == parser.getErrorCnt());
            } else {
                error("FILE-1", fnames[i], "read");
                ok = false;
            }
        }
        if (!ok) {
            throw TclError(getInterp(), "PARSE-1", "");
        }
        Tcl_Obj* pobj = Tcl_NewIntObj(ok ? 1 : 0);
        return pobj;
    }

    Tcl_Obj*
    Commands::currentDesign(const int argc, Tcl_Obj *CONST objv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        bool ok = true;
        Tcl_Obj *pobj = 0;
        TRcModule des;
        TRcLibrary lib = getDesignLib();
        if (!isEq(objv[0], stNilArg)) {
            string nm = to_s(objv[0]);
            des = lib->getModule(nm);
            ok = des.isValid();
            if (ok) {
                des = setCurrentDesign(des);
            } else {
                throw TclError(getInterp(), "OBJ-1", "design", nm);
            }
        }
        if (ok) {
            string nm = des.isValid() ? des->getName() : "";
            pobj = newStringObj(nm);
        }
        return pobj;
    }

    Tcl_Obj*
    Commands::getToolVersion(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(0 == argc);
        return newStringObj(stToolVersion);
    }

    Tcl_Obj*
    Commands::isObject(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        bool isObj = (mp_objType == argv[0]->typePtr);
        return Tcl_NewIntObj(isObj ? 1 : 0);
    }

#ifdef TODO

    TRcModule
    Commands::getCurrentDesign() throw (TclError) {
        TRcModule currDesn = Library::getDesLib()->getCurrentDesign();
        if (!currDesn.isValid()) {
            throw TclError(getInterp(), "DES-1", "");
        }
        return currDesn;
    }

    Tcl_Obj*
    Commands::queryObjects(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE((1 == argc) || (2 == argc));
        TRcCollection coll = asCollection(argv[0]);
        int max = 100;
        if (2 == argc) {
            if (TCL_OK != Tcl_GetIntFromObj(getInterp(), argv[1], &max)) {
                throw TclError();
            }
        }
        int omax = max;
        string nm;
        TRcObject ele;
        Tcl_Obj *plist = Tcl_NewListObj(0, 0); //a new tcl list
        for (CollectionIter iter(*coll); iter.hasMore() && ((0 == omax) || (max > 0));
                ++iter) {
            ele = *iter;
            nm = ele->getTypeName() + ":" + ele->getFullName();
            if (TCL_OK != Tcl_ListObjAppendElement(getInterp(), plist, newStringObj(nm))) {
                throw TclError();
            }
            if (0 != omax) {
                max--;
            }
        }
        if ((omax != 0) && (omax < coll->length())) {
            static const string cDot3 = "...";
            if (TCL_OK != Tcl_ListObjAppendElement(getInterp(), plist, newStringObj(cDot3))) {
                throw TclError();
            }
        }
        return plist;
    }

    Tcl_Obj*
    Commands::getHierNameEles(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        Tcl_Obj *plist = Tcl_NewListObj(0, 0);
        string hierName = to_s(argv[0]);
        HierName hn(hierName);
        for (HierName::len_t i = 0; i < hn.length(); i++) {
            if (TCL_OK != Tcl_ListObjAppendElement(getInterp(), plist, newStringObj(hn[i]))) {
                throw TclError();
            }
        }
        return plist;
    }

    Tcl_Obj*
    Commands::getDesigns(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcCollection allDesigns = Library::getDesLib()->getDesigns();
        return matchByName(allDesigns, argv[0]);
    }

    //* Allowed options (in order):
    //* -hierarchical? (-filter attr eq rex)? patt
    //  ^0              ^1      ^2   ^3 ^4    ^5
    //Caller must populate argv as marked above, else _

    Tcl_Obj*
    Commands::getCells(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(6 == argc);
        TRcCollection coll;
        Tcl_Obj *CONST patt = argv[argc - 1];
        bool isHier = isEq(argv[0], stHierarchicalOpt);
        if (isHier) {
            TRcCollection allCells = getCurrentDesign()->getCells(isHier);
            coll = matchByNameColl(allCells, patt, isHier);
        } else {
            TRcStringAr eles = listAsStringAr(getInterp(), patt);
            TRcModule curr = getCurrentDesign();
            for (int i = 0; i < eles.length(); i++) {
                TRcHierName hname = new HierName(eles[i]);
                coll = curr->getCells(hname, &coll);
            }
        }
        if (!isEq(argv[1], stFilterOpt)) {
            return createTclObj(coll);
        }
        return createTclObj(filter(coll, 3, &argv[2]));
    }

    //Allowed options (in order):
    //  -hierarchical? -leaf? (-of_objects cells)? (-filter attr eq rex)? patt
    //  ^0             ^1      ^2          ^3       ^4      ^5   ^6 ^7    ^8
    //Caller must populate argv as marked above, else _

    Tcl_Obj*
    Commands::getPins(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(9 == argc);
        bool isHier = isEq(argv[0], stHierarchicalOpt);
        bool isLeaf = isEq(argv[1], "-leaf");
        bool hasObjs = isEq(argv[2], stOfObjectsOpt);
        Tcl_Obj *CONST patt = argv[8];
        TRcCollection cells;
        //Collect all the pins
        TRcCollection allPins = new Collection();
        TRcCollection byName, pins;
        if (isHier || isLeaf || hasObjs) {
            if (hasObjs) {
                cells = asCollection(argv[3]);
            } else {
                cells = getCurrentDesign()->getCells(isHier);
            }
            TRcCell cell;
            for (CollectionIter iter(*cells); iter.hasMore(); ++iter) {
                cell = toCell(*iter);
                if (!isLeaf || !cell->isHierarchical()) {
                    allPins->add(cell->getPins(cell));
                }
            }
            //Filter by patt (which is required)
            byName = matchByNameColl(allPins, patt, isHier);
        } else {
            //get specified patt
            byName = new Collection();
            TRcStringAr eles = listAsStringAr(getInterp(), patt);
            TRcModule curr = getCurrentDesign();
            for (int i = 0; i < eles.length(); i++) {
                TRcHierName hname = new HierName(eles[i]);
                TRcCollection pins = curr->getPins(hname);
                if (pins.isValid()) {
                    byName->add(pins);
                }
            }
        }
        //save to pins, in case no filter
        pins = byName;
        if (isEq(argv[4], stFilterOpt)) {
            pins = filter(byName, 3, &argv[5]);
        }
        return createTclObj(pins);
    }

    //Allowed options (in order):
    //  -hierarchical? (-of_objects pins)? (-filter attr eq rex)? patt
    //  ^0              ^1          ^2      ^3      ^4   ^5 ^6    ^7
    //Caller must populate argv as marked above, else _

    Tcl_Obj*
    Commands::getNets(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(8 == argc);
        bool isHier = isEq(argv[0], stHierarchicalOpt);
        bool hasObjs = isEq(argv[1], stOfObjectsOpt);
        if (isHier && hasObjs) {
            throw TclError(getInterp(), "OPTS-1", stHierarchicalOpt, stOfObjectsOpt);
        }
        TRcCollection allNets;
        if (hasObjs) {
            allNets = new Collection();
            TRcCollection pins = asCollection(argv[2]);
            TRcPin pin;
            for (CollectionIter iter(*pins); iter.hasMore(); ++iter) {
                pin = toPin(*iter);
                allNets->add(upcast(pin->getNet()));
            }
        } else {
            allNets = getCurrentDesign()->getNets();
        }
        if (isHier) {
            TRcCollection cells = getCurrentDesign()->getCells(true);
            TRcCell cell;
            TRcCollection cellNets;
            for (CollectionIter iter(*cells); iter.hasMore(); ++iter) {
                cell = toCell(*iter);
                cellNets = cell->getNets(cell);
                allNets->add(cellNets);
            }
        }
        Tcl_Obj *CONST patt = argv[7];
        TRcCollection byName = matchByNameColl(allNets, patt, isHier);
        TRcCollection pins = byName;
        if (isEq(argv[3], stFilterOpt)) {
            pins = filter(byName, 3, &argv[4]);
        }
        return matchByName(allNets, argv[7]);
    }

    Tcl_Obj*
    Commands::getPorts(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcCollection allPorts = getCurrentDesign()->getPorts();
        return matchByName(allPorts, argv[0]);
    }

    Tcl_Obj*
    Commands::filterCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(4 == argc);
        TRcCollection coll = asCollection(argv[0]);
        TRcCollection filColl = filter(coll, argc - 1, &argv[1]);
        return createTclObj(filColl);
    }

    Tcl_Obj*
    Commands::addToCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcCollection base = asCollection(argv[0]);
        TRcCollection toAdd = asCollection(argv[1]);
        TRcCollection rval = new Collection(base);
        for (CollectionIter iter(*toAdd); iter.hasMore(); ++iter) {
            rval->add(*iter);
        }
        return createTclObj(rval);
    }

    Tcl_Obj*
    Commands::removeFromCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcCollection base = asCollection(argv[0]);
        TRcCollection toRemove = asCollection(argv[1]);
        TRcCollection rval = new Collection();
        /**
         * TODO: currently O(N*M); change base to map so lookup = O(1)
         */
        bool removeIt = false;
        TRcObject obj, has;
        for (CollectionIter hasIter(*base); hasIter.hasMore(); ++hasIter) {
            has = *hasIter;
            removeIt = false;
            for (CollectionIter iter(*toRemove); !removeIt && iter.hasMore(); ++iter) {
                obj = *iter;
                removeIt = (has == obj);
            }
            if (!removeIt) {
                rval->add(has);
            }
        }
        return createTclObj(rval);
    }

    Tcl_Obj*
    Commands::appendToCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(3 == argc);
        bool unique = isEq(argv[0], "-unique");
        TRcCollection base;
        //TODO: not the most elegant way to detect valid collection
        try {
            base = asCollection(argv[1]);
        } catch (TclError) {
            base = new Collection();
        }
        /*TODO:
         * For unique, time complexity if O(N*M), which can get large.
         * One technique would be to convert base into hash, so can turn
         * for loop into hash-lookup.
         */
        TRcCollection toAdd = asCollection(argv[2]);
        bool addIt = false;
        TRcObject obj, has;
        for (CollectionIter iter(*toAdd); iter.hasMore(); ++iter) {
            obj = *iter;
            if (unique) {
                addIt = true;
                for (CollectionIter hasIter(*base); addIt && hasIter.hasMore(); ++hasIter) {
                    has = *hasIter;
                    addIt = (has != obj);
                }
            }
            if (!unique || addIt) {
                base->add(obj);
            }
        }
        return createTclObj(base);
    }

    Tcl_Obj*
    Commands::indexCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcCollection base = asCollection(argv[0]);
        int index = to_i(getInterp(), argv[1]);
        int n = base->length();
        if (index > n - 1) {
            throw TclError(getInterp(), "COLL-1", to_s(index), to_s(n));
        } else if (0 > index) {
            index = n + index; //from end
            if (0 > index) {
                index = 0;
            }
        }
        CollectionIter iter(*base);
        for (; (0 < index) && iter.hasMore(); ++iter, --index) {
            //do nothing but next iteration;
        }
        TRcObject obj = *iter;
        return createTclObj(obj);
    }

    Tcl_Obj*
    Commands::sizeofCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcCollection coll = asCollection(argv[0]);
        Tcl_Obj* pobj = Tcl_NewIntObj(coll->length());
        return pobj;
    }

    Tcl_Obj*
    Commands::createIterator(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcCollection coll = asCollection(argv[0]);
        TRcIterator iter = new Iterator(coll);
        return createTclObj(upcast(iter));
    }

    Tcl_Obj*
    Commands::iteratorHasNext(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcIterator iter = asIterator(argv[0]);
        bool hasMore = iter->hasNext();
        Tcl_Obj* pobj = Tcl_NewIntObj((hasMore) ? 1 : 0);
        return pobj;
    }

    Tcl_Obj*
    Commands::iteratorGetNext(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcIterator iter = asIterator(argv[0]);
        TRcObject rcobj = iter->getNext();
        ASSERT_TRUE(rcobj.isValid());
        Tcl_Obj* pobj = createTclObj(rcobj);
        return pobj;
    }

    Tcl_Obj*
    Commands::getAttribute(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcCollection coll;
        TRcObject tobj = getObject(argv[0]);
        string attrNm = to_s(argv[1]);
        if (Collection::isA(tobj)) {
            coll = toCollection(tobj);
        } else {
            coll = new Collection();
            coll->add(tobj);
        }
        Tcl_Obj *plist = Tcl_NewListObj(0, 0); //a new tcl list
        Tcl_Obj *ele;
        for (CollectionIter iter(*coll); iter.hasMore(); ++iter) {
            ele = (*iter)->getAttribute(attrNm);
            if (TCL_OK != Tcl_ListObjAppendElement(getInterp(), plist, ele)) {
                throw TclError();
            }
        }
        return plist;
    }

    Tcl_Obj*
    Commands::validate(bool ok, bool insert, string type, string nm) {
        static const string codes[2][3] = {
            {"OBJ-4", "EDIT-2", "EDIT-1"},
            {"OBJ-5", "EDIT-4", "EDIT-3"}
        };
        string code[3] = codes[insert ? 0 : 1];
        TRcModule curr = Library::getDesLib()->getCurrentDesign();
        if (!ok) {
            error(code[0], type, nm, curr->getName());
        }
        if (ok) {
            info(code[1], type, nm, curr->getName());
        } else {
            error(code[2], type, nm, curr->getName());
        }
        return Tcl_NewIntObj(ok ? 1 : 0);
    }

    Tcl_Obj*
    Commands::createCell(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        LibraryDesign libDes;
        string nm = to_s(argv[0]);
        TRcModule ref = getReference(getInterp(), libDes.m_lib, argv[1]);
        bool ok = !libDes.m_curr->hasCell(nm);
        static const string stType = "cell";
        if (ok) {
            ok = libDes.m_curr->createCell(nm, ref);
        }
        return validate(ok, true, stType, nm);
    }

    Tcl_Obj*
    Commands::changeLink(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        LibraryDesign libDes;
        TRcCollection cells = asCollection(argv[0]);
        TRcModule ref = getReference(getInterp(), libDes.m_lib, argv[1]);
        TRcCell cell;
        unsigned cnt = 0;
        for (CollectionIter iter(*cells); iter.hasMore(); ++iter) {
            cell = toCell(*iter);
            if (cell->changeLink(ref)) {
                cnt++;
            }
        }
        return Tcl_NewIntObj(cnt);
    }

    Tcl_Obj*
    Commands::createNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        LibraryDesign libDes;
        string nm = to_s(argv[0]);
        bool ok = !libDes.m_curr->hasNet(nm);
        static const string stType = "net";
        if (ok) {
            ok = libDes.m_curr->createNet(nm);
        }
        return validate(ok, true, stType, nm);
    }

    Tcl_Obj*
    Commands::removeCell(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        LibraryDesign libDes;
        string nm = to_s(argv[0]);
        bool ok = libDes.m_curr->hasCell(nm);
        static const string stType = "cell";
        if (ok) {
            ok = libDes.m_curr->removeCell(nm);
        }
        return validate(ok, false, stType, nm);
    }

    Tcl_Obj*
    Commands::createPort(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        LibraryDesign libDes;
        string nm = to_s(argv[1]);
        bool ok = libDes.m_curr->hasPort(nm);
        static const string stType = "port";
        if (!ok) {
            string dir = to_s(argv[0]);
            Port::EDir edir;
            if (dir == "in") edir = Port::eIn;
            else if (dir == "out") edir = Port::eOut;
            else if (dir == "inout") edir = Port::eInout;
            else ASSERT_NEVER;
            ok = libDes.m_curr->createPort(nm, edir);
        }
        return validate(ok, true, stType, nm);
    }

    Tcl_Obj*
    Commands::removeNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        LibraryDesign libDes;
        string nm = to_s(argv[0]);
        bool ok = libDes.m_curr->hasNet(nm);
        static const string stType = "net";
        if (ok) {
            ok = libDes.m_curr->removeNet(nm);
        }
        return validate(ok, false, stType, nm);
    }

    Tcl_Obj*
    Commands::allConnected(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        bool leaf = isEq(argv[0], "-leaf");
        TRcCollection coll = asCollection(argv[1]);
        if (1 != coll->length()) {
            throw TclError(getInterp(), "OBJ-6", "");
        }
        TRcObject obj = coll->peek();
        TRcNet net = toNet(obj);
        TRcCollection rcoll = net->getOnNet(vnltcl::Net::ePinsPorts, true, leaf);
        return createTclObj(rcoll);
    }

    Tcl_Obj*
    Commands::writeVerilog(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(3 == argc);
        config::getConfigSettings(getInterp());
        bool isHier = isEq(argv[0], stHierarchicalOpt);
        ASSERT_TRUE(isHier);
        string designNm = to_s(argv[1]);
        string outFname = to_s(argv[2]);
        TRcLibrary lib = Library::getDesLib();
        TRcModule top = lib->getDesign(designNm);
        if (top.isNull()) {
            throw TclError(getInterp(), "OBJ-1", "design", designNm);
        }
        bool ok = lib->writeVlogNetlist(outFname, top, isHier);
        return Tcl_NewIntObj(ok ? 1 : 0);
    }

    Tcl_Obj*
    Commands::connectNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcModule des = Library::getDesLib()->getCurrentDesign();
        string netnm = to_s(argv[0]);
        TRcNet net = des->getNet(netnm);
        if (net.isNull()) {
            throw TclError(getInterp(), "OBJ-1", "net", netnm);
        }
        TRcCollection objs = asCollection(argv[1]);
        unsigned cnt = vnltcl::Net::connect(net, objs);
        return Tcl_NewIntObj(cnt);
    }

    Tcl_Obj*
    Commands::disconnectNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(3 == argc);
        bool all = isEq(argv[0], stAllOpt);
        TRcCollection objs = asCollection(argv[2]);
        if (all && !objs->isEmpty()) {
            throw TclError(getInterp(), "OPTS-1", stAllOpt, "coll");
        }
        TRcModule des = Library::getDesLib()->getCurrentDesign();
        string netnm = to_s(argv[1]);
        TRcNet net = des->getNet(netnm);
        if (net.isNull()) {
            throw TclError(getInterp(), "OBJ-1", "net", netnm);
        }
        if (all) {
            objs = net->getOnNet(vnltcl::Net::ePinsPorts, false/*hier*/, false/*leaf*/);
        }
        unsigned cnt = net->disconnect(objs);
        return Tcl_NewIntObj(cnt);
    }
#endif

}

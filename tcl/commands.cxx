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
#include <sstream>
//
#include "xyzzy/assert.hxx"
#include "xyzzy/array.hxx"
#include "xyzzy/regexp.hxx"
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
#include "tcl/vnltcl.hxx"
#include "tcl/commands.hxx"
#include "tcl/collection.hxx"
#include "tcl/iterator.hxx"
#include "tcl/message.hxx"

namespace vnltcl {

    using std::map;
    using std::ostringstream;
    using xyzzy::PTArray;
    using xyzzy::PTRcArray;
    using xyzzy::TRegExp;
    using xyzzy::TRcRegExp;
    using vnl::TRcLibrary;
    using vnl::Module;
    using vnl::TRcModule;
    using vnl::TRcPort;

    typedef PTArray<string> TStringAr;
    typedef PTRcArray<string> TRcStringAr;

    static const string stToolVersion = "nl_shell v2012-07-18_18.20.58";
    static const char *stPtrPfx = "_obj";
    static const string stNilArg = "_";
    // add 2 for 0x
    static const size_t stPtrSz = (2 * sizeof (const TRcNlshObject*)) + strlen(stPtrPfx) + 2;

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
    TRcNlshDesign
    getReference(Tcl_Interp *interp, TRcLibrary &lib, Tcl_Obj *CONST obj) {
        string refnm = to_s(obj);
        //1st try designs
        TRcModule ref = lib->getModule(refnm);
        if (ref.isNull()) {
            throw TclError(interp, "OBJ-1", "reference", refnm);
        }
        TRcNlshDesign dsgn;
        if (ref.isValid()) {
            dsgn = new NlshDesign(ref);
        }
        return dsgn;
    }

    static
    TRcCollection
    filter(TRcCollection &coll, const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(3 == argc);
        string attrNm = to_s(argv[0]);
        bool ifEq = (to_s(argv[1]) == "=~");
        string rexs = to_s(argv[2]);
        TRcRegExp re;
        TRcAttrVal attrVal;
        bool match;
        TRcNlshObject obj;
        try {
            re = new TRegExp(rexs);
        } catch (const string &err) {
            throw TclError(getInterp(), "REX-1", rexs, err);
        }
        TRcCollection rval = new Collection();
        for (CollectionIter iter(*coll); iter.hasMore(); ++iter) {
            obj = *iter;
            attrVal = obj->getAttrVal(attrNm);
            if (attrVal.isValid()) {
                match = re->match(attrVal->asString());
                if (ifEq ? match : !match) {
                    rval->append(obj);
                }
            }
        }
        return rval;
    }

    /**
     * }}
     * Useful local/static methods
     */

    Commands Commands::stTheOne;

    Commands::Commands()
    : mp_objType(0), mp_interp(0) {
        //these are in alphabetical order
        m_cmdByName["add_to_collection"] = &Commands::addToCollection;
        m_cmdByName["append_to_collection"] = &Commands::appendToCollection;
        m_cmdByName["current_design"] = &Commands::currentDesign;
        m_cmdByName["filter_collection"] = &Commands::filterCollection;
        m_cmdByName["get_attribute"] = &Commands::getAttribute;
        m_cmdByName["get_cells"] = &Commands::getCells;
        m_cmdByName["get_ports"] = &Commands::getPorts;
        m_cmdByName["get_tool_version"] = &Commands::getToolVersion;
        m_cmdByName["index_collection"] = &Commands::indexCollection;
        m_cmdByName["link"] = &Commands::link;
        m_cmdByName["read_slf"] = &Commands::readSlf;
        m_cmdByName["read_verilog"] = &Commands::readVerilog;
        m_cmdByName["remove_from_collection"] = &Commands::removeFromCollection;
        m_cmdByName["sizeof_collection"] = &Commands::sizeofCollection;
        //some hidden commands
        m_cmdByName["_is_object"] = &Commands::isObject;
#ifdef TODO
        m_cmdByName["all_connected"] = &Commands::allConnected;
        m_cmdByName["change_link"] = &Commands::changeLink;
        m_cmdByName["connect_net"] = &Commands::connectNet;
        m_cmdByName["create_cell"] = &Commands::createCell;
        m_cmdByName["create_iterator"] = &Commands::createIterator;
        m_cmdByName["create_net"] = &Commands::createNet;
        m_cmdByName["create_port"] = &Commands::createPort;
        m_cmdByName["disconnect_net"] = &Commands::disconnectNet;
        m_cmdByName["get_designs"] = &Commands::getDesigns;
        m_cmdByName["get_name_eles"] = &Commands::getHierNameEles;
        m_cmdByName["get_nets"] = &Commands::getNets;
        m_cmdByName["get_pins"] = &Commands::getPins;
        m_cmdByName["iterator_get_next"] = &Commands::iteratorGetNext;
        m_cmdByName["iterator_has_next"] = &Commands::iteratorHasNext;
        m_cmdByName["query_objects"] = &Commands::queryObjects;
        m_cmdByName["remove_cell"] = &Commands::removeCell;
        m_cmdByName["remove_net"] = &Commands::removeNet;
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
        TRcNlshObject obj = getObject(tobj);
        if (!D::isA(obj)) {
            throw TclError(getInterp(), "OBJ-3",
                    to_s(tobj), D::getTypeName());
        }
        PTRcObjPtr<D> refobj = xyzzy::downcast<NlshObject, D > (obj);
        return refobj;
    }

    char*
    Commands::ptrToString(const TRcNlshObject *p) {
        char *buf = ckalloc(stPtrSz + 1); //+1 for null
        int n = sprintf(buf, "%s%p", stPtrPfx, p);
        ASSERT_TRUE(n <= stPtrSz);
        return buf;
    }

    Tcl_Obj*
    Commands::createTclObj(TRcNlshObject ref) {
        Tcl_Obj *pobj = (Tcl_Obj*) ckalloc(sizeof (Tcl_Obj));
        TRcNlshObject *pref = new TRcNlshObject(ref);
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

    TRcNlshObject
    Commands::getObject(Tcl_Obj *p) throw (TclError) {
        if (mp_objType != p->typePtr) {
            throw TclError(getInterp(), "OBJ-2", to_s(p));
        }
        TRcNlshObject *pref = static_cast<TRcNlshObject*> (p->internalRep.otherValuePtr);
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
        TRcNlshObject scalar = getObject(obj);
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
                ; //TODOpnm = (*iter)->getFullName().c_str();
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

    TRcNlshDesign&
    Commands::getCurrentDesign(bool errIfNotSet) throw (TclError) {
        TRcNlshDesign& curr = getState().m_currDesn;
        if (curr.isNull() && errIfNotSet) {
            throw TclError(getInterp(), "DES-1", "");
        }
        return curr;
    }

    TRcLibrary&
    Commands::getDesignLib(bool createIfNull) {
        TRcLibrary &lib = getState().m_designLib;
        if (lib.isNull() && createIfNull) {
            lib = new vnl::Library("design");
        }
        return lib;
    }

    TRcLibrarySet&
    Commands::getSlfLibs(bool createIfNull) {
        TRcLibrarySet &lib = getState().m_slfLib;
        if (lib.isNull() && createIfNull) {
            lib = new slf::LibrarySet();
        }
        return lib;
    }

    void
    Commands::setCurrentDesign(TRcModule &mod) {
        getState().m_currDesn = new NlshDesign(mod);
    }

    //BEGIN{ commands

    /*
     * Commands are implemented with consistent interface, so can build
     * a dispatch table/map.
     */

    Tcl_Obj*
    Commands::sizeofCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcCollection coll = asCollection(argv[0]);
        Tcl_Obj* pobj = Tcl_NewIntObj(coll->length());
        return pobj;
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
        TRcNlshObject obj, has;
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
        TRcNlshObject obj, has;
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
        TRcNlshObject obj = *iter;
        return createTclObj(obj);
    }

    Tcl_Obj*
    Commands::getAttribute(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(2 == argc);
        TRcCollection coll;
        TRcNlshObject tobj = getObject(argv[0]);
        string attrNm = to_s(argv[1]);
        if (Collection::isA(tobj)) {
            coll = toCollection(tobj);
        } else {
            coll = new Collection();
            coll->add(tobj);
        }
        Tcl_Obj *plist = Tcl_NewListObj(0, 0); //a new tcl list
        TRcAttrVal ele;
        Tcl_Obj *pobj;
        static const string cNull = "";
        for (CollectionIter iter(*coll); iter.hasMore(); ++iter) {
            ele = (*iter)->getAttrVal(attrNm);
            pobj = newStringObj(ele.isValid() ? ele->asString() : cNull);
            if (TCL_OK != Tcl_ListObjAppendElement(getInterp(), plist, pobj)) {
                throw TclError();
            }
        }
        return plist;
    }

    //* Allowed options (in order):
    //* -hierarchical? (-filter attr eq rex)? patt
    //  ^0              ^1      ^2   ^3 ^4    ^5
    Tcl_Obj*
    Commands::getCells(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(6 == argc);
        TRcCollection coll = new Collection();
        string patt = to_s(argv[argc - 1]);
        bool isHier = isEq(argv[0], stHierarchicalOpt);
        try {
            getCurrentDesign()->getCells(coll.asT(), patt, isHier);
            if (isEq(argv[1], stFilterOpt)) {
                patt = to_s(argv[4]);
                coll = filter(coll, 3, &argv[2]);
            }
        } catch (const string &err) {
            throw TclError(getInterp(), "REX-1", patt, err);
        }
        return createTclObj(coll);
    }

    Tcl_Obj*
    Commands::getPorts(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcNlshDesign curr = getCurrentDesign();
        TRcCollection allPorts = new Collection();
        typedef Module::t_ports t_ports;
        t_ports ports;
        curr->asModule()->getPortsInDeclOrder(ports);
        TRcNlshPort port;
        for (t_ports::iterator iter = ports.begin(); iter != ports.end(); ++iter) {
            port = new NlshPort(*iter);
            allPorts->add(upcast(port));
        }
        return matchByName(allPorts, argv[0]);
    }

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
        Tcl_Obj* pobj = Tcl_NewIntObj(ok ? TCL_OK : TCL_ERROR);
        return pobj;
    }

    Tcl_Obj*
    Commands::readSlf(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(1 == argc);
        TRcLibrarySet &lib = getSlfLibs();
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
        Tcl_Obj* pobj = Tcl_NewIntObj(ok ? TCL_OK : TCL_ERROR);
        return pobj;
    }

    //TODO: implementation is inconsistent since were manipulating vnl::Module

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
                setCurrentDesign(des);
            } else {
                throw TclError(getInterp(), "DES-2", nm);
            }
        } else {
            TRcNlshDesign curr = getCurrentDesign(false);
            ok = curr.isValid();
            if (ok) {
                des = curr->asModule();
            }
        }
        if (ok) {
            string nm = des.isValid() ? des->getName() : "";
            pobj = newStringObj(nm);
        }
        return pobj;
    }

    Tcl_Obj*
    Commands::link(const int argc, Tcl_Obj *CONST argv[]) throw (TclError) {
        ASSERT_TRUE(0 == argc);
        TRcNlshDesign currd = getCurrentDesign();
        list<TRcObject> libs;
        unsigned cnt = getState().getLibs(libs);
        ASSERT_TRUE(0 < cnt);
        vnl::Module::trc_unresolvedCntByName unresolved = currd->asModule()->link(libs);
        ostringstream oss;
        if (unresolved.isValid()) {
            for (vnl::Module::t_unresolvedCntByName::const_iterator iter = unresolved->begin();
                    iter != unresolved->end(); ++iter) {
                oss << iter->first << ' ' << iter->second << ' ';
                {
                    ostringstream oss;
                    oss << iter->second;
                    error("LINK-1", oss.str(), iter->first);
                }
            }
        }
        return Tcl_NewStringObj(oss.str().c_str(), -1);
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

    unsigned
    Commands::State::getLibs(list<TRcObject> &libs) {
        libs.clear();
        TRcObject libObj = upcast(m_designLib);
        libs.push_back(libObj);
        if (m_slfLib.isValid()) {
            slf::LibrarySet::t_libByName::iterator iter = m_slfLib->getEntries().begin();
            for (; iter != m_slfLib->getEntries().end(); ++iter) {
                libObj = upcast(iter->second);
                libs.push_back(libObj);
            }
        }
        return libs.size();
    }
}

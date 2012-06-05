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
#ifndef VNLTCL_COMMANDS_HXX
#define	VNLTCL_COMMANDS_HXX

#include <string>
#include <map>
#include <list>
#include "tcl.h"
#include "xyzzy/refcnt.hxx"
#include "vnl/vnl.hxx"
#include "vnl/library.hxx"
#include "tcl/vnltcl.hxx"
#include "tcl/nlshobjs.hxx"
#include "tcl/util.hxx"
#include "tcl/collection.hxx"
#include "tcl/iterator.hxx"
#include "slf/libraryset.hxx"

namespace vnltcl {
    using vnl::TRcModule;
    //TODO: move TRcLibrary to NlshLibrary
    using vnl::TRcLibrary;
    using slf::TRcLibrarySet;
    using std::map;
    using std::list;

    class Commands;

    template<typename D>
    PTRcObjPtr<D> downcast(Commands &cntxt, Tcl_Obj *CONST obj) throw (TclError);

    /**
     * Singleton object of commands.
     */
    class Commands {
    public:

        /*
         * A convenience for getting current state.
         */
        struct State {
            TRcLibrary m_designLib;
            TRcLibrarySet m_slfLib;
            TRcNlshDesign m_currDesn;
            
            /**
             * Get libraries in order: designLib, slfLibs.
             * @param libs clear and populate with libraries.
             * @return number of total libraries.
             */
            unsigned getLibs(list<TRcObject> &libs);
        };

        static Commands& getTheOne() {
            return stTheOne;
        }

        State& getState() {
            return m_state;
        }

        const State& getState() const {
            return m_state;
        }

        void info(string code, string s1, string s2 = "", string s3 = "", string s4 = "");

        void warn(string code, string s1, string s2 = "", string s3 = "", string s4 = "");

        void error(string code, string s1, string s2 = "", string s3 = "", string s4 = "");

        /**
         * Convert pointer to nul terminated string.
         * Storage is allocated here.
         * @param p
         * @return 
         */
        char* ptrToString(const TRcNlshObject *p);

        /**
         * Create Tcl object.
         * @param ref reference to object.
         * @return tcl object.
         */
        Tcl_Obj* createTclObj(TRcNlshObject ref);

        Tcl_Obj* createTclObj(TRcCollection coll) {
            return createTclObj(upcast(coll));
        }

        void setObjType(const Tcl_ObjType &r);

        /**
         * Execute command specified by argv[0].
         * @param interp interpreter to use.
         * @param argc number of arguments.
         * @param argv arguments.
         * @return result or TCL_ERROR if error. 
         */
        Tcl_Obj* dispatch(Tcl_Interp *interp, const int argc, Tcl_Obj *CONST argv[])
        throw (TclError);

        /**
         * Get interpreter used within dispatch.
         * @return the interpreter.
         */
        Tcl_Interp* getInterp() const {
            return mp_interp;
        }

    private:
        /**
         * Return current design; else error if not set.
         * @param errIfNotSet true if throw error if current design not set.
         * @return current design.
         */
        TRcNlshDesign getCurrentDesign(bool errIfNotSet = true) throw (TclError);

        /**
         * Get object from Tcl side.
         * @param p tcl object.
         * @return ana Object.
         */
        TRcNlshObject getObject(Tcl_Obj *p) throw (TclError);

		/**
         * Return collection of objects with getName() matching regular expression.
         * @param coll superset collection to filter by getName().
         * @param rex regular expression.
         * @param useFullNm set true to use getFullName(), else getName().
         * @return tcl/object collection filtered by getName().
         */
        Tcl_Obj* matchByName(TRcCollection coll, Tcl_Obj *CONST rex, bool useFullNm = false) throw (TclError);
        
        /**
         * Same as matchByName, but returns collection.
         */
        TRcCollection matchByNameColl(TRcCollection coll, Tcl_Obj *CONST rex, bool useFullNm = false) throw (TclError);

        /**
         * Tcl_Obj->TRcNlshObject->TRc(Derived_type).
         * @param obj
         * @return reference counted derived type.
         */
        template<typename D>
        PTRcObjPtr<D> downcast(Tcl_Obj *CONST obj) throw (TclError);

        /**
         * Get object as a collection.
         * @param obj Tcl object.
         * @return collection.
         */
        TRcCollection asCollection(Tcl_Obj *CONST obj) throw (TclError);

        /**
         * Get object as a iterator.
         * @param obj Tcl object.
         * @return iterator.
         */
        TRcIterator asIterator(Tcl_Obj *CONST obj) throw (TclError);

        /**
         * Return tcl list of element details (type:name) of collection.
         * @param argc expect 1 or 2
         * @param argv [0]=collection; [1]=max. number of elements to detail.
         *             Default to 100 elements if [1] not specified.
         * @return tcl list of element details.
         */
        Tcl_Obj* queryObjects(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Set current design, if specified; then, return current design.
         * @param argc 0 if no design specified; else 1.
         * @param argv current design name in [0], if specified. 
         * @return name of design or null on error.
         */
        Tcl_Obj* currentDesign(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Read verilog netlist.
         * @param argc expect 1.
         * @param argv [0]=tcl list of file(s) to read.
         * @return current design name.
         */
        Tcl_Obj* readVerilog(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Link current design.
         * @param argc expect 0.
         * @param argv not used.
         * @return flat list of {refName cnt ...} of unresolved references
         * and their count(s).
         */
        Tcl_Obj* link(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Read liberty files.
         * @param argc expect 1.
         * @param argv [0]=tcl list of file(s) to read.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* readSlf(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get collection of design objects.
         * @param argc expect 1.
         * @param argv [0]=single regular expression.
         * @return collection of matching Design objects.
         */
        Tcl_Obj* getDesigns(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get collection of cell objects in current design.
         * Allowed options (in order):
         *      -hierarchical? (-filter attr eq rex)? patt
         * @param argc expect 1, 2, 4, 5
         * @param argv [0..4] depending on usage (see .cxx for details).
         * @return collection of matching Cell objects.
         */
        Tcl_Obj* getCells(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get collection of pin objects in current design.
         * Allowed options (in order):
         *      -hierarchical? -leaf? (-of_objects cells)? (-filter attr eq rex)? patt
         * @param argc expect 1, 2, 3, ...
         * @param argv [0..?] depending on usage (see .cxx for details).
         * @return collection of matching Pin objects.
         */
        Tcl_Obj* getPins(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get collection of port objects in current design.
         * @param argc expect 1.
         * @param argv [0]=single regular expression.
         * @return collection of matching Port objects.
         */
        Tcl_Obj* getPorts(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get collection of net objects in current design.
         * @param argc expect 1.
         * @param argv [0]=single regular expression.
         * @return collection of matching Net objects.
         */
        Tcl_Obj* getNets(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get tcl list of hiername elements.
         * @param argc expect 1.
         * @param argv [0]=tcl string.
         * @return tcl list of name elements.
         */
        Tcl_Obj* getHierNameEles(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return number of elements in collection.
         * @param argc expect 1.
         * @param argv [0]=collection object.
         * @return number of elements in collection.
         */
        Tcl_Obj* sizeofCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return new collection based on results of filter by attribute value.
         * @param argc expect 4.
         * @param argv [0]=collection object; [1]=attribute name; [2]="=~"|"!~"; [3]=attr regexp 
         * @return new collection based on filter result.
         */
        Tcl_Obj* filterCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Create new collection which is base + new_collection.
         * The return collection can be heterogeneous.
         * The base collection is not disturbed.
         * @param argc expect 3.
         * @param argv [0]=-unique [1]=base collection object; [2]=collection to add 
         * @return existing collection or new collection if base object not a collection.
         */
        Tcl_Obj* addToCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Create new collection which is base - collection.
         * The return collection can be heterogeneous.
         * The base collection is not disturbed.
         * @param argc expect 3.
         * @param argv [0]=-unique [1]=base collection object; [2]=collection to remove 
         * @return existing collection or new collection if base object not a collection.
         */
        Tcl_Obj* removeFromCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Append to existing collection.
         * @param argc expect 2.
         * @param argv [0]=base collection object; [1]=collection to add 
         * @return new collection: base + new_collection.
         */
        Tcl_Obj* appendToCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return object at specified index of collection.
         * The base collection is not disturbed.
         * @param argc expect 2.
         * @param argv [0]=base collection object; [1]=integer index (+ from front; - from end). 
         * @return object at index.
         */
        Tcl_Obj* indexCollection(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Get iterator associated with a collection.
         * @param argc expect 1.
         * @param argv [0]=collection object.
         * @return tcl object Iterator.
         */
        Tcl_Obj* createIterator(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return iterator object's hasNext() result.
         * @param argc expect 1.
         * @param argv [0]=iterator object.
         * @return tcl integer value: 1 is hasMore(), else 0.
         */
        Tcl_Obj* iteratorHasNext(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return iterator object's getNext() value.
         * @param argc expect 1.
         * @param argv [0]=iterator object.
         * @return tcl TRcNlshObject value.
         */
        Tcl_Obj* iteratorGetNext(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return attribute value(s) for specified object or collection.
         * @param argc expect 2.
         * @param argv [0]=object or collection [1]=attribute name
         * @return tcl list of attribute value(s).
         */
        Tcl_Obj* getAttribute(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Create cell in current design.
         * @param argc expect 2.
         * @param argv [0] = cell name; [1] = reference name.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* createCell(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Link cell(s) to new reference.
         * @param argc expect 2.
         * @param argv [0] = cell collection; [1] = reference name.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* changeLink(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Create net in current design.
         * @param argc expect 1.
         * @param argv [0] = net name.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* createNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Create port in current design.
         * @param argc expect 2.
         * @param argv [0] = port name.
         * @param argv [1] = direction: in|out|inout
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* createPort(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Remove cell in current design.
         * @param argc expect 1.
         * @param argv [0] = cell name.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* removeCell(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Remove net in current design.
         * @param argc expect 1.
         * @param argv [0] = net name.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* removeNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return (heterogeneous) collection of ports and pins connected to net object.
         * TODO: Hierarchical traversal nor -leaf not done yet.
         * @param argc expect 1.
         * @param argv [0] TRcNet object.
         * @return collection of ports and pins.
         */
        Tcl_Obj* allConnected(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Write netlist.
         * @param argc expect 3.
         * @param argv [0] = -hierarchical; [1] = designNm; [2] = outFname.
         * @return TCL_OK on success; TCL_ERROR on fail;
         */
        Tcl_Obj* writeVerilog(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Connect net to objects.
         * @param argc expect 2.
         * @param argv [0] = net name; [1] = coll to connect to net
         * @return number of objects in coll connected.
         */
        Tcl_Obj* connectNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Disconnect net from objects.
         * @param argc expect 3.
         * @param argv [0]=-all?; [1] = net name; [2] = coll to disconnect
         * @return number of objects disconnected.
         */
        Tcl_Obj* disconnectNet(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Return version of ana_shell.
         * @param argc expect 0.
         * @param argv none.
         * @return version string.
         */
        Tcl_Obj* getToolVersion(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        /**
         * Validate insert or remove operation.
         * @param ok operation success.
         * @param insert true on insert; false on remove.
         * @param type cell | net | ...
         * @param objNm name of object being manipulated.
         * @return TCL_OK on success; TCL_ERROR on fail.
         */
        Tcl_Obj* validate(bool ok, bool insert, string type, string objNm);

        /**
         * Check if arg is an ana::Object.
         * @param argc expect 1
         * @param argv
         * @return TCL_OK if argument is (based on) ana::Object
         */
        Tcl_Obj* isObject(const int argc, Tcl_Obj *CONST argv[]) throw (TclError);

        explicit Commands();

        DECL_COPY_CONSTRUCTORS(Commands);

        ~Commands() {
        }

        static Commands stTheOne;

        //Pointer to member functions, so can build a dispatch table/map.
        typedef Tcl_Obj* (Commands::*CommandPtr)(const int, Tcl_Obj *CONST objv[]);

        /**
         * A dispatch table for commands.
         */
        map<string, CommandPtr> m_cmdByName;

        const Tcl_ObjType *mp_objType;
        Tcl_Interp *mp_interp;
        State m_state;
    };

    //Convenience methods (use vnltcl::method to distinguish from same in Commands::xxx)
    //
    inline
    Tcl_Interp*
    getInterp() {
        return Commands::getTheOne().getInterp();
    }

    TRcLibrary&
    getDesignLib(bool createIfNull = true);

    TRcLibrarySet&
    getSlfLibs(bool createIfNull = true);

    inline
    TRcNlshDesign&
    getCurrentDesign() {
        return Commands::getTheOne().getState().m_currDesn;
    }

    TRcNlshDesign&
    setCurrentDesign(TRcModule &mod);
}

#endif	/* VNLTCL_COMMANDS_HXX */


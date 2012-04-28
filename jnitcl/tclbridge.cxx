/**
 * Bridge to tcl world.
 * The otherValuePtr is to a JObject.
 * Much inspiration by: http://tuvalu.santafe.edu/~vince/EvoXandCpptcl.html
 */

#include <string>
#include <cstdlib>
#include <cstdio>
#include "tcl.h"
#include "xyzzy/assert.hxx"
#include "commands.hxx"
#include "util.hxx"

using nldb::TclError;
using nldb::JObject;
using std::string;

/* Type for a pointer to a C++ object */
Tcl_FreeInternalRepProc NldbshFreeIntRepProc;
Tcl_DupInternalRepProc NldbshDupIntRepProc;
Tcl_UpdateStringProc NldbshUpdateStringProc;
Tcl_SetFromAnyProc NldbshSetFromAnyProc;

/*extern*/ Tcl_ObjType NldbshTclObjType = {
    (char*) "NldbshObject",
    NldbshFreeIntRepProc,
    NldbshDupIntRepProc,
    NldbshUpdateStringProc,
    NldbshSetFromAnyProc
};

namespace nldb {

    static void initApp() {
        //Tcl initialization
        Tcl_RegisterObjType(&NldbshTclObjType);
        Jni *pjni = new Jni(getenv("NLSH_JOPTS"), '^');
        Commands::getTheOne().setObjType(NldbshTclObjType).setJni(pjni);
    }
}

/**
 * Create a valid internal representation from an object's string representation.
 * @param interp Tcl interpreter.
 * @param objPtr pointer containing string representation.
 * @return 
 */
int NldbshSetFromAnyProc(Tcl_Interp* interp, Tcl_Obj *objPtr) {
    if (objPtr->typePtr == &NldbshTclObjType) {
        return TCL_OK;
    }
    /*TODO: not clear when this situation occurs: i.e., when a string
     * value exists, but the backing object does not.
     */
    ASSERT_NEVER;
}

/**
 * Create a valid string representation from an object's internal representation.
 * @param objPtr pointer to object's internal representation.
 */
void NldbshUpdateStringProc(Tcl_Obj *objPtr) {
    const JObject* pobj = reinterpret_cast<const JObject*> (objPtr->internalRep.otherValuePtr);
    //From tcl docs, this should copy the string val; so dont need to alloc here.
    Tcl_SetStringObj(objPtr, pobj->objAsString().c_str(), -1);
}

void NldbshDupIntRepProc(Tcl_Obj* srcPtr, Tcl_Obj *dupPtr) {
    const JObject *pOrig = reinterpret_cast<const JObject*> (srcPtr->internalRep.otherValuePtr);
    dupPtr->internalRep.otherValuePtr = new JObject(*pOrig);
    dupPtr->typePtr = &NldbshTclObjType;
}

void NldbshFreeIntRepProc(Tcl_Obj *objPtr) {
    JObject *p = reinterpret_cast<JObject*> (objPtr->internalRep.otherValuePtr);
    delete p;
}

/**
 * The bridge command from tcl to nlsh.
 * Processes subcommands of form:  nlsh subcommand ?opts ...?
 * @param clientData the data is type ClientData, which is an opaque pointer. 
 *                   You can use this to associate state with your command. 
 *                   You register this state along with your command procedure, 
 *                   and then Tcl passes it back to you when the command is invoked.
 * @param interp the Tcl interpreter.
 * @param objc
 * @param objv
 * @return 
 */
//TODO: see Example 44–8 The BlobCmd command procedure in 
//C programming in Tcl book for excellent example of subcommands

int NldbshObjCmd(ClientData clientData, Tcl_Interp *interp,
        const int objc, Tcl_Obj *CONST objv[]) {

    if (2 > objc) {
        Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?options ...?");
        return TCL_ERROR;
    }
    Tcl_Obj *pobj = 0;
    try {
        pobj = nldb::Commands::getTheOne().dispatch(interp, objc, objv);
    } catch (TclError err) {
        return TCL_ERROR;
    }
    if (0 == pobj) {
        return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, pobj);
    return TCL_OK;
}

/**
 * The Tcl_AppInit for extended tcl app.
 * @param interp interpreter for the application.
 * @return TCL_OK or TCL_ERROR on error.
 */
extern "C" {

    int Nldbsh_AppInit(Tcl_Interp *interp) {
        //Loads in init.tcl
        if (Tcl_Init(interp) == TCL_ERROR) {
            return TCL_ERROR;
        }
        /*
         * Initialize nlsh side of things.
         */
        nldb::initApp();
        /*
         * Call Tcl_CreateObjCommand for application-specific commands, if
         * they weren't already created by the init procedures called above.
         */
        //TODO: nldb is bridge/single command
        Tcl_CreateObjCommand(interp, "nldb", NldbshObjCmd, NULL, NULL);

        {
            static const char* const stNldbshTcl = "/../../../nlsh.tcl";
            //nlsh bootstrap
            CONST char *pathName;
            pathName = Tcl_GetVar(interp, "tcl_library", TCL_GLOBAL_ONLY);
            string rpn = (string) pathName + stNldbshTcl;
            char* canon = realpath(rpn.c_str(), 0);
            bool ok = (Tcl_EvalFile(interp, canon) == TCL_OK);
            free(canon);
            if (!ok) {
                return TCL_ERROR;
            }
        }

        /*
         * Specify a user-specific startup file to invoke if the application
         * is run interactively.  Typically the startup file is "~/.apprc"
         * where "app" is the name of the application.  If this line is deleted
         * then no user-specific startup file will be run under any conditions.
         */

        Tcl_SetVar(interp, "tcl_rcFileName", "~/.nlshrc", TCL_GLOBAL_ONLY);

        return TCL_OK;
    }
}

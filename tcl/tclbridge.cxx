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
/**
 * Bridge to tcl world.
 * The otherValuePtr is to a vnl::NlshObject.
 * Much inspiration by: http://tuvalu.santafe.edu/~vince/EvoXandCpptcl.html
 */
#include <string>
#include <cstdlib>
#include <cstdio>
#include "tcl.h"
#include "xyzzy/assert.hxx"
#include "vnl/vnl.hxx"
#include "tcl/nlshobjs.hxx"
#include "tcl/commands.hxx"
#include "tcl/util.hxx"

using vnltcl::TclError;
using vnltcl::NlshObject;
using vnltcl::TRcNlshObject;
using std::string;

/* Type for a pointer to a C++ object */
Tcl_FreeInternalRepProc NlshFreeIntRepProc;
Tcl_DupInternalRepProc NlshDupIntRepProc;
Tcl_UpdateStringProc NlshUpdateStringProc;
Tcl_SetFromAnyProc NlshSetFromAnyProc;

/*extern*/ Tcl_ObjType NlshTclObjType = {
    (char*) "NlshObject",
    NlshFreeIntRepProc,
    NlshDupIntRepProc,
    NlshUpdateStringProc,
    NlshSetFromAnyProc
};

namespace vnltcl {

    static void initApp() {
        //Tcl initialization
        Tcl_RegisterObjType(&NlshTclObjType);
        Commands::getTheOne().setObjType(NlshTclObjType);
    }
}

/**
 * Create a valid internal representation from an object's string representation.
 * @param interp Tcl interpreter.
 * @param objPtr pointer containing string representation.
 * @return 
 */
void NlshUpdateStringProc(Tcl_Obj *objPtr) {
    TRcNlshObject* pobj = (TRcNlshObject*)objPtr->internalRep.otherValuePtr;
    Tcl_SetStringObj(objPtr, vnltcl::Commands::getTheOne().ptrToString(pobj), -1);
}

void NlshDupIntRepProc(Tcl_Obj* srcPtr, Tcl_Obj *dupPtr) {
    TRcNlshObject *pOrig = (TRcNlshObject*)srcPtr->internalRep.otherValuePtr;
    TRcNlshObject *pDup = new TRcNlshObject(*pOrig);
    dupPtr->internalRep.otherValuePtr = pDup;
    dupPtr->typePtr = &NlshTclObjType;
}

void NlshFreeIntRepProc(Tcl_Obj *objPtr) {
    TRcNlshObject *p = (TRcNlshObject*)objPtr->internalRep.otherValuePtr;
    delete p;
}
int NlshSetFromAnyProc(Tcl_Interp* interp, Tcl_Obj *objPtr) {
    if (objPtr->typePtr == &NlshTclObjType) {
        return TCL_OK;
    }
    /*TODO: not clear when this situation occurs: i.e., when a string
     * value exists, but the backing object does not.
     */
    ASSERT_NEVER;
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

int NlshObjCmd(ClientData clientData, Tcl_Interp *interp,
        const int objc, Tcl_Obj *CONST objv[]) {

    if (2 > objc) {
        Tcl_WrongNumArgs(interp, 1, objv, "subcommand ?options ...?");
        return TCL_ERROR;
    }
    Tcl_Obj *pobj = 0;
    try {
        pobj = vnltcl::Commands::getTheOne().dispatch(interp, objc, objv);
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

    int Nlsh_AppInit(Tcl_Interp *interp) {
        //Loads in init.tcl
        if (Tcl_Init(interp) == TCL_ERROR) {
            return TCL_ERROR;
        }
        /*
         * Initialize nlsh side of things.
         */
        vnltcl::initApp();
        /*
         * Call Tcl_CreateObjCommand for application-specific commands, if
         * they weren't already created by the init procedures called above.
         */
        //TODO: nlsh is bridge/single command
        Tcl_CreateObjCommand(interp, "nlsh", NlshObjCmd, NULL, NULL);

        {
            //static const char* const stNlshTcl = "/../../../nlsh.tcl";
            static const char* const stNlshTcl = "/../../nlsh.tcl";
            //nlsh bootstrap
            CONST char *pathName;
            pathName = Tcl_GetVar(interp, "tcl_library", TCL_GLOBAL_ONLY);
            string rpn = (string) pathName + stNlshTcl;
            char* canon = realpath(rpn.c_str(), 0);
            bool ok = (Tcl_EvalFile(interp, canon) == TCL_OK);
            free(canon);
            if (!ok) {
                return TCL_ERROR;
            }
        }

        //Create global var versions of TCL_OK and TCL_ERROR
        {
            const char buf[2] = {'0'+TCL_OK, '\0'};
            ASSERT_TRUE(NULL != Tcl_SetVar(interp, "TCL_OK", &buf[0], TCL_GLOBAL_ONLY));
        }
        {
            const char buf[2] = {'0'+TCL_ERROR, '\0'};
            ASSERT_TRUE(NULL != Tcl_SetVar(interp, "TCL_ERROR", &buf[0], TCL_GLOBAL_ONLY));
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

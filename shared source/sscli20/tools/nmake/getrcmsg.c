// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
// getrcmsg.c

#include "rotor_pal.h"

static HINSTANCE    hInstModule = NULL;

HINSTANCE
SetHInstance(HINSTANCE h)
{
    HINSTANCE   hRet = hInstModule;
    hInstModule = h;
    return hRet;
}


//
// The error message list is hard-coded into the executable
// instead of using the PALRT Satellite APIs, to avoid having 
// nmake depend on the PALRT, since nmake is needed for
// bootstrapping Rotor's build.
//
typedef struct _errmsglist {
    int mnum;
    const char *mtext;
    } ERRMSGLIST;

const ERRMSGLIST ErrMsgList[] = {

#include "errstr.rc"
    {0, NULL}
};

#if __GNUC__
__cdecl char * get_err(unsigned msg_num)
#else
char  * 
get_err(unsigned msg_num)
#endif
{

    static char rgchErr[1024];

    int i;

    for (i=0; ErrMsgList[i].mtext; ++i) {
	if (ErrMsgList[i].mnum == msg_num) {
	    strcpy(rgchErr, ErrMsgList[i].mtext);
	    return rgchErr;
	}
    }
    rgchErr[0]='\0';
    return rgchErr;

    return rgchErr;
}

int
SetErrorFile(char *pFilename, char *pExeName, int fSearchExePath)
{
    return 1;
}

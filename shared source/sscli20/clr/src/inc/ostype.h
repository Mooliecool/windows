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


#ifndef WRAPPER_CONTRACT
#define WRAPPER_CONTRACT
#endif

#ifndef LEAF_CONTRACT
#define LEAF_CONTRACT
#endif




inline BOOL RunningOnWinNT()
{
    LEAF_CONTRACT;
    return TRUE;
}

inline BOOL RunningOnWin95()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL RunningOnWinNT5()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL RunningOnWinXP()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL RunningOnWin2003()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL RunningOnLonghorn()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL ExOSInfoAvailable()
{
    LEAF_CONTRACT;
    return FALSE;
}

inline BOOL ExOSInfoRunningOnServer()
{
    LEAF_CONTRACT;
    return FALSE;
}




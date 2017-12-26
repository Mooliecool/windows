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
// getmsg.h

#ifndef __GETMSG_H__
#define __GETMSG_H__

#if __GNUC__
extern "C" __cdecl char *  get_err(unsigned);
#else
char *  get_err(unsigned);
#endif
int SetErrorFile(char *szFilename, char *szExeName, int fSearchExePath);
long SetHInstace(long hInstModule);

#endif // __GETMSG_H__

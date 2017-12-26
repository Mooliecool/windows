/*============================================================
**
** Source: test2.h
**
** 
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**=========================================================*/


const WCHAR szChildFileW[] = {'c','h','i','l','d','p','r','o','c','e','s','s','\0'};
const WCHAR szArgs[] = {' ',0x41,' ','B',' ','C','\0'};
const WCHAR szArg1[] = {0x41,'\0'};
const WCHAR szArg2[] = {'B','\0'};
const WCHAR szArg3[] = {'C','\0'};

const char *szTestString = "An uninteresting test string (it works though)";

const DWORD EXIT_OK_CODE   = 100;
const DWORD EXIT_ERR_CODE1 = 101;
const DWORD EXIT_ERR_CODE2 = 102;
const DWORD EXIT_ERR_CODE3 = 103;
const DWORD EXIT_ERR_CODE4 = 104;
const DWORD EXIT_ERR_CODE5 = 105;

#define BUF_LEN  128


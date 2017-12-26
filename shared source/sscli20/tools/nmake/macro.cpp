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
//  Macro.C - contains routines that have to do with macros
//
// Purpose:
//  Contains routines that have to do with macros

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif

static STRINGLIST **lastMacroChain = NULL;

// findMacro - look up a string in a hash table
//
//  Look up a macro name in a hash table and return the entry
//  or NULL.
//  If a macro and undefined, return NULL.

MACRODEF * findMacro(char *str)
{
    unsigned n;
    char *string = str;
    STRINGLIST *found;

    if (*string) {
        for (n = 0; *string; n += *string++);   	//Hash
        n %= MAXMACRO;
#if defined(STATISTICS)
        CntfindMacro++;
#endif
        lastMacroChain = (STRINGLIST **)&macroTable[n];
        for (found = *lastMacroChain; found; found = found->next) {
#if defined(STATISTICS)
            CntmacroChains++;
#endif
            if (!_tcscmp(found->text, str)) {
                return((((MACRODEF *)found)->flags & M_UNDEFINED) ? NULL : (MACRODEF *)found);
            }
        }
    } else {
        // set lastMacroChain, even for an empty name
        lastMacroChain = (STRINGLIST **)&macroTable[0];
    }
    return(NULL);
}

// insertMacro
//
// Macro insertion requires that we JUST did a findMacro, which action set lastMacroChain.

void insertMacro(STRINGLIST * p)
{
#ifdef STATISTICS
    CntinsertMacro++;
#endif
    assert(lastMacroChain != NULL);
    prependItem(lastMacroChain, p);
    lastMacroChain = NULL;
}

// Init the macro table to a known state before continuing.

void initMacroTable(MACRODEF *table[])
{
    unsigned num;
    for (num = 0; num < MAXMACRO; num++) {
        table[num] = NULL;
    }
}

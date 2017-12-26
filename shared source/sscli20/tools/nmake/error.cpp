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
//  ERROR.C -- error handling functions

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "verstamp.h"

#define FATAL       1               // error levels for
#define ERROR       2               // systems lanuguages
#define RESERVED    3               // products
#define WARNING     4

#define NmakeStr "NMAKE"


void __cdecl
makeError (
    unsigned lineNumber,
    unsigned msg,
    ...)
{
    unsigned exitCode = 2;          // general program err
    unsigned level;
    va_list args;                   // More arguments

    va_start(args, msg);            // Point 'args' at first extra arg

    if (ON(gFlags,F1_CRYPTIC_OUTPUT) && (msg / 1000) == WARNING) {
        return;
    }

    displayBanner();

    if (lineNumber) {
        fprintf(stderr, "%s(%u) : ", fName, lineNumber);
    } else {
        fprintf(stderr, "%s : ", NmakeStr);
    }

    switch (level = msg / 1000) {
        case FATAL:
            makeMessage(FATAL_ERROR_MESSAGE);
            if (msg == OUT_OF_MEMORY) {
                exitCode = 4;
            }
            break;

        case ERROR:
            makeMessage(ERROR_MESSAGE);
            break;

        case WARNING:
            makeMessage(WARNING_MESSAGE);
            break;
    }

    fprintf(stderr, " U%04d: ",msg);     // U for utilities
    vfprintf(stderr, get_err(msg), args);
    putc('\n', stderr);
    fflush(stderr);

    if (level == FATAL) {
        fprintf(stderr, "Stop.\n");
        delScriptFiles();

#if !defined(NDEBUG)
        printStats();
#endif
        exit(exitCode);
    }
}


void __cdecl
makeMessage(
    unsigned msg,
    ...)
{
    va_list args;
    FILE *stream = stdout;

    va_start(args, msg);

    if (msg != USER_MESSAGE && ON(gFlags, F1_CRYPTIC_OUTPUT)) {
        return;
    }

    displayBanner();

    if (msg >= FATAL_ERROR_MESSAGE && msg <= COPYRIGHT_MESSAGE_2) {
        stream = stderr;
    }

    if (msg == COPYRIGHT_MESSAGE_1) {
        putc('\n', stream);
    }

    vfprintf(stream, get_err(msg), args);

    if ((msg < COMMANDS_MESSAGE || msg > STOP_MESSAGE) && msg != MESG_LAST) {
        putc('\n', stream);
    }

    fflush(stream);
}


//  displayBanner - display SignOn Banner
//
// Scope:       Global
//
// Purpose:     Displays SignOn Banner (Version & Copyright Message)
//
// Assumes:     If rup is 0 then build version is to be suppressed
//
// Modifies Globals:
//  bannerDisplayed -- Set to TRUE
//
// Notes:
//  1> Display Banner to stderr for compatibility with Microsoft C Compiler.
//  2> rmj, rmm, rup are set by SLM as #define's in VERSION.H
//  3> szCopyrightYrs is a macro set in this file

void
displayBanner()
{
    if (bannerDisplayed) {
        return;
    }

    bannerDisplayed = TRUE;

    makeMessage(COPYRIGHT_MESSAGE_1, VER_PRODUCTVERSION_STR);
    makeMessage(COPYRIGHT_MESSAGE_2);

    fflush(stderr);
}

//  usage - prints the usage message
//
// Scope:   Extern
//
// Purpose: Prints a usage message
//
// Output:  to screen
//
// Assumes: The usage messages are in order between MESG_FIRST and MESG_LAST in the
// messages file.


void
usage(void)
{
    unsigned mesg;

    for (mesg = MESG_FIRST; mesg < MESG_A; ++mesg) {
        makeMessage(mesg, "NMAKE");
    }

    for (mesg = MESG_A; mesg <= MESG_LAST; mesg++) {
        if (mesg == MESG_M) {
            mesg++;
        }

        if (mesg == MESG_V) {
            mesg++;
        }
        makeMessage(mesg);
    }
}

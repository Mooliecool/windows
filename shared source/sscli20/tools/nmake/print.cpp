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
//  PRINT.C -- routines to display info for -p option
//
// Purpose:
//  Contains routines that print stuff for -p (and also -z, ifdef'ed)

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "nmtime.h"

// for formatting -p info
#define PAD1        40

size_t   checkLineLength(size_t i, char *s);
void     showDependents(STRINGLIST*, STRINGLIST*);

size_t
checkLineLength(
    size_t i,       // current length
    char *s         // string whose length is to be checked
    )
{
    if ((i += _tcslen(s)) > 40) {
        printf("\n\t\t\t");
        i = 0;
    }
    return(i);
}


void
printDate(
    unsigned spaces,        // spaces to print
    char *name,             // name of file whose date is to be printed
    time_t dateTime         // dateTime of file
    )
{
    if (dateTime == 0) {
        makeMessage(TARGET_DOESNT_EXIST, spaces, "", name);
    } else {
	char s[19]; // enough for 16-character time_t, plus "0x" and \0

	sprintf(s, "%p", (void *)dateTime);

        makeMessage(TIME_FORMAT, spaces, "", name, PAD1-spaces, s);
    }
}


void
showDependents(
    STRINGLIST *q,          // list of dependents
    STRINGLIST *macros      // macros in the dependents
    )
{
    char *u, *v;
    char *w;
    size_t i;
    struct _finddata_t finddata;
    NMHANDLE searchHandle;

    makeMessage(DEPENDENTS_MESSAGE);
    for (i = 0; q; q = q->next) {
        char *szFilename;

        if (_tcschr(q->text, '$')) {
            u = expandMacros(q->text, &macros);

            for (v = _tcstok(u, " \t"); v; v = _tcstok(NULL, " \t")) {
                if (_tcspbrk(v, "*?")) {
                    if ((szFilename = findFirst(v, &finddata, &searchHandle))) {
                        do {
                            w = prependPath(v, szFilename);
                            printf("%s ", w);
                            i = checkLineLength(i, w);
                            FREE(w);
                        }
                        while ((szFilename = findNext(&finddata, searchHandle)));
                    }
                } else {
                    printf("%s ", v);
                    i = checkLineLength(i, v);
                }
            }

            FREE(u);
        } else if (_tcspbrk(q->text, "*?")) {
            if ((szFilename = findFirst(q->text, &finddata, &searchHandle))) {
                do {
                    v = prependPath(q->text, szFilename);
                    printf("%s ", v);
                    i = checkLineLength(i, v);
                    FREE(v);
                }
                while ((szFilename = findNext(&finddata, searchHandle)));
            }
        } else {
            printf("%s ", q->text);
            i = checkLineLength(i, q->text);
        }
    }
}


void
showMacros(
    void
    )
{
    MACRODEF *p;
    STRINGLIST *q;
    int n = 0;

    makeMessage(MACROS_MESSAGE);

    for (n = 0; n < MAXMACRO; ++n) {
        for (p = macroTable[n]; p; p = p->next) {
            if (p->values && p->values->text) {
                makeMessage(MACRO_DEFINITION, p->name, p->values->text);
                for (q = p->values->next; q; q = q->next) {
                    if (q->text) {
                        printf("\t\t%s\n", q->text);
                    }
                }
            }
        }
    }

    putchar('\n');

    fflush(stdout);
}


void
showRules(
    void
    )
{
    RULELIST *p;
    STRINGLIST *q;
    unsigned n;

    makeMessage(INFERENCE_MESSAGE);

    for (p = rules, n = 1; p; p = p->next, ++n)  {
        printf(p->fBatch? "%s::" : "%s:", p->name);

        makeMessage(COMMANDS_MESSAGE);

        if ((q = p->buildCommands)) {
            printf("%s\n", q->text);

            while ((q = q->next)) {
                printf("\t\t\t%s\n", q->text);
            }
        }

	putchar('\n');
    }

    printf("%s: ", suffixes);

    for (q = dotSuffixList; q; q = q->next) {
        printf("%s ", q->text);
    }

    putchar('\n');

    fflush(stdout);
}


void
showTargets(
    void
    )
{
    unsigned bit, i;
    STRINGLIST *q;
    BUILDLIST  *s;
    BUILDBLOCK *r;
    MAKEOBJECT *t;
    unsigned n;
    LOCAL char *flagLetters = "dinsb";

    makeMessage(TARGETS_MESSAGE);
    for (n = 0; n < MAXTARGET; ++n) {
        for (t = targetTable[n]; t; t = t->next, putchar('\n')) {
            printf("%s:%c", t->name,
                   ON(t->buildList->buildBlock->flags, F2_DOUBLECOLON)
                       ? ':' : ' ');
            dollarStar = dollarAt = dollarDollarAt = t->name;
            for (s = t->buildList; s; s = s->next) {
                r = s->buildBlock;
                makeMessage(FLAGS_MESSAGE);
                for (i = 0, bit = F2_DISPLAY_FILE_DATES;
                     bit < F2_FORCE_BUILD;
                     ++i, bit <<= 1)
                     if (ON(r->flags, bit))
                        printf("-%c ", flagLetters[i]);
                showDependents(r->dependents, r->dependentMacros);
                makeMessage(COMMANDS_MESSAGE);
                if ((q = r->buildCommands)) {
                    if (q->text) printf("%s\n", q->text);
                    while ((q = q->next))
                        if (q->text) printf("\t\t\t%s\n", q->text);
                }
                else putchar('\n');
            }
        }
    }
    dollarStar = dollarAt = dollarDollarAt = NULL;
    putchar('\n');
    fflush(stdout);
}

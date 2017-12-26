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
//  UTILB.C -- Data structure manipulation functions specific to OS/2
//
// Purpose:
//  This file was created from functions in util.c & esetdrv.c which were system
//  dependent. This was done so that the build of the project became simpler and
//  there was a clear flow in the build process.
//
// Method of Creation:
//  1. Identified all functions having mixed mode code.
//  2. Deleted all code blocked out by '#ifndef BOUND' preprocessor directives
//     in these functions
//  3. Deleted all local function & their prototypes not referred by these
//  4. Deleted all global data unreferenced by these, including data blocked
//     of by '#ifdef DEBUG'

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif


STRINGLIST *
expandWildCards(
    char *s                             // text to expand
    )
{
    struct _finddata_t finddata;
    NMHANDLE searchHandle;
    STRINGLIST *xlist,                  // list of expanded names
               *p;
    char *namestr;

    if (!(namestr = findFirst(s, &finddata, &searchHandle))) {
        return(NULL);
    }

    xlist = makeNewStrListElement();
    xlist->text = prependPath(s, namestr);

    while ((namestr = findNext(&finddata, searchHandle))) {
        p = makeNewStrListElement();
        p->text = prependPath(s, namestr);
        prependItem(&xlist, p);
    }

    return(xlist);
}


//  QueryFileInfo -- it does a DosFindFirst which circumvents FAPI restrictions
//
// Scope:   Global (used by Build.c also)
//
// Purpose:
//  DosFindFirst() has a FAPI restriction in Real mode. You cannot ask it give
//  you a handle to a DTA structure other than the default handle. This function
//  calls C Library Function _dos_findfirst in real mode (which sets the DTA) and
//  does the job. In protect mode it asks OS/2 for a new handle.
//
// Input:
//  file -- the file to be searched for
//  dta  -- the struct containing result of the search
//
// Output:  Returns a pointer to the filename found (if any)
//
// Assumes: That dta points to a structure which has been allocated enough memory
//
// Uses Globals:
//  _osmode --  to determine whether in Real or Bound mode

char *
QueryFileInfo(
    char *file,
    void **dta
    )
{
    NMHANDLE  hDir;
    char *t;

    // Remove Quotes around filename, if existing
    t = file + _tcslen(file) - 1;
    if (*file == '"' && *t == '"') {
        file = unQuote(file);           // file is quoted, so remove quote
    }


    if ((hDir = _findfirst(file, (struct _finddata_t *) dta)) == -1) {
        return(NULL);
    }

    _findclose(hDir);

    return(((struct _finddata_t *) dta)->name);
}


//
// Truncate filename to system limits
//
void
truncateFilename(
    char * s
    )
{
    char szDrive[_MAX_DRIVE];
    char szDir[_MAX_DIR];
    char szName[_MAX_FNAME];
    char szExtension[_MAX_EXT];

    // pathname incorrectly truncated.  Solution: first parse it
    // using _splitpath(), then truncate the filename and extension part.
    // Finally reconstruct the pathname by calling _makepath().

    _splitpath(s, szDrive, szDir, szName, szExtension);
    _makepath(s, szDrive, szDir, szName, szExtension);
}


char *
findFirst(
    char *s,                            // text to expand
    void *dta,
    NMHANDLE *dirHandle
    )
{
    BOOL anyspecial;                   // flag set if s contains special characters.
    char buf[_MAX_PATH];               // buffer for removing ESCH

    // Check if name contains any special characters

    anyspecial = (_tcspbrk(s, "\"^*?") != NULL);

    if (anyspecial) {
        char *t;
        char *x;                       // Pointers for truncation, walking for ESCH

        t = s + _tcslen(s) - 1;

        // Copy pathname, skipping ESCHs and quotes
        x = buf;
        while( *s ) {
            if (*s == '^' || *s == '"') {
                s++;
            }
			else {
				if (_istlead(*(unsigned char *)s)) 
					*x++ = *s++;
            *x++ = *s++;
			}
        }

        *x = '\0';
        s = buf;                       // only elide ESCH the first time!
    }

    truncateFilename(s);

    if ((*dirHandle = _findfirst(s, (struct _finddata_t *) dta)) == -1) {
        return(NULL);
    }

    // If it had no wildcard then close the search handle

    if (!anyspecial || (!_tcschr(s, '*') && !_tcschr(s, '?'))) {
        _findclose(*dirHandle);
    }

    return(((struct _finddata_t *) dta)->name);
}

char *
findNext(
    void *dta,
    NMHANDLE dirHandle
    )
{
    if (_findnext(dirHandle, (struct _finddata_t *) dta)) {
        _findclose(dirHandle);

        return(NULL);
    }

    return(((struct _finddata_t *) dta)->name);
}


char *
getCurDir(void)
{
	// Convert $ to $$ before returning current dir
        // This allows $(MAKEDIR) to work properly in 
	// case the current path contains a $ sign.
	//
    char *pszPath;
    char pbPath[_MAX_DIR+1];
	char *pchSrc = pbPath;
	char *pchDst;
	char ch;

	GetCurrentDirectoryA(_MAX_DIR+1, pbPath);
	pszPath = (char *) rallocate(2 * _tcslen(pbPath) + 1);

	pchDst = pszPath;

	// non-MBCS aware implementation ('$' can't be a trailbyte)
	while ((ch = *pchSrc)) {
		*pchDst++ = *pchSrc++;
		if ('$' == ch)
			*pchDst++ = ch;
	}
	*pchDst = '\0';

    return(pszPath);
}


void
curTime(
    time_t *plTime
    )
{
    time(plTime);
}

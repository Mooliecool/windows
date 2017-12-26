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
//  NMAKE.C - main module
//
// Purpose:
//  This is the main module of nmake

#include "precomp.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "verstamp.h"
#include <palstartup.h>

int       readEnvironmentVars(void);
void      readMakeFiles(void);
void      useDefaultMakefile(void);
BOOL      filename(const char*, char**);
BOOL PALAPI chkPrecious(DWORD CtrlType);
UCHAR     isPrecious(char*);
      void      removeTrailChars(char *);

void usage (void);

char *makeStr;                         // this make invocation name

UCHAR  okToDelete;                     // do not del unless exec'ing cmd

#ifdef _M_IX86
UCHAR  fRunningUnderChicago;

extern UCHAR FIsChicago(void);
#endif

const char * const builtInTarg[] = {
    ".SUFFIXES",
    ".c.obj",
    ".c.exe",
    ".cpp.obj",
    ".cpp.exe",
    ".cxx.obj",
    ".cxx.exe",
#if defined(_M_IX86) || defined(_M_MRX000)
    ".asm.obj",
    ".asm.exe",
#endif
#if !defined(_M_IX86)
    ".s.obj",
#endif
    ".bas.obj",
    ".cbl.obj",
    ".cbl.exe",
    ".f.obj",
    ".f.exe",
    ".f90.obj",
    ".f90.exe",
    ".for.obj",
    ".for.exe",
    ".pas.obj",
    ".pas.exe",
    ".rc.res",
    NULL
};

const char * const bltInCmd0[] = {
    ":",
    ".exe",
    ".obj",
#if defined(_M_IX86) || defined(_M_MRX000)
    ".asm",
#endif
#if !defined(_M_IX86)
    ".s",
#endif
    ".c",
    ".cpp",
    ".cxx",
    ".bas",
    ".cbl",
    ".f",
    ".f90",
    ".for",
    ".pas",
    ".res",
    ".rc",
    NULL
};

// Single colon (":") specifies ordinary rules
// Double colon ("::") specifies batch rules
const char * const bltInCmd1[]  = {":", "$(CC) $(CFLAGS) /c $<", NULL};
const char * const bltInCmd2[]  = {":", "$(CC) $(CFLAGS) $<", NULL};
const char * const bltInCmd3[]  = {":", "$(CPP) $(CPPFLAGS) /c $<", NULL};
const char * const bltInCmd4[]  = {":", "$(CPP) $(CPPFLAGS) $<", NULL};
const char * const bltInCmd5[]  = {":", "$(CXX) $(CXXFLAGS) /c $<", NULL};
const char * const bltInCmd6[]  = {":", "$(CXX) $(CXXFLAGS) $<", NULL};
#if defined(_M_IX86) || defined(_M_MRX000)
const char * const bltInCmd7[]  = {":", "$(AS) $(AFLAGS) /c $*.asm", NULL};
const char * const bltInCmd8[]  = {":", "$(AS) $(AFLAGS) $*.asm", NULL};
#endif
#if !defined(_M_IX86)
const char * const bltInCmd9[]  = {":", "$(AS) $(AFLAGS) $*.s", NULL};
#endif
const char * const bltInCmd10[] = {":", "$(BC) $(BFLAGS) $*.bas;", NULL};
const char * const bltInCmd11[] = {":", "$(COBOL) $(COBFLAGS) $*.cbl;", NULL};
const char * const bltInCmd12[] = {":", "$(COBOL) $(COBFLAGS) $*.cbl, $*.exe;", NULL};
const char * const bltInCmd13[] = {":", "$(FOR) /c $(FFLAGS) $*.f", NULL};
const char * const bltInCmd14[] = {":", "$(FOR) $(FFLAGS) $*.f", NULL};
const char * const bltInCmd15[] = {":", "$(FOR) /c $(FFLAGS) $*.f90", NULL};
const char * const bltInCmd16[] = {":", "$(FOR) $(FFLAGS) $*.f90", NULL};
const char * const bltInCmd17[] = {":", "$(FOR) /c $(FFLAGS) $*.for", NULL};
const char * const bltInCmd18[] = {":", "$(FOR) $(FFLAGS) $*.for", NULL};
const char * const bltInCmd19[] = {":", "$(PASCAL) /c $(PFLAGS) $*.pas", NULL};
const char * const bltInCmd20[] = {":", "$(PASCAL) $(PFLAGS) $*.pas", NULL};
const char * const bltInCmd21[] = {":", "$(RC) $(RFLAGS) /r $*", NULL};

const char * const * const builtInCom[] = {
    bltInCmd0,
    bltInCmd1,
    bltInCmd2,
    bltInCmd3,
    bltInCmd4,
    bltInCmd5,
    bltInCmd6,
#if defined(_M_IX86) || defined(_M_MRX000)
    bltInCmd7,
    bltInCmd8,
#endif
#if !defined(_M_IX86)
    bltInCmd9,
#endif
    bltInCmd10,
    bltInCmd11,
    bltInCmd12,
    bltInCmd13,
    bltInCmd14,
    bltInCmd15,
    bltInCmd16,
    bltInCmd17,
    bltInCmd18,
    bltInCmd19,
    bltInCmd20,
    bltInCmd21,
    NULL
};

//  main
//
// actions:  saves the initial global variables in a
//           block. calls doMake() and then delTempScriptFiles()

int __cdecl
main(
    int argc,
    char **argv
    )
{
    int status;                         // returned by doMake

    _pgmptr = argv[0];

#ifdef _M_IX86
    fRunningUnderChicago = FIsChicago();
#endif

    initCharmap();

    initMacroTable(macroTable);


    if (!makeStr) {
        // extract file name
        if (!filename(_pgmptr, &makeStr)) {
            makeStr = "NMAKE";
        }
    }

    // set up handler for .PRECIOUS  the handler tries to remove the
    // current target when control-C'd, unless it is "precious"

    SetConsoleCtrlHandler(chkPrecious, TRUE);

    status = doMake(argc, argv, NULL);

    delScriptFiles();

    if (!fSlashKStatus) {
        status = 1;                     // error when slashK specified
    }

#if !defined(NDEBUG)
    printStats();
#endif
    return status;
}

extern void endNameList(void);
extern void addItemToList(void);
extern void assignDependents(void);
extern void assignBuildCommands(void);

//  loadBuiltInRules() -- Loads built in Rules to the NMAKE Tables
//
// Modifies:
//  fInheritUserEnv  --    is set to TRUE to inherit CC, AS
//
// Notes:
//  Does this by calls to defineMacro(), which calls putMacro(). Since,
//  fInheritUserEnv is set to TRUE, putMacro() will add to the Environment.

void
loadBuiltInRules(
    void
    )
{
    const char *tempTarg;
    const char * const *tempCom;
    unsigned index;
    char *macroName, *macroValue;

    // We dynamically allocate CC and AS because they need to be freed in a
    // recursive MAKE

    macroName = makeString("CC");
    macroValue = makeString("cl");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("CXX");
    macroValue = makeString("cl");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("CPP");
    macroValue = makeString("cl");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("AS");

    macroValue = makeString("ml");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("BC");
    macroValue = makeString("bc");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("COBOL");
    macroValue = makeString("cobol");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("FOR");
    macroValue = makeString("fl32");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("PASCAL");
    macroValue = makeString("pl");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("RC");
    macroValue = makeString("rc");
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("_NMAKE_VER");
    macroValue = makeString(VER_PRODUCTVERSION_STR);
    defineMacro(macroName, macroValue, 0);
    macroName = makeString("MAKE");
    macroValue = makeString(makeStr);
    // From environment so it won't get exported ; user can reset MAKE

    defineMacro(macroName, macroValue, M_ENVIRONMENT_DEF|M_WARN_IF_RESET);

    for (index = 0; (tempTarg = builtInTarg[index]); index++) {
        name = makeString(tempTarg);
        tempCom = builtInCom[index];
        // tempCom should now contain a single or double colon
        assert (tempCom && *tempCom && **tempCom == ':');
        _tcscpy(buf, *tempCom);
        endNameList();
        for (tempCom++; *tempCom; tempCom++) {
            _tcscpy(buf, *tempCom);
            addItemToList();
        }
        if (index == 0) {
            assignDependents();
        }
        assignBuildCommands();
    }
}


//  doMake()
//
// actions:  prints a version message
//           reads the environment variable MAKEFLAGS
//           if MAKEFLAGS defined
//           defines MAKEFLAGS to have that value w/in nmake
//           sets a flag for each option if MAKEFLAGS defined
//           else defines the macro MAKEFLAGS to be NULL
//           parses commandline (adding option letters to MAKEFLAGS)
//           reads all environment variables
//           reads tools.ini
//           reads makefile(s) (if -e flag set, new definitions in
//           makefile won't override environment variable defs)
//           prints information if -p flag
//           processes makefile(s)
//           prints information if -d flag and not -p flag (using both
//           is overkill)
//
// In effect, the order for making assignments is (1 = least binding,
//   4 = most binding):
//
//   1)  TOOLS.INI
//   2)  environment (if -e flag, makefile)
//   3)  makefile    (if -e flag, environment)
//   4)  command line
//
// The user can put anything he wants in the MAKEFLAGS environment variable.
// I don't check it for illegal flag values, because there are many xmake
// flags that we don't support.  He shouldn't have to change his MAKEFLAGS
// to use nmake. Xmake always puts 'b' in MAKEFLAGS for "backward com-
// patibility" (or "botch") for the original Murray Hill version of make.
// It doesn't make sense to use -f in MAKEFLAGS, thus it is disallowed.
// It also makes little sense to let the default flags be -r, -p, or -d,
// so they aren't allowed in MAKEFLAGS, either.
//
// Even though DOS only uses uppercase in environment variables, this
// program may be ported to xenix in the future, thus we allow for the
// possibility that MAKEFLAGS and commandline options will be in upper
// and/or lower case.
//
// modifies:   init    global flag set if tools.ini is being parsed...

int
doMake(
    unsigned argc,
    char *argv[],
    char *parentBlkPtr          // state of parent, restored prior to return
    )
{
    int status = 0;
    char *p;
    // extern char *makeStr;               // the initial make invok name
    char *makeDir, *curDir;

#ifdef DEBUG_ALL
    printf ("DEBUG: In doMake\n");
#endif

    assert(parentBlkPtr == NULL);

    // Load built-ins here rather than in main().  Otherwise in a recursive
    // make, doMake() will initialize rules to some value which has been
    // freed by sortRules().            

    inlineFileList = (STRINGLIST *)NULL;
    makeDir = makeString("MAKEDIR");
    curDir  = getCurDir();
    // Use M_LITERAL flag to prevent nmake from trying to
    // interpret $ in path as an embedded macro. [DS 14983]
    defineMacro(makeDir, curDir, M_LITERAL);

    // TEMPFIX: We are truncating MAKEFLAGS environment variable to its limit
    // to avoid GP Faults
    if ((p = getenv("MAKEFLAGS"))) {      // but not MAKEFLAGS
        _tcsncpy(makeflags+10, p, _tcslen(makeflags + 10));
    }

    // fInheritUserEnv is set to TRUE so that the changes made get inherited

    fInheritUserEnv = TRUE;

    // Simply adding global strings to the macro array
    // causes problems later when you go to free them
    // from a recursive $(MAKE).  Both the macro name
    // and the macro's value must be created with
    // makeString.

    defineMacro(makeString("MAKEFLAGS"), makeString(makeflags+10), M_NON_RESETTABLE|M_ENVIRONMENT_DEF);

    for (;p && *p; p++) {               // set flags in MAKEFLAGS
        setFlags(*p, TRUE);             // TRUE says turn bits ON
    }

    parseCommandLine(--argc, ++argv);   // skip over program name

#ifdef DEBUG_ALL
    printf ("DEBUG: Command Line parsed\n");
#endif

    if (!bannerDisplayed) {
        displayBanner();                // version number, etc.
    }

    if (OFF(gFlags, F1_IGNORE_EXTERN_RULES)) {  // read tools.ini
#ifdef DEBUG_ALL
        printf ("DEBUG: Read Tools.ini\n");
#endif
        loadBuiltInRules();
#ifdef DEBUG_ALL
        printf ("DEBUG: loadBuiltInRules\n");
#endif
        fName = "tools.ini";

        if (tagOpen("INIT", fName, makeStr)) {
            ++line;
            init = TRUE;                // tools.ini being parsed

#ifdef DEBUG_ALL
            printf ("DEBUG: Start Parse\n");
#endif
            parse();

#ifdef DEBUG_ALL
            printf ("DEBUG: Parsed\n");
#endif
            if (fclose(file) == EOF)
                makeError(0, ERROR_CLOSING_FILE, fName);
        }
    }

#ifdef DEBUG_ALL
    printf ("after tagopen\n");
#endif

    // For XMake Compatibility MAKEFLAGS should always be inherited to the Env
    // Put copy of makeflags so that the environment can be freed on return
    // from a recursive make

    if (PutEnv(makeString(makeflags)) == -1) {
        makeError(0, OUT_OF_ENV_SPACE);
    }

#ifdef DEBUG_ALL
    printf ("after putenv\n");
#endif

    if (!makeFiles) {
        useDefaultMakefile();           // if no -f makefile given
    }

    if (readEnvironmentVars() == -1) {
	makeError(0, OUT_OF_MEMORY);
    }
    readMakeFiles();                    // read description files

#ifdef DEBUG_ALL
    printf ("DEBUG: Read makefile\n");
#endif

    currentLine = 0;                    // reset line after done
    sortRules();                        // reading files (for error messages)

    if (ON(gFlags, F1_PRINT_INFORMATION)) {
        showMacros();
        showRules();
        showTargets();
    }

    // free buffer used for conditional processing - not required now
    if (lbufPtr) {
        FREE(lbufPtr);
    }

    status = processTree();

    // We ignore retval from chdir because we cannot do anything if it fails
    // This accomplishes a 'cd $(MAKEDIR)'.
    _chdir(curDir);
    return(status);
}


//  filename -- filename part of a name
//
// Scope:   Local
//
// Purpose:
//  A complete file name is of the form  <drive:><path><filename><.ext>. This
//  function returns the filename part of the name.
//
// Input:   src -- The complete file name
//          dst -- filename part of the complete file name
//
// Output:  Returns TRUE if src has a filename part & FALSE otherwise
//
// Assumes: That the file name could have either '/' or '\' as path separator.
//
// Notes:
//  Allocates memory for filename part. Function was rewritten to support OS/2
//  Ver 1.2 filenames.
//

BOOL
filename(
    const char *src,
    char **dst
    )
{
    char szFilename[_MAX_FNAME];        // The filename part

    // Split the full pathname to components
    _splitpath(src, NULL, NULL, szFilename, NULL);

    // Allocate & copy the filename part to the return string
    *dst = makeString(szFilename);

    // Finished
    return (BOOL) _tcslen(*dst);
}


// readMakeFiles()
//
// actions:  walks through the list calling parse on each makefile
//           resets the line number before parsing each file
//           removes name of parsed file from list
//           frees removed element's storage space
//
// modifies: file      global file pointer (FILE*)
//           fName     global pointer to file name (char*)
//           line      global line number used and updated by the lexer
//           init      global flag reset for parsing makefiles
//                      ( files other than tools.ini )
//           makeFiles in main() by modifying contents of local pointer (list)
//
// We keep from fragmenting memory by not allocating and then freeing space
// for the (probably few) names in the files and targets lists.  Instead
// we use the space already allocated for the argv[] vars, and use the space
// we alloc for the commandfile vars.  The commandfile vars that could be
// freed here, but they aren't because we can't tell them from the argv[]
// vars.  They will be freed at the end of the program.

void
readMakeFiles(
    void
    )
{
    STRINGLIST *q;

    for (q = makeFiles; q ; q = q->next) {          // for each name in list
        if ((q->text)[0] == '-' && !(q->text)[1]) {
            fName = makeString("STDIN");
            file = stdin;
        } else {
            fName = makeString(q->text);
            if (!(file = FILEOPEN(fName, "rt")))    // open to read, text mode
                makeError(0, CANT_OPEN_FILE, fName);
            if (!IsValidMakefile(file))
                makeError(0, CANT_SUPPORT_UNICODE, fName);
        }
        line = 0;
        init = FALSE;                   // not parsing tools.ini
        parse();
        if (file != stdin && fclose(file) == EOF)
            makeError(0, ERROR_CLOSING_FILE, fName);
    }

    // free the list of makefiles
    freeStringList(makeFiles);
}


//  readEnvironmentVars - Read in environment variables into Macro table
//
// Scope:   Local.
//
// Purpose:
//  Reads environment variables into the NMAKE macro Table. It walks through envp
//  using environ making entries in NMAKE's hash table of macros for each string
//  in the table.
//
// Assumes: That the env contains strings of the form "VAR=value" i.e. '=' present.
//
// Modifies Globals:    fInheritUserEnv - set to false.
//
// Uses Globals:
//  environ - Null terminated table of pointers to environment variable
//         definitions of the form "name=value" (Std C Runtime variable)
//
// Notes:
//  If the user specifies "set name=value" as a build command for a target being
//  built, the change in the environment will not be reflected in nmake's set of
//  defined variables in the macro table.

int
readEnvironmentVars(
    void
    )
{
    char *macro, *value;
    char *t;
    char *envPtr;
    char *envPtrBase;

    int cbMultiByte;
    int cchWideVar;
    int cchWide;
    WCHAR *tW;
    WCHAR *envW = GetEnvironmentStringsW();
    if (!envW) {
	return -1;
    }
    cchWide = 0;
    cbMultiByte = 1; // need at least the terminating NULL
    for (tW = envW; *tW; tW+=cchWideVar+1) {
	cchWideVar = wcslen(tW);
	cchWide += cchWideVar+1;
	cbMultiByte += cchWideVar*2+1;
    }
    envPtrBase = (char *)malloc(cbMultiByte);
    if (!envPtrBase) {
	FreeEnvironmentStringsW(envW);
	return -1;
    }
    if (WideCharToMultiByte(CP_ACP, 0,
                            envW, cchWide+1,
                            envPtrBase, cbMultiByte, 
			    NULL, NULL) == 0) {
	FreeEnvironmentStringsW(envW);
	free(envPtrBase);
	return -1;
    }
    envPtr = envPtrBase;

    for (envPtr = envPtrBase;*envPtr; envPtr+=strlen(envPtr)+1) {
        if ((t = _tcschr(envPtr, '='))) {   // should always be TRUE
            if (!_tcsnicmp(envPtr, "MAKEFLAGS", 8))
                continue;
            *t = '\0';
            // Don't add empty names.
            if (*envPtr == '\0')
                continue;
            // ALLOC: here we make copies of the macro name and value to define
            macro = _tcsupr(makeString(envPtr));

            value = makeString(t+1);
            *t = '=';
            fInheritUserEnv = (BOOL)FALSE;
            if (!defineMacro(macro, value, M_ENVIRONMENT_DEF)) {
                // ALLOC: here we free the copies if they were not added.
                FREE(macro);
                FREE(value);
            }
        }
    }
    FreeEnvironmentStringsW(envW);
    free(envPtrBase);
    return 0;
}


//  parseCommandLine()
//
// arguments:  argc    count of arguments in argv vector
//             argv    table of pointers to commandline arguments
//
// actions:    reads a command file if necessary
//             sets switches
//             defines macros
//             makes a list of makefiles to read
//             makes a list of targets to build
//
// modifies:   makeFiles   in main() by modifying contents of parameter
//                          pointer (list) to STRINGLIST pointer
//                          (makeFiles)
//             makeTargets     in main() by modifying contents of param
//                              pointer (targets) to STRINGLIST pointer
//             fInheritUserEnv set to TRUE so that user defined changes in the
//                              environment variables get inherited by the Env
//
// nmake doesn't make new copies of command line macro values or environment
// variables, but instead uses pointers to the space already allocated.
// This can cause problems if the envp, the environment pointer, is accessed
// elsewhere in the program (because the vector's strings will contain '\0'
// where they used to contain '=').  I don't foresee any need for envp[] to
// be used elsewhere.  Even if we did need to use the environment, we could
// access the environ variable or use getenv().
//
// I don't care what the current DOS "switch" character is -- I always
// let the user give either.

void
parseCommandLine(
    unsigned argc,
    char *argv[]
    )
{
    STRINGLIST *p;
    char *s;
    char *t;
    BOOL fUsage = FALSE;

    for (; argc; --argc, ++argv) {
        if (**argv == '@') {           // cmdfile
            readCommandFile((char *) *argv+1);
            // On Unix, '/' can be used when specifying a fully qualified path
        } else if (**argv == '-') {   // switch
            s = *argv + 1;
            if (!_tcsicmp(s, "help")) {
                fUsage = TRUE;
                break;
            }

            // if '-' and '/' specified then ignores it
            for (; *s; ++s) {
                if (!_tcsicmp(s, "nologo")) {
                    setFlags(s[2], TRUE);
                    break;
                } else if (*s == '?') {
                    fUsage = TRUE;
                    break;
                } else if (*s == 'f' || *s == 'F') {
                    char *mkfl = s+1;

                    //if '/ffoo' then use 'foo'; else use next argument
                    if (!*mkfl && (!--argc || !*++argv || !*(mkfl = *argv))) {
                        makeError(0, CMDLINE_F_NO_FILENAME);
                    }
                    p = makeNewStrListElement();
                    p->text = makeString(mkfl);
                    appendItem(&makeFiles, p);
                    break;
                } else {
                    setFlags(*s, TRUE);
                }
            }
        } else {
            if ((s = _tcschr(*argv, '='))) {         // macro
                if (s == *argv) {
                    makeError(0, CMDLINE_NO_MACRONAME);    // User has specified "=value"
                }
                *s = '\0';
                for (t = s++ - 1; WHITESPACE(*t); --t)
                    ;
                *(t+1) = '\0';
                fInheritUserEnv = (BOOL)TRUE;
                defineMacro(makeString(*argv+_tcsspn(*argv, " \t")),
                makeString( s+_tcsspn(s," \t")),
                M_NON_RESETTABLE);
            } else {
                removeTrailChars(*argv);
                if (**argv) {
                    p = makeNewStrListElement();    // target
                    // use quotes around name if it contains spaces
                    if (_tcschr(*argv, ' ')) {
                        p->text = makeQuotedString(*argv);
                    }
                    else {
                        p->text = makeString(*argv);    // needs to be on heap [rm]
                    }
                    appendItem(&makeTargets, p);
                }
            }
            *argv = NULL;               // so we won't try to free this space
        }                               //  if processing command file stuff
    }

    if (fUsage) {
        usage();
        exit(0);
    }
}


//  useDefaultMakefile -- tries to use the default makefile
//
// Scope:
//  Local
//
// Purpose:
//  When no makefile has been specified by the user, set up the default makefile
//  to be used.
//
// Input:
// Output:
// Errors/Warnings:
//  CMDLINE_NO_MAKEFILE -- 'makefile' does not exist & no target specified
//
// Assumes:
// Modifies Globals:
//  makeTargets -- if 'makefile' does not exist then the first target is removed
//                   from this list,
//  makeFiles -- if 'makefile' does not exist then the first target is attached
//                   to this list.
//
// Uses Globals:
//  makeTargets -- the list of targets to be made
//
// Notes:
//  Given a commandline not containing a '-f makefile', this is how NMAKE
//  behaves --
//      If ['makefile' exists] then use it as the makefile,
//      if [(the first target exists and has no extension) or
//       (if it exists and has an extension for which no inference rule
//        exists)]
//      then use it as the makefile.

void
useDefaultMakefile(
    void
    )
{
    STRINGLIST *p;
    char *s, *ext;
    char nameBuf[MAXNAME];
    struct _finddata_t finddata;

    if (!_access("makefile", READ)) {
        // if 'makefile' exists then use it
        p = makeNewStrListElement();
        p->text = makeString("makefile");
        makeFiles = p;
    } else if (makeTargets) {
        //check first target
        s = makeTargets->text;
        if (_access(s, READ) ||         // 1st target does not exist
              ((ext = _tcsrchr(s, '.'))
            && findRule(nameBuf, s, ext, &finddata))) {  //has no ext or inf rule
            return;
        }

        p = makeTargets;
        makeTargets = makeTargets->next;    // one less target
        makeFiles = p;                      // 1st target is the makefile
    } else if (OFF(gFlags, F1_PRINT_INFORMATION)) {
        //if -p and no makefile, simply give information ...
        makeError(0, CMDLINE_NO_MAKEFILE);  //  no 'makefile' or target
    }
}


//  setFlags()
//
// arguments:  line    current line number in makefile (or 0
//                      if still parsing commandline)
//             c       letter presumed to be a commandline option
//             value   TRUE if flag should be turned on, FALSE for off
//
// actions:    checks to see if c is a valid option-letter
//             if no, error, halt
//             if value is TRUE, sets corresponding flag bit
//               and adds flag letter to MAKEFLAGS macro def
//             else if flag is resettable, clears corresponding bit
//               and removes letter from MAKEFLAGS macro def
//
// modifies:   flags       external resettable-flags
//             gFlags      external non-resettable flags
//             (MAKEFLAGS  nmake internal macrodefs)
//
// Only the flags w/in the "flags" variable can be turned off.  Once the
// bits in "gFlags" are set, they remain unchanged.  The bits in "flags"
// are modified via the !CMDSWITCHES directive.

void
setFlags(
    char c,
    BOOL value
    )
{
    // Use lexer's line count.  If this gets called w/in mkfil, might be from
    // directive, which never makes it to the parser, so parser's line count
    // might be out of sync.

    char d = c;
    UCHAR arg=0;
    UCHAR *f;
    char *s;
    extern MACRODEF * pMacros;
    extern STRINGLIST * pValues;

    f = &flags;
    switch(c = (char) _totupper(c)) {
        case 'A':
            arg = F2_FORCE_BUILD;
            break;

        case 'B':
            fRebuildOnTie = TRUE;
            return;

        case 'C':
            arg = F1_CRYPTIC_OUTPUT;
            f = &gFlags;
            bannerDisplayed = TRUE;
            break;

        case 'D':
            arg = F2_DISPLAY_FILE_DATES;
            break;

        case 'E':
            arg = F1_USE_ENVIRON_VARS;
            f = &gFlags;
            break;

        case 'I':
            arg = F2_IGNORE_EXIT_CODES;
            break;

        case 'K':
            fOptionK = TRUE;
            return;

        case 'L':
            arg = F1_NO_LOGO;
            f = &gFlags;
            bannerDisplayed = TRUE;
            break;

        case 'N':
            arg = F2_NO_EXECUTE;
            break;

        case 'O':
            fDescRebuildOrder = TRUE;
            return;

        case 'P':
            arg = F1_PRINT_INFORMATION;
            f = &gFlags;
            break;

        case 'Q':
            arg = F1_QUESTION_STATUS;
            f = &gFlags;
            break;

        case 'R':
            arg = F1_IGNORE_EXTERN_RULES;
            f = &gFlags;
            break;

        case 'S':
            arg = F2_NO_ECHO;
            break;

        case 'T':
            arg = F1_TOUCH_TARGETS;
            f = &gFlags;
            break;

        case 'U':
            arg = F2_DUMP_INLINE;
            break;

        case 'Y':
            arg = F1_NO_BATCH;
            f = &gFlags;
            break;

        case ' ':
            return;                     // recursive make problem

        default:
            makeError(0, CMDLINE_BAD_OPTION, d);
    }

    if (!pMacros) {
        pMacros = findMacro("MAKEFLAGS");
        pValues = pMacros->values;
    }

    if (value) {
        SET(*f, arg);                   // set bit in flags variable
        if (c == 'Q') SET(*f, F1_CRYPTIC_OUTPUT);
            if (!_tcschr(pValues->text, c)) {          // don't want to dup any chars
                if ((s = _tcschr(pValues->text, ' '))) // append ch to MAKEFLAGS
                    *s = c;
            if (PutEnv(makeString(makeflags)) == -1)    // pValues->text pts into makeflags
                makeError(line, OUT_OF_ENV_SPACE);
        }
    } else if (f == &flags
        ) {
        // make sure pointer is valid (we can't change gFlags, except if /Z
        CLEAR(*f, arg);
        if ((s = _tcschr(pValues->text, c))) // adjust MAKEFLAGS
            do {
                *s = *(s+1);                //  move remaining chars over
            } while (*(++s));
        if (PutEnv(makeString(makeflags)) == -1)
            makeError(line, OUT_OF_ENV_SPACE);
    }
}

//  chkPrecious -- handle ^c or ^Break, etc.
//
// Actions:    unlink all non-precious files and unrequired scriptFiles
//             quit with error message (makeError unlinks temp. files)

BOOL PALAPI 
chkPrecious(
	    DWORD CtrlType
	    )
{
    if (okToDelete &&
        OFF(flags, F2_NO_EXECUTE) &&
        OFF(gFlags, F1_TOUCH_TARGETS) &&
        dollarAt &&
        _access(dollarAt, 0x00) &&      // existence check
        !isPrecious(dollarAt)
       ) {
        if (DeleteFileA(dollarAt) == FALSE)
            makeError(line, REMOVED_TARGET, dollarAt);
    }
    makeError(0, USER_INTERRUPT);
    delScriptFiles();

    return TRUE;
}


UCHAR
isPrecious(
    char *p
    )
{
    STRINGLIST *temp;

    for (temp = dotPreciousList; temp; temp = temp->next)
        if (!_tcsicmp(temp->text, p))
            return(1);
    return(0);
}

//  delScriptFiles -- deletes script files
//
// Scope:   Global
//
// Purpose:
//  Since script files may be reused in the makefile the script files which have
//  NOKEEP action specified are deleted at the end of the make.
//
// Uses Globals:    delList -- the list of script files to be deleted
//
// Notes:
//  We ignore the exit code as a result of a delete because the system will
//  inform the user that a delete failed.

void
delScriptFiles(
    void
    )
{
    STRINGLIST *del;


    for (del = delList; del;del = del->next) {
        _unlink(del->text);
        if (ON(flags, F2_NO_EXECUTE)) {
            printf("\tdel %s\n", del->text);
            fflush(stdout);
        }
    }
}


//  removeTrailChars - removes trailing blanks and dots
//
// Scope:   Local.
//
// Purpose:
//  OS/2 1.2 filenames dictate removal of trailing blanks and periods. This
//  function removes them from filenames provided to it.
//
// Input:   szFile - name of file
//
// Notes:
//  This function handles Quoted filenames as well. It maintains the quotes if
//  they were present. This is basically for OS/2 1.2 filename support.

void
removeTrailChars(
    char *szFile
    )
{
    char *t = szFile + _tcslen(szFile) - 1;
    BOOL fQuoted = FALSE;

    if (*szFile == '"' && *t == '"') {
        // Quoted so set flag
        t--;
        fQuoted = TRUE;
    }

    // Scan backwards for trailing characters
    while (t > szFile && (*t == ' ' || *t == '.'))
        t--;

    // t points to last non-trailing character.  It it was quited, add quotes
    // to the end
    if (fQuoted)
        *++t = '"';

    t[1] = '\0';
}

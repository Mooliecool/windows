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
//  PROTO.H -- function prototypes
//
// Purpose:
//  This include file contains global function prototypes for all modules.

void        displayBanner(void);
void __cdecl makeError(unsigned, unsigned, ...);
void __cdecl makeMessage(unsigned, ...);
UCHAR       getToken(unsigned, UCHAR);
int         skipWhiteSpace(UCHAR);
int         skipBackSlash(int, UCHAR);
void        parse(void);
void        appendItem(STRINGLIST **, STRINGLIST *);
void        prependItem(STRINGLIST **, STRINGLIST *);
STRINGLIST * removeFirstString(STRINGLIST **);
void      * allocate(size_t);
void      * alloc_stringlist(void);
void      * rallocate(size_t);
char      * makeString(const char *);
char	  * makeQuotedString(const char *);
char      * reallocString(char * pszTarget, const char *szAppend);
BOOL        tagOpen(char *, char *, char *);
void        parseCommandLine(unsigned, char **);
void        getRestOfLine(char **, size_t *);
BOOL        defineMacro(char *, char *, UCHAR);
STRINGLIST * find(char *, unsigned, STRINGLIST **, BOOL);
MACRODEF *  findMacro(char *);
void        insertMacro(STRINGLIST *);
unsigned    hash(char *, unsigned, BOOL);
void        prependList(STRINGLIST **, STRINGLIST **);
BOOL        findMacroValues(char *, STRINGLIST **, STRINGLIST **, char *, unsigned, unsigned, UCHAR);
BOOL        findMacroValuesInRule(RULELIST *, char *, STRINGLIST **);
char      * removeMacros(char *);
void        delScriptFiles(void);
char      * expandMacros(char *, STRINGLIST **);
STRINGLIST * expandWildCards(char *);
void        readCommandFile(char *);
void        setFlags(char, BOOL);
void        showTargets(void);
void        showRules(void);
void        showMacros(void);
char      * findFirst(char*, void *, NMHANDLE*);
char      * findNext(void *, NMHANDLE);

int         processTree(void);
void        expandFileNames(char *, STRINGLIST **, STRINGLIST **);
void        sortRules(void);
BOOL        isRule(char *);
char      * prependPath(const char *, const char *);
char      * searchPath(char *, char *, void *, NMHANDLE*);
BOOL        putMacro(char *, char *, UCHAR);
int         execLine(char *, BOOL, BOOL, BOOL, char **);
RULELIST  * findRule(char *, char *, char *, void *);
int         lgetc(void);
UCHAR       processIncludeFile(char *);
BOOL        evalExpr(char *, UCHAR);
int         doMake(unsigned, char **, char *);
void        freeList(STRINGLIST *);
void        freeStringList(STRINGLIST *);
#ifdef _MBCS
int         GetTxtChr(FILE*);
int         UngetTxtChr (int, FILE *);
#endif
int         putEnvStr(char *, char *);
#define PutEnv(x) _putenv(x)
void        expandExtmake(char *, char *, char*);
BOOL		ZFormat(char *, unsigned, char *, char *);
void        printReverseFile(void);
void        freeRules(RULELIST *, BOOL);
char      * getFileName(void *);
time_t      getDateTime(const _finddata_t *);
void        putDateTime(_finddata_t *, time_t);
char      * getCurDir(void);

void        free_memory(void *);
void        free_stringlist(STRINGLIST *);
void      * realloc_memory(void *, unsigned);

FILE      * open_file(char *, char *);
void        initMacroTable(MACRODEF *table[]);
void        TruncateString(char *, unsigned);
BOOL        IsValidMakefile(FILE *fp);
FILE      * OpenValidateMakefile(char *name,char *mode);

// from util.c
char      * unQuote(char*);
int         strcmpiquote(char *, char*);
char     ** copyEnviron(char **environ);
void        printStats(void);
void        curTime(time_t *);

// from charmap.c
void        initCharmap(void);

// from print.c
void        printDate(unsigned, char*, time_t);

// from build.c
int         invokeBuild(char*, UCHAR, time_t *, char *);
void        DumpList(STRINGLIST *pList);

// from exec.c
extern int  doCommands(char*, STRINGLIST*, STRINGLIST*, UCHAR, char *);
extern int  doCommandsEx(STRINGLIST*, STRINGLIST*, STRINGLIST*, UCHAR, char *);

// from rule.c
extern RULELIST * useRule(MAKEOBJECT*, char*, time_t,
              STRINGLIST**, STRINGLIST**, int*, time_t *,
              char **);

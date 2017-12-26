/*
 * Created by Microsoft VCBU Internal YACC
 */

#line 2 "asmparse.y"

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

#include <asmparse.h>
#include <crtdbg.h>             // FOR ASSERTE
#include <string.h>             // for strcmp    
#include <mbstring.h>           // for _mbsinc    
#include <ctype.h>                      // for isspace    
#include <stdlib.h>             // for strtoul
#include "openum.h"             // for CEE_*
#include <stdarg.h>         // for vararg macros 

// Disable the "initialization of static local vars is no thread safe" error
#ifdef _MSC_VER
#ifdef _MSC_VER
#pragma warning(disable : 4640)
#endif
#endif

#define YYMAXDEPTH 0x80000
//#define YYRECURSIVE

//#define DEBUG_PARSING
#ifdef DEBUG_PARSING
bool parseDBFlag = true;
#define dbprintf(x)     if (parseDBFlag) printf x
#define YYDEBUG 1
#else
#define dbprintf(x)     
#endif

#define FAIL_UNLESS(cond, msg) if (!(cond)) { parser->success = false; parser->error msg; }

static AsmParse* parser = 0;
#define PASM    (parser->assem)
#define PASMM   (parser->assem->m_pManifest)
#define PENV    (parser->penv)



PFN_NEXTCHAR    nextchar;
PFN_SYM         Sym;
PFN_NEWSTRFROMTOKEN NewStrFromToken;
PFN_NEWSTATICSTRFROMTOKEN NewStaticStrFromToken;
PFN_GETDOUBLE   GetDouble;

void SetFunctionPtrs()
{
    nextchar = PENV->pfn_nextchar;
    Sym = PENV->pfn_Sym;
    NewStrFromToken = PENV->pfn_NewStrFromToken;
    NewStaticStrFromToken = PENV->pfn_NewStaticStrFromToken;
    GetDouble = PENV->pfn_GetDouble;
}    
    

static char* newStringWDel(__in __nullterminated char* str1, char delimiter, char* str3 = 0);
static char* newString(__in __nullterminated char* str1);
static void corEmitInt(BinStr* buff, unsigned data);
static void AppendStringWithLength(BinStr* pbs, __in __nullterminated char* sz);
bool bParsingByteArray = FALSE;
int iOpcodeLen = 0;
int iCallConv = 0;
unsigned IfEndif = 0;
unsigned IfEndifSkip = 0;
unsigned nCustomBlobNVPairs = 0;
unsigned nSecAttrBlobs = 0;
unsigned  nCurrPC = 0;
BOOL SkipToken = FALSE;
BOOL neg = FALSE;
BOOL newclass = FALSE;

extern unsigned int g_uConsoleCP;

struct VarName
{
    char* name;
    BinStr* pbody;
    VarName(__in_opt __nullterminated char* sz, BinStr* pbs) { name = sz; pbody = pbs; };
    ~VarName() { delete [] name; delete pbody; };
    int ComparedTo(VarName* pN) { return strcmp(name,pN->name); };
};
SORTEDARRAY<VarName> VarNameList;
void DefineVar(__in __nullterminated char* sz, BinStr* pbs) { VarNameList.PUSH(new VarName(sz,pbs)); };
void UndefVar(__in __nullterminated char* sz) 
{ 
    CHECK_LOCAL_STATIC_VAR(static VarName VN(NULL,NULL)); 

    VN.name = sz; 
    VarNameList.DEL(&VN); 
    VN.name = NULL; 
    delete [] sz;
}
VarName* FindVarDef(__in __nullterminated char* sz)
{
    CHECK_LOCAL_STATIC_VAR(static VarName VN(NULL,NULL));

    VarName* Ret = NULL;
    VN.name = sz;
    Ret = VarNameList.FIND(&VN);
    VN.name = NULL; 
    delete [] sz;
    return Ret;
}
BOOL IsVarDefined(__in __nullterminated char* sz)
{
    return (FindVarDef(sz) != NULL);
}

int  nTemp=0;

unsigned int uMethodBeginLine,uMethodBeginColumn;

#define ELEMENT_TYPE_VARFIXUP ELEMENT_TYPE_MAX+2
#define ELEMENT_TYPE_MVARFIXUP ELEMENT_TYPE_MAX+3

FIFO<char> TyParFixupList;
void FixupTyPars(PCOR_SIGNATURE pSig, ULONG cSig);
void FixupTyPars(BinStr* pbstype);
void FixupConstraints()
{
    if((TyParFixupList.COUNT()) && (PASM->m_TyParList))
    {
        TyParList* pTPL;
        for(pTPL = PASM->m_TyParList; pTPL; pTPL=pTPL->Next())
        {
            mdToken* ptk;
            for(ptk = (mdToken*)(pTPL->Bound()->ptr()); *ptk; ptk++)
            {
                if(TypeFromToken(*ptk)==mdtTypeSpec)
                {
                    PCOR_SIGNATURE pSig;
                    ULONG cSig;
                    PASM->m_pImporter->GetTypeSpecFromToken(*ptk,(PCCOR_SIGNATURE*)&pSig,&cSig);
                    if((pSig)&&(cSig))
                    {
                        FixupTyPars(pSig,cSig);    
                    } // end if((pSig)&&(cSig))
                } // end if(TypeFromToken(*ptk)==mdtTypeSpec)
            } //end for(ptk    
        } // end for(pTPL
    } //end if((TyParFixupList.COUNT()) 
}

#define SET_PA(x,y,z) {x = (CorAssemblyFlags)(((y) & ~afPA_FullMask)|(z)|afPA_Specified);}


#line 164 "asmparse.y"

#define UNION 1
typedef union  {
        CorRegTypeAttr classAttr;
        CorMethodAttr methAttr;
        CorFieldAttr fieldAttr;
        CorMethodImpl implAttr;
        CorEventAttr  eventAttr;
        CorPropertyAttr propAttr;
        CorPinvokeMap pinvAttr;
        CorDeclSecurity secAct;
        CorFileFlags fileAttr;
        CorAssemblyFlags asmAttr;
        CorAssemblyFlags asmRefAttr;
        CorTypeAttr exptAttr;
        CorManifestResourceFlags manresAttr;
        double*  float64;
        __int64* int64;
        __int32  int32;
        char*    string;
        BinStr*  binstr;
        Labels*  labels;
        Instr*   instr;         // instruction opcode
        NVPair*  pair;
        pTyParList typarlist;
        mdToken token;
        TypeDefDescr* tdd;
        CustomDescr*  cad;
        unsigned short opcode;
} YYSTYPE;
# define ERROR_ 257 
# define BAD_COMMENT_ 258 
# define BAD_LITERAL_ 259 
# define ID 260 
# define DOTTEDNAME 261 
# define QSTRING 262 
# define SQSTRING 263 
# define INT32 264 
# define INT64 265 
# define FLOAT64 266 
# define HEXBYTE 267 
# define TYPEDEF_T 268 
# define TYPEDEF_M 269 
# define TYPEDEF_F 270 
# define TYPEDEF_TS 271 
# define TYPEDEF_MR 272 
# define TYPEDEF_CA 273 
# define DCOLON 274 
# define ELIPSIS 275 
# define VOID_ 276 
# define BOOL_ 277 
# define CHAR_ 278 
# define UNSIGNED_ 279 
# define INT_ 280 
# define INT8_ 281 
# define INT16_ 282 
# define INT32_ 283 
# define INT64_ 284 
# define FLOAT_ 285 
# define FLOAT32_ 286 
# define FLOAT64_ 287 
# define BYTEARRAY_ 288 
# define UINT_ 289 
# define UINT8_ 290 
# define UINT16_ 291 
# define UINT32_ 292 
# define UINT64_ 293 
# define FLAGS_ 294 
# define CALLCONV_ 295 
# define MDTOKEN_ 296 
# define OBJECT_ 297 
# define STRING_ 298 
# define NULLREF_ 299 
# define DEFAULT_ 300 
# define CDECL_ 301 
# define VARARG_ 302 
# define STDCALL_ 303 
# define THISCALL_ 304 
# define FASTCALL_ 305 
# define CLASS_ 306 
# define TYPEDREF_ 307 
# define UNMANAGED_ 308 
# define FINALLY_ 309 
# define HANDLER_ 310 
# define CATCH_ 311 
# define FILTER_ 312 
# define FAULT_ 313 
# define EXTENDS_ 314 
# define IMPLEMENTS_ 315 
# define TO_ 316 
# define AT_ 317 
# define TLS_ 318 
# define TRUE_ 319 
# define FALSE_ 320 
# define VALUE_ 321 
# define VALUETYPE_ 322 
# define NATIVE_ 323 
# define INSTANCE_ 324 
# define SPECIALNAME_ 325 
# define FORWARDER_ 326 
# define STATIC_ 327 
# define PUBLIC_ 328 
# define PRIVATE_ 329 
# define FAMILY_ 330 
# define FINAL_ 331 
# define SYNCHRONIZED_ 332 
# define INTERFACE_ 333 
# define SEALED_ 334 
# define NESTED_ 335 
# define ABSTRACT_ 336 
# define AUTO_ 337 
# define SEQUENTIAL_ 338 
# define EXPLICIT_ 339 
# define ANSI_ 340 
# define UNICODE_ 341 
# define AUTOCHAR_ 342 
# define IMPORT_ 343 
# define ENUM_ 344 
# define VIRTUAL_ 345 
# define NOINLINING_ 346 
# define UNMANAGEDEXP_ 347 
# define BEFOREFIELDINIT_ 348 
# define STRICT_ 349 
# define RETARGETABLE_ 350 
# define METHOD_ 351 
# define FIELD_ 352 
# define PINNED_ 353 
# define MODREQ_ 354 
# define MODOPT_ 355 
# define SERIALIZABLE_ 356 
# define PROPERTY_ 357 
# define TYPE_ 358 
# define ASSEMBLY_ 359 
# define FAMANDASSEM_ 360 
# define FAMORASSEM_ 361 
# define PRIVATESCOPE_ 362 
# define HIDEBYSIG_ 363 
# define NEWSLOT_ 364 
# define RTSPECIALNAME_ 365 
# define PINVOKEIMPL_ 366 
# define _CTOR 367 
# define _CCTOR 368 
# define LITERAL_ 369 
# define NOTSERIALIZED_ 370 
# define INITONLY_ 371 
# define REQSECOBJ_ 372 
# define CIL_ 373 
# define OPTIL_ 374 
# define MANAGED_ 375 
# define FORWARDREF_ 376 
# define PRESERVESIG_ 377 
# define RUNTIME_ 378 
# define INTERNALCALL_ 379 
# define _IMPORT 380 
# define NOMANGLE_ 381 
# define LASTERR_ 382 
# define WINAPI_ 383 
# define AS_ 384 
# define BESTFIT_ 385 
# define ON_ 386 
# define OFF_ 387 
# define CHARMAPERROR_ 388 
# define INSTR_NONE 389 
# define INSTR_VAR 390 
# define INSTR_I 391 
# define INSTR_I8 392 
# define INSTR_R 393 
# define INSTR_BRTARGET 394 
# define INSTR_METHOD 395 
# define INSTR_FIELD 396 
# define INSTR_TYPE 397 
# define INSTR_STRING 398 
# define INSTR_SIG 399 
# define INSTR_TOK 400 
# define INSTR_SWITCH 401 
# define _CLASS 402 
# define _NAMESPACE 403 
# define _METHOD 404 
# define _FIELD 405 
# define _DATA 406 
# define _THIS 407 
# define _BASE 408 
# define _NESTER 409 
# define _EMITBYTE 410 
# define _TRY 411 
# define _MAXSTACK 412 
# define _LOCALS 413 
# define _ENTRYPOINT 414 
# define _ZEROINIT 415 
# define _EVENT 416 
# define _ADDON 417 
# define _REMOVEON 418 
# define _FIRE 419 
# define _OTHER 420 
# define _PROPERTY 421 
# define _SET 422 
# define _GET 423 
# define _PERMISSION 424 
# define _PERMISSIONSET 425 
# define REQUEST_ 426 
# define DEMAND_ 427 
# define ASSERT_ 428 
# define DENY_ 429 
# define PERMITONLY_ 430 
# define LINKCHECK_ 431 
# define INHERITCHECK_ 432 
# define REQMIN_ 433 
# define REQOPT_ 434 
# define REQREFUSE_ 435 
# define PREJITGRANT_ 436 
# define PREJITDENY_ 437 
# define NONCASDEMAND_ 438 
# define NONCASLINKDEMAND_ 439 
# define NONCASINHERITANCE_ 440 
# define _LINE 441 
# define P_LINE 442 
# define _LANGUAGE 443 
# define _CUSTOM 444 
# define INIT_ 445 
# define _SIZE 446 
# define _PACK 447 
# define _VTABLE 448 
# define _VTFIXUP 449 
# define FROMUNMANAGED_ 450 
# define CALLMOSTDERIVED_ 451 
# define _VTENTRY 452 
# define RETAINAPPDOMAIN_ 453 
# define _FILE 454 
# define NOMETADATA_ 455 
# define _HASH 456 
# define _ASSEMBLY 457 
# define _PUBLICKEY 458 
# define _PUBLICKEYTOKEN 459 
# define ALGORITHM_ 460 
# define _VER 461 
# define _LOCALE 462 
# define EXTERN_ 463 
# define _MRESOURCE 464 
# define _MODULE 465 
# define _EXPORT 466 
# define LEGACY_ 467 
# define LIBRARY_ 468 
# define X86_ 469 
# define IA64_ 470 
# define AMD64_ 471 
# define MARSHAL_ 472 
# define CUSTOM_ 473 
# define SYSSTRING_ 474 
# define FIXED_ 475 
# define VARIANT_ 476 
# define CURRENCY_ 477 
# define SYSCHAR_ 478 
# define DECIMAL_ 479 
# define DATE_ 480 
# define BSTR_ 481 
# define TBSTR_ 482 
# define LPSTR_ 483 
# define LPWSTR_ 484 
# define LPTSTR_ 485 
# define OBJECTREF_ 486 
# define IUNKNOWN_ 487 
# define IDISPATCH_ 488 
# define STRUCT_ 489 
# define SAFEARRAY_ 490 
# define BYVALSTR_ 491 
# define LPVOID_ 492 
# define ANY_ 493 
# define ARRAY_ 494 
# define LPSTRUCT_ 495 
# define IIDPARAM_ 496 
# define IN_ 497 
# define OUT_ 498 
# define OPT_ 499 
# define PARAM_ 500 
# define _OVERRIDE 501 
# define WITH_ 502 
# define NULL_ 503 
# define HRESULT_ 504 
# define CARRAY_ 505 
# define USERDEFINED_ 506 
# define RECORD_ 507 
# define FILETIME_ 508 
# define BLOB_ 509 
# define STREAM_ 510 
# define STORAGE_ 511 
# define STREAMED_OBJECT_ 512 
# define STORED_OBJECT_ 513 
# define BLOB_OBJECT_ 514 
# define CF_ 515 
# define CLSID_ 516 
# define VECTOR_ 517 
# define _SUBSYSTEM 518 
# define _CORFLAGS 519 
# define ALIGNMENT_ 520 
# define _IMAGEBASE 521 
# define _STACKRESERVE 522 
# define _TYPEDEF 523 
# define _TEMPLATE 524 
# define _TYPELIST 525 
# define _MSCORLIB 526 
# define P_DEFINE 527 
# define P_UNDEF 528 
# define P_IFDEF 529 
# define P_IFNDEF 530 
# define P_ELSE 531 
# define P_ENDIF 532 
# define P_INCLUDE 533 
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
#ifndef YYFARDATA
#define	YYFARDATA	/*nothing*/
#endif
#if ! defined YYSTATIC
#define	YYSTATIC	/*nothing*/
#endif
#if ! defined YYCONST
#define	YYCONST	/*nothing*/
#endif
#ifndef	YYACT
#define	YYACT	yyact
#endif
#ifndef	YYPACT
#define	YYPACT	yypact
#endif
#ifndef	YYPGO
#define	YYPGO	yypgo
#endif
#ifndef	YYR1
#define	YYR1	yyr1
#endif
#ifndef	YYR2
#define	YYR2	yyr2
#endif
#ifndef	YYCHK
#define	YYCHK	yychk
#endif
#ifndef	YYDEF
#define	YYDEF	yydef
#endif
#ifndef	YYV
#define	YYV	yyv
#endif
#ifndef	YYS
#define	YYS	yys
#endif
#ifndef	YYLOCAL
#define	YYLOCAL
#endif
#ifndef YYR_T
#define	YYR_T	int
#endif
typedef	YYR_T	yyr_t;
#ifndef YYEXIND_T
#define	YYEXIND_T	unsigned int
#endif
typedef	YYEXIND_T	yyexind_t;
#ifndef YYOPTTIME
#define	YYOPTTIME	0
#endif
# define YYERRCODE 256

#line 2166 "asmparse.y"

/********************************************************************************/
/* Code goes here */

/********************************************************************************/

struct Keywords {
    const char* name;
    unsigned short token;
    unsigned short tokenVal;// this holds the instruction enumeration for those keywords that are instrs
    size_t   stname;
};

#define NO_VALUE        ((unsigned short)-1)              // The token has no value

static Keywords keywords[] = {
// Attention! Because of aliases, the instructions MUST go first!
// Redefine all the instructions (defined in assembler.h <- asmenum.h <- opcode.def)
#undef InlineNone
#undef InlineVar        
#undef ShortInlineVar
#undef InlineI          
#undef ShortInlineI     
#undef InlineI8         
#undef InlineR          
#undef ShortInlineR     
#undef InlineBrTarget
#undef ShortInlineBrTarget
#undef InlineMethod
#undef InlineField 
#undef InlineType 
#undef InlineString
#undef InlineSig        
#undef InlineTok        
#undef InlineSwitch
#undef InlineVarTok     


#define InlineNone              INSTR_NONE
#define InlineVar               INSTR_VAR
#define ShortInlineVar          INSTR_VAR
#define InlineI                 INSTR_I
#define ShortInlineI            INSTR_I
#define InlineI8                INSTR_I8
#define InlineR                 INSTR_R
#define ShortInlineR            INSTR_R
#define InlineBrTarget          INSTR_BRTARGET
#define ShortInlineBrTarget             INSTR_BRTARGET
#define InlineMethod            INSTR_METHOD
#define InlineField             INSTR_FIELD
#define InlineType              INSTR_TYPE
#define InlineString            INSTR_STRING
#define InlineSig               INSTR_SIG
#define InlineTok               INSTR_TOK
#define InlineSwitch            INSTR_SWITCH

#define InlineVarTok            0
#define NEW_INLINE_NAMES
                // The volatile instruction collides with the volatile keyword, so 
                // we treat it as a keyword everywhere and modify the grammar accordingly (Yuck!) 
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) { s, args, c, lengthof(s)-1 },
#define OPALIAS(alias_c, s, c) { s, NO_VALUE, c, lengthof(s)-1 },
#include "opcode.def"
#undef OPALIAS
#undef OPDEF

                /* keywords */
#define KYWD(name, sym, val)    { name, sym, val, lengthof(name)-1 },
#include "il_kywd.h"
#undef KYWD

};

/********************************************************************************/
/* File encoding-dependent functions */
/*--------------------------------------------------------------------------*/
char* nextcharA(__in __nullterminated char* pos)
{
    return (*pos > 0) ? ++pos : (char *)_mbsinc((const unsigned char *)pos);
}

char* nextcharU(__in __nullterminated char* pos)
{
    return ++pos;
}

char* nextcharW(__in __nullterminated char* pos)
{
    return (pos+2);
}
/*--------------------------------------------------------------------------*/
unsigned SymAU(__in __nullterminated char* curPos)
{
    return (unsigned)*curPos;
}

unsigned SymW(__in __nullterminated char* curPos)
{
    return (unsigned)*((WCHAR*)curPos);
}
/*--------------------------------------------------------------------------*/
char* NewStrFromTokenAU(__in_ecount(tokLen) char* curTok, size_t tokLen)
{
    char *nb = new char[tokLen+1];
    if(nb != NULL)
    {
        memcpy(nb, curTok, tokLen);
        nb[tokLen] = 0;
    }
    return nb;
}
char* NewStrFromTokenW(__in_ecount(tokLen) char* curTok, size_t tokLen)
{
    WCHAR* wcurTok = (WCHAR*)curTok;
    char *nb = new char[(tokLen<<1) + 2];
    if(nb != NULL)
    {
        tokLen = WszWideCharToMultiByte(CP_UTF8,0,(LPCWSTR)wcurTok,(int)(tokLen >> 1),nb,(int)(tokLen<<1) + 2,NULL,NULL);
        nb[tokLen] = 0;
    }
    return nb;
}
/*--------------------------------------------------------------------------*/
char* NewStaticStrFromTokenAU(__in_ecount(tokLen) char* curTok, size_t tokLen, __out_ecount(bufSize) char* staticBuf, size_t bufSize)
{
    if(tokLen >= bufSize) return NULL;
    memcpy(staticBuf, curTok, tokLen);
    staticBuf[tokLen] = 0;
    return staticBuf;
}
char* NewStaticStrFromTokenW(__in_ecount(tokLen) char* curTok, size_t tokLen, __out_ecount(bufSize) char* staticBuf, size_t bufSize)
{
    WCHAR* wcurTok = (WCHAR*)curTok;
    if(tokLen >= bufSize/2) return NULL;
    tokLen = WszWideCharToMultiByte(CP_UTF8,0,(LPCWSTR)wcurTok,(int)(tokLen >> 1),staticBuf,(int)bufSize,NULL,NULL);
    staticBuf[tokLen] = 0;
    return staticBuf;
}
/*--------------------------------------------------------------------------*/
unsigned GetDoubleAU(__in __nullterminated char* begNum, unsigned L, double** ppRes)
{
    static char dbuff[128];
    char* pdummy;
    if(L > 127) L = 127;
    memcpy(dbuff,begNum,L);
    dbuff[L] = 0;
    *ppRes = new double(strtod(dbuff, &pdummy));
    return ((unsigned)(pdummy - dbuff));
}

unsigned GetDoubleW(__in __nullterminated char* begNum, unsigned L, double** ppRes)
{
    static char dbuff[256];
    char* pdummy;
    if(L > 254) L = 254;
    memcpy(dbuff,begNum,L);
    dbuff[L] = 0;
    dbuff[L+1] = 0;
    *ppRes = new double(wcstod((const wchar_t*)dbuff, (wchar_t**)&pdummy));
    return ((unsigned)(pdummy - dbuff));
}
/*--------------------------------------------------------------------------*/
char* yygetline(int Line)
{
    static char buff[0x4000];
    char *pLine=NULL, *pNextLine=NULL;
    char *pBegin=NULL, *pEnd = NULL;
    unsigned uCount = parser->getAll(&pBegin);
    pEnd = pBegin + uCount;
    buff[0] = 0;
    for(uCount=0, pLine=pBegin; pLine < pEnd; pLine = nextchar(pLine))
    {
        if(Sym(pLine) == '\n') uCount++;
        if(uCount == (unsigned int)(Line-1)) break;
    }
    pLine = nextchar(pLine);
    if(pLine < pEnd)
    {
        for(pNextLine = pLine; pNextLine < pEnd; pNextLine = nextchar(pNextLine))
        {
            if(Sym(pNextLine) == '\n') break;
        }
        if(Sym == SymW) // Unicode file
        {
            if(*((WCHAR*)pNextLine - 1) == '\r') pNextLine -= 2;
            uCount = (unsigned)(pNextLine - pLine);
            uCount &= 0x1FFF; // limit: 8K wchars
            WCHAR* wzBuff = (WCHAR*)buff;
            memcpy(buff,pLine,uCount);
            wzBuff[uCount >> 1] = 0;
        }
        else
        {
            if(*(pNextLine-1)=='\r') pNextLine--;
            uCount = (unsigned)(pNextLine - pLine);
            uCount &= 0x3FFF; // limit: 16K chars
            memcpy(buff,pLine,uCount);
            buff[uCount]=0;
        }
    }    
    return buff;
}

void yyerror(__in __nullterminated char* str) {
    char tokBuff[64];
    WCHAR *wzfile = (WCHAR*)(PENV->in->namew());
    int iline = PENV->curLine;
    
    size_t len = PENV->curPos - PENV->curTok;
    if (len > 62) len = 62;
    memcpy(tokBuff, PENV->curTok, len);
    tokBuff[len] = 0;
    tokBuff[len+1] = 0;
    if(PENV->bExternSource)
    {
        wzfile = PASM->m_wzSourceFileName;
        iline = PENV->nExtLine;
    }
    if(Sym == SymW) // Unicode file
        fprintf(stderr, "%S(%d) : error : %s at token '%S' in: %S\n", 
                wzfile, iline, str, (WCHAR*)tokBuff, (WCHAR*)yygetline(PENV->curLine));
    else
        fprintf(stderr, "%S(%d) : error : %s at token '%s' in: %s\n", 
                wzfile, iline, str, tokBuff, yygetline(PENV->curLine));
    parser->success = false;
}

/********************************************************************************/
/* looks up the typedef 'name' of length 'nameLen' (name does not need to be 
   null terminated)   Returns 0 on failure */
TypeDefDescr* findTypedef(__in_ecount(NameLen) char* name, size_t NameLen)
{
    TypeDefDescr* pRet = NULL;
    static char Name[4096];
    if(PASM->NumTypeDefs())
    {
        if(NewStaticStrFromToken(name,NameLen,Name,4096))
            pRet = PASM->FindTypeDef(Name);
    }
    return pRet;
}

int TYPEDEF(TypeDefDescr* pTDD)
{
    switch(TypeFromToken(pTDD->m_tkTypeSpec))
    {
        case mdtTypeDef:
        case mdtTypeRef:
            return TYPEDEF_T;
        case mdtMethodDef:
        case 0x99000000:
            return TYPEDEF_M;
        case mdtFieldDef:
        case 0x98000000:
            return TYPEDEF_F;
        case mdtMemberRef:
            return TYPEDEF_MR;
        case mdtTypeSpec:
            return TYPEDEF_TS;
        case mdtCustomAttribute:
            return TYPEDEF_CA;
    }
    return ERROR_;
            
}

/********************************************************************************/
void indexKeywords(Indx* indx)  // called in Assembler constructor (assem.cpp)
{
    Keywords* low = keywords;
    Keywords* high = keywords + (sizeof(keywords) / sizeof(Keywords));
    Keywords* mid;
    for(mid = low; mid < high; mid++)
    {
        indx->IndexString((char*)(mid->name),mid);
    }
}

Instr* SetupInstr(unsigned short opcode)
{
    Instr* pVal = NULL;
    if((pVal = PASM->GetInstr()))
    {
        pVal->opcode = opcode;
        if((pVal->pWriter = PASM->m_pSymDocument)!=NULL)
        {
            if(PENV->bExternSource)
            {
                pVal->linenum = PENV->nExtLine;
                pVal->column = PENV->nExtCol;
                pVal->linenum_end = PENV->nExtLineEnd;
                pVal->column_end = PENV->nExtColEnd;
                pVal->pc = nCurrPC;
            }
            else
            {
                pVal->linenum = PENV->curLine;
                pVal->column = 1;
                pVal->linenum_end = PENV->curLine;
                pVal->column_end = 0;
                pVal->pc = PASM->m_CurPC;
            }
        }
    }
    return pVal;
}
/* looks up the keyword 'name' of length 'nameLen' (name does not need to be 
   null terminated)   Returns 0 on failure */
int findKeyword(const char* name, size_t nameLen, unsigned short* pOpcode) 
{
    static char Name[128];
    Keywords* mid;

    if(NULL == NewStaticStrFromToken((char*)name,nameLen,Name,128)) return 0; // can't be a keyword
    mid = (Keywords*)(PASM->indxKeywords.FindString(Name));
    if(mid == NULL) return 0;
    *pOpcode = mid->tokenVal;

    return(mid->token);
}

/********************************************************************************/
/* convert str to a uint64 */
unsigned digits[128];
void Init_str2uint64()
{
    int i;
    memset(digits,255,sizeof(digits));
    for(i='0'; i <= '9'; i++) digits[i] = i - '0';
    for(i='A'; i <= 'Z'; i++) digits[i] = i + 10 - 'A';
    for(i='a'; i <= 'z'; i++) digits[i] = i + 10 - 'a';
}
static unsigned __int64 str2uint64(const char* str, const char** endStr, unsigned radix) 
{
    unsigned __int64 ret = 0;
    unsigned digit,ix;
    _ASSERTE(radix <= 36);
    for(;;str = nextchar((char*)str)) 
    {
        ix = Sym((char*)str);
        if(ix <= 0x7F)
        {
            digit = digits[ix];
            if(digit < radix)
            {
                ret = ret * radix + digit;
                continue;
            }
        }
        *endStr = str;
        return(ret);
    }
}
/********************************************************************************/
/* Append an UTF-8 string preceded by compressed length, no zero terminator, to a BinStr */
static void AppendStringWithLength(BinStr* pbs, __in __nullterminated char* sz)
{
    if((pbs != NULL) && (sz != NULL))
    {
        unsigned L = (unsigned) strlen(sz);
        BYTE* pb = NULL;
        corEmitInt(pbs,L);
        if((pb = pbs->getBuff(L)) != NULL)
            memcpy(pb,sz,L);
    }
}

/********************************************************************************/
/* fetch the next token, and return it   Also set the yylval.union if the
   lexical token also has a value */
   

BOOL _Alpha[128];
BOOL _Digit[128];
BOOL _AlNum[128];
BOOL _ValidSS[128];
BOOL _ValidCS[128];
void SetSymbolTables()
{
    unsigned i;
    memset(_Alpha,0,sizeof(_Alpha)); 
    memset(_Digit,0,sizeof(_Digit)); 
    memset(_AlNum,0,sizeof(_AlNum)); 
    memset(_ValidSS,0,sizeof(_ValidSS)); 
    memset(_ValidCS,0,sizeof(_ValidCS));
    for(i = 'A'; i <= 'Z'; i++)
    {
        _Alpha[i] = TRUE;
        _AlNum[i] = TRUE;
        _ValidSS[i] = TRUE;
        _ValidCS[i] = TRUE;
    } 
    for(i = 'a'; i <= 'z'; i++)
    {
        _Alpha[i] = TRUE;
        _AlNum[i] = TRUE;
        _ValidSS[i] = TRUE;
        _ValidCS[i] = TRUE;
    }
    for(i = '0'; i <= '9'; i++)
    {
        _Digit[i] = TRUE;
        _AlNum[i] = TRUE;
        _ValidCS[i] = TRUE;
    }
    _ValidSS[(unsigned char)'_'] = TRUE;
    _ValidSS[(unsigned char)'#'] = TRUE;
    _ValidSS[(unsigned char)'$'] = TRUE;
    _ValidSS[(unsigned char)'@'] = TRUE;
     
    _ValidCS[(unsigned char)'_'] = TRUE;
    _ValidCS[(unsigned char)'?'] = TRUE;
    _ValidCS[(unsigned char)'$'] = TRUE;
    _ValidCS[(unsigned char)'@'] = TRUE;
    _ValidCS[(unsigned char)'`'] = TRUE;
}
BOOL IsAlpha(unsigned x) { return (x < 128)&&_Alpha[x]; }
BOOL IsDigit(unsigned x) { return (x < 128)&&_Digit[x]; }     
BOOL IsAlNum(unsigned x) { return (x < 128)&&_AlNum[x]; }     
BOOL IsValidStartingSymbol(unsigned x) { return (x < 128)&&_ValidSS[x]; }     
BOOL IsValidContinuingSymbol(unsigned x) { return (x < 128)&&_ValidCS[x]; } 


char* nextBlank(__in __nullterminated char* curPos)
{
    for(;;) 
    {   
        switch(Sym(curPos)) 
        {
            case '/' :
                if ((Sym(nextchar(curPos)) == '/')|| (Sym(nextchar(curPos)) == '*'))
                    return curPos;
                else
                {
                    curPos = nextchar(curPos);
                    break;
                }
            case 0: 
            case '\n':
            case '\r':
            case ' ' :
            case '\t':
            case '\f':
                return curPos;

            default:
                curPos = nextchar(curPos);
        }
    }
}

char* skipBlanks(__in __nullterminated char* curPos, unsigned* pstate)
{
    const unsigned eolComment = 1;
    const unsigned multiComment = 2;
    unsigned nextSym, state = *pstate;
    char* nextPos;
    for(;;) 
    {   // skip whitespace and comments
        if (curPos >= PENV->endPos) 
        {
            *pstate = state;    
            return NULL;
        }
        switch(Sym(curPos)) 
        {
            case 0: 
                return NULL;       // EOF
            case '\n':
                state &= ~eolComment;
                PENV->curLine++;
                if(PENV->bExternSource)
                {
                    if(PENV->bExternSourceAutoincrement) PENV->nExtLine++;
                    PASM->m_ulCurLine = PENV->nExtLine;
                    PASM->m_ulCurColumn = PENV->nExtCol;
                }
                else
                {
                    PASM->m_ulCurLine = PENV->curLine;
                    PASM->m_ulCurColumn = 1;
                }
                break;
            case '\r':
            case ' ' :
            case '\t':
            case '\f':
                break;

            case '*' :
                if(state == 0) goto PAST_WHITESPACE;
                if(state & multiComment)
                {
                    nextPos = nextchar(curPos);
                    if (Sym(nextPos) == '/') 
                    {
                        curPos = nextPos;
                        state &= ~multiComment;
                    }
                }
                break;

            case '/' :
                if(state == 0)
                {
                    nextPos = nextchar(curPos);
                    nextSym = Sym(nextPos);
                    if (nextSym == '/')
                    {
                        curPos = nextPos;
                        state |= eolComment;
                    }
                    else if (nextSym == '*') 
                    {
                        curPos = nextPos;
                        state |= multiComment;
                    }
                    else goto PAST_WHITESPACE;
                }
                break;

            default:
                if (state == 0)  goto PAST_WHITESPACE;
        }
        curPos = nextchar(curPos);
    }
PAST_WHITESPACE:
    *pstate = state;
    return curPos;
}

char* FullFileName(__in __nullterminated WCHAR* wzFileName, unsigned uCodePage);

int ProcessEOF()
{
    PARSING_ENVIRONMENT* prev_penv = parser->PEStack.POP();
    if(prev_penv != NULL)
    {
        //delete [] (WCHAR*)(PENV->in->namew());
        delete PENV->in;
        delete PENV;
        parser->penv = prev_penv;
        SetFunctionPtrs();
        char* szFileName = new char[strlen(PENV->szFileName)+1];
        strcpy_s(szFileName,strlen(PENV->szFileName)+1,PENV->szFileName);
        PASM->SetSourceFileName(szFileName); // deletes the argument!
        return ';';
    }
    //PENV->in = NULL;
    return 0;
}

#define NEXT_TOKEN  {state=0; curPos=PENV->curPos; goto NextToken;}

int parse_literal(unsigned curSym, __inout __nullterminated char* &curPos, BOOL translate_escapes)
{
    unsigned quote = curSym;
    curPos = nextchar(curPos);
    char* fromPtr = curPos;
    bool escape = false;

    for(;;) 
    {     // Find matching quote
        curSym = (curPos >= PENV->endPos) ? 0 : Sym(curPos);
        if(curSym == 0)
        {
            PENV->curPos = curPos; 
            return(BAD_LITERAL_);
        }
        else if(curSym == '\\')
            escape = !escape;
        else
        {
            if(curSym == '\n')
            {
                PENV->curLine++;
                if(PENV->bExternSource)
                {
                    if(PENV->bExternSourceAutoincrement) PENV->nExtLine++;
                    PASM->m_ulCurLine = PENV->nExtLine;
                    PASM->m_ulCurColumn = PENV->nExtCol;
                }
                else
                {
                    PASM->m_ulCurLine = PENV->curLine;
                    PASM->m_ulCurColumn = 1;
                }
                if (!escape) { PENV->curPos = curPos; return(BAD_LITERAL_); }
            }
            else if ((curSym == quote) && (!escape)) break;
            escape = false;
        }        
        curPos = nextchar(curPos);
    }
    // translate escaped characters
    unsigned tokLen = (unsigned)(curPos - fromPtr);
    char* newstr = NewStrFromToken(fromPtr, tokLen);
    char* toPtr;
    curPos = nextchar(curPos);  // skip closing quote
    if(translate_escapes)
    {
        fromPtr = newstr;
        //_ASSERTE(0);
        tokLen = (unsigned)strlen(newstr);
        toPtr = new char[tokLen+1];
        if(toPtr==NULL) return BAD_LITERAL_;
        yylval.string = toPtr;
        char* endPtr = fromPtr+tokLen;
        while(fromPtr < endPtr) 
        {
            if (*fromPtr == '\\') 
            {
                fromPtr++;
                switch(*fromPtr) 
                {
                    case 't':
                            *toPtr++ = '\t';
                            break;
                    case 'n':
                            *toPtr++ = '\n';
                            break;
                    case 'b':
                            *toPtr++ = '\b';
                            break;
                    case 'f':
                            *toPtr++ = '\f';
                            break;
                    case 'v':
                            *toPtr++ = '\v';
                            break;
                    case '?':
                            *toPtr++ = '\?';
                            break;
                    case 'r':
                            *toPtr++ = '\r';
                            break;
                    case 'a':
                            *toPtr++ = '\a';
                            break;
                    case '\n':
                            do      fromPtr++;
                            while(isspace(*fromPtr));
                            --fromPtr;              // undo the increment below   
                            break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                            if (IsDigit(fromPtr[1]) && IsDigit(fromPtr[2])) 
                            {
                                *toPtr++ = ((fromPtr[0] - '0') * 8 + (fromPtr[1] - '0')) * 8 + (fromPtr[2] - '0');
                                fromPtr+= 2;                                                            
                            }
                            else if(*fromPtr == '0') *toPtr++ = 0;
                            else *toPtr++ = *fromPtr;
                            break;
                    default:
                            *toPtr++ = *fromPtr;
                }
                fromPtr++;
            }
            else
            //  *toPtr++ = *fromPtr++;
            {
                char* tmpPtr = fromPtr;
                fromPtr = (nextchar == nextcharW) ? nextcharU(fromPtr) : nextchar(fromPtr);
                while(tmpPtr < fromPtr) *toPtr++ = *tmpPtr++;
            }
    
        } //end while(fromPtr < endPtr)
        *toPtr = 0;                     // terminate string
        delete [] newstr;
    }
    else
    {
        yylval.string = newstr;
        toPtr = newstr + strlen(newstr);
    }
            
    PENV->curPos = curPos;
    if(quote == '"')
    {
        BinStr* pBS = new BinStr();
        unsigned size = (unsigned)(toPtr - yylval.string);
        memcpy(pBS->getBuff(size),yylval.string,size);
        delete [] yylval.string;
        yylval.binstr = pBS;
        return QSTRING;
    }
    else
    {
        if(PASM->NumTypeDefs())
        {
            TypeDefDescr* pTDD = PASM->FindTypeDef(yylval.string);
            if(pTDD != NULL)
            {
                delete [] yylval.string;
                yylval.tdd = pTDD;
                return(TYPEDEF(pTDD));
            }
        }
        return SQSTRING;
    }
}

int yylex() 
{
    char* curPos = PENV->curPos;
    unsigned state = 0;
    const unsigned multiComment = 2;
    unsigned curSym;
    
    char* newstr;
    
NextToken:    
    // Skip any leading whitespace and comments
    curPos = skipBlanks(curPos, &state);
    if(curPos == NULL)
    {
        if (state & multiComment) return (BAD_COMMENT_);
        if(ProcessEOF() == 0) return 0;       // EOF
        NEXT_TOKEN;
    }
    char* curTok = curPos;
    PENV->curTok = curPos;
    PENV->curPos = curPos;
    int tok = ERROR_;
    yylval.string = 0;

    curSym = Sym(curPos);
    if(bParsingByteArray) // only hexadecimals w/o 0x, ')' and white space allowed!
    {
        int i,s=0;
        for(i=0; i<2; i++, curPos = nextchar(curPos), curSym = Sym(curPos))
        {
            if(('0' <= curSym)&&(curSym <= '9')) s = s*16+(curSym - '0');
            else if(('A' <= curSym)&&(curSym <= 'F')) s = s*16+(curSym - 'A' + 10);
            else if(('a' <= curSym)&&(curSym <= 'f')) s = s*16+(curSym - 'a' + 10);
            else break; // don't increase curPos!
        }
        if(i)
        {
            tok = HEXBYTE;
            yylval.int32 = s;
        }
        else
        {
            if(curSym == ')') 
            {
                bParsingByteArray = FALSE;
                goto Just_A_Character;
            }
        }
        PENV->curPos = curPos;
        return(tok);
    }
    if(curSym == '?') // '?' may be part of an identifier, if it's not followed by punctuation
    {
        if(IsValidContinuingSymbol(Sym(nextchar(curPos)))) goto Its_An_Id;
        goto Just_A_Character;
    }

    if (IsValidStartingSymbol(curSym)) 
    { // is it an ID
Its_An_Id:
        size_t offsetDot = (size_t)-1; // first appearance of '.'
                size_t offsetDotDigit = (size_t)-1; // first appearance of '.<digit>' (not DOTTEDNAME!)
        do 
        {
            curPos = nextchar(curPos);
            if (Sym(curPos) == '.') 
            {
                if (offsetDot == (size_t)-1) offsetDot = curPos - curTok;
                curPos = nextchar(curPos);
                if((offsetDotDigit==(size_t)-1)&&(Sym(curPos) >= '0')&&(Sym(curPos) <= '9')) 
                        offsetDotDigit = curPos - curTok - 1;
            }
        } while(IsValidContinuingSymbol(Sym(curPos)));
        
        size_t tokLen = curPos - curTok;
        // check to see if it is a keyword
        int token = findKeyword(curTok, tokLen, &yylval.opcode);
        if (token != 0) 
        {
            //printf("yylex: TOK = %d, curPos=0x%8.8X\n",token,curPos);
            PENV->curPos = curPos;
            PENV->curTok = curTok;
            if(!SkipToken)
            {
                switch(token)
                {
                    case P_INCLUDE:
                        //if(include_first_pass)
                        //{
                        //    PENV->curPos = curTok;
                        //    include_first_pass = FALSE;
                        //    return ';';
                        //}
                        //include_first_pass = TRUE;
                        curPos = skipBlanks(curPos,&state);
                        if(curPos == NULL)
                        {
                            if (state & multiComment) return (BAD_COMMENT_);
                            if(ProcessEOF() == 0) return 0;       // EOF
                            NEXT_TOKEN;
                        }
                        if(Sym(curPos) != '"') return ERROR_;
                        curPos = nextchar(curPos);
                        curTok = curPos;
                        PENV->curTok = curPos;
                        while(Sym(curPos) != '"')
                        {
                            curPos = nextchar(curPos);
                            if(curPos >= PENV->endPos) return ERROR_;
                            PENV->curPos = curPos;
                        }
                        tokLen = PENV->curPos - curTok;
                        curPos = nextchar(curPos);
                        PENV->curPos = curPos;
                        {
                            WCHAR* wzFile=NULL;
                            if(Sym == SymW)
                            {
                                if((wzFile = new WCHAR[tokLen/2 + 1]) != NULL)
                                {
                                    memcpy(wzFile,curTok,tokLen);
                                    wzFile[tokLen/2] = 0;
                                }
                            }
                            else
                            {
                                if((wzFile = new WCHAR[tokLen+1]) != NULL)
                                {
                                    tokLen = WszMultiByteToWideChar(g_uCodePage,0,curTok,(int)tokLen,wzFile,(int)tokLen+1);
                                    wzFile[tokLen] = 0;
                                }
                            }
                            if(wzFile != NULL)
                            {
                                if((parser->wzIncludePath != NULL)
                                 &&(wcschr(wzFile,'\\')==NULL)&&(wcschr(wzFile,':')==NULL))
                                {
                                    WCHAR* wzFullName =  new WCHAR[MAX_FILENAME_LENGTH+1];
                                    if(wzFullName != NULL)
                                    {
                                        WCHAR* pwz;
                                        DWORD dw = WszSearchPath(parser->wzIncludePath,wzFile,NULL,
                                                   MAX_FILENAME_LENGTH+1,wzFullName,&pwz);
                                        if(dw != 0)
                                        {
                                            wzFullName[dw] = 0;
                                            delete [] wzFile;
                                            wzFile = wzFullName;
                                        }
                                        else
                                        {
                                            delete [] wzFullName;
                                        }
                                    }
                                }
                                if(PASM->m_fReportProgress)
                                    parser->msg("\nIncluding '%S'\n",wzFile);
                                MappedFileStream *pIn = new MappedFileStream(wzFile);
                                if((pIn != NULL)&&pIn->IsValid())
                                {
                                    parser->PEStack.PUSH(PENV);
                                    PASM->SetSourceFileName(FullFileName(wzFile,CP_UTF8)); // deletes the argument!
                                    parser->CreateEnvironment(pIn);
                                    NEXT_TOKEN;
                                }
                                else
                                {
                                    delete [] wzFile;
                                    PASM->report->error("#include failed\n");
                                    return ERROR_;
                                }
                            }
                            else
                            {
                                PASM->report->error("Out of memory\n");
                                return ERROR_;
                            }
                        }
                        curPos = PENV->curPos;
                        curTok = PENV->curTok;
                        break;
                    case P_IFDEF:
                    case P_IFNDEF:
                    case P_DEFINE:
                    case P_UNDEF:
                        curPos = skipBlanks(curPos,&state);
                        if(curPos == NULL)
                        {
                            if (state & multiComment) return (BAD_COMMENT_);
                            if(ProcessEOF() == 0) return 0;       // EOF
                            NEXT_TOKEN;
                        }
                        curTok = curPos;
                        PENV->curTok = curPos;
                        PENV->curPos = curPos;
                        if (!IsValidStartingSymbol(Sym(curPos))) return ERROR_;
                        do 
                        {
                            curPos = nextchar(curPos);
                        } while(IsValidContinuingSymbol(Sym(curPos)));
                        tokLen = curPos - curTok;
                            
                        newstr = NewStrFromToken(curTok, tokLen);
                        if((token==P_DEFINE)||(token==P_UNDEF))
                        {
                            if(token == P_DEFINE)
                            {
                                curPos = skipBlanks(curPos,&state);
                                curSym = Sym(curPos);
                                if(curSym != '"')
                                    DefineVar(newstr, NULL);
                                else
                                {
                                    tok = parse_literal(curSym, curPos, FALSE);
                                    if(tok == QSTRING)
                                    {
                                        // if not ANSI, then string is in UTF-8,
                                        // insert prefix
                                        if(nextchar != nextcharA)
                                        {
                                            yylval.binstr->insertInt8(0xEF);
                                            yylval.binstr->insertInt8(0xBB);
                                            yylval.binstr->insertInt8(0xBF);
                                        }
                                        yylval.binstr->appendInt8(' ');
                                        DefineVar(newstr, yylval.binstr);
                                    }
                                    else
                                        return tok;
                                }
                            }
                            else UndefVar(newstr);
                        }
                        else
                        {
                            SkipToken = IsVarDefined(newstr);
                            if(token == P_IFDEF) SkipToken = !SkipToken;
                            IfEndif++;
                            if(SkipToken) IfEndifSkip=IfEndif;
                        }
                        break;
                    case P_ELSE:
                        SkipToken = TRUE;
                        IfEndifSkip=IfEndif;
                        break;
                    case P_ENDIF:
                        if(IfEndif == 0)
                        {
                            PASM->report->error("Unmatched #endif\n");
                            return ERROR_;
                        }
                        IfEndif--;
                        break;
                    default:
                        return(token);
                }
                goto NextToken;
            }
            if(SkipToken)
            {
                switch(token)
                {
                    case P_IFDEF:
                    case P_IFNDEF:
                        IfEndif++;
                        break;
                    case P_ELSE:
                        if(IfEndif == IfEndifSkip) SkipToken = FALSE;
                        break;
                    case P_ENDIF:
                        if(IfEndif == IfEndifSkip) SkipToken = FALSE;
                        IfEndif--;
                        break;
                    default:
                        break;
                }
                //if(yylval.instr) yylval.instr->opcode = -1;
                goto NextToken;
            }
            return(token);
        } // end if token != 0
        if(SkipToken) { curPos = nextBlank(curPos); goto NextToken; }
        
        VarName* pVarName = FindVarDef(NewStrFromToken(curTok, tokLen));
        if(pVarName != NULL)
        {
            if(pVarName->pbody != NULL)
            {
                BinStrStream *pIn = new BinStrStream(pVarName->pbody);
                if((pIn != NULL)&&pIn->IsValid())
                {
                    PENV->curPos = curPos;
                    parser->PEStack.PUSH(PENV);
                    parser->CreateEnvironment(pIn);
                    NEXT_TOKEN;
                }
            }
        }
        
        TypeDefDescr* pTDD = findTypedef(curTok,tokLen);
        
        if(pTDD != NULL)
        {
            yylval.tdd = pTDD;
            PENV->curPos = curPos;
            PENV->curTok = curTok;
            return(TYPEDEF(pTDD));
        }
        if(Sym(curTok) == '#') 
        {
            PENV->curPos = curPos;
            PENV->curTok = curTok;
            return(ERROR_);
        }
        // Not a keyword, normal identifiers don't have '.' in them
        if (offsetDot < (size_t)-1) 
        {
            if(offsetDotDigit < (size_t)-1)
            {
                curPos = curTok+offsetDotDigit;
                tokLen = offsetDotDigit;
            }
            // protection against something like Foo.Bar..123 or Foo.Bar.
            unsigned D = (Sym == SymW) ? 2 : 1; // Unicode or ANSI/UTF8!
            while((Sym(curPos-D)=='.')&&(tokLen))
            {
                curPos -= D;
                tokLen -= D;
            }
        }
        if((yylval.string = NewStrFromToken(curTok,tokLen)))
        {
            tok = (offsetDot == (size_t)(-1))? ID : DOTTEDNAME;
            //printf("yylex: ID = '%s', curPos=0x%8.8X\n",yylval.string,curPos);
        }
        else return BAD_LITERAL_;
    }
    else if(SkipToken) { curPos = nextBlank(curPos); goto NextToken; }
    else if (IsDigit(curSym) 
        || (curSym == '.' && IsDigit(Sym(nextchar(curPos))))
        || (curSym == '-' && IsDigit(Sym(nextchar(curPos))))) 
    {
        const char* begNum = curPos;
        unsigned radix = 10;

        neg = (curSym == '-');    // always make it unsigned 
        if (neg) curPos = nextchar(curPos);

        if (Sym(curPos) == '0' && Sym(nextchar(curPos)) != '.') 
        {
            curPos = nextchar(curPos);
            radix = 8;
            if (Sym(curPos) == 'x' || Sym(curPos) == 'X') 
            {
                curPos = nextchar(curPos);
                radix = 16;
            }
        }
        begNum = curPos;
        {
            unsigned __int64 i64 = str2uint64(begNum, const_cast<const char**>(&curPos), radix);
            unsigned __int64 mask64 = neg ? UI64(0xFFFFFFFF80000000) : UI64(0xFFFFFFFF00000000);
            unsigned __int64 largestNegVal32 = UI64(0x0000000080000000);
            if ((i64 & mask64) && (i64 != largestNegVal32))
            {
                yylval.int64 = new __int64(i64);
                tok = INT64;                    
                if (neg) *yylval.int64 = -*yylval.int64;
            }
            else
            {
                yylval.int32 = (__int32)i64;
                tok = INT32;
                if(neg) yylval.int32 = -yylval.int32;
            }
        }
        if (radix == 10 && ((Sym(curPos) == '.' && Sym(nextchar(curPos)) != '.') || Sym(curPos) == 'E' || Sym(curPos) == 'e')) 
        {
            unsigned L = (unsigned)(PENV->endPos - begNum);
            curPos = (char*)begNum + GetDouble((char*)begNum,L,&yylval.float64);
            if (neg) *yylval.float64 = -*yylval.float64;
            tok = FLOAT64;
        }
    }
    else 
    {   //      punctuation
        if (curSym == '"' || curSym == '\'') 
        {
            return parse_literal(curSym, curPos, TRUE);
        } // end if (*curPos == '"' || *curPos == '\'')
        else if (curSym==':' && Sym(nextchar(curPos))==':') 
        {
            curPos = nextchar(nextchar(curPos));
            tok = DCOLON;
        }       
        else if(curSym == '.') 
        {
            if (Sym(nextchar(curPos))=='.' && Sym(nextchar(nextchar(curPos)))=='.') 
            {
                curPos = nextchar(nextchar(nextchar(curPos)));
                tok = ELIPSIS;
            }
            else
            {
                do
                {
                    curPos = nextchar(curPos);
                    if (curPos >= PENV->endPos) 
                    return ERROR_;
                    curSym = Sym(curPos);
                }
                while(IsAlNum(curSym) || curSym == '_' || curSym == '$'|| curSym == '@'|| curSym == '?');
                size_t tokLen = curPos - curTok;
    
                // check to see if it is a keyword
                int token = findKeyword(curTok, tokLen, &yylval.opcode);
                if(token)
                        {
                    //printf("yylex: TOK = %d, curPos=0x%8.8X\n",token,curPos);
                    PENV->curPos = curPos;
                    PENV->curTok = curTok; 
                    return(token);
                }
                tok = '.';
                curPos = nextchar(curTok);
            }
        }
        else 
        {
Just_A_Character:
            tok = curSym;
            curPos = nextchar(curPos);
        }
        //printf("yylex: PUNCT curPos=0x%8.8X\n",curPos);
    }
    dbprintf(("    Line %d token %d (%c) val = %s\n", PENV->curLine, tok, 
            (tok < 128 && isprint(tok)) ? tok : ' ', 
            (tok > 255 && tok != INT32 && tok != INT64 && tok!= FLOAT64) ? yylval.string : ""));

    PENV->curPos = curPos;
    PENV->curTok = curTok; 
    return(tok);
}

/**************************************************************************/
static char* newString(__in __nullterminated char* str1) 
{
    char* ret = new char[strlen(str1)+1];
    if(ret) strcpy_s(ret, strlen(str1)+1, str1);
    return(ret);
}

/**************************************************************************/
/* concatenate strings and release them */

static char* newStringWDel(__in __nullterminated char* str1, char delimiter, __in __nullterminated char* str3) 
{
    size_t len1 = strlen(str1);
    size_t len = len1+2;
    if (str3) len += strlen(str3);
    char* ret = new char[len];
    if(ret)
    {
        strcpy_s(ret, len, str1);
        delete [] str1;
        ret[len1] = delimiter;
        ret[len1+1] = 0;
        if (str3)
        {
            strcat_s(ret, len, str3);
            delete [] str3;
        }
    }
    return(ret);
}

/**************************************************************************/
static void corEmitInt(BinStr* buff, unsigned data) 
{
    unsigned cnt = CorSigCompressData(data, buff->getBuff(5));
    buff->remove(5 - cnt);
}


/**************************************************************************/
/* move 'ptr past the exactly one type description */

unsigned __int8* skipType(unsigned __int8* ptr, BOOL fFixupType) 
{
    mdToken  tk;
AGAIN:
    switch(*ptr++) {
        case ELEMENT_TYPE_VOID         :
        case ELEMENT_TYPE_BOOLEAN      :
        case ELEMENT_TYPE_CHAR         :
        case ELEMENT_TYPE_I1           :
        case ELEMENT_TYPE_U1           :
        case ELEMENT_TYPE_I2           :
        case ELEMENT_TYPE_U2           :
        case ELEMENT_TYPE_I4           :
        case ELEMENT_TYPE_U4           :
        case ELEMENT_TYPE_I8           :
        case ELEMENT_TYPE_U8           :
        case ELEMENT_TYPE_R4           :
        case ELEMENT_TYPE_R8           :
        case ELEMENT_TYPE_U            :
        case ELEMENT_TYPE_I            :
        case ELEMENT_TYPE_R            :
        case ELEMENT_TYPE_STRING       :
        case ELEMENT_TYPE_OBJECT       :
        case ELEMENT_TYPE_TYPEDBYREF   :
        case ELEMENT_TYPE_SENTINEL     :
                /* do nothing */
                break;

        case ELEMENT_TYPE_VALUETYPE   :
        case ELEMENT_TYPE_CLASS        :
                ptr += CorSigUncompressToken(ptr, &tk);
                break;

        case ELEMENT_TYPE_CMOD_REQD    :
        case ELEMENT_TYPE_CMOD_OPT     :
                ptr += CorSigUncompressToken(ptr, &tk);
                goto AGAIN;

                /*                                                                   
                        */

        case ELEMENT_TYPE_ARRAY         :
                {
                    ptr = skipType(ptr, fFixupType);                    // element Type
                    unsigned rank = CorSigUncompressData((PCCOR_SIGNATURE&) ptr);
                    if (rank != 0)
                    {
                        unsigned numSizes = CorSigUncompressData((PCCOR_SIGNATURE&) ptr);
                        while(numSizes > 0)
                                                {
                            CorSigUncompressData((PCCOR_SIGNATURE&) ptr);
                                                        --numSizes;
                                                }
                        unsigned numLowBounds = CorSigUncompressData((PCCOR_SIGNATURE&) ptr);
                        while(numLowBounds > 0)
                                                {
                            CorSigUncompressData((PCCOR_SIGNATURE&) ptr);
                                                        --numLowBounds;
                                                }
                    }
                }
                break;

                // Modifiers or depedant types
        case ELEMENT_TYPE_PINNED                :
        case ELEMENT_TYPE_PTR                   :
        case ELEMENT_TYPE_BYREF                 :
        case ELEMENT_TYPE_SZARRAY               :
                // tail recursion optimization
                // ptr = skipType(ptr, fFixupType);
                // break
                goto AGAIN;

        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
                CorSigUncompressData((PCCOR_SIGNATURE&) ptr);  // bound
                break;
        
        case ELEMENT_TYPE_VARFIXUP:
        case ELEMENT_TYPE_MVARFIXUP:
                if(fFixupType)
                {
                    BYTE* pb = ptr-1; // ptr incremented in switch
                    int n = CorSigUncompressData((PCCOR_SIGNATURE&) ptr);  // fixup #
                    int m = -1;
                    if(PASM->m_TyParList)
                        m = PASM->m_TyParList->IndexOf(TyParFixupList.PEEK(n));
                    if(m == -1)
                    {
                        PASM->report->error("(fixupType) Invalid %stype parameter '%s'\n",
                            (*pb == ELEMENT_TYPE_MVARFIXUP)? "method ": "",
                            TyParFixupList.PEEK(n));
                        m = 0;
                    }
                    *pb = (*pb == ELEMENT_TYPE_MVARFIXUP)? ELEMENT_TYPE_MVAR : ELEMENT_TYPE_VAR;
                    CorSigCompressData(m,pb+1);
                }
                else
                    CorSigUncompressData((PCCOR_SIGNATURE&) ptr);  // bound
                break;

        case ELEMENT_TYPE_FNPTR: 
                {
                    CorSigUncompressData((PCCOR_SIGNATURE&) ptr);    // calling convention
                    unsigned argCnt = CorSigUncompressData((PCCOR_SIGNATURE&) ptr);    // arg count
                    ptr = skipType(ptr, fFixupType);                             // return type
                    while(argCnt > 0)
                    {
                        ptr = skipType(ptr, fFixupType);
                        --argCnt;
                    }
                }
                break;

        case ELEMENT_TYPE_GENERICINST: 
               {
                   ptr = skipType(ptr, fFixupType);                 // type constructor
                   unsigned argCnt = CorSigUncompressData((PCCOR_SIGNATURE&)ptr);               // arg count
                   while(argCnt > 0) {
                       ptr = skipType(ptr, fFixupType);
                       --argCnt;
                   }
               }
               break;                        

        default:
        case ELEMENT_TYPE_END                   :
                _ASSERTE(!"Unknown Type");
                break;
    }
    return(ptr);
}

/**************************************************************************/
void FixupTyPars(PCOR_SIGNATURE pSig, ULONG cSig)
{
    if(TyParFixupList.COUNT() > 0)
    {
        BYTE* ptr = (BYTE*)pSig;
        BYTE* ptrEnd = ptr + cSig;
        while(ptr < ptrEnd)
        {
            ptr = skipType(ptr, TRUE);
        } // end while
    } // end if(COUNT>0)
}
void FixupTyPars(BinStr* pbstype) 
{
    FixupTyPars((PCOR_SIGNATURE)(pbstype->ptr()),(ULONG)(pbstype->length()));
}
/**************************************************************************/
static unsigned corCountArgs(BinStr* args) 
{
    unsigned __int8* ptr = args->ptr();
    unsigned __int8* end = &args->ptr()[args->length()];
    unsigned ret = 0;
    while(ptr < end) 
    {
        if (*ptr != ELEMENT_TYPE_SENTINEL) 
        {
            ptr = skipType(ptr, FALSE);
            ret++;
        }
        else ptr++;
    }
    return(ret);
}

/********************************************************************************/
AsmParse::AsmParse(ReadStream* aIn, Assembler *aAssem) 
{
#ifdef DEBUG_PARSING
    extern int yydebug;
    yydebug = 1;
#endif

    assem = aAssem;
    assem->SetErrorReporter((ErrorReporter *)this);

    assem->m_ulCurLine = 1;
    assem->m_ulCurColumn = 1;
    m_bOnUnicode = TRUE;
    
    wzIncludePath = NULL;
    penv = NULL;

    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hstderr = GetStdHandle(STD_ERROR_HANDLE);

    success = true; 
    _ASSERTE(parser == 0);          // Should only be one parser instance at a time

   // Resolve aliases
   for (unsigned int i = 0; i < sizeof(keywords) / sizeof(Keywords); i++)
   {
       if (keywords[i].token == NO_VALUE)
           keywords[i].token = keywords[keywords[i].tokenVal].token;
   }
    SetSymbolTables();
    Init_str2uint64();
    parser = this;
    //yyparse();
}

/********************************************************************************/
AsmParse::~AsmParse() 
{
    parser = 0;
    delete penv;
    while(m_ANSLast.POP());
}

/**************************************************************************/
DWORD AsmParse::IsItUnicode(CONST LPVOID pBuff, int cb, LPINT lpi)
{
        if(*((WORD*)pBuff) == 0xFEFF)
        {
                if(lpi) *lpi = IS_TEXT_UNICODE_SIGNATURE;
                return 1;
        }
        return 0;
}

/**************************************************************************/
void AsmParse::CreateEnvironment(ReadStream* stream) 
{ 
    penv = new PARSING_ENVIRONMENT;
    memset(penv,0,sizeof(PARSING_ENVIRONMENT));
    penv->in = stream; 
    penv->curLine = 1;
    strcpy_s(penv->szFileName, MAX_FILENAME_LENGTH*3+1,assem->m_szSourceFileName); 
    
    penv->curPos = fillBuff(NULL);
    penv->uCodePage = g_uCodePage;
    
    SetFunctionPtrs();
};

/**************************************************************************/
void AsmParse::ParseFile(ReadStream* stream) 
{ 
    CreateEnvironment(stream);
    yyparse(); 
    penv->in = NULL; 
};

/**************************************************************************/
char* AsmParse::fillBuff(__in_opt __nullterminated char* pos) 
{
    int iPutToBuffer;
    int iOptions = IS_TEXT_UNICODE_UNICODE_MASK;
    g_uCodePage = CP_ACP;
    iPutToBuffer = (int)penv->in->getAll(&(penv->curPos));
    
    penv->endPos = penv->curPos + iPutToBuffer;
    if(iPutToBuffer > 128) iPutToBuffer = 128;
    if(IsItUnicode(penv->curPos,iPutToBuffer,&iOptions))
    {
        g_uCodePage = CP_UTF8;
        if(iOptions & IS_TEXT_UNICODE_SIGNATURE)
        {
            penv->curPos += 2;
        }
        if(assem->m_fReportProgress) printf("Source file is UNICODE\n\n");
        penv->pfn_Sym = SymW;
        penv->pfn_nextchar = nextcharW;
        penv->pfn_NewStrFromToken = NewStrFromTokenW;
        penv->pfn_NewStaticStrFromToken = NewStaticStrFromTokenW;
        penv->pfn_GetDouble = GetDoubleW;
    }
    else
    {
        if(((penv->curPos[0]&0xFF)==0xEF)&&((penv->curPos[1]&0xFF)==0xBB)&&((penv->curPos[2]&0xFF)==0xBF))
        {
            g_uCodePage = CP_UTF8;
            penv->curPos += 3;
            if(assem->m_fReportProgress) printf("Source file is UTF-8\n\n");
            penv->pfn_nextchar = nextcharU;
        }
        else
        {
            if(assem->m_fReportProgress) printf("Source file is ANSI\n\n");
            penv->pfn_nextchar = nextcharA;
        }    
        penv->pfn_Sym = SymAU;
        penv->pfn_NewStrFromToken = NewStrFromTokenAU;
        penv->pfn_NewStaticStrFromToken = NewStaticStrFromTokenAU;
        penv->pfn_GetDouble = GetDoubleAU;
    }
    return(penv->curPos);
}

/********************************************************************************/
BinStr* AsmParse::MakeSig(unsigned callConv, BinStr* retType, BinStr* args, int ntyargs) 
{
    _ASSERTE((ntyargs != 0) == ((callConv & IMAGE_CEE_CS_CALLCONV_GENERIC) != 0));
    BinStr* ret = new BinStr();
    if(ret)
    {
        //if (retType != 0) 
        ret->insertInt8(callConv);
        if (ntyargs != 0)
            corEmitInt(ret, ntyargs);
        corEmitInt(ret, corCountArgs(args));

        if (retType != 0) 
        {
            ret->append(retType); 
            delete retType;
        }
        ret->append(args); 
    }
    else
        error("\nOut of memory!\n");

    delete args;
    return(ret);
}

/********************************************************************************/
BinStr* AsmParse::MakeTypeArray(CorElementType kind, BinStr* elemType, BinStr* bounds) 
{
    // 'bounds' is a binary buffer, that contains an array of 'struct Bounds' 
    struct Bounds {
        int lowerBound;
        unsigned numElements;
    };

    _ASSERTE(bounds->length() % sizeof(Bounds) == 0);
    unsigned boundsLen = bounds->length() / sizeof(Bounds);
    _ASSERTE(boundsLen > 0);
    Bounds* boundsArr = (Bounds*) bounds->ptr();

    BinStr* ret = new BinStr();

    ret->appendInt8(kind);
    ret->append(elemType);
    corEmitInt(ret, boundsLen);                     // emit the rank

    unsigned lowerBoundsDefined = 0;
    unsigned numElementsDefined = 0;
    unsigned i;
    for(i=0; i < boundsLen; i++) 
    {
        if(boundsArr[i].lowerBound < 0x7FFFFFFF) lowerBoundsDefined = i+1;
        else boundsArr[i].lowerBound = 0;

        if(boundsArr[i].numElements < 0x7FFFFFFF) numElementsDefined = i+1;
        else boundsArr[i].numElements = 0;
    }

    corEmitInt(ret, numElementsDefined);                    // emit number of bounds

    for(i=0; i < numElementsDefined; i++) 
    {
        _ASSERTE (boundsArr[i].numElements >= 0);               // enforced at rule time
        corEmitInt(ret, boundsArr[i].numElements);

    }

    corEmitInt(ret, lowerBoundsDefined);    // emit number of lower bounds
    for(i=0; i < lowerBoundsDefined; i++)
        {
                unsigned cnt = CorSigCompressSignedInt(boundsArr[i].lowerBound, ret->getBuff(5));
                ret->remove(5 - cnt);
        }
    delete elemType;
    delete bounds;
    return(ret);
}

/********************************************************************************/
BinStr* AsmParse::MakeTypeClass(CorElementType kind, mdToken tk) 
{

    BinStr* ret = new BinStr();
    _ASSERTE(kind == ELEMENT_TYPE_CLASS || kind == ELEMENT_TYPE_VALUETYPE ||
                     kind == ELEMENT_TYPE_CMOD_REQD || kind == ELEMENT_TYPE_CMOD_OPT);
    ret->appendInt8(kind);
    unsigned cnt = CorSigCompressToken(tk, ret->getBuff(5));
    ret->remove(5 - cnt);
    return(ret);
}
/**************************************************************************/
void PrintANSILine(FILE* pF, __in __nullterminated char* sz)
{
        WCHAR *wz = &wzUniBuf[0];
        if(g_uCodePage != CP_ACP)
        {
                memset(wz,0,dwUniBuf); // dwUniBuf/2 WCHARs = dwUniBuf bytes
                WszMultiByteToWideChar(g_uCodePage,0,sz,-1,wz,(dwUniBuf >> 1)-1);

                memset(sz,0,dwUniBuf);
                WszWideCharToMultiByte(g_uConsoleCP,0,wz,-1,sz,dwUniBuf-1,NULL,NULL);
        }
        fprintf(pF,"%s",sz);
}
/**************************************************************************/
void AsmParse::error(const char* fmt, ...)
{
    char *sz = (char*)(&wzUniBuf[(dwUniBuf >> 1)]);
    char *psz=&sz[0];
    FILE* pF = ((!assem->m_fReportProgress)&&(assem->OnErrGo)) ? stdout : stderr;
    success = false;
    va_list args;
    va_start(args, fmt);

    if((penv) && (penv->in)) psz+=sprintf_s(psz, (dwUniBuf >> 1), "%S(%d) : ", penv->in->namew(), penv->curLine);
    psz+=sprintf_s(psz, (dwUniBuf >> 1), "error -- ");
    _vsnprintf_s(psz, (dwUniBuf >> 1),(dwUniBuf >> 1)-strlen(sz)-1, fmt, args);
    PrintANSILine(pF,sz);
}

/**************************************************************************/
void AsmParse::warn(const char* fmt, ...)
{
    char *sz = (char*)(&wzUniBuf[(dwUniBuf >> 1)]);
    char *psz=&sz[0];
    FILE* pF = ((!assem->m_fReportProgress)&&(assem->OnErrGo)) ? stdout : stderr;
    va_list args;
    va_start(args, fmt);

    if((penv) && (penv->in)) psz+=sprintf_s(psz, (dwUniBuf >> 1), "%S(%d) : ", penv->in->namew(), penv->curLine);
    psz+=sprintf_s(psz, (dwUniBuf >> 1), "warning -- ");
    _vsnprintf_s(psz, (dwUniBuf >> 1),(dwUniBuf >> 1)-strlen(sz)-1, fmt, args);
    PrintANSILine(pF,sz);
}
/**************************************************************************/
void AsmParse::msg(const char* fmt, ...)
{
    char *sz = (char*)(&wzUniBuf[(dwUniBuf >> 1)]);
    va_list args;
    va_start(args, fmt);

    _vsnprintf_s(sz, (dwUniBuf >> 1),(dwUniBuf >> 1)-1, fmt, args);
    PrintANSILine(stdout,sz);
}


/**************************************************************************/
/*
#include <stdio.h>

int main(int argc, char* argv[]) {
    printf ("Beginning\n");
    if (argc != 2)
        return -1;

    FileReadStream in(argv[1]);
    if (!in) {
        printf("Could not open %s\n", argv[1]);
        return(-1);
        }

    Assembler assem;
    AsmParse parser(&in, &assem);
    printf ("Done\n");
    return (0);
}
*/

//#undef __cplusplus

#ifdef _MSC_VER
#ifdef _MSC_VER
#pragma warning(default : 4640)
#endif
#endif


YYSTATIC YYCONST short yyexca[] = {
#if !(YYOPTTIME)
-1, 1,
#endif
	0, -1,
	-2, 0,
#if !(YYOPTTIME)
-1, 448,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 617,
#endif
	274, 540,
	47, 540,
	-2, 222,
#if !(YYOPTTIME)
-1, 638,
#endif
	40, 299,
	60, 299,
	-2, 540,
#if !(YYOPTTIME)
-1, 658,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 681,
#endif
	274, 540,
	47, 540,
	-2, 500,
#if !(YYOPTTIME)
-1, 796,
#endif
	123, 227,
	-2, 540,
#if !(YYOPTTIME)
-1, 821,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 943,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 976,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1289,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1293,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1300,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1421,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1454,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1520,
#endif
	41, 523,
	-2, 300,
#if !(YYOPTTIME)
-1, 1538,
#endif
	41, 523,
	-2, 300,

};

# define YYNPROD 827
#if YYOPTTIME
YYSTATIC YYCONST yyexind_t yyexcaind[] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    4,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    8,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   14,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   20,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   24,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   30,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   34,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   38,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   42,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   46,
    0,    0,    0,   50,    0,    0,    0,    0,    0,    0,
   54,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   58,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   62,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   66,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   70
};
#endif
# define YYLAST 3662

YYSTATIC YYCONST short YYFARDATA YYACT[] = {
  694, 1441, 1106,  635, 1440, 1374, 1439,  191,  275, 1442,
 1010,  870,  953,  748,  779,  221,  693,   26,  411,  720,
   73,   75,  653,  620,  783,  273,  770,  956, 1479,  531,
  150,  473,  751,  746, 1050,  106,  954,  110, 1119,  845,
  685,  599,  655,  262,  176,    6,  771,    5,  668,    3,
  190,   78,   81,  594,  217,  301,   17,  136,  204,  188,
  153,  511,  214,   76,    7, 1526,  219, 1223,   24,  218,
  973,   10, 1176, 1220,  137,   85, 1219,  264,  133, 1042,
 1098, 1096, 1097,  576,   74,  115,   88,   87,  710,   89,
  667,  269,  707,  300,  178,  179,  180,  181,  278, 1043,
  202,  203,   74,  272,   98,  113,  112,  220,  268,   18,
  322,  691,  457,  515,  448,  335,  532,  225,  277,  586,
 1221,  277,  265,  277,  349, 1210, 1211, 1007,  350,   56,
 1208, 1209,  358,  374,  305,  357,  340,  921,  922,   56,
  154,  356,  355,  185,   68,  649,  354,  365,  342,  456,
 1530,   88,   87,  363,   89,  192,  327,  374, 1013,  336,
  371,  339,  345, 1111, 1112,   98, 1495,  359,  377, 1012,
  361,  920,  374,  690,  773,  277,  774,  348,   88,   87,
  277,   89,   86,  689,   88,   87,  271,   89,  105,   84,
  258,  366,  385,  360,  307,  509,  362,  372,  186, 1245,
  413,  610, 1044,  414,  415,  525,  475,  343, 1463,  446,
  657,  447,  656,  465,   56,  463,  467,  466,   74,  758,
  439,   26, 1535,  592,  452, 1453,  476,  477,  386,  595,
 1292,  468,  470, 1182,  818, 1244,  479,  192,  791,  782,
  488,  328,  768,  453,  373,  496,  482,  661,  490,  108,
  429,  216,  425,  487,  426,   88,   87,  486,   89,  431,
   17,  471,  474,   74,  330,  475,  267,  430,    7,  321,
  478,  438,   24, 1519,  432,   10,  507,  507,  524,  530,
 1389,  154,   74,  705,  539,  476,  477,  481,  695,  116,
  536,  636,  637,  494,  268,  310,  312,  314,  316,  318,
 1298, 1297, 1296, 1295,  923,  924,  152,  925,  544,  567,
  489, 1103,  433,   18,  570,  500,  571, 1518,  572,  537,
 1515,  581,  540,  580,  579,  574,  575, 1392,  192,  483,
  582,   88, 1517,  154,   89,  847,  848,  849,  566, 1516,
  569,  568,  417,   46,  418,  419,  420,   80,   79,  596,
 1329,   88,   87,   46,   89, 1486,   88,   87,  329,   89,
  331,  332,  333,  600, 1313,  608,  373,  407,  506,  506,
  523,  529,  607,  266, 1101,   88,   87,  669,   89,  577,
  578, 1157, 1156, 1155, 1154,  619,  606,  177,  493,  937,
   88,   87,  591,   89,   74,  590,  616, 1478,  135,  198,
  602,  603,  604,  605,  199,  595,  200,  514,  345,  609,
  345,  177,  201,  469,  277, 1494,  455,  639,  480,  750,
  614,  615,  617,  630,  337,  338,  177,  495,  195,  801,
 1465,  736,  634,  592,  344,  644,  645,  775,  182,  351,
  352,  353,   56,  196, 1111, 1112,  869, 1369,  739,  740,
  741,  840,  659,  776,  697, 1113,  647,  665,  638,  738,
  852,  583,  662,  952,  933,  676, 1464,  501,   56,  800,
   88,   87,  152,   89,   74,   88,   87, 1165,  936,  742,
  743,  744,  673,  155,  368,  367,  532,  564,   88,  541,
  370,   89,   74,  687,  369,  561,   74, 1472,  777,  684,
  664,  680, 1228, 1224, 1225, 1226, 1227, 1126, 1319,  410,
   74,  177, 1127,  674,   63,  533, 1500, 1233,  696,  449,
 1115,  706,  749, 1215,   63,  683,  515,  681,  508,  516,
 1102,  503,  504, 1489,  527,  534,  646,  528,  692, 1488,
   74,  698,  225,  718,  152, 1487, 1222,  700,  715,  701,
  716,  670,  671,  672, 1475,   74,  538,  719,  846,  652,
 1164,  198,   80,   79,  704,  632,  199,  643,  200, 1484,
  640,  666,  709, 1213,  201, 1482,  152,  725,   80,   79,
  375,  642,  641,  714,  380,  381,  382,  383,   56,  721,
  195, 1480,  349,  711,  821,  538,  350,   49,   50,   51,
   52,   53,   54,   55,   74,  196,  753,   49,   50,   51,
   52,   53,   54,   55,  354,  595,  759,  760,  724,  596,
   80,   79,  156,  157,  158,  588,  634, 1422,  745,  778,
  345,   82,   72,   98,   71,   80,   79,  475,   74,   56,
 1323,   88,   87,   70,   89,  348,   69,  675,  631,  155,
  735,  277,  638,  949,  979,  345,  792,  476,  477,  799,
   67,  808,  152,  537,  812,  809,  805,   66, 1425,  795,
  815,  811,  764,  765,  766,  686,  216,  177,  816,  757,
  277,  806,   49,   50,   51,   52,   53,   54,   55,  225,
  790,  826,  827,  781,  794,  796,  819,  802,  844,  345,
  787,  622,  623,  624, 1305,  978,  810,  817,  587,  839,
  373, 1218,  325,   88,   87,  345,   89,   74,  850, 1414,
   74,  155,  475, 1412,  912,  192,  324,  767,  756,  512,
  472,  843,  625,  626,  627, 1126, 1108,  926,  927,  853,
 1127,  258,  476,  477,   74,   88,   87,  842,   89,  177,
  837,  491,  838,  155, 1304,  841,   74, 1410,  596,  939,
  277, 1126,   74,  364,  932, 1408, 1127,  111,   74, 1391,
  942,  276, 1111, 1112,  764,  277, 1382,   80,   79,  475,
  359,  177, 1381,  928, 1249, 1379,  975,  345,  156,  157,
  158, 1247,   74,  600, 1001,  999,  955, 1368,   74,  476,
  477, 1000,   46,  439,  988,  997,  524,  713, 1003,   88,
   87,  938,   89,  945,   74, 1366,  948,   56, 1356,  950,
  145, 1354,  611,  216,  687,  687,  977,  810,  944, 1352,
 1008,   88,   87,  429,   89,  425,  810,  426, 1350,  155,
 1002, 1023,  431, 1348, 1018, 1005, 1021, 1004,  793, 1025,
  430,   74,  814,   46,  438,  987,  996,  432,  985,  995,
  156,  157,  158, 1016, 1019, 1020, 1031,  177, 1315, 1316,
 1317, 1346,  825,   88,   87, 1344,   89, 1324,   88, 1036,
  334,   89,   74,  989, 1049, 1045, 1046, 1047, 1048, 1342,
  810, 1340,  156,  157,  158,  433,  986,  994,  523,  663,
   88,   87, 1337,   89, 1334,  944, 1332,  351,  352,  353,
   49,   50,   51,   52,   53,   54,   55, 1328, 1312, 1178,
 1030, 1029,  914, 1028,  915,  916,  917,  918,  919, 1027,
  326,  323, 1107, 1052, 1009, 1053,   49,   50,   51,   52,
   53,   54,   55,  813,  807, 1109,  981,  982,  983,  984,
  728,  613,  612,  753,   80,   79,  475, 1110,  573,  349,
  560,  145, 1039,  350, 1104, 1217, 1117, 1114, 1123, 1125,
   61,   62,   47,   63,  308,  451,  476,  477,  156,  157,
  158,  354,  444,  109,   58,   32,   59, 1501, 1163, 1281,
   88,   87, 1118,   89,   74,   92,  441, 1301, 1039, 1167,
 1166,  442,    1, 1466,   41,   43,  686,  686, 1522, 1162,
  946, 1040,  348,  145, 1201, 1038, 1039,   88,   87, 1177,
   89,   61,   62,   47,   63, 1185,  434,  435, 1279, 1011,
  277,   46, 1169, 1170, 1171, 1172, 1212, 1310, 1206, 1184,
 1282, 1214, 1173, 1174, 1175, 1390,  443, 1040, 1179, 1205,
 1277, 1207, 1037,  152, 1204, 1216,   49,   50,   51,   52,
   53,   54,   55, 1275, 1160, 1040, 1116, 1158,   88,   87,
  205,   89, 1152,  192,  192,  192,  192,  974, 1150, 1280,
  440,  437, 1148,  192,  192,  192,  118,  119,  120,  121,
  122,  123,  124,  125,  126,  127,  128,  129,  130,  131,
  132, 1278,  935, 1100, 1229,  703,  761,   49,   50,   51,
   52,   53,   54,   55, 1276, 1161,  702,  349, 1159,  761,
 1146,  350, 1144, 1153, 1168,   56, 1142, 1375, 1232, 1151,
 1246, 1140, 1250, 1149, 1252, 1254, 1255,  762, 1258,  354,
 1260, 1261, 1262, 1263, 1264, 1265, 1266, 1236, 1268, 1269,
 1270, 1271, 1272, 1273, 1274, 1240, 1248, 1235, 1513, 1283,
 1284, 1138, 1286, 1285, 1136, 1257, 1253, 1259, 1256, 1251,
  348, 1147, 1506, 1145,  586, 1267,  584, 1143,  993, 1288,
  991,  992, 1141,  699, 1291, 1134, 1294, 1502, 1180,  629,
 1181,  409,  931, 1132, 1130, 1200, 1511,  585, 1128,   61,
   62,   47,   63,  317,  315,  187,  621,  277,  313,  311,
 1512,  309, 1139,  306, 1387, 1137, 1306,  784, 1308, 1309,
 1299,  621,   88,   87, 1386,   89,  376,  810, 1385,  277,
  155, 1314, 1318, 1376, 1311,   56, 1135, 1503, 1504, 1505,
 1507, 1508, 1509, 1510, 1133, 1131, 1320, 1203, 1125, 1129,
 1034, 1033,  829,  737,  308,  308,   97, 1321,  177,  308,
  308, 1467,  308, 1302,  308,  660,  450, 1238, 1468, 1325,
   45,  586, 1514,   44,  351,  352,  353, 1358, 1359, 1360,
 1361, 1362, 1363, 1364, 1322,   49,   50,   51,   52,   53,
   54,   55, 1120,  934,  277,   56, 1372, 1293,  349,  258,
 1371,  786,  350,  277, 1357,  347, 1378, 1427,  930,  586,
 1426, 1383, 1370,  412,  586, 1307, 1373,   56,  824,  140,
  354, 1380,  139, 1377,  349, 1528,  823,  586,  763,  804,
 1017,   56,  586,  913,  755,  586, 1125,  810,  712,   88,
   87,   94,   89,  972,  971,  970,  354,  965,  964,  963,
  962,  348,  960,  961,  105,  562,  969,  968,  967,  966,
  346,  998,  138,  959,  957,  193,  586,  117,  194,  156,
  157,  158,  303, 1540, 1531, 1529, 1521,  348, 1419, 1499,
 1462,  258, 1461,  688, 1460, 1429, 1430, 1431, 1432, 1424,
 1418,  198,  177, 1423, 1428, 1420,  199,  497,  200, 1417,
 1413, 1411, 1409, 1407,  201, 1394, 1388, 1445, 1384, 1444,
 1443,  134, 1367, 1365, 1355, 1353, 1446, 1451, 1351, 1349,
  195, 1347, 1345, 1457,  958, 1343, 1341, 1433, 1339, 1338,
 1336, 1455,  351,  352,  353,  196,   88, 1335, 1474,   89,
 1459, 1481, 1483, 1485, 1456, 1481, 1483, 1485, 1333,   46,
 1331, 1492, 1483, 1491, 1490, 1496, 1330, 1497, 1327, 1326,
 1493, 1469, 1538,  189,  526, 1303, 1290, 1477, 1287, 1473,
 1476, 1237, 1234, 1471, 1202, 1099, 1024, 1498, 1022, 1015,
   56, 1014, 1006,  951,  941,  940,  929,  851,  834,  833,
  831,  828, 1481, 1483, 1485,   97,   96,  822,  810,  104,
  103,  102,  101,  820,   99,  100,  105,  803,  785,   46,
  769,  733,   56,  732,  731,  730,  867,  729,   88,  727,
  726,   89,  679,  861, 1527,  862,  863,  864, 1524,  633,
  565,   46,  522,  422,  421,  341,  320, 1523, 1532, 1536,
 1520, 1533, 1537, 1525,  258,   46,  206, 1454, 1450, 1449,
 1448, 1447, 1534, 1421, 1416, 1415, 1406, 1405, 1404, 1403,
 1402, 1539,  856,  857,  858,  245,  246,  247,  248,  249,
  250,  251,  252,  253,  254,  255,  256,  257,  445, 1401,
  810, 1400,   59, 1399, 1398, 1397,  208,  259,  210,  228,
  212,  213, 1396,   49,   50,   51,   52,   53,   54,   55,
   41,   43, 1395,  855,  859,  860, 1393,  865, 1281, 1279,
  866, 1277, 1275,  351,  352,  353, 1300,   61,   62,   47,
   63, 1160, 1158,   56, 1152, 1150, 1148, 1146,  223, 1144,
 1142,  518, 1140, 1138, 1136, 1134, 1289, 1231,  520,  351,
  352,  353,  222, 1230, 1051, 1041, 1035,  245,  246,  247,
  248,  249,  250,  251,  252,  253,  254,  255,  256,  257,
 1026,  976,  943,  836,   59,  835,   56,  832,  208,  259,
  210,  228,  212,  213,  830,  723,  226,  224,  722,  708,
   63,   88,   41,   43,   89,  682,  678,  677,  658,  628,
  519,   56,  193,  521,   46,  194,   41,   43,  598,   61,
   62,   47,   63,   49,   50,   51,   52,   53,   54,   55,
  223,  597,  589,  563,  543,  542,   63,  492,  198,  177,
  416,  408,  384,  199,  222,  200,   46,  319,  499,  304,
  508,  201,  517,  503,  504,   96,  302,  510,  104,  103,
  102,  101,  513,   99,  100,  105,  505,  195,  502,  498,
   36,  184,   93,   33,  464,  462,  461,  423,  226,  224,
  460,  459,  196,   49,   50,   51,   52,   53,   54,   55,
   95,  244,  458,  227,  243,  215,  209,  207,  211,   31,
  990,  980,  436,  789,  428,   49,   50,   51,   52,   53,
   54,   55,  788,  427,  424,  535,  270,   83,   29,   49,
   50,   51,   52,   53,   54,   55,   57,   34,   25,   16,
  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,
  255,  256,  257,  263,   15,   14,  261,   59,   13,  260,
   12,  208,  259,  210,  228,  212,  213,   46,   11,    9,
    8,    4,    2,  234,  242,   41,   43,  241,  240,  239,
  238,   88,   87,  237,   89,   38,   30,   58,   32,   59,
  236,  235,   61,   62,   47,   63, 1071,  233,  232,  231,
  230,  229,  114,  223,   77,   42,  747,   41,   43,  651,
   46,  650, 1458,  299,   90,  406,  183,  222, 1124,  752,
  780, 1239,  947, 1122,   61,   62,   47,   63, 1121,  601,
 1436,   60,   35,  522, 1435,   46, 1434,   21, 1452, 1438,
   37, 1437,  389, 1183, 1105,  107,  404,   39,   40,  387,
  388,  226,  224,  593,  391,  392,  402,  393,  394,  395,
  396,  397,  398,  399,  400,  390,  654,  772,   91,  403,
 1054,  734,   65,   64,  197,  868,    0,  401,   49,   50,
   51,   52,   53,   54,   55,    0,  405,    0,    0,  146,
    0,    0,    0,    0,    0,    0,   38,   30,   58,   32,
   59,   19,   20,    0,   22,   23,   48,    0,   27,   28,
   49,   50,   51,   52,   53,   54,   55,    0,   41,   43,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   61,   62,   47,   63,  520,
    0,    0,   60,   35,    0,    0,    0,    0,   21,    0,
    0,   37,    0,    0,    0,    0,    0,    0,   39,   40,
    0,    0,    0,    0,  444,    0,   58,   32,   59,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  441,    0,
    0,   63,    0,  442,    0,    0,   41,   43,    0,    0,
    0,  519,    0,    0,  521,    0,    0,    0,    0,    0,
    0,    0,    0,   61,   62,   47,   63,    0,  434,  435,
    0, 1082,   19,   20,    0,   22,   23,   48,    0,   27,
   28,   49,   50,   51,   52,   53,   54,   55,  443,    0,
 1058, 1059,    0, 1066, 1080, 1060, 1061, 1062, 1063,    0,
 1064, 1065,    0, 1081, 1067, 1068, 1069, 1070,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  440,  437,   49,   50,   51,   52,   53,   54,
   55,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  884,    0,    0,    0,    0,    0,    0,   49,
   50,   51,   52,   53,   54,   55,  911,  146,    0,    0,
    0,  876,  877,    0,  885,  902,  878,  879,  880,  881,
    0,  882,  883,    0,  903,  886,  887,  888,  889,    0,
    0,    0,    0,    0,    0,    0,    0,  175,    0,    0,
    0,  151,  148,  163,  161,  170,    0,  164,  165,  166,
  167,    0,  168,  169,    0,    0,  171,  172,  173,  174,
  559,    0,    0,  142,  162,  152,    0, 1470,  900,    0,
  904,    0,  141,  147,    0,  906,    0,    0,    0,    0,
    0,    0,    0,    0,  345,    0,  908,  143,  144,  149,
    0,  551,    0,  545,  546,  547,  548,    0,    0, 1199,
 1198, 1193,  146, 1192, 1191, 1190, 1189,  349, 1187, 1188,
  105,  763, 1197, 1196, 1195, 1194,    0,  160,    0,  909,
    0, 1186,    0,    0,    0,  553,  554,  555,  556,  354,
    0,  550,    0,    0,    0,  557,  558,  549,    0,    0,
 1056, 1057, 1032, 1072, 1073, 1074,    0, 1075, 1076,    0,
    0, 1077, 1078,    0, 1079,    0,    0,    0,    0,    0,
  618,    0,    0,    0,    0,    0,    0, 1055, 1083, 1084,
 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094,
 1095,  972,  971,  970,    0,  965,  964,  963,  962,    0,
  960,  961,  105,    0,  969,  968,  967,  966,    0,    0,
    0,  959,  957,    0,    0,    0,    0,    0,  871,    0,
  872,  873,  874,  875,  890,  891,  892,  907,  893,  894,
  895,  896,  897,  898,  899,  901,  905,    0,    0,    0,
  910,    0,    0,    0,   88,   87,    0,   89,  552,    0,
    0,    0,  155,    0,    0,  175,    0,    0,    0,  151,
  148,  163,  161,  170,    0,  164,  165,  166,  167,    0,
  168,  169,  958,    0,  171,  172,  173,  174,    0,    0,
  177,  142,  162,  146,    0,    0,    0,    0,  349,    0,
  141,  147,  350,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  143,  144,  149,    0,    0,
  354,    0,    0,  972,  971,  970,    0,  965,  964,  963,
  962,    0,  960,  961,  105,    0,  969,  968,  967,  966,
    0,    0,    0,  959,  957,  160,  159,    0,    0,   88,
   87,  618,   89,    0,    0,    0,    0,  155,    0,    0,
  175,    0,    0,    0,  151,  148,  163,  161,  170,    0,
  164,  165,  166,  167,    0,  168,  169,    0,    0,  171,
  172,  173,  174,    0,    0,  177,  142,  162,    0,    0,
    0,    0,    0,    0,    0,  141,  147,    0,    0,    0,
    0,  156,  157,  158,  958,    0,    0,    0,    0,    0,
  143,  144,  149,    0,    0,  867,    0,    0,    0,    0,
    0,    0,  861,    0,  862,  863,  864,    0,  146,    0,
    0,    0,    0,  349,    0,    0,    0,  350,   63,    0,
  160,    0,  351,  352,  353,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  354,  636,  637,    0,    0,
    0,  856,  857,  858,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  618,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  156,  157,  158,    0,
    0,    0,  855,  859,  860,    0,  865,    0,    0,  866,
    0,    0,    0,    0,    0,    0,    0,  146,    0,    0,
   88,   87,    0,   89,    0,    0,    0,    0,  155,    0,
    0,  175,    0,    0,    0,  151,  148,  163,  161,  170,
    0,  164,  165,  166,  167,    0,  168,  169,    0,    0,
  171,  172,  173,  174,    0,    0,  177,  142,  162,    0,
    0,    0,    0,    0,    0,    0,  141,  147,    0,    0,
    0,    0,    0,    0,    0,  274,    0,    0,    0,    0,
    0,  143,  144,  149,    0,    0,    0,    0,  146,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  160,    0,  351,  352,  353,    0,    0,    0,    0,
    0,    0,    0,  175,    0,    0,    0,  636,  637,  163,
  161,  170,    0,  164,  165,  166,  167,    0,  168,  169,
    0,    0,  171,  172,  173,  174,  274,    0,    0, 1242,
  162,    0,    0,    0,    0,   88,   87,    0,   89,    0,
    0,    0,    0,  155,    0,    0,  175,  156,  157,  158,
  151,  148,  163,  161,  170,    0,  164,  165,  166,  167,
    0,  168,  169,  146,    0,  171,  172,  173,  174,    0,
    0,  177,  142,  162,    0,    0, 1243,    0,    0,    0,
    0,  141,  147,    0,    0,    0,    0,    0,    0,    0,
 1241,    0,    0,    0,    0,    0,  143,  144,  149,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  274,    0,    0,   88,   87,  160,   89,  351,  352,
  353,    0,  155,    0,  146,  175,    0,    0,    0,  151,
  148,  163,  161,  170,    0,  164,  165,  166,  167,    0,
  168,  169,    0,    0,  171,  172,  173,  174,    0,    0,
  177,  142,  162,    0,    0,    0,    0,    0,    0,    0,
  141,  147,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  156,  157,  158,  143,  144,  149,    0,  798,
    0,    0,  274,    0,    0,   88,   87,    0,   89,    0,
    0,    0,    0,  155,    0,  146,  175,    0,    0,    0,
  151,  148,  163,  161,  170,  160,  164,  165,  166,  167,
    0,  168,  169,    0,    0,  171,  172,  173,  174,  797,
    0,  177,  142,  162,    0,    0,    0,    0,    0,    0,
    0,  141,  147,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  143,  144,  149,    0,
    0,    0,    0,  274,    0,    0,    0,    0,    0,    0,
    0,  156,  157,  158,    0,    0,  146,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  160,  159,    0,    0,
   88,   87,  854,   89,    0,    0,    0,    0,  155,    0,
    0,  175,    0,    0,    0,  151,  148,  163,  161,  170,
    0,  164,  165,  166,  167,    0,  168,  169,    0,    0,
  171,  172,  173,  174,    0,    0,  177,  142,  162,    0,
    0,    0,    0,    0,  274,    0,  141,  147,    0,    0,
    0,    0,  156,  157,  158,    0,    0,    0,    0,    0,
    0,  143,  144,  149,    0,    0,    0,    0,    0,    0,
    0,   88,   87,    0,   89,    0,    0,    0,    0,  155,
    0,    0,  175,    0,    0,    0,  151,  148,  163,  161,
  170,  160,  164,  165,  166,  167,    0,  168,  169,    0,
    0,  171,  172,  173,  174,    0,    0,  177,  142,  162,
    0,    0,    0,    0,    0,    0,    0,  754,  147,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  146,    0,
    0,    0,  143,  144,  149,    0,    0,    0,    0,    0,
    0,    0,   88,   87,    0,   89,    0,  156,  157,  158,
  155,    0,    0,  175,    0,    0,    0,  151,  148,  163,
  161,  170,  160,  164,  165,  166,  167,    0,  168,  169,
    0,    0,  171,  172,  173,  174,    0,    0,  177,  142,
  162,    0,    0,  146,    0,    0,    0,    0,  141,  147,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  143,  144,  149,    0,    0,    0,    0,
    0,    0,    0,   88,   87,    0,   89,    0,  156,  157,
  158,  155,    0,    0,  175,    0,    0,    0,  151,  148,
  163,  161,  170,  648,  164,  165,  166,  167,  146,  168,
  169,    0,    0,  171,  172,  173,  174,    0,    0,  177,
  142,  162,    0,    0,    0,  867,    0,    0,    0,  141,
  147,    0,  861,    0,  862,  863,  864,    0,    0,    0,
    0,    0,    0,    0,  143,  144,  149,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  297,  198,    0,  156,
  157,  158,  199,  146,  200,    0,  717,    0,    0,    0,
  201,  856,  857,  858,  454,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  195,  284,    0,  279,
  280,  281,  282,  283,    0,    0,    0,    0,  287,    0,
    0,  196,    0,    0,    0,    0,    0,  285,    0,  295,
    0,  286,  855,  859,  860,    0,  865,    0,    0,  866,
    0,  288,  289,  290,  291,  292,  293,  294,  298,    0,
  156,  157,  158,    0,  296,  484,  175,  485,    0,    0,
  151,  148,  163,  161,  170,    0,  164,  165,  166,  167,
    0,  168,  169,    0,    0,  171,  172,  173,  174,    0,
    0,  177,  142,  162,    0,    0,    0,    0,    0,    0,
    0,  141,  147,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  143,  144,  149,    0,
  378,  175,  379,    0,    0,  151,  148,  163,  161,  170,
    0,  164,  165,  166,  167,    0,  168,  169,    0,    0,
  171,  172,  173,  174,    0,    0,  160,  142,  162,    0,
    0,    0,    0,    0,    0,    0,  141,  147,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  143,  144,  149,    0,    0,  175,    0,    0,    0,
  151,  148,  163,  161,  170,    0,  164,  165,  166,  167,
    0,  168,  169,    0,    0,  171,  172,  173,  174,    0,
    0,  160,  142,  162,    0,    0,    0,    0,    0,    0,
    0,  141,  147,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  143,  144,  149,    0,
    0,  175,    0,    0,    0,  151,  148,  163,  161,  170,
    0,  164,  165,  166,  167,    0,  168,  169,    0,    0,
  171,  172,  173,  174,    0,    0,  160,  142,  162,    0,
    0,    0,    0,    0,    0,    0,  141,  147,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  143,  144,  149,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  160
};

YYSTATIC YYCONST short YYFARDATA YYPACT[] = {
-1000, 1564,-1000,  544,  537,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,  523,  520,  511,  509,-1000,-1000,-1000,   18,
   18, -457,   83,   83,-1000,-1000,-1000,  508,-1000, -125,
  640,-1000,  904, 1218,  -18,  892,   18, -357, -358,-1000,
 -174,  660,  -18,  660,-1000,-1000,-1000,  135, 2134,  640,
  640,  640,  640,-1000,-1000,  176,-1000,-1000,-1000, -175,
 1144,-1000,-1000, 1423,  -18,  -18,-1000,-1000, 1421,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,   18,  -82,-1000,-1000,
-1000,-1000,  381, -129, 2790, 1183,-1000,-1000,-1000,-1000,
 3042,-1000,   18,-1000, 1457,-1000, 1330, 1689,  -18, 1173,
 1171, 1169, 1168, 1164, 1163, 1687, 1495,    2,-1000,   18,
  668, -109,-1000,-1000,   96, 1183,  640, 2790,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000, 1494,  146, 1316,  921, -242, -243, -249,
 -252,  381,-1000, -113,  381, 1096,  730,-1000,-1000,  205,
-1000, 3310,  115, 1179,-1000,-1000,-1000,-1000,-1000, 3200,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
  303,-1000,-1000,-1000,-1000,-1000, 1183, 1682,  368, 1183,
 1183, 1183,-1000, 1591,   71,-1000,-1000, 1681, 1130, 2695,
-1000, 3310,-1000,-1000,-1000,  266,  266,-1000, 1680,-1000,
-1000,   41, 1493, 1492, 1632, 1453,-1000,-1000,   18,-1000,
   18,   74,-1000,-1000,-1000,-1000, 1208,-1000,-1000,-1000,
-1000,-1000,  884,   18, 3003,-1000,   58,  -96,-1000,-1000,
  228,   18,   83,  690,  -18,  228, 1096, 3145, 2790, -100,
  266, 2695, 1677,-1000,  618,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
 1272,   70, 1607,   80,-1000,   72,-1000,  390,  381,-1000,
-1000, 2790,-1000,-1000,   91, 1260,  266,  640,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000, 1675, 1674, 1926,
  867,  370, 1311, 1673,   71, 1489,  -60,-1000,   18,  -60,
-1000,   83,-1000,   18,-1000,   18,-1000,   18,-1000,-1000,
-1000,-1000,  865,-1000,   18,   18,-1000, 1183,-1000, -385,
-1000,-1000,-1000,-1000, -109,   -5,   77,-1000,-1000, 1183,
 1136,-1000, 1323,  585, 1672,-1000,  132,  640,  340,-1000,
-1000,-1000, 1671, 1658, 3310,  640,  640,  640,  640,-1000,
  381,-1000,-1000, 3310,  210,-1000, 1183,-1000,  -79,-1000,
-1000, 1260,  729,  859,  858,  640,  640, 2535,-1000,-1000,
-1000,-1000,-1000,-1000,   18, 1323, 1161,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,  373,-1000,-1000,-1000, 1649, 1128,-1000,  525,
 1488,-1000,-1000, 2400,-1000,-1000,   18,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,  459,  458,  444,-1000,
-1000,-1000,-1000,-1000,   18,   18,  413, 2932,-1000,-1000,
 -213,-1000,-1000,-1000,-1000,-1000,-1000,-1000,  -63, 1648,
-1000,   18, 1207,  -27,  266,  808,   18,-1000,  -96,   67,
   67,   67,   67, 2790,  618,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000, 1647, 1646, 1481,-1000,
-1000,-1000, 2535,-1000,-1000,-1000,-1000, 1323, 1645,  -18,
 3310,-1000,  228, 1258,-1000, -133, -143,-1000,-1000, -349,
-1000,-1000,  -18,  246,  393,  -18,-1000,-1000, 1122,-1000,
-1000,  -18,-1000,  -18,-1000, 1055, 1044,-1000,-1000,  640,
 -180, -371, 1639,-1000,-1000,-1000,-1000,  640, -375,-1000,
-1000, -343,-1000,-1000,-1000, 1294,-1000,  714,  640, 3310,
 1183, 3255,   18,  148, 1079,-1000,-1000,-1000,-1000,-1000,
-1000,-1000, 1638,-1000,-1000,-1000,-1000,-1000,-1000, 1635,
-1000,-1000, 1457,  148, 1479,-1000, 1478,  857, 1476, 1474,
 1473, 1472, 1470,-1000,  387, 1195,-1000,   75, 1183,-1000,
-1000,-1000,  120,  640,  148,  399,  157, 2861,-1000,-1000,
 1290, 1183,-1000,  635,-1000,-1000,  -56, 2790, 2790, 1075,
 1260,-1000, 1183, 1183, 1183, 1183,-1000, 2229,-1000, 1183,
-1000,  640,  640,  640,  634, 1183,  -32, 1183,  130, 1469,
-1000,  131,-1000,-1000,-1000,-1000,-1000,-1000,   18,-1000,
 1323,-1000,-1000, 1096,  -35, 1157,-1000,-1000, 1183, 1467,
 1176,-1000,-1000,-1000,-1000,-1000,-1000,  -36,  266,  757,
 2624,  104,   -5, 1466, 1285,-1000,-1000, 3255,  -63,  851,
   18,  -76, 3310,   18, 1183,  850,-1000,-1000,-1000,  228,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,   18,   83,-1000,
  -40, 1183,  148, 1462,  554, 1456, 1282, 1274,-1000,   71,
   18,   18, 1450, 1194,-1000,-1000, 1323, 1634, 1449, 1627,
 1448, 1447, 1625, 1623, 1183,  640,-1000,  640,   18,  134,
  640,  -18, 2790,  640,  605, 1286,   86, -162, 1446,   76,
 3011,  129, 1895,   18,-1000, 1292,-1000,  883,-1000,  883,
  883,  883,  883,  883, -146,-1000,   18,   18,  640,-1000,
-1000,-1000,-1000,-1000,-1000, 1183, 1445, 1264, 1131,-1000,
-1000,  339, 1249, 1041,  215,  126,-1000,  -46,   18, 1444,
 1443, 3310,-1000, 1622, 1179, 1179, 1179,  640,  640,-1000,
  948,  613,  131,-1000,-1000,-1000,-1000,-1000, 1442,  338,
 1066, 1016,  -76, 1621, 3310,-1000,-1000,  580,  529,  758,
 1236,  -76, 3310,   18, 1183,  640, 1183,-1000,-1000, 3310,
-1000,-1000, 1183,-1000,  -63,   86, 1441, -257,-1000,-1000,
 1183, 2535,  841,  968, -147, -158, 1440, 1438,  640, 1289,
-1000,  -63,-1000,  228,  228,-1000,-1000,-1000,-1000,  246,
-1000,-1000,-1000,-1000,-1000,-1000,-1000, 1183, 1183, 1437,
   18, 1183, 1435,-1000,  640,  -76, 1620,  836,  830,  828,
  827,-1000,  148, 2261,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000, 1193, 1192, 1606,  968,   71,
  974, 1605, -395,  -75,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,  604,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000, 1604, 1604,-1000,
 1604, 1824,-1000,-1000, -408,-1000, -399,-1000,-1000, -413,
-1000,-1000, 1434,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
   71,-1000,-1000,-1000,-1000,-1000,  111,  267, 1183,-1000,
  148,  453,  330,-1000, 2861,  397, 1005,-1000,-1000,-1000,
-1000,-1000, 1260,  -63, 1179, 1183,-1000,  640, 1248, 2790,
-1000,-1000,-1000,  409,-1000,-1000,-1000, 1158, 1154, 1153,
 1145, 1124, 1121, 1091, 1086, 1082, 1080, 1042, 1038, 1032,
  100, 1027, 1024,  -18,  437, 1157,  -63, 1062,-1000,-1000,
-1000, 1096, 1096, 1096, 1096,-1000,-1000,-1000,-1000,-1000,
-1000, 1096, 1096, 1096,-1000,-1000,-1000,-1000,-1000, -430,
 2535,  826, 1183, 1079,-1000,   71,-1000,   71,  -41,-1000,
-1000, 1982,   71,   18,-1000,-1000, 1183,-1000, 1433,-1000,
-1000, 1189,-1000,-1000, -298, 1146, 1895,-1000,-1000,-1000,
-1000, 1323,-1000, -256, -261,   18,-1000,-1000,-1000,-1000,
  480,  148,  874,  620,-1000,-1000,-1000,-1000,-1000,-1000,
-1000, -420,-1000,-1000,   29,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,  222,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,   18,-1000,-1000,-1000,-1000, 1603, 1323,
 1597,-1000,-1000,-1000,-1000,-1000,  394, 1431, 1248,-1000,
  131, 1430, 1223,-1000, 2492,-1000,-1000,-1000,  -64,   18,
  485,   18, 2064,   18,   -1,   18,  371,   18,   83,   18,
   18,   18,   18,   18,   18,   18,   83,   18,   18,   18,
   18,   18,   18,   18, 1023, 1010,  988,  949,   18,   18,
 -156,   18, 1427, 1323,-1000,-1000, 1596, 1425,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,  266,  -44,-1000, 1257,
-1000,-1000,  -76,-1000,-1000, 1323,-1000, 1595, 1594, 1593,
 1592, 1590, 1589,   19, 1587, 1586, 1585, 1584, 1582, 1581,
-1000,-1000,-1000,  246,-1000, 1576,  956, 1222,-1000,-1000,
-1000,-1000, 1424,-1000,  661,   18, 1271,   18,   18,  976,
  148,  825,-1000,-1000,-1000,-1000,-1000,-1000,-1000,  101,
   18,  587,  383,-1000,-1000,-1000,-1000,-1000, 2790,  549,
-1000,-1000,-1000,  571, 1418, 1417,  824,   87, 1415, 1409,
  813, 1407,  811, 1396, 1389,  809, 1388, 1387,  798, 1385,
  796, 1384,  782, 1381,  778, 1380,  750, 1378,  745, 1377,
  736, 1374,  728, 1373,  725,   83,   18,   18,   18,   18,
   18,   18,   18, 1372,  722, 1371,  704,-1000,  322,  -63,
-1000, 3310,  -76,  -63, 1067, 1572, 1571, 1569, 1568, 1175,
  -63,-1000,-1000,-1000,-1000,   18,  692,  148,  689,  683,
   18, 1323,-1000,-1000, 1367, 1170, 1166, 1156, 1365,-1000,
  155,-1000,  984,  676,   64,-1000,-1000,-1000, 1566, 1364,
-1000,-1000, 1562,-1000, 1552,-1000,-1000, 1545,-1000,-1000,
 1544,-1000, 1543,-1000, 1541,-1000, 1539,-1000, 1520,-1000,
 1519,-1000, 1518,-1000, 1517,-1000, 1516, 1362,  672, 1361,
  664, 1360,  630, 1359,  626,-1000, 1515,-1000, 1514,-1000,
 1358, 2535, 1067, 1354, 1513,  536,  246, 1348,  575,-1000,
 1266,-1000, 1895, 1344,-1000,   18,   18,   18,-1000,-1000,
 2064,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000, 1511,-1000,
 1510,-1000, 1509,-1000, 1508,-1000,-1000,-1000,  -49, 1507,
  968,  -63,   18,-1000,-1000,-1000,  148,-1000,  920,-1000,
 1343, 1341, 1339,-1000,  167,  962, 2186,  456,  513,  356,
  550,  534,  528,  314,  504,  498,  492,-1000,-1000,-1000,
-1000,  374,  125,  -76,  -63,-1000, 1338,  423,  864, 1228,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,   57,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
  298,  291,  276,  232,-1000,-1000,-1000, 1500, 1335,-1000,
  946,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000, 1497,  148,-1000,-1000,-1000,-1000,-1000,
  -63, -437,-1000,   18, 1284, 1334, -201, 1333,-1000,-1000,
  266,-1000, 3310, 2535,  -52,  -76, 1067, 1422,  -63, 1332,
-1000
};

YYSTATIC YYCONST short YYFARDATA YYPGO[] = {
    0,   44,  182,    3, 1945,   60,   40,    7, 1944,    0,
 1943, 1942, 1941,  210,  194, 1940, 1938,    2, 1937,   46,
   34,    5,   31,   28,   22, 1936,   42,    8,   53, 1923,
   35, 1915,   11,   10,   13, 1914,   39, 1913,   36,   27,
 1911, 1909,    9,    1,    4,    6, 1908, 1906, 1904, 1900,
   30,   24,   41, 1899, 1898, 1893, 1892,   14, 1891, 1890,
   12, 1889,   32, 1888,   16,   38,   26,   23,   25,   18,
  509,   59, 1313,   50,   91, 1886, 1884, 1883, 1882, 1881,
 1879,   19,   33, 1876, 1367, 1875, 1874,   29,  767,  115,
 1872, 1273, 1270, 1871, 1870, 1869, 1868, 1867, 1861, 1860,
 1853, 1850, 1849, 1848, 1847, 1844, 1843, 1002, 1842,   49,
   58, 1841,   47,  144,   45,   62, 1840, 1839,   69, 1838,
 1830, 1829, 1828, 1826, 1825,   43, 1824, 1823, 1809,   54,
  107,   66, 1808,   15,  266, 1807, 1806, 1798, 1797, 1796,
 1795, 1794, 1793, 1792, 1784, 1783, 1782,  771, 1781, 1780,
 1779, 1778, 1777, 1776, 1775,   48, 1774, 1773,  112, 1772,
 1771, 1761,   90, 1760, 1756, 1755, 1754, 1753, 1752, 1751,
   55, 1736,   70, 1750,   61, 1749,  467, 1748, 1746, 1742,
 1737, 1631, 1464
};
YYSTATIC YYCONST yyr_t YYFARDATA YYR1[]={

   0, 107, 107, 108, 108, 108, 108, 108, 108, 108,
 108, 108, 108, 108, 108, 108, 108, 108, 108, 108,
 108, 108, 108, 108, 108, 108, 108, 108, 108, 134,
 134,  34,  34, 131, 131, 131,   2,   2,   1,   1,
   1,   9,  23,  23,  22,  22,  22, 132, 132, 132,
 132, 132, 133, 133, 133, 133, 133, 133, 133, 133,
 133,  91,  91,  91,  91,  92,  92,  92,  92,  10,
  11,  71,  70,  70,  57,  59,  59,  59,  60,  60,
  60,  63,  63, 130, 130, 130,  58,  58,  58,  58,
  58,  58, 128, 128, 128, 117,  12,  12,  12,  12,
  12,  12, 116, 135, 111, 136, 137, 109,  75,  75,
  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,
  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,
  75,  75,  75,  75, 138, 138, 139, 139, 110, 110,
 140, 140,  54,  54,  55,  55,  67,  67,  18,  18,
  18,  18,  18,  19,  19,  66,  66,  65,  65,  56,
  21,  21, 141, 141, 141, 141, 141, 141, 141, 141,
 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
 114,  77,  77,  77,  77,  77,  77,  77,  77,  77,
  77,  77,  77,  77,  77,  77,  77,   4,   4,  33,
  33,  16,  16,  73,  73,  73,  73,  73,   7,   7,
   7,   7,   8,   8,   8,   8,   8,   8,   8,  74,
  72,  72,  72,  72,  72,  72, 142, 142,  79,  79,
  79, 143, 143, 148, 148, 148, 148, 148, 148, 148,
 148, 144,  80,  80,  80, 145, 145, 149, 149, 149,
 149, 149, 149, 149, 150,  36,  36, 112,  76,  76,
  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,
  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,
  76,  81,  81,  81,  81,  81,  81,  81,  81,  81,
  81,  81,  81,  81,  81,  81,  81,   3,   3,   3,
  13,  13,  13,  13,  13,  78,  78,  78,  78,  78,
  78,  78,  78,  78,  78,  78,  78,  78, 151, 113,
 113, 152, 152, 152, 152, 152, 152, 152, 152, 152,
 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
 152, 152, 152, 152, 152, 155, 156, 153, 158, 158,
 157, 157, 157, 160, 159, 159, 159, 159, 163, 163,
 163, 166, 161, 164, 165, 162, 162, 162, 115, 167,
 167, 169, 169, 169, 168, 168, 170, 170,  14,  14,
 171, 171, 171, 171, 171, 171, 171, 171, 171, 171,
 171, 171, 171, 171, 171,  39,  39,  39,  39,  39,
  39,  39,  39,  39,  39,  39,  39,  39,  39,  39,
  39,  39,  39,  39, 172,  30,  30,  31,  31,  37,
  37,  37,  38,  38,  38,  38,  38,  38,  38,  38,
  38,  38,  38,  38,  38,  38,  38,  38,  38,  38,
  38,  38,  38,  38,  38,  38,  38,  38,  40,  40,
  40,  41,  41,  41,  45,  45,  44,  44,  43,  43,
  42,  42,  46,  46,  47,  47,  47,  48,  48,  48,
  48,  49,  49, 147,  93,  94,  95,  96,  97,  98,
  99, 100, 101, 102, 103, 104, 105, 106, 154, 154,
 154, 154, 154, 154, 154, 154, 154, 154, 154, 154,
 154, 154, 154, 154, 154, 154, 154, 154, 154, 154,
 154,   6,   6,   6,   6,   6,  51,  51,  52,  52,
  52,  53,  53,  24,  24,  25,  25,  26,  26,  26,
  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,
   5,   5,  69,  69,  69,  69,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  32,  32,  32,  32,  32,  32,  32,  32,  32,
  32,  20,  20,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  27,  27,  27,  27,  27,  27,  27,  27,
  27,  27,  27,  27,  27,  27,  27,  27,  27,  27,
  27,  27,  27,  27,  27,  27,  27,  50,  50,  50,
  50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
  50,  50,  50,  50,  50,  29,  29,  28,  28,  28,
  28,  28, 129, 129, 129, 129, 129, 129,  62,  62,
  62,  61,  61,  85,  85,  82,  82,  83,  17,  17,
  35,  35,  35,  35,  35,  35,  35,  35,  84,  84,
  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,
  84,  84,  84, 173, 173, 118, 118, 118, 118, 118,
 118, 118, 118, 118, 118, 118, 119, 119,  86,  86,
  87,  87, 174, 120,  88,  88,  88,  88,  88,  88,
  88, 121, 121, 175, 175, 175,  64,  64, 176, 176,
 176, 176, 176, 176, 177, 179, 178, 122, 122, 123,
 123, 180, 180, 180, 180, 124, 146,  89,  89,  89,
  89,  89,  89,  89,  89,  89,  89, 125, 125, 181,
 181, 181, 181, 181, 181, 181, 126, 126,  90,  90,
  90, 127, 127, 182, 182, 182, 182 };
YYSTATIC YYCONST yyr_t YYFARDATA YYR2[]={

   0,   0,   2,   4,   4,   3,   1,   1,   1,   1,
   1,   1,   4,   4,   4,   4,   1,   1,   1,   2,
   2,   3,   2,   2,   1,   1,   1,   4,   1,   0,
   2,   1,   3,   2,   4,   6,   1,   1,   1,   1,
   3,   1,   1,   1,   1,   4,   4,   4,   4,   4,
   4,   4,   2,   3,   2,   2,   2,   1,   1,   2,
   1,   2,   4,   6,   3,   5,   7,   9,   3,   4,
   7,   1,   1,   1,   2,   0,   2,   2,   0,   6,
   2,   1,   1,   1,   1,   1,   1,   1,   1,   3,
   2,   3,   1,   2,   3,   7,   0,   2,   2,   2,
   2,   2,   3,   3,   2,   1,   4,   3,   0,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,
   2,   2,   2,   5,   0,   2,   0,   2,   0,   2,
   3,   1,   0,   1,   1,   3,   0,   3,   1,   1,
   1,   1,   1,   0,   2,   4,   3,   0,   2,   3,
   0,   5,   3,   4,   4,   4,   1,   1,   1,   1,
   1,   2,   2,   4,  13,  22,   1,   1,   5,   3,
   7,   0,   2,   2,   2,   2,   2,   2,   2,   5,
   2,   2,   2,   2,   2,   2,   5,   0,   2,   0,
   2,   0,   3,   9,   7,   1,   1,   1,   2,   2,
   1,   4,   0,   1,   1,   2,   2,   2,   2,   4,
   2,   5,   3,   2,   2,   1,   4,   3,   0,   2,
   2,   0,   2,   2,   2,   2,   2,   1,   1,   1,
   1,   9,   0,   2,   2,   0,   2,   2,   2,   2,
   1,   1,   1,   1,   1,   0,   4,  13,   0,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   5,   8,   6,
   5,   0,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   4,   4,   4,   4,   5,   1,   1,   1,
   0,   4,   4,   4,   4,   0,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   5,   1,   0,
   2,   2,   1,   2,   4,   5,   1,   1,   1,   1,
   2,   1,   1,   1,   1,   1,   4,   6,   4,   4,
  11,   1,   5,   3,   5,   3,   1,   2,   2,   1,
   2,   4,   4,   1,   2,   2,   2,   2,   2,   2,
   2,   1,   2,   1,   1,   1,   4,   4,   2,   4,
   2,   0,   1,   1,   3,   1,   3,   1,   0,   3,
   5,   4,   3,   5,   5,   5,   5,   5,   5,   2,
   2,   2,   2,   2,   2,   4,   4,   4,   4,   4,
   4,   4,   4,   5,   5,   5,   5,   4,   4,   4,
   4,   4,   4,   3,   2,   0,   1,   1,   2,   1,
   1,   1,   1,   4,   4,   5,   4,   4,   4,   7,
   7,   7,   7,   7,   7,   7,   7,   7,   7,   8,
   8,   8,   8,   7,   7,   7,   7,   7,   0,   2,
   2,   0,   2,   2,   0,   2,   0,   2,   0,   2,
   0,   2,   0,   2,   0,   2,   2,   0,   2,   3,
   2,   0,   2,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   2,   1,   2,
   2,   2,   2,   2,   2,   3,   2,   2,   2,   5,
   3,   2,   2,   2,   2,   2,   5,   4,   6,   2,
   4,   0,   3,   3,   1,   1,   0,   3,   0,   1,
   1,   3,   2,   0,   1,   1,   3,   1,   3,   4,
   4,   4,   4,   5,   1,   1,   1,   1,   1,   1,
   1,   3,   1,   3,   4,   1,   0,  10,   6,   5,
   6,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   2,   2,   2,   2,   1,   1,   1,
   1,   2,   3,   4,   6,   5,   1,   1,   1,   1,
   1,   1,   1,   2,   2,   1,   2,   2,   4,   1,
   2,   1,   2,   1,   2,   1,   2,   1,   2,   1,
   1,   0,   5,   0,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   2,   2,   2,   2,   1,
   1,   1,   1,   1,   3,   2,   2,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   2,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   2,   1,   3,   2,   3,   4,   2,   2,
   2,   5,   5,   7,   4,   3,   2,   3,   2,   1,
   1,   2,   3,   2,   2,   1,   2,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
   1,   1,   1,   1,   1,   1,   3,   0,   1,   1,
   3,   2,   6,   7,   3,   3,   3,   6,   0,   1,
   3,   5,   6,   4,   4,   1,   3,   3,   1,   1,
   1,   1,   4,   1,   6,   6,   6,   4,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   3,   2,   5,   4,   7,
   6,   7,   6,   9,   8,   3,   8,   4,   0,   2,
   0,   1,   3,   3,   0,   2,   3,   2,   2,   2,
   2,   0,   2,   3,   1,   1,   1,   1,   3,   8,
   2,   3,   1,   1,   3,   3,   3,   4,   6,   0,
   2,   3,   1,   3,   1,   4,   3,   0,   2,   2,
   2,   3,   3,   3,   3,   3,   3,   0,   2,   2,
   3,   3,   4,   2,   1,   1,   3,   5,   0,   2,
   2,   0,   2,   4,   3,   1,   1 };
YYSTATIC YYCONST short YYFARDATA YYCHK[]={

-1000,-107,-108,-109,-111,-112,-114,-115,-116,-117,
-118,-119,-120,-122,-124,-126,-128,-129,-130, 518,
 519, 454, 521, 522,-131,-132,-133, 525, 526,-137,
 403,-150, 405,-167,-135, 449,-173, 457, 402, 464,
 465, 424, -85, 425, -91, -92, 273, 443, 523, 527,
 528, 529, 530, 531, 532, 533,  59,-136, 404, 406,
 448, 441, 442, 444, -10, -11, 123, 123,-113, 123,
 123, 123, 123,  -9, 264,  -9, 520, -86, -23, 265,
 264, -23, 123,-138, 314,  -1,  -2, 261, 260, 263,
 -76, -16,  91,-168, 123,-171, 278,  38,-172, 286,
 287, 284, 283, 282, 281, 288, -30, -31, 267,  91,
  -9, -88, 463, 463, -90,  -1, 463, -84, 426, 427,
 428, 429, 430, 431, 432, 433, 434, 435, 436, 437,
 438, 439, 440, -30, -84, 263, -27, -68, -72, -91,
 -92, 306, 297, 321, 322,-147,  33, 307, 276, 323,
 -50, 275,  91,  -5, -74, 268, 407, 408, 409, 352,
 351, 278, 298, 277, 281, 282, 283, 284, 286, 287,
 279, 290, 291, 292, 293, 271,  -1, 296,  -1,  -1,
  -1,  -1, 262, -75,-169, 318, 373,  61, -71,  40,
 -73,  -7, -74, 269, 272, 324, 339,  -8, 295, 300,
 302, 308, -30, -30,-110,-107, 125,-152, 410,-153,
 412,-151, 414, 415,-115,-154,  -2,-129,-118,-131,
-130,-133, 466, 452, 501,-155, 500,-157, 413, -93,
 -94, -95, -96, -97,-106, -98, -99,-100,-101,-102,
-103,-104,-105,-156,-160, 389, 390, 391, 392, 393,
 394, 395, 396, 397, 398, 399, 400, 401, 123, 411,
-121,-123,-125,-127,  -9,  -1, 455,-134, -68, -74,
-139, 315, -69, -68,  91, -27,-147,  46,  -7, 327,
 328, 329, 330, 331, 325, 345, 349, 336, 359, 360,
 361, 362, 363, 364, 365, 347, 372, 294, 366, -77,
  -9,-170,-171,  42,  40, -30,  40, -14,  91,  40,
 -14,  40, -14,  40, -14,  40, -14,  40, -14,  40,
  41, 267,  -9, 263,  58,  44, 262,  -1, 350, 467,
 373, 469, 470, 471, -88, -89,  -1, 328, 329,  -1,
 -69,  41, -34,  61, 288, 262,  44, 384,  91,  38,
  42, 353, 354, 355,  60, 384, 384, 384, 384, -68,
 306, -68, -73,  -7,  33,  -9,  -1, 280, 279, 289,
 285, -27,  -1, -74,  42, 465,  47, -27, 270, 272,
 281, 282, 283, 284,  40, -34,  -1, 328, 329, 321,
 344, 333, 334, 336, 337, 338, 339, 340, 341, 342,
 343, 356, 335, 348, 325, 365, 294,  -2,  40,  61,
 -70, -69, -72, -27,  -7,  -7,  40, 301, 303, 304,
 305,  41,  41, 125,-141,-112,-109,-142,-144,-114,
-115,-129,-118,-130, 446, 447,-146, 501,-131,-133,
 500, 416, 421, 466, 402, 125,  -9,  -9,  40, 445,
  58,  91,  -9, -69, 351, 358,  91,-158,-159,-161,
-163,-164,-165, 311,-166, 309, 313, 312,  -9,  -2,
  -9, -23,  40, -22, -23, 266, 286, 287, -30,  -9,
  -2, -73, -27, -74, 270, 272, -69, -34, 340,-172,
  -7, -70,  40,-113,-155,  -2,  -9, 125,-175, 456,
-129,-176,-177, 461, 462,-178,-130,-133, 458, 125,
-180,-174,-176,-179, 337, 456, 459, 125,-181, 454,
 402, 457, 296,-130,-133, 125,-182, 454, 457,-130,
-133, -87, 414, 125,-134,-140, -69,  -1, 465,  -7,
  -1, -13,  40,  40, -27, 327, 328, 329, 330, 371,
 365, 325, 472, 359, 360, 361, 362, 369, 370, 294,
  93, 125,  44,  40,  -2,  41, -22,  -9, -22, -23,
  -9,  -9,  -9,  93,  -9,  -9, 468,  -1,  -1, 329,
 328, 326, 335, 384,  40,  61,  43, 123,  40,  40,
 263,  -1,  93, -29, -28, 275,  -9,  40,  40, -52,
 -27, -53,  -1,  -1,  -1,  -1, -68, -27,  -9,  -1,
 280,  93,  93,  93,  -1,  -1, -69,  -1,  91,  -9,
 -67,  60, 328, 329, 330, 359, 360, 361,  40,  61,
 -34, 123,  40,  41, -69,  -3, 367, 368,  -1,  -9,
-113, 123, 123, 123,  -9,  -9, 123, -69, 351, 358,
 -79, -80, -89, -24, -25, -26, 275, -13,  40,  -9,
  58, 274,  -7,  91,  -1,  -9,-158,-162,-155, 310,
-162,-162,-162, -69,-155,  -2,  -9,  40,  40,  41,
 -69,  -1,  40, -30, -27,  -6,  -2,  -9, 125, 316,
 316, 460, -30, -64,  -9,  42, -34,  61, -30,  61,
 -30, -30,  61,  61,  -1, 463,  -9, 463,  40,  -1,
 463,-174,  44,  93,  -1, -27, -27,  91,  -9, -34,
 -81,  -1,  40,  40,-170, -34,  41,  41,  93,  41,
  41,  41,  41,  41, -12, 263,  44,  58, 384, 328,
 329, 330, 359, 360, 361,  -1, -82, -83, -34, 123,
 262, -62, -61, -69, 306,  44,  93,  44, 275, -69,
 -69,  44,  62,  42,  -5,  -5,  -5,  93, 274,  41,
 -66, -19, -18,  43,  45, 306, 322, 367,  -9, -57,
 -59, -71, 274, -51,  60,  41, 125,-110,-143,-145,
-125, 274,  -7,  91,  -1, -69,  -1, 365, 325,  -7,
 365, 325,  -1,  41,  44, -27, -24,  93,  -9,  -3,
  -1, -27,  -9,  93,  -2,  -9,  -9, -23, 274, -34,
  41,  40,  41,  44,  44,  -2,  -9,  -9,  41,  58,
  40,  41,  40,  41,  41,  40,  40,  -1,  -1,  -9,
 317,  -1, -30, -69,  93, -36, 472, 497, 498, 499,
  -9,  41, 384, -81,  41, 381, 340, 341, 342, 382,
 383, 301, 303, 304, 305, 385, 388, 294,  -4, 317,
 -32, 473, 475, 476, 477, 478, 276, 277, 281, 282,
 283, 284, 286, 287, 257, 279, 290, 291, 292, 293,
 479, 480, 481, 483, 484, 485, 486, 487, 488, 489,
 333, 490, 280, 289, 335, 491, 340, 482, 351, 384,
 495, 271,  -9,  41, -14, -14, -14, -14, -14, -14,
 317, 283, 284, 450, 451, 453,  -9,  -9,  -1,  41,
  44,  61, -57, 125,  44,  61, 263, 263, -28,  -9,
  41,  41, -27,  40,  -5,  -1,  62, -56,  -1,  40,
 -19,  41, 125, -60, -38,-133, -39, 298, 358, 297,
 286, 287, 284, 283, 282, 281, 293, 292, 291, 290,
 279, 278, 277,-172,  61,  -3,  40, -52, 125, 125,
-148, 417, 418, 419, 420,-118,-130,-131,-133, 125,
-149, 422, 423, 420,-130,-118,-131,-133, 125,  -3,
 -27,  -9,  -1, -27, -26, -36,  41, 384, -69,  93,
 -33,  61, 316, 316,  41,  41,  -1,  41, -24,  -6,
  -6, -64,  41,  -9,  41,  -3,  40,  93,  93,  93,
  93, -34,  41,  58,  58,  40, -33,  -2,  41,  42,
  91,  40, 474, 494, 277, 281, 282, 283, 284, 280,
 -20,  40, -20, -20, -15, 503, 476, 477, 276, 277,
 281, 282, 283, 284, 286, 287, 279, 290, 291, 292,
 293,  42, 479, 480, 481, 483, 484, 487, 488, 490,
 280, 289, 257, 504, 505, 506, 507, 508, 509, 510,
 511, 512, 513, 514, 515, 516, 489, 481, 493,  41,
  -2, 263, 263,  44, -82, -35, -17,  -9, 283, -34,
 -68, 319, 320, 125, -62, 123,  61, -24,  -1, -65,
  44, -54, -55, -69, -63,-133, 352, 357,  40,  91,
  40,  91,  40,  91,  40,  91,  40,  91,  40,  91,
  40,  91,  40,  91,  40,  91,  40,  91,  40,  91,
  40,  91,  40,  91, 284, 283, 282, 281,  40,  91,
  40,  91, -30, -34, 123,  40, -51, -24,  62, -73,
 -73, -73, -73, -73, -73, -73, 502, -69,  93,  -1,
  -2,  -2, 274, -37, -39, -34, 299, 286, 287, 284,
 283, 282, 281, 279, 293, 292, 291, 290, 278, 277,
  -2,  -9,  41,  58, -87, -67, -32, -81, 386, 387,
 386, 387,  -9,  93,  -9,  43, -34,  91,  91, 496,
  44,  91, 517,  38, 281, 282, 283, 284, 280,  -9,
  40,  40, -60, 123,  41, -65, -66,  41,  44, -58,
 -50, 358, 297, 344, 299, 263,  -9, 306, -68, 299,
  -9, -38,  -9, -22,  -9,  -9, -22, -23,  -9, -23,
  -9,  -9,  -9,  -9,  -9,  -9,  -9, -23,  -9,  -9,
  -9,  -9,  -9,  -9,  -9,  40,  91,  40,  91,  40,
  91,  40,  91,  -9,  -9, -17,  -9,  41, -57,  40,
  41,  -7, 274,  40,  -3, 284, 283, 282, 281, -64,
  40,  41,  41,  41,  93,  43,  -9,  44,  -9,  -9,
  61, -34,  93, 263,  -9, 281, 282, 283,  -9, 125,
 -60, -69,  -1,  91, 306, -68,  41,  41,  93, 263,
  41,  41,  93,  41,  93,  41,  41,  93,  41,  41,
  93,  41,  93,  41,  93,  41,  93,  41,  93,  41,
  93,  41,  93,  41,  93,  41,  93, -23,  -9,  -9,
  -9,  -9,  -9,  -9,  -9,  41,  93,  41,  93, 125,
 -24, -27,  -3, -24, -21,  60,  58, -24,  -9,  93,
 -34,  93,  93,  -9,  41,  58,  58,  58,  41, 125,
  61,  93, 263,  40,  41,  40,  40,  40,  40,  40,
  40,  40,  40,  40,  40,  40,  40,  41,  93,  41,
  93,  41,  93,  41,  93,  40,  40,  41, -69, -21,
  41,  40,  91, -64,  41,  93,  44,  41, -32,  41,
  -9,  -9,  -9, -38, -47, -48, -49, -40, -41, -45,
 -44, -43, -42, -45, -44, -43, -42,  40,  40,  40,
  40, -43, -46, 274,  40, -33, -24,  -9, -78, -34,
  41,  41,  41,  41, 299, 263,  41, 299, 306, -68,
  41, -38,  41, -22,  -9,  41, -22, -23,  41, -23,
  41,  -9,  41,  -9,  41,  -9,  41,  41,  41,  41,
 -45, -44, -43, -42,  41,  41, -17,  -3, -24,  41,
  93, 123, 323, 373, 374, 375, 308, 376, 377, 378,
 379, 332, 346, 294,  44, 263,  41,  41,  41,  41,
  40,  41,  62,  40, -34, -24, 502,  -9,  41,  41,
 351,  41,  -7, -27, -69, 274,  -3, -21,  40, -24,
  41 };
YYSTATIC YYCONST short YYFARDATA YYDEF[]={

   1,  -2,   2,   0,   0, 319,   6,   7,   8,   9,
  10,  11,   0,   0,   0,   0,  16,  17,  18,   0,
   0, 758,   0,   0,  24,  25,  26,   0,  28, 134,
   0, 258, 201,   0, 415,   0,   0, 764, 105, 818,
  92,   0, 415,   0,  83,  84,  85,   0,   0,   0,
   0,   0,   0,  57,  58,   0,  60, 108, 254, 371,
   0, 743, 744, 212, 415, 415, 138,   1,   0, 771,
 789, 807, 821,  19,  41,  20,   0,   0,  22,  42,
  43,  23,  29, 136,   0, 104,  38,  39,  36,  37,
 212, 181,   0, 368,   0, 375,   0,   0, 415, 378,
 378, 378, 378, 378, 378,   0,   0, 416, 417,   0,
 746,   0, 764, 797,   0,  93,   0,   0, 728, 729,
 730, 731, 732, 733, 734, 735, 736, 737, 738, 739,
 740, 741, 742,   0,   0,  33,   0,   0,   0,   0,
   0,   0, 653,   0,   0, 212,   0, 669, 670,   0,
 675,   0,   0, 534, 225, 536, 537, 538, 539,   0,
 473, 677, 678, 679, 680, 681, 682, 683, 684, 685,
   0, 690, 691, 692, 693, 694, 540,   0,  52,  54,
  55,  56,  59,   0, 370, 372, 373,   0,  61,   0,
  71,   0, 205, 206, 207, 212, 212, 210,   0, 213,
 214,   0,   0,   0,   0,   0,   5, 320,   0, 322,
   0,   0, 326, 327, 328, 329,   0, 331, 332, 333,
 334, 335,   0,   0,   0, 341,   0,   0, 318, 488,
   0,   0,   0,   0, 415,   0, 212,   0,   0,   0,
 212,   0,   0, 319,   0, 474, 475, 476, 477, 478,
 479, 480, 481, 482, 483, 484, 485, 486, 346, 353,
   0,   0,   0,   0,  21, 760, 759,   0,  29, 535,
 107,   0, 135, 542,   0, 545, 212,   0, 300, 259,
 260, 261, 262, 263, 264, 265, 266, 267, 268, 269,
 270, 271, 272, 273, 274, 275, 276,   0,   0,   0,
   0,   0, 377,   0,   0,   0,   0, 389,   0,   0,
 390,   0, 391,   0, 392,   0, 393,   0, 394, 414,
 102, 418,   0, 745,   0,   0, 755, 763, 765,   0,
 767, 768, 769, 770,   0,   0, 816, 819, 820,  94,
 704, 705, 706,   0,   0,  31,   0,   0, 697, 658,
 659, 660,   0,   0, 518,   0,   0,   0,   0, 652,
   0, 655, 220,   0,   0, 666, 668, 671,   0, 673,
 674, 676,   0,   0,   0,   0,   0,   0, 223, 224,
 686, 687, 688, 689,   0,  53, 146, 109, 110, 111,
 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
 122, 123,   0, 130, 131, 132,   0,   0, 103,   0,
   0,  72,  73,   0, 208, 209,   0, 215, 216, 217,
 218,  64,  68,   3, 139, 319,   0,   0,   0, 166,
 167, 168, 169, 170,   0,   0,   0,   0, 176, 177,
   0, 228, 242, 797, 105,   4, 321, 323,  -2,   0,
 330,   0,   0,   0, 212,   0,   0, 347, 349,   0,
   0,   0,   0,   0,   0, 363, 364, 361, 489, 490,
 491, 492, 487, 493, 494,  44,   0,   0,   0, 496,
 497, 498,   0, 501, 502, 503, 504, 505,   0, 415,
   0, 509, 511,   0, 350,   0,   0,  12, 772,   0,
 774, 775, 415,   0,   0, 415, 782, 783,   0,  13,
 790, 415, 792, 415, 794,   0,   0,  14, 808,   0,
   0,   0,   0, 814, 815,  15, 822,   0,   0, 825,
 826, 757, 761,  27,  30, 137, 141,   0,   0,   0,
  40,   0,   0, 281,   0, 182, 183, 184, 185, 186,
 187, 188,   0, 190, 191, 192, 193, 194, 195,   0,
 202, 374,   0,   0,   0, 382,   0,   0,   0,   0,
   0,   0,   0,  96, 748,   0, 766, 787, 795, 798,
 799, 800,   0,   0,   0,   0,   0, 708, 713, 714,
  34,  47, 656,   0, 695, 698, 699,   0,   0,   0,
 519, 520,  48,  49,  50,  51, 654,   0, 665, 667,
 672,   0,   0,   0,   0, 541,   0,  -2, 697,   0,
 106, 153, 124, 125, 126, 127, 128, 129,   0, 369,
  62,  75,  69, 212,   0, 516, 297, 298,  -2,   0,
   0, 138, 231, 245, 171, 172, 807,   0, 212,   0,
   0, 212,   0,   0, 524, 525, 527,   0,  -2,   0,
   0,   0,   0,   0, 343,   0, 348, 354, 365,   0,
 355, 356, 357, 362, 358, 359, 360,   0,   0, 495,
   0,  -2,   0,   0,   0,   0, 514, 515, 345,   0,
   0,   0,   0,   0, 776, 777, 780,   0,   0,   0,
   0,   0,   0,   0, 809,   0, 813,   0,   0,   0,
   0, 415,   0, 543,   0,   0, 255,   0,   0, 281,
   0, 197, 546,   0, 376,   0, 381, 378, 379, 378,
 378, 378, 378, 378,   0, 747,   0,   0,   0, 801,
 802, 803, 804, 805, 806, 817,   0, 715,   0,  75,
  32,   0, 709,   0,   0,   0, 657, 697, 701,   0,
   0, 522, 664, 659, 530, 531, 532,   0,   0, 219,
   0,   0, 153, 148, 149, 150, 151, 152,   0,   0,
  78,  65,   0,   0, 518, 211, 162,   0,   0,   0,
   0,   0,   0,   0, 179,   0,  -2, 229, 230,   0,
 243, 244, 796, 324, 300, 255,   0, 336, 338, 339,
 299,   0,   0, 199,   0,   0,   0,   0,   0,   0,
 507,  -2, 510, 511, 511, 351, 352, 773, 778,   0,
 786, 781, 784, 791, 793, 762, 785, 810, 811,   0,
   0, 824,   0, 140, 544,   0,   0,   0,   0,   0,
   0, 277,   0,   0, 280, 282, 283, 284, 285, 286,
 287, 288, 289, 290, 291,   0,   0,   0, 199,   0,
   0,   0,   0, 551, 552, 553, 554, 555, 556, 557,
 558, 559, 560, 561, 562,   0, 567, 568, 569, 570,
 576, 577, 578, 579, 580, 581, 582, 601, 601, 585,
 601, 603, 589, 591,   0, 593,   0, 595, 597,   0,
 599, 600,   0, 380, 383, 384, 385, 386, 387, 388,
   0,  97,  98,  99, 100, 101, 750, 752, 788, 702,
   0,   0,   0, 707, 708,   0,  37,  35, 696, 700,
 661, 662, 521,  -2, 533, 221, 147,   0, 157, 142,
 154, 133,  63,  74,  76,  77, 422,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 415,   0, 516,  -2,   0, 163, 164,
 232, 212, 212, 212, 212, 237, 238, 239, 240, 165,
 246, 212, 212, 212, 250, 251, 252, 253, 173,   0,
   0,   0, 226,   0, 526, 528, 325,   0,   0, 342,
 344,   0,   0,   0,  45,  46, 499, 506,   0, 512,
 513,   0, 812, 823, 760, 146, 546, 301, 302, 303,
 304, 281, 279,   0,   0,   0, 180, 198, 189, 571,
   0,   0,   0,   0, 596, 563, 564, 565, 566, 590,
 583,   0, 584, 586, 587, 604, 605, 606, 607, 608,
 609, 610, 611, 612, 613, 614,   0, 619, 620, 621,
 622, 623, 627, 628, 629, 630, 631, 632, 633, 634,
 635, 637, 638, 639, 640, 641, 642, 643, 644, 645,
 646, 647, 648, 649, 650, 651, 592, 594, 598, 196,
  95, 749, 751,   0, 716, 717, 720, 721,   0, 723,
   0, 718, 719, 703, 710,  78,   0,   0, 157, 156,
 153,   0, 143, 144,   0,  80,  81,  82,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  66,  75,  70,   0,   0, 517, 233,
 234, 235, 236, 247, 248, 249, 212,   0, 178,   0,
 529, 337,   0, 200, 419, 420, 421,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 366, 367, 508,   0, 756,   0,   0,   0, 292, 293,
 294, 295,   0, 572,   0,   0,   0,   0,   0,   0,
   0,   0, 625, 626, 615, 616, 617, 618, 636, 754,
   0,   0,   0,  78, 663, 155, 158, 159,   0,   0,
  86,  87,  88,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 413,   0,  -2,
 204,   0,   0,  -2, 160,   0,   0,   0,   0,   0,
  -2, 256, 278, 296, 573,   0,   0,   0,   0,   0,
   0, 588, 624, 753,   0,   0,   0,   0,   0, 711,
   0, 145,   0,   0,   0,  90, 423, 424,   0,   0,
 426, 427,   0, 428,   0, 395, 397,   0, 396, 398,
   0, 399,   0, 400,   0, 401,   0, 402,   0, 407,
   0, 408,   0, 409,   0, 410,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 411,   0, 412,   0,  67,
   0,   0, 160,   0,   0,   0,   0,   0,   0, 575,
   0, 549, 546,   0, 722,   0,   0,   0, 727, 712,
   0,  91,  89, 464, 425, 467, 471, 448, 451, 454,
 456, 458, 460, 454, 456, 458, 460, 403,   0, 404,
   0, 405,   0, 406,   0, 458, 462, 203,   0,   0,
 199,  -2,   0, 779, 305, 574,   0, 548, 550, 602,
   0,   0,   0,  79,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 454, 456, 458,
 460,   0,   0,   0,  -2, 241,   0,   0,   0,   0,
 724, 725, 726, 445, 465, 466, 446, 468,   0, 470,
 447, 472, 429, 449, 450, 430, 452, 453, 431, 455,
 432, 457, 433, 459, 434, 461, 435, 436, 437, 438,
   0,   0,   0,   0, 443, 444, 463,   0,   0, 340,
   0, 257, 306, 307, 308, 309, 310, 311, 312, 313,
 314, 315, 316,   0,   0, 469, 439, 440, 441, 442,
  -2,   0, 161,   0,   0,   0,   0,   0, 547, 174,
 212, 317,   0,   0,   0,   0, 160,   0,  -2,   0,
 175 };
#ifdef YYRECOVER
YYSTATIC YYCONST short yyrecover[] = {
-1000
};
#endif

/* SCCSWHAT( "@(#)yypars.c	3.1 88/11/16 22:00:49	" ) */
#line 3 "yypars.c"
#if ! defined(YYAPI_PACKAGE)
/*
**  YYAPI_TOKENNAME		: name used for return value of yylex	
**	YYAPI_TOKENTYPE		: type of the token
**	YYAPI_TOKENEME(t)	: the value of the token that the parser should see
**	YYAPI_TOKENNONE		: the representation when there is no token
**	YYAPI_VALUENAME		: the name of the value of the token
**	YYAPI_VALUETYPE		: the type of the value of the token (if null, then the value is derivable from the token itself)
**	YYAPI_VALUEOF(v)	: how to get the value of the token.
*/
#define	YYAPI_TOKENNAME		yychar
#define	YYAPI_TOKENTYPE		int
#define	YYAPI_TOKENEME(t)	(t)
#define	YYAPI_TOKENNONE		-1
#define	YYAPI_TOKENSTR(t)	(sprintf(yytokbuf, "%d", t), yytokbuf)
#define	YYAPI_VALUENAME		yylval
#define	YYAPI_VALUETYPE		YYSTYPE
#define	YYAPI_VALUEOF(v)	(v)
#endif
#if ! defined(YYAPI_CALLAFTERYYLEX)
#define	YYAPI_CALLAFTERYYLEX(param)
#endif

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

#ifdef YYDEBUG				/* RRR - 10/9/85 */
char yytokbuf[20];
# ifndef YYDBFLG
#  define YYDBFLG (yydebug)
# endif
# define yyprintf(a, b, c, d) if (YYDBFLG) YYPRINT(a, b, c, d)
#else
# define yyprintf(a, b, c, d)
#endif

#ifndef YYPRINT
#define	YYPRINT	printf
#endif

/*	parser for yacc output	*/

#ifdef YYDUMP
int yydump = 1; /* 1 for dumping */
void yydumpinfo(void);
#endif
#ifdef YYDEBUG
YYSTATIC int yydebug = 0; /* 1 for debugging */
#endif
YYSTATIC YYSTYPE yyv[YYMAXDEPTH];	/* where the values are stored */
YYSTATIC short	yys[YYMAXDEPTH];	/* the parse stack */

#if ! defined(YYRECURSIVE)
YYSTATIC YYAPI_TOKENTYPE	YYAPI_TOKENNAME = YYAPI_TOKENNONE;
#if defined(YYAPI_VALUETYPE)
// YYSTATIC YYAPI_VALUETYPE	YYAPI_VALUENAME;	 FIX 
#endif
YYSTATIC int yynerrs = 0;			/* number of errors */
YYSTATIC short yyerrflag = 0;		/* error recovery flag */
#endif

#ifdef YYRECOVER
/*
**  yyscpy : copy f onto t and return a ptr to the null terminator at the
**  end of t.
*/
YYSTATIC	char	*yyscpy(register char*t, register char*f)
	{
	while(*t = *f++)
		t++;
	return(t);	/*  ptr to the null char  */
	}
#endif

#ifndef YYNEAR
#define YYNEAR
#endif
#ifndef YYPASCAL
#define YYPASCAL
#endif
#ifndef YYLOCAL
#define YYLOCAL
#endif
#if ! defined YYPARSER
#define YYPARSER yyparse
#endif
#if ! defined YYLEX
#define YYLEX yylex
#endif

#if defined(YYRECURSIVE)

	YYSTATIC YYAPI_TOKENTYPE	YYAPI_TOKENNAME = YYAPI_TOKENNONE;
  #if defined(YYAPI_VALUETYPE)
	YYSTATIC YYAPI_VALUETYPE	YYAPI_VALUENAME;  
  #endif
	YYSTATIC int yynerrs = 0;			/* number of errors */
	YYSTATIC short yyerrflag = 0;		/* error recovery flag */

	YYSTATIC short	yyn;
	YYSTATIC short	yystate = 0;
	YYSTATIC short	*yyps= &yys[-1];
	YYSTATIC YYSTYPE	*yypv= &yyv[-1];
	YYSTATIC short	yyj;
	YYSTATIC short	yym;

#endif

#ifdef _MSC_VER
#pragma warning(disable:102)
#endif
YYLOCAL int YYNEAR YYPASCAL YYPARSER()
{
#if ! defined(YYRECURSIVE)

	register	short	yyn;
	short		yystate, *yyps;
	YYSTYPE		*yypv;
	short		yyj, yym;

	YYAPI_TOKENNAME = YYAPI_TOKENNONE;
	yystate = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];
#endif

#ifdef YYDUMP
	yydumpinfo();
#endif
 yystack:	 /* put a state and value onto the stack */

#ifdef YYDEBUG
	if(YYAPI_TOKENNAME == YYAPI_TOKENNONE) {
		yyprintf( "state %d, token # '%d'\n", yystate, -1, 0 );
		}
	else {
		yyprintf( "state %d, token # '%s'\n", yystate, YYAPI_TOKENSTR(YYAPI_TOKENNAME), 0 );
		}
#endif
	if( ++yyps > &yys[YYMAXDEPTH] ) {
		yyerror( "yacc stack overflow" );
		return(1);
	}
	*yyps = yystate;
	++yypv;

	*yypv = yyval;

yynewstate:

	yyn = YYPACT[yystate];

	if( yyn <= YYFLAG ) {	/*  simple state, no lookahead  */
		goto yydefault;
	}
	if( YYAPI_TOKENNAME == YYAPI_TOKENNONE ) {	/*  need a lookahead */
		YYAPI_TOKENNAME = YYLEX();
		YYAPI_CALLAFTERYYLEX(YYAPI_TOKENNAME);
	}
	if( ((yyn += YYAPI_TOKENEME(YYAPI_TOKENNAME)) < 0) || (yyn >= YYLAST) ) {
		goto yydefault;
	}
	if( YYCHK[ yyn = YYACT[ yyn ] ] == YYAPI_TOKENEME(YYAPI_TOKENNAME) ) {		/* valid shift */
		yyval = YYAPI_VALUEOF(YYAPI_VALUENAME);
		yystate = yyn;
 		yyprintf( "SHIFT: saw token '%s', now in state %4d\n", YYAPI_TOKENSTR(YYAPI_TOKENNAME), yystate, 0 );
		YYAPI_TOKENNAME = YYAPI_TOKENNONE;
		if( yyerrflag > 0 ) {
			--yyerrflag;
		}
		goto yystack;
	}

 yydefault:
	/* default state action */

	if( (yyn = YYDEF[yystate]) == -2 ) {
		register	YYCONST short	*yyxi;

		if( YYAPI_TOKENNAME == YYAPI_TOKENNONE ) {
			YYAPI_TOKENNAME = YYLEX();
			YYAPI_CALLAFTERYYLEX(YYAPI_TOKENNAME);
 			yyprintf("LOOKAHEAD: token '%s'\n", YYAPI_TOKENSTR(YYAPI_TOKENNAME), 0, 0);
		}
/*
**  search exception table, we find a -1 followed by the current state.
**  if we find one, we'll look through terminal,state pairs. if we find
**  a terminal which matches the current one, we have a match.
**  the exception table is when we have a reduce on a terminal.
*/

#if YYOPTTIME
		yyxi = yyexca + yyexcaind[yystate];
		while(( *yyxi != YYAPI_TOKENEME(YYAPI_TOKENNAME) ) && ( *yyxi >= 0 )){
			yyxi += 2;
		}
#else
		for(yyxi = yyexca;
			(*yyxi != (-1)) || (yyxi[1] != yystate);
			yyxi += 2
		) {
			; /* VOID */
			}

		while( *(yyxi += 2) >= 0 ){
			if( *yyxi == YYAPI_TOKENEME(YYAPI_TOKENNAME) ) {
				break;
				}
		}
#endif
		if( (yyn = yyxi[1]) < 0 ) {
			return(0);   /* accept */
			}
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:		/* brand new error */
#ifdef YYRECOVER
			{
			register	int		i,j;

			for(i = 0;
				(yyrecover[i] != -1000) && (yystate > yyrecover[i]);
				i += 3
			) {
				;
			}
			if(yystate == yyrecover[i]) {
				yyprintf("recovered, from state %d to state %d on token # %d\n",
						yystate,yyrecover[i+2],yyrecover[i+1]
						);
				j = yyrecover[i + 1];
				if(j < 0) {
				/*
				**  here we have one of the injection set, so we're not quite
				**  sure that the next valid thing will be a shift. so we'll
				**  count it as an error and continue.
				**  actually we're not absolutely sure that the next token
				**  we were supposed to get is the one when j > 0. for example,
				**  for(+) {;} error recovery with yyerrflag always set, stops
				**  after inserting one ; before the +. at the point of the +,
				**  we're pretty sure the guy wants a 'for' loop. without
				**  setting the flag, when we're almost absolutely sure, we'll
				**  give him one, since the only thing we can shift on this
				**  error is after finding an expression followed by a +
				*/
					yyerrflag++;
					j = -j;
					}
				if(yyerrflag <= 1) {	/*  only on first insertion  */
					yyrecerr(YYAPI_TOKENNAME, j);	/*  what was, what should be first */
				}
				yyval = yyeval(j);
				yystate = yyrecover[i + 2];
				goto yystack;
				}
			}
#endif
		yyerror("syntax error");

		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = YYPACT[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && YYCHK[YYACT[yyn]] == YYERRCODE ){
			      yystate = YYACT[yyn];  /* simulate a shift of "error" */
 				  yyprintf( "SHIFT 'error': now in state %4d\n", yystate, 0, 0 );
			      goto yystack;
			      }
			   yyn = YYPACT[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

 			   yyprintf( "error recovery pops state %4d, uncovers %4d\n", *yyps, yyps[-1], 0 );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

 			yyprintf( "error recovery discards token '%s'\n", YYAPI_TOKENSTR(YYAPI_TOKENNAME), 0, 0 );

			if( YYAPI_TOKENEME(YYAPI_TOKENNAME) == 0 ) goto yyabort; /* don't discard EOF, quit */
			YYAPI_TOKENNAME = YYAPI_TOKENNONE;
			goto yynewstate;   /* try again in the same state */
			}
		}

	/* reduction by production yyn */

		{
		register	YYSTYPE	*yypvt;
		yypvt = yypv;
		yyps -= YYR2[yyn];
		yypv -= YYR2[yyn];
		yyval = yypv[1];
 		yyprintf("REDUCE: rule %4d, popped %2d tokens, uncovered state %4d, ",yyn, YYR2[yyn], *yyps);
		yym = yyn;
		yyn = YYR1[yyn];		/* consult goto table to find next state */
		yyj = YYPGO[yyn] + *yyps + 1;
		if( (yyj >= YYLAST) || (YYCHK[ yystate = YYACT[yyj] ] != -yyn) ) {
			yystate = YYACT[YYPGO[yyn]];
			}
 		yyprintf("goto state %4d\n", yystate, 0, 0);
#ifdef YYDUMP
		yydumpinfo();
#endif
		switch(yym){
			
case 3:
#line 337 "asmparse.y"
{ PASM->EndClass(); } break;
case 4:
#line 338 "asmparse.y"
{ PASM->EndNameSpace(); } break;
case 5:
#line 339 "asmparse.y"
{ if(PASM->m_pCurMethod->m_ulLines[1] ==0)
                                                                                  {  PASM->m_pCurMethod->m_ulLines[1] = PASM->m_ulCurLine;
                                                                                     PASM->m_pCurMethod->m_ulColumns[1]=PASM->m_ulCurColumn;}
                                                                                  PASM->EndMethod(); } break;
case 12:
#line 349 "asmparse.y"
{ PASMM->EndAssembly(); } break;
case 13:
#line 350 "asmparse.y"
{ PASMM->EndAssembly(); } break;
case 14:
#line 351 "asmparse.y"
{ PASMM->EndComType(); } break;
case 15:
#line 352 "asmparse.y"
{ PASMM->EndManifestRes(); } break;
case 19:
#line 356 "asmparse.y"
{ PASM->m_dwSubsystem = yypvt[-0].int32; } break;
case 20:
#line 357 "asmparse.y"
{ PASM->m_dwComImageFlags = yypvt[-0].int32; } break;
case 21:
#line 358 "asmparse.y"
{ PASM->m_dwFileAlignment = yypvt[-0].int32; 
                                                                                  if((yypvt[-0].int32 & (yypvt[-0].int32 - 1))||(yypvt[-0].int32 < 0x200)||(yypvt[-0].int32 > 0x10000))
                                                                                    PASM->report->error("Invalid file alignment, must be power of 2 from 0x200 to 0x10000\n");} break;
case 22:
#line 361 "asmparse.y"
{ PASM->m_stBaseAddress = (ULONGLONG)(*(yypvt[-0].int64)); delete yypvt[-0].int64; 
                                                                                  if(PASM->m_stBaseAddress & 0xFFFF)
                                                                                    PASM->report->error("Invalid image base, must be 0x10000-aligned\n");} break;
case 23:
#line 364 "asmparse.y"
{ PASM->m_stSizeOfStackReserve = (size_t)(*(yypvt[-0].int64)); delete yypvt[-0].int64; } break;
case 28:
#line 369 "asmparse.y"
{ PASM->m_fIsMscorlib = TRUE; } break;
case 31:
#line 376 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 32:
#line 377 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr; } break;
case 33:
#line 380 "asmparse.y"
{ LPCSTRToGuid(yypvt[-0].string,&(PASM->m_guidLang)); } break;
case 34:
#line 381 "asmparse.y"
{ LPCSTRToGuid(yypvt[-2].string,&(PASM->m_guidLang)); 
                                                                                  LPCSTRToGuid(yypvt[-0].string,&(PASM->m_guidLangVendor));} break;
case 35:
#line 383 "asmparse.y"
{ LPCSTRToGuid(yypvt[-4].string,&(PASM->m_guidLang)); 
                                                                                  LPCSTRToGuid(yypvt[-2].string,&(PASM->m_guidLangVendor));
                                                                                  LPCSTRToGuid(yypvt[-2].string,&(PASM->m_guidDoc));} break;
case 36:
#line 388 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 37:
#line 389 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 38:
#line 392 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 39:
#line 393 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 40:
#line 394 "asmparse.y"
{ yyval.string = newStringWDel(yypvt[-2].string, '.', yypvt[-0].string); } break;
case 41:
#line 397 "asmparse.y"
{ yyval.int32 = yypvt[-0].int32; } break;
case 42:
#line 400 "asmparse.y"
{ yyval.int64 = yypvt[-0].int64; } break;
case 43:
#line 401 "asmparse.y"
{ yyval.int64 = neg ? new __int64(yypvt[-0].int32) : new __int64((unsigned)yypvt[-0].int32); } break;
case 44:
#line 404 "asmparse.y"
{ yyval.float64 = yypvt[-0].float64; } break;
case 45:
#line 405 "asmparse.y"
{ float f; *((__int32*) (&f)) = yypvt[-1].int32; yyval.float64 = new double(f); } break;
case 46:
#line 406 "asmparse.y"
{ yyval.float64 = (double*) yypvt[-1].int64; } break;
case 47:
#line 410 "asmparse.y"
{ PASM->AddTypeDef(yypvt[-2].binstr,yypvt[-0].string); } break;
case 48:
#line 411 "asmparse.y"
{ PASM->AddTypeDef(yypvt[-2].token,yypvt[-0].string); } break;
case 49:
#line 412 "asmparse.y"
{ PASM->AddTypeDef(yypvt[-2].token,yypvt[-0].string); } break;
case 50:
#line 413 "asmparse.y"
{ yypvt[-2].cad->tkOwner = 0; PASM->AddTypeDef(yypvt[-2].cad,yypvt[-0].string); } break;
case 51:
#line 414 "asmparse.y"
{ PASM->AddTypeDef(yypvt[-2].cad,yypvt[-0].string); } break;
case 52:
#line 419 "asmparse.y"
{ DefineVar(yypvt[-0].string, NULL); } break;
case 53:
#line 420 "asmparse.y"
{ DefineVar(yypvt[-1].string, yypvt[-0].binstr); } break;
case 54:
#line 421 "asmparse.y"
{ UndefVar(yypvt[-0].string); } break;
case 55:
#line 422 "asmparse.y"
{ SkipToken = !IsVarDefined(yypvt[-0].string);
                                                                                  IfEndif++;
                                                                                } break;
case 56:
#line 425 "asmparse.y"
{ SkipToken = IsVarDefined(yypvt[-0].string);
                                                                                  IfEndif++;
                                                                                } break;
case 57:
#line 428 "asmparse.y"
{ if(IfEndif == 1) SkipToken = !SkipToken;} break;
case 58:
#line 429 "asmparse.y"
{ if(IfEndif == 0)
                                                                                    PASM->report->error("Unmatched #endif\n");
                                                                                  else IfEndif--;
                                                                                } break;
case 59:
#line 433 "asmparse.y"
{ _ASSERTE(!"yylex should have dealt with this"); } break;
case 60:
#line 434 "asmparse.y"
{ } break;
case 61:
#line 438 "asmparse.y"
{ yyval.cad = new CustomDescr(PASM->m_tkCurrentCVOwner, yypvt[-0].token, NULL); } break;
case 62:
#line 439 "asmparse.y"
{ yyval.cad = new CustomDescr(PASM->m_tkCurrentCVOwner, yypvt[-2].token, yypvt[-0].binstr); } break;
case 63:
#line 440 "asmparse.y"
{ yyval.cad = new CustomDescr(PASM->m_tkCurrentCVOwner, yypvt[-4].token, yypvt[-1].binstr); } break;
case 64:
#line 441 "asmparse.y"
{ yyval.cad = new CustomDescr(PASM->m_tkCurrentCVOwner, yypvt[-2].int32, yypvt[-1].binstr); } break;
case 65:
#line 444 "asmparse.y"
{ yyval.cad = new CustomDescr(yypvt[-2].token, yypvt[-0].token, NULL); } break;
case 66:
#line 445 "asmparse.y"
{ yyval.cad = new CustomDescr(yypvt[-4].token, yypvt[-2].token, yypvt[-0].binstr); } break;
case 67:
#line 447 "asmparse.y"
{ yyval.cad = new CustomDescr(yypvt[-6].token, yypvt[-4].token, yypvt[-1].binstr); } break;
case 68:
#line 448 "asmparse.y"
{ yyval.cad = new CustomDescr(PASM->m_tkCurrentCVOwner, yypvt[-2].int32, yypvt[-1].binstr); } break;
case 69:
#line 451 "asmparse.y"
{ yyval.int32 = yypvt[-2].token; bParsingByteArray = TRUE; } break;
case 70:
#line 455 "asmparse.y"
{ PASM->m_pCustomDescrList = NULL;
                                                                                  PASM->m_tkCurrentCVOwner = yypvt[-4].token;
                                                                                  yyval.int32 = yypvt[-2].token; bParsingByteArray = TRUE; } break;
case 71:
#line 460 "asmparse.y"
{ yyval.token = yypvt[-0].token; } break;
case 72:
#line 463 "asmparse.y"
{ yyval.token = yypvt[-0].token; } break;
case 73:
#line 464 "asmparse.y"
{ yyval.token = yypvt[-0].token; } break;
case 74:
#line 468 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                                                  yyval.binstr->appendInt16(nCustomBlobNVPairs);
                                                                                  yyval.binstr->append(yypvt[-0].binstr);
                                                                                  nCustomBlobNVPairs = 0; } break;
case 75:
#line 474 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt16(VAL16(0x0001)); } break;
case 76:
#line 475 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendFrom(yypvt[-0].binstr,1); } break;
case 77:
#line 476 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 78:
#line 479 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 79:
#line 481 "asmparse.y"
{ yyval.binstr = yypvt[-5].binstr; yyval.binstr->appendInt8(yypvt[-4].int32);
                                                                                  yyval.binstr->append(yypvt[-3].binstr); 
                                                                                  AppendStringWithLength(yyval.binstr,yypvt[-2].string);
                                                                                  yyval.binstr->appendFrom(yypvt[-0].binstr,1); 
                                                                                  nCustomBlobNVPairs++;} break;
case 80:
#line 486 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 81:
#line 489 "asmparse.y"
{ yyval.int32 = SERIALIZATION_TYPE_FIELD; } break;
case 82:
#line 490 "asmparse.y"
{ yyval.int32 = SERIALIZATION_TYPE_PROPERTY; } break;
case 83:
#line 493 "asmparse.y"
{ if(yypvt[-0].cad->tkOwner) 
                                                                                    PASM->DefineCV(yypvt[-0].cad);
                                                                                  else if(PASM->m_pCustomDescrList)
                                                                                    PASM->m_pCustomDescrList->PUSH(yypvt[-0].cad); } break;
case 84:
#line 497 "asmparse.y"
{ PASM->DefineCV(yypvt[-0].cad); } break;
case 85:
#line 498 "asmparse.y"
{ CustomDescr* pNew = new CustomDescr(yypvt[-0].tdd->m_pCA);
                                                                                  if(pNew->tkOwner == 0) pNew->tkOwner = PASM->m_tkCurrentCVOwner;
                                                                                  if(pNew->tkOwner) 
                                                                                    PASM->DefineCV(pNew);
                                                                                  else if(PASM->m_pCustomDescrList)
                                                                                    PASM->m_pCustomDescrList->PUSH(pNew); } break;
case 86:
#line 506 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 87:
#line 507 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_TYPE); } break;
case 88:
#line 508 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_TAGGED_OBJECT); } break;
case 89:
#line 509 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                AppendStringWithLength(yyval.binstr,yypvt[-0].string); } break;
case 90:
#line 511 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                AppendStringWithLength(yyval.binstr,PASM->ReflectionNotation(yypvt[-0].token)); } break;
case 91:
#line 513 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 92:
#line 518 "asmparse.y"
{ PASMM->SetModuleName(NULL); PASM->m_tkCurrentCVOwner=1; } break;
case 93:
#line 519 "asmparse.y"
{ PASMM->SetModuleName(yypvt[-0].string); PASM->m_tkCurrentCVOwner=1; } break;
case 94:
#line 520 "asmparse.y"
{ BinStr* pbs = new BinStr();
                                                                                  unsigned L = (unsigned)strlen(yypvt[-0].string);
                                                                                  memcpy((char*)(pbs->getBuff(L)),yypvt[-0].string,L);
                                                                                  PASM->EmitImport(pbs); delete pbs;} break;
case 95:
#line 527 "asmparse.y"
{ /*PASM->SetDataSection(); PASM->EmitDataLabel($7);*/
                                                                                  PASM->m_VTFList.PUSH(new VTFEntry((USHORT)yypvt[-4].int32, (USHORT)yypvt[-2].int32, yypvt[-0].string)); } break;
case 96:
#line 531 "asmparse.y"
{ yyval.int32 = 0; } break;
case 97:
#line 532 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | COR_VTABLE_32BIT; } break;
case 98:
#line 533 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | COR_VTABLE_64BIT; } break;
case 99:
#line 534 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | COR_VTABLE_FROM_UNMANAGED; } break;
case 100:
#line 535 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | COR_VTABLE_CALL_MOST_DERIVED; } break;
case 101:
#line 536 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | COR_VTABLE_FROM_UNMANAGED_RETAIN_APPDOMAIN; } break;
case 102:
#line 539 "asmparse.y"
{ PASM->m_pVTable = yypvt[-1].binstr; } break;
case 103:
#line 542 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 104:
#line 546 "asmparse.y"
{ PASM->StartNameSpace(yypvt[-0].string); } break;
case 105:
#line 549 "asmparse.y"
{ newclass = TRUE; } break;
case 106:
#line 552 "asmparse.y"
{ if(yypvt[-0].typarlist) FixupConstraints();
                                                                                  PASM->StartClass(yypvt[-1].string, yypvt[-2].classAttr, yypvt[-0].typarlist); 
                                                                                  TyParFixupList.RESET(false);
                                                                                  newclass = FALSE;
                                                                                } break;
case 107:
#line 558 "asmparse.y"
{ PASM->AddClass(); } break;
case 108:
#line 561 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) 0; } break;
case 109:
#line 562 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdVisibilityMask) | tdPublic); } break;
case 110:
#line 563 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdVisibilityMask) | tdNotPublic); } break;
case 111:
#line 564 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | 0x80000000 | tdSealed); } break;
case 112:
#line 565 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | 0x40000000); } break;
case 113:
#line 566 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdInterface | tdAbstract); } break;
case 114:
#line 567 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdSealed); } break;
case 115:
#line 568 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdAbstract); } break;
case 116:
#line 569 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdLayoutMask) | tdAutoLayout); } break;
case 117:
#line 570 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdLayoutMask) | tdSequentialLayout); } break;
case 118:
#line 571 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdLayoutMask) | tdExplicitLayout); } break;
case 119:
#line 572 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdStringFormatMask) | tdAnsiClass); } break;
case 120:
#line 573 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdStringFormatMask) | tdUnicodeClass); } break;
case 121:
#line 574 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-1].classAttr & ~tdStringFormatMask) | tdAutoClass); } break;
case 122:
#line 575 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdImport); } break;
case 123:
#line 576 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdSerializable); } break;
case 124:
#line 577 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedPublic); } break;
case 125:
#line 578 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedPrivate); } break;
case 126:
#line 579 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedFamily); } break;
case 127:
#line 580 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedAssembly); } break;
case 128:
#line 581 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedFamANDAssem); } break;
case 129:
#line 582 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) ((yypvt[-2].classAttr & ~tdVisibilityMask) | tdNestedFamORAssem); } break;
case 130:
#line 583 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdBeforeFieldInit); } break;
case 131:
#line 584 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr | tdSpecialName); } break;
case 132:
#line 585 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].classAttr); } break;
case 133:
#line 586 "asmparse.y"
{ yyval.classAttr = (CorRegTypeAttr) (yypvt[-1].int32); } break;
case 135:
#line 590 "asmparse.y"
{ PASM->m_crExtends = yypvt[-0].token; } break;
case 140:
#line 601 "asmparse.y"
{ PASM->AddToImplList(yypvt[-0].token); } break;
case 141:
#line 602 "asmparse.y"
{ PASM->AddToImplList(yypvt[-0].token); } break;
case 142:
#line 606 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 143:
#line 607 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 144:
#line 610 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt32(yypvt[-0].token); } break;
case 145:
#line 611 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->appendInt32(yypvt[-0].token); } break;
case 146:
#line 614 "asmparse.y"
{ yyval.typarlist = NULL; PASM->m_TyParList = NULL;} break;
case 147:
#line 615 "asmparse.y"
{ yyval.typarlist = yypvt[-1].typarlist;   PASM->m_TyParList = yypvt[-1].typarlist;} break;
case 148:
#line 618 "asmparse.y"
{ yyval.int32 = gpCovariant; } break;
case 149:
#line 619 "asmparse.y"
{ yyval.int32 = gpContravariant; } break;
case 150:
#line 620 "asmparse.y"
{ yyval.int32 = gpReferenceTypeConstraint; } break;
case 151:
#line 621 "asmparse.y"
{ yyval.int32 = gpNotNullableValueTypeConstraint; } break;
case 152:
#line 622 "asmparse.y"
{ yyval.int32 = gpDefaultConstructorConstraint; } break;
case 153:
#line 625 "asmparse.y"
{ yyval.int32 = 0; } break;
case 154:
#line 626 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | yypvt[-0].int32; } break;
case 155:
#line 629 "asmparse.y"
{yyval.typarlist = new TyParList(yypvt[-3].int32, yypvt[-2].binstr, yypvt[-1].string, yypvt[-0].typarlist);} break;
case 156:
#line 630 "asmparse.y"
{yyval.typarlist = new TyParList(yypvt[-2].int32, NULL, yypvt[-1].string, yypvt[-0].typarlist);} break;
case 157:
#line 633 "asmparse.y"
{ yyval.typarlist = NULL; } break;
case 158:
#line 634 "asmparse.y"
{ yyval.typarlist = yypvt[-0].typarlist; } break;
case 159:
#line 637 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 160:
#line 640 "asmparse.y"
{ yyval.int32= 0; } break;
case 161:
#line 641 "asmparse.y"
{ yyval.int32 = yypvt[-2].int32; } break;
case 162:
#line 645 "asmparse.y"
{ if(PASM->m_pCurMethod->m_ulLines[1] ==0)
                                                              {  PASM->m_pCurMethod->m_ulLines[1] = PASM->m_ulCurLine;
                                                                 PASM->m_pCurMethod->m_ulColumns[1]=PASM->m_ulCurColumn;}
                                                              PASM->EndMethod(); } break;
case 163:
#line 649 "asmparse.y"
{ PASM->EndClass(); } break;
case 164:
#line 650 "asmparse.y"
{ PASM->EndEvent(); } break;
case 165:
#line 651 "asmparse.y"
{ PASM->EndProp(); } break;
case 171:
#line 657 "asmparse.y"
{ PASM->m_pCurClass->m_ulSize = yypvt[-0].int32; } break;
case 172:
#line 658 "asmparse.y"
{ PASM->m_pCurClass->m_ulPack = yypvt[-0].int32; } break;
case 173:
#line 659 "asmparse.y"
{ PASMM->EndComType(); } break;
case 174:
#line 661 "asmparse.y"
{ BinStr *sig1 = parser->MakeSig(yypvt[-7].int32, yypvt[-6].binstr, yypvt[-1].binstr); 
                                                                  BinStr *sig2 = new BinStr(); sig2->append(sig1); 
                                                                  PASM->AddMethodImpl(yypvt[-11].token,yypvt[-9].string,sig1,yypvt[-5].token,yypvt[-3].string,sig2);
                                                                  PASM->ResetArgNameList(); 
                                                                } break;
case 175:
#line 667 "asmparse.y"
{ PASM->AddMethodImpl(yypvt[-17].token,yypvt[-15].string,
                                                                      (yypvt[-14].int32==0 ? parser->MakeSig(yypvt[-19].int32,yypvt[-18].binstr,yypvt[-12].binstr) :
                                                                      parser->MakeSig(yypvt[-19].int32| IMAGE_CEE_CS_CALLCONV_GENERIC,yypvt[-18].binstr,yypvt[-12].binstr,yypvt[-14].int32)),
                                                                      yypvt[-6].token,yypvt[-4].string,
                                                                      (yypvt[-3].int32==0 ? parser->MakeSig(yypvt[-8].int32,yypvt[-7].binstr,yypvt[-1].binstr) :
                                                                      parser->MakeSig(yypvt[-8].int32| IMAGE_CEE_CS_CALLCONV_GENERIC,yypvt[-7].binstr,yypvt[-1].binstr,yypvt[-3].int32))); 
                                                                   PASM->ResetArgNameList();
                                                                 } break;
case 178:
#line 677 "asmparse.y"
{ if((yypvt[-1].int32 > 0) && (yypvt[-1].int32 <= (int)PASM->m_pCurClass->m_NumTyPars))
                                                                PASM->m_pCustomDescrList = PASM->m_pCurClass->m_TyPars[yypvt[-1].int32-1].CAList();
                                                              else
                                                                PASM->report->error("Type parameter index out of range\n");
                                                            } break;
case 179:
#line 682 "asmparse.y"
{ int n = PASM->m_pCurClass->FindTyPar(yypvt[-0].string);
                                                              if(n >= 0)
                                                                PASM->m_pCustomDescrList = PASM->m_pCurClass->m_TyPars[n].CAList();
                                                              else
                                                                PASM->report->error("Type parameter '%s' undefined\n",yypvt[-0].string);
                                                            } break;
case 180:
#line 692 "asmparse.y"
{ yypvt[-3].binstr->insertInt8(IMAGE_CEE_CS_CALLCONV_FIELD);
                                                              PASM->AddField(yypvt[-2].string, yypvt[-3].binstr, yypvt[-4].fieldAttr, yypvt[-1].string, yypvt[-0].binstr, yypvt[-5].int32); } break;
case 181:
#line 696 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) 0; } break;
case 182:
#line 697 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].fieldAttr | fdStatic); } break;
case 183:
#line 698 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdPublic); } break;
case 184:
#line 699 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdPrivate); } break;
case 185:
#line 700 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdFamily); } break;
case 186:
#line 701 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].fieldAttr | fdInitOnly); } break;
case 187:
#line 702 "asmparse.y"
{ yyval.fieldAttr = yypvt[-1].fieldAttr; } break;
case 188:
#line 703 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].fieldAttr | fdSpecialName); } break;
case 189:
#line 716 "asmparse.y"
{ PASM->m_pMarshal = yypvt[-1].binstr; } break;
case 190:
#line 717 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdAssembly); } break;
case 191:
#line 718 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdFamANDAssem); } break;
case 192:
#line 719 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdFamORAssem); } break;
case 193:
#line 720 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) ((yypvt[-1].fieldAttr & ~mdMemberAccessMask) | fdPrivateScope); } break;
case 194:
#line 721 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].fieldAttr | fdLiteral); } break;
case 195:
#line 722 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].fieldAttr | fdNotSerialized); } break;
case 196:
#line 723 "asmparse.y"
{ yyval.fieldAttr = (CorFieldAttr) (yypvt[-1].int32); } break;
case 197:
#line 726 "asmparse.y"
{ yyval.string = 0; } break;
case 198:
#line 727 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 199:
#line 730 "asmparse.y"
{ yyval.binstr = NULL; } break;
case 200:
#line 731 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 201:
#line 734 "asmparse.y"
{ yyval.int32 = 0xFFFFFFFF; } break;
case 202:
#line 735 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32; } break;
case 203:
#line 740 "asmparse.y"
{ PASM->ResetArgNameList();
                                                               if (yypvt[-3].binstr == NULL)
                                                               {
                                                                 if((iCallConv)&&((yypvt[-8].int32 & iCallConv) != iCallConv)) parser->warn("'instance' added to method's calling convention\n"); 
                                                                 yyval.token = PASM->MakeMemberRef(yypvt[-6].token, yypvt[-4].string, parser->MakeSig(yypvt[-8].int32|iCallConv, yypvt[-7].binstr, yypvt[-1].binstr));
                                                               }
                                                               else
                                                               {
                                                                 mdToken mr;
                                                                 if((iCallConv)&&((yypvt[-8].int32 & iCallConv) != iCallConv)) parser->warn("'instance' added to method's calling convention\n"); 
                                                                 mr = PASM->MakeMemberRef(yypvt[-6].token, yypvt[-4].string, 
                                                                   parser->MakeSig(yypvt[-8].int32 | IMAGE_CEE_CS_CALLCONV_GENERIC|iCallConv, yypvt[-7].binstr, yypvt[-1].binstr, corCountArgs(yypvt[-3].binstr)));
                                                                 yyval.token = PASM->MakeMethodSpec(mr, 
                                                                   parser->MakeSig(IMAGE_CEE_CS_CALLCONV_INSTANTIATION, 0, yypvt[-3].binstr));
                                                               }
                                                             } break;
case 204:
#line 757 "asmparse.y"
{ PASM->ResetArgNameList();
                                                               if (yypvt[-3].binstr == NULL)
                                                               {
                                                                 if((iCallConv)&&((yypvt[-6].int32 & iCallConv) != iCallConv)) parser->warn("'instance' added to method's calling convention\n"); 
                                                                 yyval.token = PASM->MakeMemberRef(mdTokenNil, yypvt[-4].string, parser->MakeSig(yypvt[-6].int32|iCallConv, yypvt[-5].binstr, yypvt[-1].binstr));
                                                               }
                                                               else
                                                               {
                                                                 mdToken mr;
                                                                 if((iCallConv)&&((yypvt[-6].int32 & iCallConv) != iCallConv)) parser->warn("'instance' added to method's calling convention\n"); 
                                                                 mr = PASM->MakeMemberRef(mdTokenNil, yypvt[-4].string, parser->MakeSig(yypvt[-6].int32 | IMAGE_CEE_CS_CALLCONV_GENERIC|iCallConv, yypvt[-5].binstr, yypvt[-1].binstr, corCountArgs(yypvt[-3].binstr)));
                                                                 yyval.token = PASM->MakeMethodSpec(mr, 
                                                                   parser->MakeSig(IMAGE_CEE_CS_CALLCONV_INSTANTIATION, 0, yypvt[-3].binstr));
                                                               }
                                                             } break;
case 205:
#line 772 "asmparse.y"
{ yyval.token = yypvt[-0].token; } break;
case 206:
#line 773 "asmparse.y"
{ yyval.token = yypvt[-0].tdd->m_tkTypeSpec; } break;
case 207:
#line 774 "asmparse.y"
{ yyval.token = yypvt[-0].tdd->m_tkTypeSpec; } break;
case 208:
#line 777 "asmparse.y"
{ yyval.int32 = (yypvt[-0].int32 | IMAGE_CEE_CS_CALLCONV_HASTHIS); } break;
case 209:
#line 778 "asmparse.y"
{ yyval.int32 = (yypvt[-0].int32 | IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS); } break;
case 210:
#line 779 "asmparse.y"
{ yyval.int32 = yypvt[-0].int32; } break;
case 211:
#line 780 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32; } break;
case 212:
#line 783 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_DEFAULT; } break;
case 213:
#line 784 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_DEFAULT; } break;
case 214:
#line 785 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_VARARG; } break;
case 215:
#line 786 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_C; } break;
case 216:
#line 787 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_STDCALL; } break;
case 217:
#line 788 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_THISCALL; } break;
case 218:
#line 789 "asmparse.y"
{ yyval.int32 = IMAGE_CEE_CS_CALLCONV_FASTCALL; } break;
case 219:
#line 792 "asmparse.y"
{ yyval.token = yypvt[-1].int32; } break;
case 220:
#line 795 "asmparse.y"
{ yyval.token = yypvt[-0].token; 
                                                               PASM->delArgNameList(PASM->m_firstArgName);
                                                               PASM->m_firstArgName = parser->m_ANSFirst.POP();
                                                               PASM->m_lastArgName = parser->m_ANSLast.POP();
                                                               PASM->SetMemberRefFixup(yypvt[-0].token,iOpcodeLen); } break;
case 221:
#line 801 "asmparse.y"
{ yypvt[-3].binstr->insertInt8(IMAGE_CEE_CS_CALLCONV_FIELD); 
                                                               yyval.token = PASM->MakeMemberRef(yypvt[-2].token, yypvt[-0].string, yypvt[-3].binstr); 
                                                               PASM->SetMemberRefFixup(yyval.token,iOpcodeLen); } break;
case 222:
#line 805 "asmparse.y"
{ yypvt[-1].binstr->insertInt8(IMAGE_CEE_CS_CALLCONV_FIELD); 
                                                               yyval.token = PASM->MakeMemberRef(NULL, yypvt[-0].string, yypvt[-1].binstr); 
                                                               PASM->SetMemberRefFixup(yyval.token,iOpcodeLen); } break;
case 223:
#line 808 "asmparse.y"
{ yyval.token = yypvt[-0].tdd->m_tkTypeSpec;
                                                               PASM->SetMemberRefFixup(yyval.token,iOpcodeLen); } break;
case 224:
#line 810 "asmparse.y"
{ yyval.token = yypvt[-0].tdd->m_tkTypeSpec;
                                                               PASM->SetMemberRefFixup(yyval.token,iOpcodeLen); } break;
case 225:
#line 812 "asmparse.y"
{ yyval.token = yypvt[-0].token; 
                                                               PASM->SetMemberRefFixup(yyval.token,iOpcodeLen); } break;
case 226:
#line 817 "asmparse.y"
{ PASM->ResetEvent(yypvt[-0].string, yypvt[-1].token, yypvt[-2].eventAttr); } break;
case 227:
#line 818 "asmparse.y"
{ PASM->ResetEvent(yypvt[-0].string, mdTypeRefNil, yypvt[-1].eventAttr); } break;
case 228:
#line 822 "asmparse.y"
{ yyval.eventAttr = (CorEventAttr) 0; } break;
case 229:
#line 823 "asmparse.y"
{ yyval.eventAttr = yypvt[-1].eventAttr; } break;
case 230:
#line 824 "asmparse.y"
{ yyval.eventAttr = (CorEventAttr) (yypvt[-1].eventAttr | evSpecialName); } break;
case 233:
#line 831 "asmparse.y"
{ PASM->SetEventMethod(0, yypvt[-0].token); } break;
case 234:
#line 832 "asmparse.y"
{ PASM->SetEventMethod(1, yypvt[-0].token); } break;
case 235:
#line 833 "asmparse.y"
{ PASM->SetEventMethod(2, yypvt[-0].token); } break;
case 236:
#line 834 "asmparse.y"
{ PASM->SetEventMethod(3, yypvt[-0].token); } break;
case 241:
#line 843 "asmparse.y"
{ PASM->ResetProp(yypvt[-4].string, 
                                                              parser->MakeSig((IMAGE_CEE_CS_CALLCONV_PROPERTY |
                                                              (yypvt[-6].int32 & IMAGE_CEE_CS_CALLCONV_HASTHIS)),yypvt[-5].binstr,yypvt[-2].binstr), yypvt[-7].propAttr, yypvt[-0].binstr);} break;
case 242:
#line 848 "asmparse.y"
{ yyval.propAttr = (CorPropertyAttr) 0; } break;
case 243:
#line 849 "asmparse.y"
{ yyval.propAttr = yypvt[-1].propAttr; } break;
case 244:
#line 850 "asmparse.y"
{ yyval.propAttr = (CorPropertyAttr) (yypvt[-1].propAttr | prSpecialName); } break;
case 247:
#line 858 "asmparse.y"
{ PASM->SetPropMethod(0, yypvt[-0].token); } break;
case 248:
#line 859 "asmparse.y"
{ PASM->SetPropMethod(1, yypvt[-0].token); } break;
case 249:
#line 860 "asmparse.y"
{ PASM->SetPropMethod(2, yypvt[-0].token); } break;
case 254:
#line 868 "asmparse.y"
{ PASM->ResetForNextMethod(); 
                                                              uMethodBeginLine = PASM->m_ulCurLine;
                                                              uMethodBeginColumn=PASM->m_ulCurColumn;
                                                            } break;
case 255:
#line 874 "asmparse.y"
{ yyval.binstr = NULL; } break;
case 256:
#line 875 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 257:
#line 879 "asmparse.y"
{ BinStr* sig;
                                                              if (yypvt[-5].typarlist == NULL) sig = parser->MakeSig(yypvt[-10].int32, yypvt[-8].binstr, yypvt[-3].binstr);
                                                              else {
                                                               FixupTyPars(yypvt[-8].binstr);
                                                               sig = parser->MakeSig(yypvt[-10].int32 | IMAGE_CEE_CS_CALLCONV_GENERIC, yypvt[-8].binstr, yypvt[-3].binstr, yypvt[-5].typarlist->Count());
                                                               FixupConstraints();
                                                              }
                                                              PASM->StartMethod(yypvt[-6].string, sig, yypvt[-11].methAttr, yypvt[-7].binstr, yypvt[-9].int32, yypvt[-5].typarlist);
                                                              TyParFixupList.RESET(false);
                                                              PASM->SetImplAttr((USHORT)yypvt[-1].implAttr);  
                                                              PASM->m_pCurMethod->m_ulLines[0] = uMethodBeginLine;
                                                              PASM->m_pCurMethod->m_ulColumns[0]=uMethodBeginColumn; 
                                                            } break;
case 258:
#line 894 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) 0; } break;
case 259:
#line 895 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdStatic); } break;
case 260:
#line 896 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdPublic); } break;
case 261:
#line 897 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdPrivate); } break;
case 262:
#line 898 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdFamily); } break;
case 263:
#line 899 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdFinal); } break;
case 264:
#line 900 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdSpecialName); } break;
case 265:
#line 901 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdVirtual); } break;
case 266:
#line 902 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdCheckAccessOnOverride); } break;
case 267:
#line 903 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdAbstract); } break;
case 268:
#line 904 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdAssem); } break;
case 269:
#line 905 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdFamANDAssem); } break;
case 270:
#line 906 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdFamORAssem); } break;
case 271:
#line 907 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) ((yypvt[-1].methAttr & ~mdMemberAccessMask) | mdPrivateScope); } break;
case 272:
#line 908 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdHideBySig); } break;
case 273:
#line 909 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdNewSlot); } break;
case 274:
#line 910 "asmparse.y"
{ yyval.methAttr = yypvt[-1].methAttr; } break;
case 275:
#line 911 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdUnmanagedExport); } break;
case 276:
#line 912 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].methAttr | mdRequireSecObject); } break;
case 277:
#line 913 "asmparse.y"
{ yyval.methAttr = (CorMethodAttr) (yypvt[-1].int32); } break;
case 278:
#line 915 "asmparse.y"
{ PASM->SetPinvoke(yypvt[-4].binstr,0,yypvt[-2].binstr,yypvt[-1].pinvAttr); 
                                                              yyval.methAttr = (CorMethodAttr) (yypvt[-7].methAttr | mdPinvokeImpl); } break;
case 279:
#line 918 "asmparse.y"
{ PASM->SetPinvoke(yypvt[-2].binstr,0,NULL,yypvt[-1].pinvAttr); 
                                                              yyval.methAttr = (CorMethodAttr) (yypvt[-5].methAttr | mdPinvokeImpl); } break;
case 280:
#line 921 "asmparse.y"
{ PASM->SetPinvoke(new BinStr(),0,NULL,yypvt[-1].pinvAttr); 
                                                              yyval.methAttr = (CorMethodAttr) (yypvt[-4].methAttr | mdPinvokeImpl); } break;
case 281:
#line 925 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) 0; } break;
case 282:
#line 926 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmNoMangle); } break;
case 283:
#line 927 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCharSetAnsi); } break;
case 284:
#line 928 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCharSetUnicode); } break;
case 285:
#line 929 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCharSetAuto); } break;
case 286:
#line 930 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmSupportsLastError); } break;
case 287:
#line 931 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCallConvWinapi); } break;
case 288:
#line 932 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCallConvCdecl); } break;
case 289:
#line 933 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCallConvStdcall); } break;
case 290:
#line 934 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCallConvThiscall); } break;
case 291:
#line 935 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].pinvAttr | pmCallConvFastcall); } break;
case 292:
#line 936 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-3].pinvAttr | pmBestFitEnabled); } break;
case 293:
#line 937 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-3].pinvAttr | pmBestFitDisabled); } break;
case 294:
#line 938 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-3].pinvAttr | pmThrowOnUnmappableCharEnabled); } break;
case 295:
#line 939 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-3].pinvAttr | pmThrowOnUnmappableCharDisabled); } break;
case 296:
#line 940 "asmparse.y"
{ yyval.pinvAttr = (CorPinvokeMap) (yypvt[-1].int32); } break;
case 297:
#line 943 "asmparse.y"
{ yyval.string = newString(COR_CTOR_METHOD_NAME); } break;
case 298:
#line 944 "asmparse.y"
{ yyval.string = newString(COR_CCTOR_METHOD_NAME); } break;
case 299:
#line 945 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 300:
#line 948 "asmparse.y"
{ yyval.int32 = 0; } break;
case 301:
#line 949 "asmparse.y"
{ yyval.int32 = yypvt[-3].int32 | pdIn; } break;
case 302:
#line 950 "asmparse.y"
{ yyval.int32 = yypvt[-3].int32 | pdOut; } break;
case 303:
#line 951 "asmparse.y"
{ yyval.int32 = yypvt[-3].int32 | pdOptional; } break;
case 304:
#line 952 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 + 1; } break;
case 305:
#line 955 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (miIL | miManaged); } break;
case 306:
#line 956 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) ((yypvt[-1].implAttr & 0xFFF4) | miNative); } break;
case 307:
#line 957 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) ((yypvt[-1].implAttr & 0xFFF4) | miIL); } break;
case 308:
#line 958 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) ((yypvt[-1].implAttr & 0xFFF4) | miOPTIL); } break;
case 309:
#line 959 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) ((yypvt[-1].implAttr & 0xFFFB) | miManaged); } break;
case 310:
#line 960 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) ((yypvt[-1].implAttr & 0xFFFB) | miUnmanaged); } break;
case 311:
#line 961 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miForwardRef); } break;
case 312:
#line 962 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miPreserveSig); } break;
case 313:
#line 963 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miRuntime); } break;
case 314:
#line 964 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miInternalCall); } break;
case 315:
#line 965 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miSynchronized); } break;
case 316:
#line 966 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].implAttr | miNoInlining); } break;
case 317:
#line 967 "asmparse.y"
{ yyval.implAttr = (CorMethodImpl) (yypvt[-1].int32); } break;
case 318:
#line 970 "asmparse.y"
{ PASM->delArgNameList(PASM->m_firstArgName); PASM->m_firstArgName = NULL;PASM->m_lastArgName = NULL; 
                                                            } break;
case 321:
#line 978 "asmparse.y"
{ PASM->EmitByte(yypvt[-0].int32); } break;
case 322:
#line 979 "asmparse.y"
{ delete PASM->m_SEHD; PASM->m_SEHD = PASM->m_SEHDstack.POP(); } break;
case 323:
#line 980 "asmparse.y"
{ PASM->EmitMaxStack(yypvt[-0].int32); } break;
case 324:
#line 981 "asmparse.y"
{ PASM->EmitLocals(parser->MakeSig(IMAGE_CEE_CS_CALLCONV_LOCAL_SIG, 0, yypvt[-1].binstr)); 
                                                            } break;
case 325:
#line 983 "asmparse.y"
{ PASM->EmitZeroInit(); 
                                                              PASM->EmitLocals(parser->MakeSig(IMAGE_CEE_CS_CALLCONV_LOCAL_SIG, 0, yypvt[-1].binstr)); 
                                                            } break;
case 326:
#line 986 "asmparse.y"
{ PASM->EmitEntryPoint(); } break;
case 327:
#line 987 "asmparse.y"
{ PASM->EmitZeroInit(); } break;
case 330:
#line 990 "asmparse.y"
{ PASM->AddLabel(PASM->m_CurPC,yypvt[-1].string); /*PASM->EmitLabel($1);*/ } break;
case 336:
#line 996 "asmparse.y"
{ if(PASM->m_pCurMethod->m_dwExportOrdinal == 0xFFFFFFFF)
                                                              {
                                                                PASM->m_pCurMethod->m_dwExportOrdinal = yypvt[-1].int32;
                                                                PASM->m_pCurMethod->m_szExportAlias = NULL;
                                                                if(PASM->m_pCurMethod->m_wVTEntry == 0) PASM->m_pCurMethod->m_wVTEntry = 1;
                                                                if(PASM->m_pCurMethod->m_wVTSlot  == 0) PASM->m_pCurMethod->m_wVTSlot = yypvt[-1].int32 + 0x8000;
                                                              }
                                                              else
                                                                PASM->report->warn("Duplicate .export directive, ignored\n");
                                                            } break;
case 337:
#line 1006 "asmparse.y"
{ if(PASM->m_pCurMethod->m_dwExportOrdinal == 0xFFFFFFFF)
                                                              {
                                                                PASM->m_pCurMethod->m_dwExportOrdinal = yypvt[-3].int32;
                                                                PASM->m_pCurMethod->m_szExportAlias = yypvt[-0].string;
                                                                if(PASM->m_pCurMethod->m_wVTEntry == 0) PASM->m_pCurMethod->m_wVTEntry = 1;
                                                                if(PASM->m_pCurMethod->m_wVTSlot  == 0) PASM->m_pCurMethod->m_wVTSlot = yypvt[-3].int32 + 0x8000;
                                                              }
                                                              else
                                                                PASM->report->warn("Duplicate .export directive, ignored\n");
                                                            } break;
case 338:
#line 1016 "asmparse.y"
{ PASM->m_pCurMethod->m_wVTEntry = (WORD)yypvt[-2].int32;
                                                              PASM->m_pCurMethod->m_wVTSlot = (WORD)yypvt[-0].int32; } break;
case 339:
#line 1019 "asmparse.y"
{ PASM->AddMethodImpl(yypvt[-2].token,yypvt[-0].string,NULL,NULL,NULL,NULL); } break;
case 340:
#line 1022 "asmparse.y"
{ PASM->AddMethodImpl(yypvt[-6].token,yypvt[-4].string,
                                                              (yypvt[-3].int32==0 ? parser->MakeSig(yypvt[-8].int32,yypvt[-7].binstr,yypvt[-1].binstr) :
                                                              parser->MakeSig(yypvt[-8].int32| IMAGE_CEE_CS_CALLCONV_GENERIC,yypvt[-7].binstr,yypvt[-1].binstr,yypvt[-3].int32))
                                                              ,NULL,NULL,NULL); 
                                                              PASM->ResetArgNameList();
                                                            } break;
case 342:
#line 1029 "asmparse.y"
{ if((yypvt[-1].int32 > 0) && (yypvt[-1].int32 <= (int)PASM->m_pCurMethod->m_NumTyPars))
                                                                PASM->m_pCustomDescrList = PASM->m_pCurMethod->m_TyPars[yypvt[-1].int32-1].CAList();
                                                              else
                                                                PASM->report->error("Type parameter index out of range\n");
                                                            } break;
case 343:
#line 1034 "asmparse.y"
{ int n = PASM->m_pCurMethod->FindTyPar(yypvt[-0].string);
                                                              if(n >= 0)
                                                                PASM->m_pCustomDescrList = PASM->m_pCurMethod->m_TyPars[n].CAList();
                                                              else
                                                                PASM->report->error("Type parameter '%s' undefined\n",yypvt[-0].string);
                                                            } break;
case 344:
#line 1041 "asmparse.y"
{ if( yypvt[-2].int32 ) {
                                                                ARG_NAME_LIST* pAN=PASM->findArg(PASM->m_pCurMethod->m_firstArgName, yypvt[-2].int32 - 1);
                                                                if(pAN)
                                                                {
                                                                    PASM->m_pCustomDescrList = &(pAN->CustDList);
                                                                    pAN->pValue = yypvt[-0].binstr;
                                                                }
                                                                else
                                                                {
                                                                    PASM->m_pCustomDescrList = NULL;
                                                                    if(yypvt[-0].binstr) delete yypvt[-0].binstr;
                                                                }
                                                              } else {
                                                                PASM->m_pCustomDescrList = &(PASM->m_pCurMethod->m_RetCustDList);
                                                                PASM->m_pCurMethod->m_pRetValue = yypvt[-0].binstr;
                                                              }
                                                              PASM->m_tkCurrentCVOwner = 0;
                                                            } break;
case 345:
#line 1061 "asmparse.y"
{ PASM->m_pCurMethod->CloseScope(); } break;
case 346:
#line 1064 "asmparse.y"
{ PASM->m_pCurMethod->OpenScope(); } break;
case 350:
#line 1075 "asmparse.y"
{ PASM->m_SEHD->tryTo = PASM->m_CurPC; } break;
case 351:
#line 1076 "asmparse.y"
{ PASM->SetTryLabels(yypvt[-2].string, yypvt[-0].string); } break;
case 352:
#line 1077 "asmparse.y"
{ if(PASM->m_SEHD) {PASM->m_SEHD->tryFrom = yypvt[-2].int32;
                                                              PASM->m_SEHD->tryTo = yypvt[-0].int32;} } break;
case 353:
#line 1081 "asmparse.y"
{ PASM->NewSEHDescriptor();
                                                              PASM->m_SEHD->tryFrom = PASM->m_CurPC; } break;
case 354:
#line 1086 "asmparse.y"
{ PASM->EmitTry(); } break;
case 355:
#line 1087 "asmparse.y"
{ PASM->EmitTry(); } break;
case 356:
#line 1088 "asmparse.y"
{ PASM->EmitTry(); } break;
case 357:
#line 1089 "asmparse.y"
{ PASM->EmitTry(); } break;
case 358:
#line 1093 "asmparse.y"
{ PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 359:
#line 1094 "asmparse.y"
{ PASM->SetFilterLabel(yypvt[-0].string); 
                                                               PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 360:
#line 1096 "asmparse.y"
{ PASM->m_SEHD->sehFilter = yypvt[-0].int32; 
                                                               PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 361:
#line 1100 "asmparse.y"
{ PASM->m_SEHD->sehClause = COR_ILEXCEPTION_CLAUSE_FILTER;
                                                               PASM->m_SEHD->sehFilter = PASM->m_CurPC; } break;
case 362:
#line 1104 "asmparse.y"
{  PASM->m_SEHD->sehClause = COR_ILEXCEPTION_CLAUSE_NONE;
                                                               PASM->SetCatchClass(yypvt[-0].token); 
                                                               PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 363:
#line 1109 "asmparse.y"
{ PASM->m_SEHD->sehClause = COR_ILEXCEPTION_CLAUSE_FINALLY;
                                                               PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 364:
#line 1113 "asmparse.y"
{ PASM->m_SEHD->sehClause = COR_ILEXCEPTION_CLAUSE_FAULT;
                                                               PASM->m_SEHD->sehHandler = PASM->m_CurPC; } break;
case 365:
#line 1117 "asmparse.y"
{ PASM->m_SEHD->sehHandlerTo = PASM->m_CurPC; } break;
case 366:
#line 1118 "asmparse.y"
{ PASM->SetHandlerLabels(yypvt[-2].string, yypvt[-0].string); } break;
case 367:
#line 1119 "asmparse.y"
{ PASM->m_SEHD->sehHandler = yypvt[-2].int32;
                                                               PASM->m_SEHD->sehHandlerTo = yypvt[-0].int32; } break;
case 369:
#line 1127 "asmparse.y"
{ PASM->EmitDataLabel(yypvt[-1].string); } break;
case 371:
#line 1131 "asmparse.y"
{ PASM->SetDataSection(); } break;
case 372:
#line 1132 "asmparse.y"
{ PASM->SetTLSSection(); } break;
case 373:
#line 1133 "asmparse.y"
{ PASM->SetILSection(); } break;
case 378:
#line 1144 "asmparse.y"
{ yyval.int32 = 1; } break;
case 379:
#line 1145 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32;
                                                               if(yypvt[-1].int32 <= 0) { PASM->report->error("Illegal item count: %d\n",yypvt[-1].int32);
                                                                  if(!PASM->OnErrGo) yyval.int32 = 1; }} break;
case 380:
#line 1150 "asmparse.y"
{ PASM->EmitDataString(yypvt[-1].binstr); } break;
case 381:
#line 1151 "asmparse.y"
{ PASM->EmitDD(yypvt[-1].string); } break;
case 382:
#line 1152 "asmparse.y"
{ PASM->EmitData(yypvt[-1].binstr->ptr(),yypvt[-1].binstr->length()); } break;
case 383:
#line 1154 "asmparse.y"
{ float f = (float) (*yypvt[-2].float64); float* p = new (nothrow) float[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int i=0; i < yypvt[-0].int32; i++) p[i] = f;
                                                                 PASM->EmitData(p, sizeof(float)*yypvt[-0].int32); delete yypvt[-2].float64; delete [] p; 
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(float)*yypvt[-0].int32); } break;
case 384:
#line 1161 "asmparse.y"
{ double* p = new (nothrow) double[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int i=0; i<yypvt[-0].int32; i++) p[i] = *(yypvt[-2].float64);
                                                                 PASM->EmitData(p, sizeof(double)*yypvt[-0].int32); delete yypvt[-2].float64; delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(double)*yypvt[-0].int32); } break;
case 385:
#line 1168 "asmparse.y"
{ __int64* p = new (nothrow) __int64[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int i=0; i<yypvt[-0].int32; i++) p[i] = *(yypvt[-2].int64);
                                                                 PASM->EmitData(p, sizeof(__int64)*yypvt[-0].int32); delete yypvt[-2].int64; delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int64)*yypvt[-0].int32); } break;
case 386:
#line 1175 "asmparse.y"
{ __int32* p = new (nothrow) __int32[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int i=0; i<yypvt[-0].int32; i++) p[i] = yypvt[-2].int32;
                                                                 PASM->EmitData(p, sizeof(__int32)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int32)*yypvt[-0].int32); } break;
case 387:
#line 1182 "asmparse.y"
{ __int16 i = (__int16) yypvt[-2].int32; FAIL_UNLESS(i == yypvt[-2].int32, ("Value %d too big\n", yypvt[-2].int32));
                                                               __int16* p = new (nothrow) __int16[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int j=0; j<yypvt[-0].int32; j++) p[j] = i;
                                                                 PASM->EmitData(p, sizeof(__int16)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int16)*yypvt[-0].int32); } break;
case 388:
#line 1190 "asmparse.y"
{ __int8 i = (__int8) yypvt[-2].int32; FAIL_UNLESS(i == yypvt[-2].int32, ("Value %d too big\n", yypvt[-2].int32));
                                                               __int8* p = new (nothrow) __int8[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 for(int j=0; j<yypvt[-0].int32; j++) p[j] = i;
                                                                 PASM->EmitData(p, sizeof(__int8)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int8)*yypvt[-0].int32); } break;
case 389:
#line 1197 "asmparse.y"
{ float* p = new (nothrow) float[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(float)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(float)*yypvt[-0].int32); } break;
case 390:
#line 1202 "asmparse.y"
{ double* p = new (nothrow) double[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(double)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(double)*yypvt[-0].int32); } break;
case 391:
#line 1207 "asmparse.y"
{ __int64* p = new (nothrow) __int64[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(__int64)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int64)*yypvt[-0].int32); } break;
case 392:
#line 1212 "asmparse.y"
{ __int32* p = new (nothrow) __int32[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(__int32)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int32)*yypvt[-0].int32); } break;
case 393:
#line 1217 "asmparse.y"
{ __int16* p = new (nothrow) __int16[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(__int16)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int16)*yypvt[-0].int32); } break;
case 394:
#line 1222 "asmparse.y"
{ __int8* p = new (nothrow) __int8[yypvt[-0].int32];
                                                               if(p != NULL) {
                                                                 PASM->EmitData(p, sizeof(__int8)*yypvt[-0].int32); delete [] p;
                                                               } else PASM->report->error("Out of memory emitting data block %d bytes\n",
                                                                     sizeof(__int8)*yypvt[-0].int32); } break;
case 395:
#line 1230 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R4);
                                                               float f = (float)(*yypvt[-1].float64);
                                                               yyval.binstr->appendInt32(*((__int32*)&f)); delete yypvt[-1].float64; } break;
case 396:
#line 1233 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R8); 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-1].float64); delete yypvt[-1].float64; } break;
case 397:
#line 1235 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R4); 
                                                               yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 398:
#line 1237 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R8); 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-1].int64); delete yypvt[-1].int64; } break;
case 399:
#line 1239 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I8); 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-1].int64); delete yypvt[-1].int64; } break;
case 400:
#line 1241 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I4); 
                                                               yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 401:
#line 1243 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I2); 
                                                               yyval.binstr->appendInt16(yypvt[-1].int32); } break;
case 402:
#line 1245 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I1); 
                                                               yyval.binstr->appendInt8(yypvt[-1].int32); } break;
case 403:
#line 1247 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U8); 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-1].int64); delete yypvt[-1].int64; } break;
case 404:
#line 1249 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U4); 
                                                               yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 405:
#line 1251 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U2); 
                                                               yyval.binstr->appendInt16(yypvt[-1].int32); } break;
case 406:
#line 1253 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U1); 
                                                               yyval.binstr->appendInt8(yypvt[-1].int32); } break;
case 407:
#line 1255 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U8); 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-1].int64); delete yypvt[-1].int64; } break;
case 408:
#line 1257 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U4); 
                                                               yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 409:
#line 1259 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U2); 
                                                               yyval.binstr->appendInt16(yypvt[-1].int32); } break;
case 410:
#line 1261 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U1); 
                                                               yyval.binstr->appendInt8(yypvt[-1].int32); } break;
case 411:
#line 1263 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_CHAR); 
                                                               yyval.binstr->appendInt16(yypvt[-1].int32); } break;
case 412:
#line 1265 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_BOOLEAN); 
                                                               yyval.binstr->appendInt8(yypvt[-1].int32);} break;
case 413:
#line 1267 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_STRING);
                                                               yyval.binstr->append(yypvt[-1].binstr); delete yypvt[-1].binstr;} break;
case 414:
#line 1271 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 415:
#line 1274 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 416:
#line 1275 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 417:
#line 1278 "asmparse.y"
{ __int8 i = (__int8) yypvt[-0].int32; yyval.binstr = new BinStr(); yyval.binstr->appendInt8(i); } break;
case 418:
#line 1279 "asmparse.y"
{ __int8 i = (__int8) yypvt[-0].int32; yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt8(i); } break;
case 419:
#line 1283 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 420:
#line 1284 "asmparse.y"
{ yyval.binstr = BinStrToUnicode(yypvt[-0].binstr,true); yyval.binstr->insertInt8(ELEMENT_TYPE_STRING);} break;
case 421:
#line 1285 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_CLASS); 
                                                               yyval.binstr->appendInt32(0); } break;
case 422:
#line 1290 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 423:
#line 1291 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_STRING); yyval.binstr->appendInt8(0xFF); } break;
case 424:
#line 1292 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_STRING); 
                                                               AppendStringWithLength(yyval.binstr,yypvt[-1].string); delete [] yypvt[-1].string;} break;
case 425:
#line 1294 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_TYPE); 
                                                               AppendStringWithLength(yyval.binstr,yypvt[-1].string); delete [] yypvt[-1].string;} break;
case 426:
#line 1296 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_TYPE); 
                                                               AppendStringWithLength(yyval.binstr,PASM->ReflectionNotation(yypvt[-1].token));} break;
case 427:
#line 1298 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(SERIALIZATION_TYPE_TYPE); yyval.binstr->appendInt8(0xFF); } break;
case 428:
#line 1299 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt8(SERIALIZATION_TYPE_TAGGED_OBJECT);} break;
case 429:
#line 1301 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 430:
#line 1304 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 431:
#line 1307 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 432:
#line 1310 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 433:
#line 1313 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 434:
#line 1316 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 435:
#line 1319 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 436:
#line 1322 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 437:
#line 1325 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 438:
#line 1328 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 439:
#line 1331 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 440:
#line 1334 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 441:
#line 1337 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 442:
#line 1340 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 443:
#line 1343 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 444:
#line 1346 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 445:
#line 1349 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 446:
#line 1352 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 447:
#line 1355 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt32(yypvt[-4].int32);
                                                               yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 448:
#line 1360 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 449:
#line 1361 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr;
                                                               float f = (float) (*yypvt[-0].float64); yyval.binstr->appendInt32(*((__int32*)&f)); delete yypvt[-0].float64; } break;
case 450:
#line 1363 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               yyval.binstr->appendInt32(yypvt[-0].int32); } break;
case 451:
#line 1367 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 452:
#line 1368 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-0].float64); delete yypvt[-0].float64; } break;
case 453:
#line 1370 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-0].int64); delete yypvt[-0].int64; } break;
case 454:
#line 1374 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 455:
#line 1375 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               yyval.binstr->appendInt64((__int64 *)yypvt[-0].int64); delete yypvt[-0].int64; } break;
case 456:
#line 1379 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 457:
#line 1380 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt32(yypvt[-0].int32);} break;
case 458:
#line 1383 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 459:
#line 1384 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt16(yypvt[-0].int32);} break;
case 460:
#line 1387 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 461:
#line 1388 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt8(yypvt[-0].int32); } break;
case 462:
#line 1391 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 463:
#line 1392 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               yyval.binstr->appendInt8(yypvt[-0].int32);} break;
case 464:
#line 1396 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 465:
#line 1397 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt8(0xFF); } break;
case 466:
#line 1398 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               AppendStringWithLength(yyval.binstr,yypvt[-0].string); delete [] yypvt[-0].string;} break;
case 467:
#line 1402 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 468:
#line 1403 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->appendInt8(0xFF); } break;
case 469:
#line 1404 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; 
                                                               AppendStringWithLength(yyval.binstr,yypvt[-0].string); delete [] yypvt[-0].string;} break;
case 470:
#line 1406 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; 
                                                               AppendStringWithLength(yyval.binstr,PASM->ReflectionNotation(yypvt[-0].token));} break;
case 471:
#line 1410 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 472:
#line 1411 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr; } break;
case 473:
#line 1415 "asmparse.y"
{ parser->m_ANSFirst.PUSH(PASM->m_firstArgName);
                                                               parser->m_ANSLast.PUSH(PASM->m_lastArgName);   
                                                               PASM->m_firstArgName = NULL;
                                                               PASM->m_lastArgName = NULL; } break;
case 474:
#line 1421 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 475:
#line 1424 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 476:
#line 1427 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 477:
#line 1430 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 478:
#line 1433 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 479:
#line 1436 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 480:
#line 1439 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); 
                                                               if((!PASM->OnErrGo)&&
                                                               ((yypvt[-0].opcode == CEE_NEWOBJ)||
                                                                (yypvt[-0].opcode == CEE_CALLVIRT))) 
                                                                  iCallConv = IMAGE_CEE_CS_CALLCONV_HASTHIS;
                                                             } break;
case 481:
#line 1447 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 482:
#line 1450 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 483:
#line 1453 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 484:
#line 1456 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 485:
#line 1459 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); iOpcodeLen = PASM->OpcodeLen(yyval.instr); } break;
case 486:
#line 1462 "asmparse.y"
{ yyval.instr = SetupInstr(yypvt[-0].opcode); } break;
case 487:
#line 1465 "asmparse.y"
{ yyval.instr = yypvt[-1].instr; bParsingByteArray = TRUE; } break;
case 488:
#line 1469 "asmparse.y"
{ PASM->EmitOpcode(yypvt[-0].instr); } break;
case 489:
#line 1470 "asmparse.y"
{ PASM->EmitInstrVar(yypvt[-1].instr, yypvt[-0].int32); } break;
case 490:
#line 1471 "asmparse.y"
{ PASM->EmitInstrVarByName(yypvt[-1].instr, yypvt[-0].string); } break;
case 491:
#line 1472 "asmparse.y"
{ PASM->EmitInstrI(yypvt[-1].instr, yypvt[-0].int32); } break;
case 492:
#line 1473 "asmparse.y"
{ PASM->EmitInstrI8(yypvt[-1].instr, yypvt[-0].int64); } break;
case 493:
#line 1474 "asmparse.y"
{ PASM->EmitInstrR(yypvt[-1].instr, yypvt[-0].float64); delete (yypvt[-0].float64);} break;
case 494:
#line 1475 "asmparse.y"
{ double f = (double) (*yypvt[-0].int64); PASM->EmitInstrR(yypvt[-1].instr, &f); } break;
case 495:
#line 1476 "asmparse.y"
{ unsigned L = yypvt[-1].binstr->length();
                                                               FAIL_UNLESS(L >= sizeof(float), ("%d hexbytes, must be at least %d\n",
                                                                           L,sizeof(float))); 
                                                               if(L < sizeof(float)) {YYERROR; } 
                                                               else {
                                                                   double f = (L >= sizeof(double)) ? *((double *)(yypvt[-1].binstr->ptr()))
                                                                                    : (double)(*(float *)(yypvt[-1].binstr->ptr())); 
                                                                   PASM->EmitInstrR(yypvt[-2].instr,&f); }
                                                               delete yypvt[-1].binstr; } break;
case 496:
#line 1485 "asmparse.y"
{ PASM->EmitInstrBrOffset(yypvt[-1].instr, yypvt[-0].int32); } break;
case 497:
#line 1486 "asmparse.y"
{ PASM->EmitInstrBrTarget(yypvt[-1].instr, yypvt[-0].string); } break;
case 498:
#line 1488 "asmparse.y"
{ PASM->SetMemberRefFixup(yypvt[-0].token,PASM->OpcodeLen(yypvt[-1].instr));
                                                               PASM->EmitInstrI(yypvt[-1].instr,yypvt[-0].token);
                                                               PASM->m_tkCurrentCVOwner = yypvt[-0].token;
                                                               PASM->m_pCustomDescrList = NULL;
                                                               iCallConv = 0;
                                                             } break;
case 499:
#line 1495 "asmparse.y"
{ yypvt[-3].binstr->insertInt8(IMAGE_CEE_CS_CALLCONV_FIELD); 
                                                               mdToken mr = PASM->MakeMemberRef(yypvt[-2].token, yypvt[-0].string, yypvt[-3].binstr);
                                                               PASM->SetMemberRefFixup(mr, PASM->OpcodeLen(yypvt[-4].instr));
                                                               PASM->EmitInstrI(yypvt[-4].instr,mr);
                                                               PASM->m_tkCurrentCVOwner = mr;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 500:
#line 1503 "asmparse.y"
{ yypvt[-1].binstr->insertInt8(IMAGE_CEE_CS_CALLCONV_FIELD); 
                                                               mdToken mr = PASM->MakeMemberRef(mdTokenNil, yypvt[-0].string, yypvt[-1].binstr);
                                                               PASM->SetMemberRefFixup(mr, PASM->OpcodeLen(yypvt[-2].instr));
                                                               PASM->EmitInstrI(yypvt[-2].instr,mr);
                                                               PASM->m_tkCurrentCVOwner = mr;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 501:
#line 1510 "asmparse.y"
{ mdToken mr = yypvt[-0].token;
                                                               PASM->SetMemberRefFixup(mr, PASM->OpcodeLen(yypvt[-1].instr));
                                                               PASM->EmitInstrI(yypvt[-1].instr,mr);
                                                               PASM->m_tkCurrentCVOwner = mr;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 502:
#line 1516 "asmparse.y"
{ mdToken mr = yypvt[-0].tdd->m_tkTypeSpec;
                                                               PASM->SetMemberRefFixup(mr, PASM->OpcodeLen(yypvt[-1].instr));
                                                               PASM->EmitInstrI(yypvt[-1].instr,mr);
                                                               PASM->m_tkCurrentCVOwner = mr;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 503:
#line 1522 "asmparse.y"
{ mdToken mr = yypvt[-0].tdd->m_tkTypeSpec;
                                                               PASM->SetMemberRefFixup(mr, PASM->OpcodeLen(yypvt[-1].instr));
                                                               PASM->EmitInstrI(yypvt[-1].instr,mr);
                                                               PASM->m_tkCurrentCVOwner = mr;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 504:
#line 1528 "asmparse.y"
{ PASM->EmitInstrI(yypvt[-1].instr, yypvt[-0].token); 
                                                               PASM->m_tkCurrentCVOwner = yypvt[-0].token;
                                                               PASM->m_pCustomDescrList = NULL;
                                                             } break;
case 505:
#line 1532 "asmparse.y"
{ PASM->EmitInstrStringLiteral(yypvt[-1].instr, yypvt[-0].binstr,TRUE); } break;
case 506:
#line 1534 "asmparse.y"
{ PASM->EmitInstrStringLiteral(yypvt[-4].instr, yypvt[-1].binstr,FALSE); } break;
case 507:
#line 1536 "asmparse.y"
{ PASM->EmitInstrStringLiteral(yypvt[-3].instr, yypvt[-1].binstr,FALSE,TRUE); } break;
case 508:
#line 1538 "asmparse.y"
{ PASM->EmitInstrSig(yypvt[-5].instr, parser->MakeSig(yypvt[-4].int32, yypvt[-3].binstr, yypvt[-1].binstr)); 
                                                               PASM->ResetArgNameList();
                                                             } break;
case 509:
#line 1542 "asmparse.y"
{ PASM->EmitInstrI(yypvt[-1].instr,yypvt[-0].token);
                                                               PASM->m_tkCurrentCVOwner = yypvt[-0].token;
                                                               PASM->m_pCustomDescrList = NULL;
                                                               iOpcodeLen = 0;
                                                             } break;
case 510:
#line 1547 "asmparse.y"
{ PASM->EmitInstrSwitch(yypvt[-3].instr, yypvt[-1].labels); } break;
case 511:
#line 1550 "asmparse.y"
{ yyval.labels = 0; } break;
case 512:
#line 1551 "asmparse.y"
{ yyval.labels = new Labels(yypvt[-2].string, yypvt[-0].labels, TRUE); } break;
case 513:
#line 1552 "asmparse.y"
{ yyval.labels = new Labels((char *)(UINT_PTR)yypvt[-2].int32, yypvt[-0].labels, FALSE); } break;
case 514:
#line 1553 "asmparse.y"
{ yyval.labels = new Labels(yypvt[-0].string, NULL, TRUE); } break;
case 515:
#line 1554 "asmparse.y"
{ yyval.labels = new Labels((char *)(UINT_PTR)yypvt[-0].int32, NULL, FALSE); } break;
case 516:
#line 1558 "asmparse.y"
{ yyval.binstr = NULL; } break;
case 517:
#line 1559 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 518:
#line 1562 "asmparse.y"
{ yyval.binstr = NULL; } break;
case 519:
#line 1563 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 520:
#line 1564 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 521:
#line 1567 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr; } break;
case 522:
#line 1568 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; } break;
case 523:
#line 1571 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 524:
#line 1572 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr;} break;
case 525:
#line 1575 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 526:
#line 1576 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr; } break;
case 527:
#line 1579 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_SENTINEL); } break;
case 528:
#line 1580 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->append(yypvt[-1].binstr); PASM->addArgName(NULL, yypvt[-1].binstr, yypvt[-0].binstr, yypvt[-2].int32); } break;
case 529:
#line 1581 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->append(yypvt[-2].binstr); PASM->addArgName(yypvt[-0].string, yypvt[-2].binstr, yypvt[-1].binstr, yypvt[-3].int32);} break;
case 530:
#line 1585 "asmparse.y"
{ yyval.token = PASM->ResolveClassRef(PASM->GetAsmRef(yypvt[-2].string), yypvt[-0].string, NULL); delete[] yypvt[-2].string;} break;
case 531:
#line 1586 "asmparse.y"
{ yyval.token = PASM->ResolveClassRef(yypvt[-2].token, yypvt[-0].string, NULL); } break;
case 532:
#line 1587 "asmparse.y"
{ yyval.token = PASM->ResolveClassRef(mdTokenNil, yypvt[-0].string, NULL); } break;
case 533:
#line 1588 "asmparse.y"
{ yyval.token = PASM->ResolveClassRef(PASM->GetModRef(yypvt[-2].string),yypvt[-0].string, NULL); delete[] yypvt[-2].string;} break;
case 534:
#line 1589 "asmparse.y"
{ yyval.token = PASM->ResolveClassRef(1,yypvt[-0].string,NULL); } break;
case 535:
#line 1590 "asmparse.y"
{ yyval.token = yypvt[-0].token; } break;
case 536:
#line 1591 "asmparse.y"
{ yyval.token = yypvt[-0].tdd->m_tkTypeSpec; } break;
case 537:
#line 1592 "asmparse.y"
{ if(PASM->m_pCurClass != NULL) yyval.token = PASM->m_pCurClass->m_cl;
                                                                else { yyval.token = 0; PASM->report->error(".this outside class scope\n"); } 
                                                              } break;
case 538:
#line 1595 "asmparse.y"
{ if(PASM->m_pCurClass != NULL) {
                                                                  yyval.token = PASM->m_pCurClass->m_crExtends;
                                                                  if(RidFromToken(yyval.token) == 0)
                                                                    PASM->report->error(".base undefined\n");
                                                                } else { yyval.token = 0; PASM->report->error(".base outside class scope\n"); } 
                                                              } break;
case 539:
#line 1601 "asmparse.y"
{ if(PASM->m_pCurClass != NULL) {
                                                                  if(PASM->m_pCurClass->m_pEncloser != NULL) yyval.token = PASM->m_pCurClass->m_pEncloser->m_cl;
                                                                  else { yyval.token = 0; PASM->report->error(".nester undefined\n"); }
                                                                } else { yyval.token = 0; PASM->report->error(".nester outside class scope\n"); } 
                                                              } break;
case 540:
#line 1608 "asmparse.y"
{ yyval.string = yypvt[-0].string; } break;
case 541:
#line 1609 "asmparse.y"
{ yyval.string = newStringWDel(yypvt[-2].string, NESTING_SEP, yypvt[-0].string); } break;
case 542:
#line 1612 "asmparse.y"
{ yyval.token = yypvt[-0].token;} break;
case 543:
#line 1613 "asmparse.y"
{ yyval.token = PASM->GetAsmRef(yypvt[-1].string); delete[] yypvt[-1].string;} break;
case 544:
#line 1614 "asmparse.y"
{ yyval.token = PASM->GetModRef(yypvt[-1].string); delete[] yypvt[-1].string;} break;
case 545:
#line 1615 "asmparse.y"
{ yyval.token = PASM->ResolveTypeSpec(yypvt[-0].binstr); } break;
case 546:
#line 1619 "asmparse.y"
{ yyval.binstr = new BinStr(); } break;
case 547:
#line 1621 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_CUSTOMMARSHALER);
                                                                corEmitInt(yyval.binstr,yypvt[-7].binstr->length()); yyval.binstr->append(yypvt[-7].binstr);
                                                                corEmitInt(yyval.binstr,yypvt[-5].binstr->length()); yyval.binstr->append(yypvt[-5].binstr);
                                                                corEmitInt(yyval.binstr,yypvt[-3].binstr->length()); yyval.binstr->append(yypvt[-3].binstr);
                                                                corEmitInt(yyval.binstr,yypvt[-1].binstr->length()); yyval.binstr->append(yypvt[-1].binstr); 
                                                                PASM->report->warn("Deprecated 4-string form of custom marshaler, first two strings ignored\n");} break;
case 548:
#line 1628 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_CUSTOMMARSHALER);
                                                                corEmitInt(yyval.binstr,0);
                                                                corEmitInt(yyval.binstr,0);
                                                                corEmitInt(yyval.binstr,yypvt[-3].binstr->length()); yyval.binstr->append(yypvt[-3].binstr);
                                                                corEmitInt(yyval.binstr,yypvt[-1].binstr->length()); yyval.binstr->append(yypvt[-1].binstr); } break;
case 549:
#line 1633 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_FIXEDSYSSTRING);
                                                                corEmitInt(yyval.binstr,yypvt[-1].int32); } break;
case 550:
#line 1636 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_FIXEDARRAY);
                                                                corEmitInt(yyval.binstr,yypvt[-2].int32); yyval.binstr->append(yypvt[-0].binstr); } break;
case 551:
#line 1638 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_VARIANT); 
                                                                PASM->report->warn("Deprecated native type 'variant'\n"); } break;
case 552:
#line 1640 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_CURRENCY); } break;
case 553:
#line 1641 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_SYSCHAR); 
                                                                PASM->report->warn("Deprecated native type 'syschar'\n"); } break;
case 554:
#line 1643 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_VOID); 
                                                                PASM->report->warn("Deprecated native type 'void'\n"); } break;
case 555:
#line 1645 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_BOOLEAN); } break;
case 556:
#line 1646 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_I1); } break;
case 557:
#line 1647 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_I2); } break;
case 558:
#line 1648 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_I4); } break;
case 559:
#line 1649 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_I8); } break;
case 560:
#line 1650 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_R4); } break;
case 561:
#line 1651 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_R8); } break;
case 562:
#line 1652 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_ERROR); } break;
case 563:
#line 1653 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U1); } break;
case 564:
#line 1654 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U2); } break;
case 565:
#line 1655 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U4); } break;
case 566:
#line 1656 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U8); } break;
case 567:
#line 1657 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U1); } break;
case 568:
#line 1658 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U2); } break;
case 569:
#line 1659 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U4); } break;
case 570:
#line 1660 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_U8); } break;
case 571:
#line 1661 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt8(NATIVE_TYPE_PTR); 
                                                                PASM->report->warn("Deprecated native type '*'\n"); } break;
case 572:
#line 1663 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; if(yyval.binstr->length()==0) yyval.binstr->appendInt8(NATIVE_TYPE_MAX);
                                                                yyval.binstr->insertInt8(NATIVE_TYPE_ARRAY); } break;
case 573:
#line 1665 "asmparse.y"
{ yyval.binstr = yypvt[-3].binstr; if(yyval.binstr->length()==0) yyval.binstr->appendInt8(NATIVE_TYPE_MAX); 
                                                                yyval.binstr->insertInt8(NATIVE_TYPE_ARRAY);
                                                                corEmitInt(yyval.binstr,0);
                                                                corEmitInt(yyval.binstr,yypvt[-1].int32); 
                                                                corEmitInt(yyval.binstr,0); } break;
case 574:
#line 1670 "asmparse.y"
{ yyval.binstr = yypvt[-5].binstr; if(yyval.binstr->length()==0) yyval.binstr->appendInt8(NATIVE_TYPE_MAX); 
                                                                yyval.binstr->insertInt8(NATIVE_TYPE_ARRAY);
                                                                corEmitInt(yyval.binstr,yypvt[-1].int32);
                                                                corEmitInt(yyval.binstr,yypvt[-3].int32); } break;
case 575:
#line 1674 "asmparse.y"
{ yyval.binstr = yypvt[-4].binstr; if(yyval.binstr->length()==0) yyval.binstr->appendInt8(NATIVE_TYPE_MAX); 
                                                                yyval.binstr->insertInt8(NATIVE_TYPE_ARRAY);
                                                                corEmitInt(yyval.binstr,yypvt[-1].int32); } break;
case 576:
#line 1677 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_DECIMAL); 
                                                                PASM->report->warn("Deprecated native type 'decimal'\n"); } break;
case 577:
#line 1679 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_DATE); 
                                                                PASM->report->warn("Deprecated native type 'date'\n"); } break;
case 578:
#line 1681 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_BSTR); } break;
case 579:
#line 1682 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_LPSTR); } break;
case 580:
#line 1683 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_LPWSTR); } break;
case 581:
#line 1684 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_LPTSTR); } break;
case 582:
#line 1685 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_OBJECTREF); 
                                                                PASM->report->warn("Deprecated native type 'objectref'\n"); } break;
case 583:
#line 1687 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_IUNKNOWN);
                                                                if(yypvt[-0].int32 != -1) corEmitInt(yyval.binstr,yypvt[-0].int32); } break;
case 584:
#line 1689 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_IDISPATCH); 
                                                                if(yypvt[-0].int32 != -1) corEmitInt(yyval.binstr,yypvt[-0].int32); } break;
case 585:
#line 1691 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_STRUCT); } break;
case 586:
#line 1692 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_INTF);
                                                                if(yypvt[-0].int32 != -1) corEmitInt(yyval.binstr,yypvt[-0].int32); } break;
case 587:
#line 1694 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_SAFEARRAY); 
                                                                corEmitInt(yyval.binstr,yypvt[-0].int32); 
                                                                corEmitInt(yyval.binstr,0);} break;
case 588:
#line 1697 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_SAFEARRAY); 
                                                                corEmitInt(yyval.binstr,yypvt[-2].int32); 
                                                                corEmitInt(yyval.binstr,yypvt[-0].binstr->length()); yyval.binstr->append(yypvt[-0].binstr); } break;
case 589:
#line 1701 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_INT); } break;
case 590:
#line 1702 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_UINT); } break;
case 591:
#line 1703 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_UINT); } break;
case 592:
#line 1704 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_NESTEDSTRUCT); 
                                                                PASM->report->warn("Deprecated native type 'nested struct'\n"); } break;
case 593:
#line 1706 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_BYVALSTR); } break;
case 594:
#line 1707 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_ANSIBSTR); } break;
case 595:
#line 1708 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_TBSTR); } break;
case 596:
#line 1709 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_VARIANTBOOL); } break;
case 597:
#line 1710 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_FUNC); } break;
case 598:
#line 1711 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_ASANY); } break;
case 599:
#line 1712 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(NATIVE_TYPE_LPSTRUCT); } break;
case 600:
#line 1713 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->append(yypvt[-0].tdd->m_pbsTypeSpec); } break;
case 601:
#line 1716 "asmparse.y"
{ yyval.int32 = -1; } break;
case 602:
#line 1717 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32; } break;
case 603:
#line 1720 "asmparse.y"
{ yyval.int32 = VT_EMPTY; } break;
case 604:
#line 1721 "asmparse.y"
{ yyval.int32 = VT_NULL; } break;
case 605:
#line 1722 "asmparse.y"
{ yyval.int32 = VT_VARIANT; } break;
case 606:
#line 1723 "asmparse.y"
{ yyval.int32 = VT_CY; } break;
case 607:
#line 1724 "asmparse.y"
{ yyval.int32 = VT_VOID; } break;
case 608:
#line 1725 "asmparse.y"
{ yyval.int32 = VT_BOOL; } break;
case 609:
#line 1726 "asmparse.y"
{ yyval.int32 = VT_I1; } break;
case 610:
#line 1727 "asmparse.y"
{ yyval.int32 = VT_I2; } break;
case 611:
#line 1728 "asmparse.y"
{ yyval.int32 = VT_I4; } break;
case 612:
#line 1729 "asmparse.y"
{ yyval.int32 = VT_I8; } break;
case 613:
#line 1730 "asmparse.y"
{ yyval.int32 = VT_R4; } break;
case 614:
#line 1731 "asmparse.y"
{ yyval.int32 = VT_R8; } break;
case 615:
#line 1732 "asmparse.y"
{ yyval.int32 = VT_UI1; } break;
case 616:
#line 1733 "asmparse.y"
{ yyval.int32 = VT_UI2; } break;
case 617:
#line 1734 "asmparse.y"
{ yyval.int32 = VT_UI4; } break;
case 618:
#line 1735 "asmparse.y"
{ yyval.int32 = VT_UI8; } break;
case 619:
#line 1736 "asmparse.y"
{ yyval.int32 = VT_UI1; } break;
case 620:
#line 1737 "asmparse.y"
{ yyval.int32 = VT_UI2; } break;
case 621:
#line 1738 "asmparse.y"
{ yyval.int32 = VT_UI4; } break;
case 622:
#line 1739 "asmparse.y"
{ yyval.int32 = VT_UI8; } break;
case 623:
#line 1740 "asmparse.y"
{ yyval.int32 = VT_PTR; } break;
case 624:
#line 1741 "asmparse.y"
{ yyval.int32 = yypvt[-2].int32 | VT_ARRAY; } break;
case 625:
#line 1742 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | VT_VECTOR; } break;
case 626:
#line 1743 "asmparse.y"
{ yyval.int32 = yypvt[-1].int32 | VT_BYREF; } break;
case 627:
#line 1744 "asmparse.y"
{ yyval.int32 = VT_DECIMAL; } break;
case 628:
#line 1745 "asmparse.y"
{ yyval.int32 = VT_DATE; } break;
case 629:
#line 1746 "asmparse.y"
{ yyval.int32 = VT_BSTR; } break;
case 630:
#line 1747 "asmparse.y"
{ yyval.int32 = VT_LPSTR; } break;
case 631:
#line 1748 "asmparse.y"
{ yyval.int32 = VT_LPWSTR; } break;
case 632:
#line 1749 "asmparse.y"
{ yyval.int32 = VT_UNKNOWN; } break;
case 633:
#line 1750 "asmparse.y"
{ yyval.int32 = VT_DISPATCH; } break;
case 634:
#line 1751 "asmparse.y"
{ yyval.int32 = VT_SAFEARRAY; } break;
case 635:
#line 1752 "asmparse.y"
{ yyval.int32 = VT_INT; } break;
case 636:
#line 1753 "asmparse.y"
{ yyval.int32 = VT_UINT; } break;
case 637:
#line 1754 "asmparse.y"
{ yyval.int32 = VT_UINT; } break;
case 638:
#line 1755 "asmparse.y"
{ yyval.int32 = VT_ERROR; } break;
case 639:
#line 1756 "asmparse.y"
{ yyval.int32 = VT_HRESULT; } break;
case 640:
#line 1757 "asmparse.y"
{ yyval.int32 = VT_CARRAY; } break;
case 641:
#line 1758 "asmparse.y"
{ yyval.int32 = VT_USERDEFINED; } break;
case 642:
#line 1759 "asmparse.y"
{ yyval.int32 = VT_RECORD; } break;
case 643:
#line 1760 "asmparse.y"
{ yyval.int32 = VT_FILETIME; } break;
case 644:
#line 1761 "asmparse.y"
{ yyval.int32 = VT_BLOB; } break;
case 645:
#line 1762 "asmparse.y"
{ yyval.int32 = VT_STREAM; } break;
case 646:
#line 1763 "asmparse.y"
{ yyval.int32 = VT_STORAGE; } break;
case 647:
#line 1764 "asmparse.y"
{ yyval.int32 = VT_STREAMED_OBJECT; } break;
case 648:
#line 1765 "asmparse.y"
{ yyval.int32 = VT_STORED_OBJECT; } break;
case 649:
#line 1766 "asmparse.y"
{ yyval.int32 = VT_BLOB_OBJECT; } break;
case 650:
#line 1767 "asmparse.y"
{ yyval.int32 = VT_CF; } break;
case 651:
#line 1768 "asmparse.y"
{ yyval.int32 = VT_CLSID; } break;
case 652:
#line 1772 "asmparse.y"
{ if(yypvt[-0].token == PASM->m_tkSysString)
                                                                {     yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_STRING); }
                                                                else if(yypvt[-0].token == PASM->m_tkSysObject)
                                                                {     yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_OBJECT); }
                                                                else  
                                                                 yyval.binstr = parser->MakeTypeClass(ELEMENT_TYPE_CLASS, yypvt[-0].token); } break;
case 653:
#line 1778 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_OBJECT); } break;
case 654:
#line 1779 "asmparse.y"
{ yyval.binstr = parser->MakeTypeClass(ELEMENT_TYPE_VALUETYPE, yypvt[-0].token); } break;
case 655:
#line 1780 "asmparse.y"
{ yyval.binstr = parser->MakeTypeClass(ELEMENT_TYPE_VALUETYPE, yypvt[-0].token); } break;
case 656:
#line 1781 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_SZARRAY); } break;
case 657:
#line 1782 "asmparse.y"
{ yyval.binstr = parser->MakeTypeArray(ELEMENT_TYPE_ARRAY, yypvt[-3].binstr, yypvt[-1].binstr); } break;
case 658:
#line 1786 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_BYREF); } break;
case 659:
#line 1787 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_PTR); } break;
case 660:
#line 1788 "asmparse.y"
{ yyval.binstr = yypvt[-1].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_PINNED); } break;
case 661:
#line 1789 "asmparse.y"
{ yyval.binstr = parser->MakeTypeClass(ELEMENT_TYPE_CMOD_REQD, yypvt[-1].token);
                                                                yyval.binstr->append(yypvt[-4].binstr); } break;
case 662:
#line 1791 "asmparse.y"
{ yyval.binstr = parser->MakeTypeClass(ELEMENT_TYPE_CMOD_OPT, yypvt[-1].token);
                                                                yyval.binstr->append(yypvt[-4].binstr); } break;
case 663:
#line 1794 "asmparse.y"
{ yyval.binstr = parser->MakeSig(yypvt[-5].int32, yypvt[-4].binstr, yypvt[-1].binstr);
                                                                yyval.binstr->insertInt8(ELEMENT_TYPE_FNPTR); 
                                                                PASM->delArgNameList(PASM->m_firstArgName);
                                                                PASM->m_firstArgName = parser->m_ANSFirst.POP();
                                                                PASM->m_lastArgName = parser->m_ANSLast.POP();
                                                              } break;
case 664:
#line 1800 "asmparse.y"
{ if(yypvt[-1].binstr == NULL) yyval.binstr = yypvt[-3].binstr;
                                                                else {
                                                                  yyval.binstr = new BinStr(); 
                                                                  yyval.binstr->appendInt8(ELEMENT_TYPE_GENERICINST); 
                                                                  yyval.binstr->append(yypvt[-3].binstr);
                                                                  corEmitInt(yyval.binstr, corCountArgs(yypvt[-1].binstr));
                                                                  yyval.binstr->append(yypvt[-1].binstr); delete yypvt[-3].binstr; delete yypvt[-1].binstr; }} break;
case 665:
#line 1807 "asmparse.y"
{ //if(PASM->m_pCurMethod)  {
                                                                //  if(($3 < 0)||((DWORD)$3 >= PASM->m_pCurMethod->m_NumTyPars))
                                                                //    PASM->report->error("Invalid method type parameter '%d'\n",$3);
                                                                  yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_MVAR); corEmitInt(yyval.binstr, yypvt[-0].int32); 
                                                                //} else PASM->report->error("Method type parameter '%d' outside method scope\n",$3);
                                                              } break;
case 666:
#line 1813 "asmparse.y"
{ //if(PASM->m_pCurClass)  {
                                                                //  if(($2 < 0)||((DWORD)$2 >= PASM->m_pCurClass->m_NumTyPars))
                                                                //    PASM->report->error("Invalid type parameter '%d'\n",$2);
                                                                  yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_VAR); corEmitInt(yyval.binstr, yypvt[-0].int32); 
                                                                //} else PASM->report->error("Type parameter '%d' outside class scope\n",$2);
                                                              } break;
case 667:
#line 1819 "asmparse.y"
{ int eltype = ELEMENT_TYPE_MVAR;
                                                                int n=-1;
                                                                if(PASM->m_pCurMethod) n = PASM->m_pCurMethod->FindTyPar(yypvt[-0].string);
                                                                else {
                                                                  if(PASM->m_TyParList) n = PASM->m_TyParList->IndexOf(yypvt[-0].string);
                                                                  if(n == -1)
                                                                  { n = TyParFixupList.COUNT();
                                                                    TyParFixupList.PUSH(yypvt[-0].string);
                                                                    eltype = ELEMENT_TYPE_MVARFIXUP;
                                                                  }
                                                                }
                                                                if(n == -1) { PASM->report->error("Invalid method type parameter '%s'\n",yypvt[-0].string);
                                                                n = 0x1FFFFFFF; }
                                                                yyval.binstr = new BinStr(); yyval.binstr->appendInt8(eltype); corEmitInt(yyval.binstr,n); 
                                                              } break;
case 668:
#line 1834 "asmparse.y"
{ int eltype = ELEMENT_TYPE_VAR;
                                                                int n=-1;
                                                                if(PASM->m_pCurClass && !newclass) n = PASM->m_pCurClass->FindTyPar(yypvt[-0].string);
                                                                else {
                                                                  if(PASM->m_TyParList) n = PASM->m_TyParList->IndexOf(yypvt[-0].string);
                                                                  if(n == -1)
                                                                  { n = TyParFixupList.COUNT();
                                                                    TyParFixupList.PUSH(yypvt[-0].string);
                                                                    eltype = ELEMENT_TYPE_VARFIXUP;
                                                                  }
                                                                }
                                                                if(n == -1) { PASM->report->error("Invalid type parameter '%s'\n",yypvt[-0].string);
                                                                n = 0x1FFFFFFF; }
                                                                yyval.binstr = new BinStr(); yyval.binstr->appendInt8(eltype); corEmitInt(yyval.binstr,n); 
                                                              } break;
case 669:
#line 1849 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_TYPEDBYREF); } break;
case 670:
#line 1850 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_VOID); } break;
case 671:
#line 1851 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I); } break;
case 672:
#line 1852 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U); } break;
case 673:
#line 1853 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U); } break;
case 674:
#line 1854 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R); } break;
case 675:
#line 1855 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 676:
#line 1856 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; yyval.binstr->insertInt8(ELEMENT_TYPE_SENTINEL); } break;
case 677:
#line 1859 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_CHAR); } break;
case 678:
#line 1860 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_STRING); } break;
case 679:
#line 1861 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_BOOLEAN); } break;
case 680:
#line 1862 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I1); } break;
case 681:
#line 1863 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I2); } break;
case 682:
#line 1864 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I4); } break;
case 683:
#line 1865 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_I8); } break;
case 684:
#line 1866 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R4); } break;
case 685:
#line 1867 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_R8); } break;
case 686:
#line 1868 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U1); } break;
case 687:
#line 1869 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U2); } break;
case 688:
#line 1870 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U4); } break;
case 689:
#line 1871 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U8); } break;
case 690:
#line 1872 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U1); } break;
case 691:
#line 1873 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U2); } break;
case 692:
#line 1874 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U4); } break;
case 693:
#line 1875 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt8(ELEMENT_TYPE_U8); } break;
case 694:
#line 1876 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->append(yypvt[-0].tdd->m_pbsTypeSpec); } break;
case 695:
#line 1879 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; } break;
case 696:
#line 1880 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yypvt[-2].binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr; } break;
case 697:
#line 1883 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt32(0x7FFFFFFF); yyval.binstr->appendInt32(0x7FFFFFFF);  } break;
case 698:
#line 1884 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt32(0x7FFFFFFF); yyval.binstr->appendInt32(0x7FFFFFFF);  } break;
case 699:
#line 1885 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt32(0); yyval.binstr->appendInt32(yypvt[-0].int32); } break;
case 700:
#line 1886 "asmparse.y"
{ FAIL_UNLESS(yypvt[-2].int32 <= yypvt[-0].int32, ("lower bound %d must be <= upper bound %d\n", yypvt[-2].int32, yypvt[-0].int32));
                                                                if (yypvt[-2].int32 > yypvt[-0].int32) { YYERROR; };        
                                                                yyval.binstr = new BinStr(); yyval.binstr->appendInt32(yypvt[-2].int32); yyval.binstr->appendInt32(yypvt[-0].int32-yypvt[-2].int32+1); } break;
case 701:
#line 1889 "asmparse.y"
{ yyval.binstr = new BinStr(); yyval.binstr->appendInt32(yypvt[-1].int32); yyval.binstr->appendInt32(0x7FFFFFFF); } break;
case 702:
#line 1894 "asmparse.y"
{ PASM->AddPermissionDecl(yypvt[-4].secAct, yypvt[-3].token, yypvt[-1].pair); } break;
case 703:
#line 1896 "asmparse.y"
{ PASM->AddPermissionDecl(yypvt[-5].secAct, yypvt[-4].token, yypvt[-1].binstr); } break;
case 704:
#line 1897 "asmparse.y"
{ PASM->AddPermissionDecl(yypvt[-1].secAct, yypvt[-0].token, (NVPair *)NULL); } break;
case 705:
#line 1898 "asmparse.y"
{ PASM->AddPermissionSetDecl(yypvt[-2].secAct, yypvt[-1].binstr); } break;
case 706:
#line 1900 "asmparse.y"
{ PASM->AddPermissionSetDecl(yypvt[-1].secAct,BinStrToUnicode(yypvt[-0].binstr,true));} break;
case 707:
#line 1902 "asmparse.y"
{ BinStr* ret = new BinStr();
                                                                ret->insertInt8('.');
                                                                corEmitInt(ret, nSecAttrBlobs);
                                                                ret->append(yypvt[-1].binstr);
                                                                PASM->AddPermissionSetDecl(yypvt[-4].secAct,ret);
                                                                nSecAttrBlobs = 0; } break;
case 708:
#line 1910 "asmparse.y"
{ yyval.binstr = new BinStr(); nSecAttrBlobs = 0;} break;
case 709:
#line 1911 "asmparse.y"
{ yyval.binstr = yypvt[-0].binstr; nSecAttrBlobs = 1; } break;
case 710:
#line 1912 "asmparse.y"
{ yyval.binstr = yypvt[-2].binstr; yyval.binstr->append(yypvt[-0].binstr); nSecAttrBlobs++; } break;
case 711:
#line 1916 "asmparse.y"
{ yyval.binstr = PASM->EncodeSecAttr(PASM->ReflectionNotation(yypvt[-4].token),yypvt[-1].binstr,nCustomBlobNVPairs); 
                                                                nCustomBlobNVPairs = 0; } break;
case 712:
#line 1919 "asmparse.y"
{ yyval.binstr = PASM->EncodeSecAttr(yypvt[-4].string,yypvt[-1].binstr,nCustomBlobNVPairs); 
                                                                nCustomBlobNVPairs = 0; } break;
case 713:
#line 1923 "asmparse.y"
{ yyval.secAct = yypvt[-2].secAct; bParsingByteArray = TRUE; } break;
case 714:
#line 1925 "asmparse.y"
{ yyval.secAct = yypvt[-2].secAct; bParsingByteArray = TRUE; } break;
case 715:
#line 1928 "asmparse.y"
{ yyval.pair = yypvt[-0].pair; } break;
case 716:
#line 1929 "asmparse.y"
{ yyval.pair = yypvt[-2].pair->Concat(yypvt[-0].pair); } break;
case 717:
#line 1932 "asmparse.y"
{ yypvt[-2].binstr->appendInt8(0); yyval.pair = new NVPair(yypvt[-2].binstr, yypvt[-0].binstr); } break;
case 718:
#line 1935 "asmparse.y"
{ yyval.int32 = 1; } break;
case 719:
#line 1936 "asmparse.y"
{ yyval.int32 = 0; } break;
case 720:
#line 1939 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_BOOLEAN);
                                                                yyval.binstr->appendInt8(yypvt[-0].int32); } break;
case 721:
#line 1942 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_I4);
                                                                yyval.binstr->appendInt32(yypvt[-0].int32); } break;
case 722:
#line 1945 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_I4);
                                                                yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 723:
#line 1948 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_STRING);
                                                                yyval.binstr->append(yypvt[-0].binstr); delete yypvt[-0].binstr;
                                                                yyval.binstr->appendInt8(0); } break;
case 724:
#line 1952 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                char* sz = PASM->ReflectionNotation(yypvt[-5].token);
                                                                strcpy_s((char *)yyval.binstr->getBuff((unsigned)strlen(sz) + 1), strlen(sz) + 1,sz);
                                                                yyval.binstr->appendInt8(1);
                                                                yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 725:
#line 1958 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                char* sz = PASM->ReflectionNotation(yypvt[-5].token);
                                                                strcpy_s((char *)yyval.binstr->getBuff((unsigned)strlen(sz) + 1), strlen(sz) + 1,sz);
                                                                yyval.binstr->appendInt8(2);
                                                                yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 726:
#line 1964 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                char* sz = PASM->ReflectionNotation(yypvt[-5].token);
                                                                strcpy_s((char *)yyval.binstr->getBuff((unsigned)strlen(sz) + 1), strlen(sz) + 1,sz);
                                                                yyval.binstr->appendInt8(4);
                                                                yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 727:
#line 1970 "asmparse.y"
{ yyval.binstr = new BinStr();
                                                                yyval.binstr->appendInt8(SERIALIZATION_TYPE_ENUM);
                                                                char* sz = PASM->ReflectionNotation(yypvt[-3].token);
                                                                strcpy_s((char *)yyval.binstr->getBuff((unsigned)strlen(sz) + 1), strlen(sz) + 1,sz);
                                                                yyval.binstr->appendInt8(4);
                                                                yyval.binstr->appendInt32(yypvt[-1].int32); } break;
case 728:
#line 1978 "asmparse.y"
{ yyval.secAct = dclRequest; } break;
case 729:
#line 1979 "asmparse.y"
{ yyval.secAct = dclDemand; } break;
case 730:
#line 1980 "asmparse.y"
{ yyval.secAct = dclAssert; } break;
case 731:
#line 1981 "asmparse.y"
{ yyval.secAct = dclDeny; } break;
case 732:
#line 1982 "asmparse.y"
{ yyval.secAct = dclPermitOnly; } break;
case 733:
#line 1983 "asmparse.y"
{ yyval.secAct = dclLinktimeCheck; } break;
case 734:
#line 1984 "asmparse.y"
{ yyval.secAct = dclInheritanceCheck; } break;
case 735:
#line 1985 "asmparse.y"
{ yyval.secAct = dclRequestMinimum; } break;
case 736:
#line 1986 "asmparse.y"
{ yyval.secAct = dclRequestOptional; } break;
case 737:
#line 1987 "asmparse.y"
{ yyval.secAct = dclRequestRefuse; } break;
case 738:
#line 1988 "asmparse.y"
{ yyval.secAct = dclPrejitGrant; } break;
case 739:
#line 1989 "asmparse.y"
{ yyval.secAct = dclPrejitDenied; } break;
case 740:
#line 1990 "asmparse.y"
{ yyval.secAct = dclNonCasDemand; } break;
case 741:
#line 1991 "asmparse.y"
{ yyval.secAct = dclNonCasLinkDemand; } break;
case 742:
#line 1992 "asmparse.y"
{ yyval.secAct = dclNonCasInheritance; } break;
case 743:
#line 1996 "asmparse.y"
{ nCurrPC = PASM->m_CurPC; PENV->bExternSource = TRUE; PENV->bExternSourceAutoincrement = FALSE; } break;
case 744:
#line 1997 "asmparse.y"
{ nCurrPC = PASM->m_CurPC; PENV->bExternSource = TRUE; PENV->bExternSourceAutoincrement = TRUE; } break;
case 745:
#line 2000 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-1].int32;
                                                                PENV->nExtCol = 1; PENV->nExtColEnd  = 1;
                                                                PASM->SetSourceFileName(yypvt[-0].string);} break;
case 746:
#line 2003 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-0].int32;
                                                                PENV->nExtCol = 1; PENV->nExtColEnd  = 1; } break;
case 747:
#line 2005 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-3].int32; 
                                                                PENV->nExtCol=yypvt[-1].int32; PENV->nExtColEnd = PENV->nExtCol;
                                                                PASM->SetSourceFileName(yypvt[-0].string);} break;
case 748:
#line 2008 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-2].int32; 
                                                                PENV->nExtCol=yypvt[-0].int32; PENV->nExtColEnd = PENV->nExtCol;} break;
case 749:
#line 2011 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-5].int32; 
                                                                PENV->nExtCol=yypvt[-3].int32; PENV->nExtColEnd = yypvt[-1].int32;
                                                                PASM->SetSourceFileName(yypvt[-0].string);} break;
case 750:
#line 2015 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-4].int32; 
                                                                PENV->nExtCol=yypvt[-2].int32; PENV->nExtColEnd = yypvt[-0].int32; } break;
case 751:
#line 2018 "asmparse.y"
{ PENV->nExtLine = yypvt[-5].int32; PENV->nExtLineEnd = yypvt[-3].int32; 
                                                                PENV->nExtCol=yypvt[-1].int32; PENV->nExtColEnd = PENV->nExtCol;
                                                                PASM->SetSourceFileName(yypvt[-0].string);} break;
case 752:
#line 2022 "asmparse.y"
{ PENV->nExtLine = yypvt[-4].int32; PENV->nExtLineEnd = yypvt[-2].int32; 
                                                                PENV->nExtCol=yypvt[-0].int32; PENV->nExtColEnd = PENV->nExtCol; } break;
case 753:
#line 2025 "asmparse.y"
{ PENV->nExtLine = yypvt[-7].int32; PENV->nExtLineEnd = yypvt[-5].int32; 
                                                                PENV->nExtCol=yypvt[-3].int32; PENV->nExtColEnd = yypvt[-1].int32;
                                                                PASM->SetSourceFileName(yypvt[-0].string);} break;
case 754:
#line 2029 "asmparse.y"
{ PENV->nExtLine = yypvt[-6].int32; PENV->nExtLineEnd = yypvt[-4].int32; 
                                                                PENV->nExtCol=yypvt[-2].int32; PENV->nExtColEnd = yypvt[-0].int32; } break;
case 755:
#line 2031 "asmparse.y"
{ PENV->nExtLine = PENV->nExtLineEnd = yypvt[-1].int32 - 1;
                                                                PENV->nExtCol = 1; PENV->nExtColEnd  = 1;
                                                                PASM->SetSourceFileName(yypvt[-0].binstr);} break;
case 756:
#line 2038 "asmparse.y"
{ PASMM->AddFile(yypvt[-5].string, yypvt[-6].fileAttr|yypvt[-4].fileAttr|yypvt[-0].fileAttr, yypvt[-2].binstr); } break;
case 757:
#line 2039 "asmparse.y"
{ PASMM->AddFile(yypvt[-1].string, yypvt[-2].fileAttr|yypvt[-0].fileAttr, NULL); } break;
case 758:
#line 2042 "asmparse.y"
{ yyval.fileAttr = (CorFileFlags) 0; } break;
case 759:
#line 2043 "asmparse.y"
{ yyval.fileAttr = (CorFileFlags) (yypvt[-1].fileAttr | ffContainsNoMetaData); } break;
case 760:
#line 2046 "asmparse.y"
{ yyval.fileAttr = (CorFileFlags) 0; } break;
case 761:
#line 2047 "asmparse.y"
{ yyval.fileAttr = (CorFileFlags) 0x80000000; } break;
case 762:
#line 2050 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 763:
#line 2053 "asmparse.y"
{ PASMM->StartAssembly(yypvt[-0].string, NULL, (DWORD)yypvt[-1].asmAttr, FALSE); } break;
case 764:
#line 2056 "asmparse.y"
{ yyval.asmAttr = (CorAssemblyFlags) 0; } break;
case 765:
#line 2057 "asmparse.y"
{ yyval.asmAttr = (CorAssemblyFlags) (yypvt[-1].asmAttr | afRetargetable); } break;
case 766:
#line 2058 "asmparse.y"
{ yyval.asmAttr = yypvt[-2].asmAttr; } break;
case 767:
#line 2059 "asmparse.y"
{ SET_PA(yyval.asmAttr,yypvt[-1].asmAttr,afPA_MSIL); } break;
case 768:
#line 2060 "asmparse.y"
{ SET_PA(yyval.asmAttr,yypvt[-1].asmAttr,afPA_x86); } break;
case 769:
#line 2061 "asmparse.y"
{ SET_PA(yyval.asmAttr,yypvt[-1].asmAttr,afPA_IA64); } break;
case 770:
#line 2062 "asmparse.y"
{ SET_PA(yyval.asmAttr,yypvt[-1].asmAttr,afPA_AMD64); } break;
case 773:
#line 2069 "asmparse.y"
{ PASMM->SetAssemblyHashAlg(yypvt[-0].int32); } break;
case 776:
#line 2074 "asmparse.y"
{ yyval.int32 = yypvt[-0].int32; } break;
case 777:
#line 2075 "asmparse.y"
{ yyval.int32 = 0xFFFF; } break;
case 778:
#line 2078 "asmparse.y"
{ PASMM->SetAssemblyPublicKey(yypvt[-1].binstr); } break;
case 779:
#line 2080 "asmparse.y"
{ PASMM->SetAssemblyVer((USHORT)yypvt[-6].int32, (USHORT)yypvt[-4].int32, (USHORT)yypvt[-2].int32, (USHORT)yypvt[-0].int32); } break;
case 780:
#line 2081 "asmparse.y"
{ yypvt[-0].binstr->appendInt8(0); PASMM->SetAssemblyLocale(yypvt[-0].binstr,TRUE); } break;
case 781:
#line 2082 "asmparse.y"
{ PASMM->SetAssemblyLocale(yypvt[-1].binstr,FALSE); } break;
case 784:
#line 2087 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 785:
#line 2090 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 786:
#line 2093 "asmparse.y"
{ bParsingByteArray = TRUE; } break;
case 787:
#line 2097 "asmparse.y"
{ PASMM->StartAssembly(yypvt[-0].string, NULL, yypvt[-1].asmAttr, TRUE); } break;
case 788:
#line 2099 "asmparse.y"
{ PASMM->StartAssembly(yypvt[-2].string, yypvt[-0].string, yypvt[-3].asmAttr, TRUE); } break;
case 791:
#line 2106 "asmparse.y"
{ PASMM->SetAssemblyHashBlob(yypvt[-1].binstr); } break;
case 793:
#line 2108 "asmparse.y"
{ PASMM->SetAssemblyPublicKeyToken(yypvt[-1].binstr); } break;
case 794:
#line 2109 "asmparse.y"
{ PASMM->SetAssemblyAutodetect(); } break;
case 795:
#line 2112 "asmparse.y"
{ PASMM->StartComType(yypvt[-0].string, yypvt[-1].exptAttr);} break;
case 796:
#line 2115 "asmparse.y"
{ PASMM->StartComType(yypvt[-0].string, yypvt[-1].exptAttr); } break;
case 797:
#line 2118 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) 0; } break;
case 798:
#line 2119 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-1].exptAttr | tdNotPublic); } break;
case 799:
#line 2120 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-1].exptAttr | tdPublic); } break;
case 800:
#line 2121 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-1].exptAttr | tdForwarder); } break;
case 801:
#line 2122 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedPublic); } break;
case 802:
#line 2123 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedPrivate); } break;
case 803:
#line 2124 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedFamily); } break;
case 804:
#line 2125 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedAssembly); } break;
case 805:
#line 2126 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedFamANDAssem); } break;
case 806:
#line 2127 "asmparse.y"
{ yyval.exptAttr = (CorTypeAttr) (yypvt[-2].exptAttr | tdNestedFamORAssem); } break;
case 809:
#line 2134 "asmparse.y"
{ PASMM->SetComTypeFile(yypvt[-0].string); } break;
case 810:
#line 2135 "asmparse.y"
{ PASMM->SetComTypeComType(yypvt[-0].string); } break;
case 811:
#line 2136 "asmparse.y"
{ PASMM->SetComTypeAsmRef(yypvt[-0].string); } break;
case 812:
#line 2137 "asmparse.y"
{ if(!PASMM->SetComTypeImplementationTok(yypvt[-1].int32))
                                                                  PASM->report->error("Invalid implementation of exported type\n"); } break;
case 813:
#line 2139 "asmparse.y"
{ if(!PASMM->SetComTypeClassTok(yypvt[-0].int32))
                                                                  PASM->report->error("Invalid TypeDefID of exported type\n"); } break;
case 816:
#line 2145 "asmparse.y"
{ PASMM->StartManifestRes(yypvt[-0].string, yypvt[-0].string, yypvt[-1].manresAttr); } break;
case 817:
#line 2147 "asmparse.y"
{ PASMM->StartManifestRes(yypvt[-2].string, yypvt[-0].string, yypvt[-3].manresAttr); } break;
case 818:
#line 2150 "asmparse.y"
{ yyval.manresAttr = (CorManifestResourceFlags) 0; } break;
case 819:
#line 2151 "asmparse.y"
{ yyval.manresAttr = (CorManifestResourceFlags) (yypvt[-1].manresAttr | mrPublic); } break;
case 820:
#line 2152 "asmparse.y"
{ yyval.manresAttr = (CorManifestResourceFlags) (yypvt[-1].manresAttr | mrPrivate); } break;
case 823:
#line 2159 "asmparse.y"
{ PASMM->SetManifestResFile(yypvt[-2].string, (ULONG)yypvt[-0].int32); } break;
case 824:
#line 2160 "asmparse.y"
{ PASMM->SetManifestResAsmRef(yypvt[-0].string); } break;/* End of actions */
#line 329 "yypars.c"
			}
		}
		goto yystack;  /* stack new state and value */
	}
#ifdef _MSC_VER
#pragma warning(default:102)
#endif


#ifdef YYDUMP
YYLOCAL void YYNEAR YYPASCAL yydumpinfo(void)
{
	short stackindex;
	short valindex;

	//dump yys
	printf("short yys[%d] {\n", YYMAXDEPTH);
	for (stackindex = 0; stackindex < YYMAXDEPTH; stackindex++){
		if (stackindex)
			printf(", %s", stackindex % 10 ? "\0" : "\n");
		printf("%6d", yys[stackindex]);
		}
	printf("\n};\n");

	//dump yyv
	printf("YYSTYPE yyv[%d] {\n", YYMAXDEPTH);
	for (valindex = 0; valindex < YYMAXDEPTH; valindex++){
		if (valindex)
			printf(", %s", valindex % 5 ? "\0" : "\n");
		printf("%#*x", 3+sizeof(YYSTYPE), yyv[valindex]);
		}
	printf("\n};\n");
	}
#endif

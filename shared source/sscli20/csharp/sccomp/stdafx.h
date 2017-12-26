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
// ===========================================================================
// File: stdafx.h
//
//      include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently
// ===========================================================================

#if !defined(AFX_STDAFX_H__1FB1B422_B183_11D2_88B7_00C04F990355__INCLUDED_)
#define AFX_STDAFX_H__1FB1B422_B183_11D2_88B7_00C04F990355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_DCOM

#define VOLATILEFIELDS 1   // Should we allow volatile modifier on fields.




// Insert your headers here
#include <windows.h>
#include <shlwapi.h> // For the PathIs* functions

#include <malloc.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <oleauto.h>
#include <fcntl.h>
#include <io.h>

	#define BEGIN_NAMESPACE_CSHARP
	#define END_NAMESPACE_CSHARP
	#define NAMESPACE_CSHARP


#define _ATL_NO_UUIDOF

// This has to come before vsmem because it has members named "Alloc"
#include <mscoree.h>


#define VSMEM_ALLOW_MALLOC
#define VSMEM_NO_REDEFINE_HEAPALLOC
#include <vsmem.h>

#define _ATL_NO_DEBUG_CRT
#define _ATL_APARTMENT_THREADED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "atl.h"

extern CComModule _Module;

#include <vsassert.h>

#undef new
#define COR_ILEXCEPTION_OFFSETLEN_SUPPORTED 1
#define _META_DATA_NO_SCOPE_ 1
#include <cor.h>
#include <corsym.h>
#include <corhlpr.h>
#include <iceefilegen.h>
#include <strongname.h>
#include <fusion.h>
#include <unilib.h>
#include <alink.h>
#include <verstamp.h>
#include <corerror.h>

// Define simple assert macro.
//
// This is ripped off from vs\src\common\inc\vsassert.h
//

#ifdef DEBUG
#define ASSERT(fTest)  _ASSERTE(fTest)
#define BASSERT(fTest) _ASSERTE(fTest)
#else // !DEBUG
#define ASSERT(fTest)  do {} while (0)
#define BASSERT(fTest) do {} while (0)
#endif // !DEBUG


// ASSERT needs to be defined before cscommon.h
#include "cscommon.h"




// This allows the compiler to create nodes
#define DECLARE_BASENODE_NEW

// Define lengthof macro - length of an array.
#define lengthof(a) (sizeof(a) / sizeof((a)[0]))

// Make sure people don't use malloc et. al. in the compiler.
#ifdef calloc
#  undef calloc
#endif
#ifdef malloc
#  undef malloc
#endif
#ifdef realloc
#  undef realloc
#endif
#ifdef free
#  undef free
#endif

#define calloc(num, size)     Dont_use_calloc_in_the_compiler;
#define malloc(size)          Dont_use_malloc_in_the_compiler;
#define realloc(pv, size)     Dont_use_realloc_in_the_compiler;
#define free(pv)              Dont_use_free_in_the_compiler;

#undef VSReallocZero
#define VSReallocZero not_portable

#if defined(_MSC_VER)
#pragma warning(disable: 4200)
#endif  // defined(_MSC_VER)


// ROTORTODO:
// While CLEANUP_STACK should stay undefined, we need an actual implementation
// of GetExceptionCode
inline DWORD GetExceptionCode() { return 0; }
#define CLEANUP_STACK


// define this to output a list of unbound methods to stdout at the conclusion of the compilation
//#define USAGEHACK 1

// We currently enable TRACKMEM in debug and retail so that QA can run daily tests on the retail builds.
// REVIEW MAnder:  This should be removed from retail drops before we ship. </STRIP>


#define CSEEONLY(x)
#define NONCSEEONLY(x) x


#include "cordebug.h"

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed char sbyte;

#define MEM_SET_ZERO(var) memset(&(var), 0, sizeof(var))
#define STACK_ALLOC_ZERO(t, n) (t*)memset(STACK_ALLOC(t,n), 0, (n)*sizeof(t))



////////////////////////////////////////////////////////////////////////////////
// PAL macros

// portable exception handling


// portable 64-bit constants
#ifndef I64
#define I64(x)    x ## i64
#endif
#ifndef UI64
#define UI64(x)   x ## ui64
#endif

// portable access to nameless unions in DECIMALs
#ifndef DECIMAL_LO32
#define DECIMAL_LO32(dec)       ((dec).Lo32)
#endif
#ifndef DECIMAL_MID32
#define DECIMAL_MID32(dec)      ((dec).Mid32)
#endif
#ifndef DECIMAL_HI32
#define DECIMAL_HI32(dec)       ((dec).Hi32)
#endif
#ifndef DECIMAL_SCALE
#define DECIMAL_SCALE(dec)      ((dec).scale)
#endif
#ifndef DECIMAL_SIGN
#define DECIMAL_SIGN(dec)       ((dec).sign)
#endif
#ifndef DECIMAL_SIGNSCALE
#define DECIMAL_SIGNSCALE(dec)  ((dec).signscale)
#endif

// the required data alignment of the platform

// portable helpers for access to unaligned pointers
#ifdef _MSC_VER
// Get unaligned values from a potentially unaligned object
#define GET_UNALIGNED_16(_pObject)  (*(UINT16 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_32(_pObject)  (*(UINT32 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_64(_pObject)  (*(UINT64 UNALIGNED *)(_pObject))

// Set Value on an potentially unaligned object 
#define SET_UNALIGNED_16(_pObject, _Value)  (*(UNALIGNED UINT16 *)(_pObject)) = (UINT16)(_Value)
#define SET_UNALIGNED_32(_pObject, _Value)  (*(UNALIGNED UINT32 *)(_pObject)) = (UINT32)(_Value)
#define SET_UNALIGNED_64(_pObject, _Value)  (*(UNALIGNED UINT64 *)(_pObject)) = (UINT64)(_Value) 
#endif

// helpers for bigendian swapping
#if !BIGENDIAN

#define VAL16(x) x
#define VAL32(x) x
#define VAL64(x) x
#define SwapString(x)
#define SwapStringLength(x, y)
#define SwapGuid(x)

#define GET_UNALIGNED_VAL16(_pObject) VAL16(GET_UNALIGNED_16(_pObject))
#define GET_UNALIGNED_VAL32(_pObject) VAL32(GET_UNALIGNED_32(_pObject))
#define GET_UNALIGNED_VAL64(_pObject) VAL64(GET_UNALIGNED_64(_pObject))

#define SET_UNALIGNED_VAL16(_pObject, _Value) SET_UNALIGNED_16(_pObject, VAL16((UINT16)_Value))
#define SET_UNALIGNED_VAL32(_pObject, _Value) SET_UNALIGNED_32(_pObject, VAL32((UINT32)_Value))
#define SET_UNALIGNED_VAL64(_pObject, _Value) SET_UNALIGNED_64(_pObject, VAL64((UINT64)_Value))

#endif

////////////////////////////////////////////////////////////////////////////////
// CComObjectRootMT
//
// ATL didn't seem to have a way to do this explicitly...
//

template <>
class CComObjectRootEx<CComMultiThreadModelNoCS> : public CComObjectRootBase
{
public:

    typedef CComMultiThreadModelNoCS _ThreadModel;

    ULONG InternalAddRef()
    {
        ATLASSERT(m_dwRef != -1L);
        return _ThreadModel::Increment(&m_dwRef);
    }
    ULONG InternalRelease()
    {
        return _ThreadModel::Decrement(&m_dwRef);
    }
    void Lock() {}
    void Unlock() {}

    // New member added to ATL.
    HRESULT _AtlInitialConstruct()
    {
        return S_OK;
    }
};

typedef CComObjectRootEx<CComMultiThreadModelNoCS> CComObjectRootMT;

#ifdef DEBUG
#define DebugOnly(x) x
#else
#define DebugOnly(x)
#endif

#ifndef __min
#define __min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define MOREPCH 1

/******************************************************************************
    These are used to declare pseudo-C# style enums and sets of flags. For
    example,

        DECLARE_ENUM(Foo)
            A, B, C,
        END_ENUM(Foo)

    allows use of Foo::A, Foo::B, etc as values and FooEnum as a type.

    Adding:

        DECLARE_FLAGS(Foo)

    outside of any type (at the namespace level) allows:

        FooEnum foo = Foo::A | Foo::B & Foo::C & ~Foo::A;

    In debug these are strongly typed and employ user defined operators. In
    non-debug, FooEnum is a typedef for uint and these use the standard
    bitwise operators. A downside for this is that in order for a compound
    expression to be considered a compile time constant, you must cast to
    uint before operating:

        int a[(uint)Foo::A | Foo::B];

******************************************************************************/
#define DECLARE_ENUM(name) struct name { private: name() { } public: enum _Enum {

#ifdef DEBUG
#define END_ENUM(name) }; }; typedef name::_Enum name##Enum;
#define DECLARE_FLAGS(name) \
    inline name::_Enum operator | (name::_Enum e1, name::_Enum e2) { return name::_Enum((uint)e1 | (uint)e2); } \
    inline name::_Enum operator & (name::_Enum e1, name::_Enum e2) { return name::_Enum((uint)e1 & (uint)e2); } \
    inline name::_Enum operator ~ (name::_Enum e) { return name::_Enum(~(int)e); }
#else
#define END_ENUM(name) }; }; typedef uint name##Enum;
#define DECLARE_FLAGS(name)
#endif

/******************************************************************************
    Unfortunately, intellisense doesn't pick up the enum values when
    using the macros above. Until it does, use this idiom:

        namespace Foo { enum _Enum {
            A, B, C,
        }; };
        DECLARE_ENUM_TYPE(Foo)

    or (for flags operators):

        namespace Foo { enum _Enum {
            A = 0x01, B = 0x02, C = 0x04,
        }; };
        DECLARE_FLAGS_TYPE(Foo)

    If the enum is nested in a type use this:

        class Bar {
        public:
            struct Foo { enum _Enum { // Use struct instead of namespace
                A = 0x01, B = 0x02, C = 0x04,
            }; };
        };

        DECLARE_FLAGS_TYPE(Bar::Foo) // put this outside the outer type(s).

******************************************************************************/
#define DECLARE_ENUM_TYPE(name) typedef name::_Enum name##Enum;
#ifdef DEBUG
#define DECLARE_FLAGS_TYPE(name) typedef name::_Enum name##Enum; \
    inline name::_Enum operator |= (name::_Enum &e1, name::_Enum e2) { return e1 = name::_Enum((uint)e1 | (uint)e2); } \
    inline name::_Enum operator | (name::_Enum e1, name::_Enum e2) { return name::_Enum((uint)e1 | (uint)e2); } \
    inline name::_Enum operator & (name::_Enum e1, name::_Enum e2) { return name::_Enum((uint)e1 & (uint)e2); } \
    inline name::_Enum operator ~ (name::_Enum e) { return name::_Enum(~(uint)e); }
#else
#define DECLARE_FLAGS_TYPE(name) typedef uint name##Enum;
#endif

// new factory types
BEGIN_NAMESPACE_CSHARP

class LocalContext;
class GlobalContext;
class CFactory;
class EEDisplayOptions;
class EEExceptionFilterLocal;

END_NAMESPACE_CSHARP

// Forward declarations.
class COMPILER;
class CInputSet;
class CError;
class MemberLookup;
class ERRLOC;
class BSYMHOST;
class LSYMHOST;
class ISTHOST;
class SYMMGR;
class BSYMMGR;
class LSYMMGR;
class PEFile;
class TypeArray;
struct BASENODE;
struct NAMENODE;
struct TYPEBASENODE;
struct PARAMETERNODE;
class EXPR;
class EXPRLOCAL;
class EXPRSTMT;
struct SymWithType;
struct MethPropWithInst;
struct SourceExtent;
interface ICompileCallback;
class CErrorSuppression;
struct AnonMethInfo;

// SYMs
class SYM;
class TYPESYM;
class METHPROPSYM;
class METHSYM;
class PROPSYM;
class EVENTSYM;
class MEMBVARSYM;
class AGGSYM;
class AGGTYPESYM;
class DECLSYM;
class BAGSYM;
class INFILESYM;

#include "msgsids.h"
#include "csiface.h"
#include "namemgr.h"
#include "nodes.h"
#include "satellite.h"
#include "compres.h"
#include "alloc.h"
#include "bitset.h"
#include "hashtab.h"
#include "posdata.h"
#include "symkind.h"
#include "error.h"
#include "options.h"
#include "controller.h"
#include "errorsuppression.h"

#include "errorids.h"

// withtype.h is included twice - once before symbol.h and
// once after symbol.h (with WITHTYPE_INLINES defined).
#include "withtype.h"
#include "symbol.h"
#define WITHTYPE_INLINES
#include "withtype.h"

#include "enum.h"
#include "symmgr.h"
#include "srcmod.h"
#include "import.h"
#include "metahelp.h"
#include "exprlist.h"
#include "typebind.h"
#include "memlook.h"
#include "fncbind.h"
#include "clsdrec.h"
#include "pefile.h"
#include "emitter.h"
#include "ilgen.h"
#include "srcdata.h"
#include "metaattr.h"

#include "compiler.h"
#include "common.h"

#include "factory.h"
#include "parser.h"
#include "lexer.h"
#include "file_can.h"
#include "fileiter.h"
#include "privguid.h"
#include "inputset.h"
#include "tokdata.h"
#define DBGOUT(a)
#include "inttree.h"
#include "map.h"

#include "cscmgd.h"

#include "atlarrayqsort.h"

#endif // !defined(AFX_STDAFX_H__1FB1B422_B183_11D2_88B7_00C04F990355__INCLUDED_)

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
// File: symbol.h
//
// Defines the shapes of various symbols in the symbol table.
// ===========================================================================

#ifndef __symbol_h__
#define __symbol_h__

#include "constval.h"



// Alias ID's are indices into BitSets.
// 0 is reserved for the global namespace alias.
// 1 is reserved for this assembly.
// Start assigning at kaidStartAssigning.
enum {
    kaidNil = -1,

    kaidGlobal = 0,
    kaidErrorAssem,
    kaidThisAssembly,
    kaidUnresolved,
    kaidStartAssigning,

    // Module id's are in their own range.
    kaidMinModule = 0x10000000,
};

class UNRESAGGSYM;


// Number of simple types in predeftype.h. You must
// change appropriate conversion table definitions
// if this changes.
#define NUM_SIMPLE_TYPES 13
#define NUM_QSIMPLE_TYPES 4
#define NUM_EXT_TYPES 15

namespace AUTOEXP {
    class ATTRIBUTE;
};

/*
 * Fundemental types. These are the fundemental storage types
 * that are available and used by the code generator.
 */
enum FUNDTYPE {
    FT_NONE,        // No fundemental type
    FT_I1,
    FT_I2,
    FT_I4,
    FT_U1,
    FT_U2,
    FT_U4,
    FT_LASTNONLONG = FT_U4,  // Last one that fits in a int.
    FT_I8,
    FT_U8,          // integral types
    FT_LASTINTEGRAL = FT_U8,
    FT_R4,
    FT_R8,          // floating types
    FT_LASTNUMERIC = FT_R8,
    FT_REF,         // reference type
    FT_STRUCT,      // structure type
    FT_PTR,         // pointer to unmanaged memory
    FT_VAR,         // polymorphic, unbounded, not yet committed

    FT_COUNT        // number of enumerators.
};

/*
 * Define the different access levels that symbols can have.
 */
enum ACCESS {
    ACC_UNKNOWN,    // Not yet determined.
    ACC_PRIVATE,
    ACC_INTERNAL,
    ACC_PROTECTED,
    ACC_INTERNALPROTECTED,  // internal OR protected
    ACC_PUBLIC
};

#include "symkind.h"

/*
 * Forward declare all symbol types.
 */
#define SYMBOLDEF(kind, global, local) typedef class kind * P ## kind;
#include "symkinds.h"

typedef __int64 symbmask_t;
/*
 * Define values for symbol masks.
 */
#define SYMBOLDEF(kind, global, local) const symbmask_t MASK_ ## kind = (((symbmask_t)1) << SK_ ## kind);
#define SYMBOLDEF_EXTRA(kind, global, local)
#include "symkinds.h"
const symbmask_t MASK_ALL = ~(MASK_FAKEMETHSYM | MASK_FWDAGGSYM);

// Typedefs for some pointer types up front.
typedef class SYM * PSYM;
typedef class PARENTSYM * PPARENTSYM;
typedef class TYPESYM * PTYPESYM;

struct METHODNODE;
struct BASENODE;
class  CSourceData;


/*
 *  SYMLIST - a list of symbols.
 */
struct SYMLIST {
    PSYM sym;
    SYMLIST * next;

    bool contains(PSYM sym);
};
typedef SYMLIST * PSYMLIST;

#define FOREACHSYMLIST(_list, _elem, _elemtype)             \
{                                                           \
    for (SYMLIST * _next = (_list);                         \
         _next;                                             \
         _next = _next->next) {                             \
        _elemtype* _elem = _next->sym->as ## _elemtype();

#define ENDFOREACHSYMLIST               \
    }                                   \
}
typedef int (*PFNSymCompare) (SYM * first, SYM * second);

struct SYMLISTALLOC {
private:
    SYMLIST * list;
public:
    SYMLISTALLOC() { list = NULL; }
    operator SYMLIST* () { return list; }
    SYMLIST ** Addr() { return &list; }
    HRESULT Prepend(NRHEAP * alloc, SYM * sym) {
        SYMLIST * n = (SYMLIST*) alloc->Alloc(sizeof(SYMLIST));
        if (!n) return E_OUTOFMEMORY;
        n->next = list;
        n->sym = sym;
        list = n;
        return S_OK;
    }
    SYMLIST * FindPreceding(SYM * sym, PFNSymCompare func) {
        SYMLIST * rval = NULL;
        SYMLIST * next = list;
        while (true) {
            if (!next) return rval;
            if (func(next->sym, sym) > 0) {
                return rval;
            }
            rval = next;
            next = rval->next;
        }

    }
    HRESULT InsertSorted(NRHEAP * alloc, SYM * sym, PFNSymCompare func) {
        SYMLIST * preceding = FindPreceding(sym, func);
        SYMLIST * n = (SYMLIST*) alloc->Alloc(sizeof(SYMLIST));
        if (!n) return E_OUTOFMEMORY;
        if (preceding) {
            n->next = preceding->next;
            preceding->next = n;
        } else {
            n->next = list;
            list = n;
        }
        n->sym = sym;
        return S_OK;
    }
};

/*
 *  NAMELIST - a list of names.
 */
struct NAMELIST {
    PNAME name;
    NAMELIST * next;

    bool contains(PNAME name);
};
typedef NAMELIST * PNAMELIST;

#define FOREACHNAMELIST(_list, _elem)    \
{                                       \
    for (NAMELIST * _next = (_list);     \
         _next;                         \
         _next = _next->next) {         \
        PNAME _elem = _next->name;

#define ENDFOREACHNAMELIST               \
    }                                   \
}

/*
 *  ATTRLIST - a list of attribute nodes and their context (i.e. decl).
 */
struct ATTRLIST {
    BASENODE    *attr;
    PARENTSYM   *context;       // Context of this attribute (AGGDECL for agg tyvars, METHSYM for method type variables)
    bool         fHadError;      // whether or not there was an error binding this attribute
    ATTRLIST    *next;
};
typedef ATTRLIST* PATTRLIST;

#define FOREACHATTRLIST(_list, _node)    \
{                                       \
    for (ATTRLIST * _node = (_list);     \
         _node;                         \
         _node = _node->next) {         \

#define ENDFOREACHATTRLIST               \
    }                                   \
}


template <typename T, int i>
class TLIST {

private:
    T item[i];
    int count;
    TLIST<T,i> * next;
    TLIST<T,i> * prev;
public:

    class INVOKER {
    public:
        virtual HRESULT invoke(T item) = 0;
    };

    class LOCATION {
    public:
        LOCATION () : loc(NULL), x(0) { };
        TLIST<T,i> * loc;
        int x;
        bool isValid() { return (loc != NULL); }
        HRESULT decrement() {
            ASSERT(isValid());
            if (x == 0 && !loc->prev) {
                return S_FALSE;
            }
            if (x > 0) {
                x --;
            } else {
                loc = loc->prev;
                x = i - 1;
            }
            return S_OK;
        }
    };

    TLIST () { memset(this, 0, sizeof(TLIST<T,i>)); };
    HRESULT foreach(INVOKER * invoker) {
        HRESULT HR;
        if (count) {
            for(int x = 0; x < i; x++) {
                if (item[x]) {
                    HR = invoker->invoke(item[x]);
                    if (FAILED(HR)) return HR;
                }
            }
        }
        if (next) return next->foreach(invoker);
        return S_OK;
    };
    bool contains(T item, LOCATION * loc) { 
        if (count != 0) { 
            for(int x = 0; x < i; x++) { 
                if (this->item[x] == item) {
                    if (loc) {
                        loc->loc = this;
                        loc->x = x;
                    }
                    return true;
                }
            }
        }
        if (next) return next->contains(item, loc);
        if (loc) loc->loc = this;
        return false;
    };
    HRESULT add(LOCATION * loc, NRHEAP * heap, T item) {
        TLIST<T,i> * current = this;
        int start = 0;
        if (loc) {
            if (loc->loc) {
                current = loc->loc;
            }
            start = loc->x;
        }
        if (current->count == i) {
            if (!current->next) {
                current->next = (TLIST<T,i> *)heap->Alloc(sizeof(TLIST<T,i>));
                if (!current->next) return E_OUTOFMEMORY;
                memset(current->next, 0, sizeof(TLIST<T,i>));
                current->next->prev = current;
            }
            if (loc) { loc->loc = NULL; loc->x = 0; }
            return current->next->add(loc, heap, item);
        }
AGAIN:
        for(int x = start; x < i; x++) {
            if (!current->item[x]) {
                current->item[x] = item;
                current->count++;
                if (loc) {
                    loc->x = x;
                    loc->loc = current;
                }
                return S_OK;
            }
        }
        if (start) {
            start = 0; 
            goto AGAIN;
        }
        return E_FAIL;
    };
    HRESULT remove(LOCATION * loc, T item) {
        LOCATION l;
        if (!loc) {
            loc = &l;
            bool b = contains(item, loc);
            if (!b) return S_FALSE;
        }
        return remove(loc);
    };
    HRESULT remove(LOCATION * loc, T * item) {
        *item = loc->loc->item[loc->x];
        return remove(loc);
    }
    HRESULT remove(LOCATION * loc) {
        ASSERT(loc->loc->item[loc->x]);
        loc->loc->count --;
        memset(&(loc->loc->item[loc->x]), 0, sizeof(T));
        return S_OK;
    };
    bool isEmpty() {
        if (count) return false;
        if (!next) return true;
        return next->isEmpty();        
    };

};
typedef TLIST<void *, 10> VOIDLIST;

template <typename T, int i>
class TLISTLOC : public TLIST<T, i>
{
private:
    typename TLIST<T, i>::LOCATION loc;
public:
    HRESULT addToTop(NRHEAP * heap, T item) {
        return add(&loc, heap, item);
    }
};

// used to give compile time errors when asking a symbol for something which
// it can never be
#define NOT_A(k) \
    void as ## k(); \
    void is ## k();

// used to give a compile time error when asking a  symbol
// to be something which you already know it is
#define IS_A(k) \
    void as ## k(); \
    void is ## k();

//
//
//
//
namespace AggState {
    enum _Enum {
        None,

        Declared,


        ResolvingInheritance,   // In ResolveInheritanceRec.
        Inheritance,            // Done with ResolveInheritanceRec.

        Bounds,

        DefiningMembers,
        DefinedMembers,

        Preparing,              // Preparing base types of this AGG
        Prepared,               // Base types are prepared
        PreparedMembers,        // Members are prepared

        Compiled,

        Last = Compiled,

        Lim
    };
};
DECLARE_ENUM_TYPE(AggState);


/*
 * SYM - the base symbol.
 */
class SYM {
private:
    uint kind: 8;     // the symbol kind
    uint isBogus: 1;     // can't be used in our language -- unsupported type(s)
    uint checkedBogus: 1;// Have we checked a method args/return for bogus types
    uint isDeprecated: 1;// symbol is deprecated and should give a warning when used
    uint isDeprecatedError: 1; // If isDeprecated, indicates error rather than warning

    uint access: 4;    // access level

public:
    ACCESS GetAccess() { ASSERT(access != ACC_UNKNOWN); return (ACCESS)access; }
    void SetAccess(ACCESS access) { this->access = (uint)access; }

    uint isLocal: 1;     // a local or global symbol?
    uint fHasSid: 1;     // Whether the symbol was given an "ID" - used for testing refactoring support
    uint fDumped: 1;    // Used for testing refactoring

    uint hasCLSattribute: 1; // True iff this symbol has an attribute
    uint isCLS: 1;           // if hasCLSattribute, indicates whether symbol is CLS compliant
    uint isDead: 1;          // If this is true, then this sym has been removed from the symbol table so will not be returned from LookupSym
    uint isFabricated: 1;    // Is compiler generated
    uint fHadAttributeError: 1; // set for symbols where we check attributes twice (i.e. parameters for indexers or delegates, CLSCompliance in EarlyAttrBind, etc.  
                                // If this is true, then we had an error the first time so do not give an error the second time.

    PNAME name;         // name of the symbol
    PPARENTSYM parent;  // parent of the symbol
    PSYM nextChild;     // next child of this parent
    PSYM nextSameName;  // next child of this parent with same name.

    // We have member functions here to do casts that, in DEBUG, check the
    // symbol kind to make sure it is right. For example, the casting method
    // for METHODSYM is called "asMETHODSYM".

    // Define all the concrete casting methods.
    // We define them explicitly so that VC's ide knows about them

    // used for FOREACHSYMLIST(list, elem, SYM)
    class SYM                       * asSYM() { return this; }

    class BAGSYM                    * asBAGSYM();
    class DECLSYM                   * asDECLSYM();
    class NSSYM                     * asNSSYM();
    class NSAIDSYM                  * asNSAIDSYM();
    class NSDECLSYM                 * asNSDECLSYM();
    class AGGSYM                    * asAGGSYM();
    class AGGDECLSYM                * asAGGDECLSYM();
    class FWDAGGSYM                 * asFWDAGGSYM();
    class INFILESYM                 * asINFILESYM();
    class MODULESYM                 * asMODULESYM();
    class RESFILESYM                * asRESFILESYM();
    class OUTFILESYM                * asOUTFILESYM();
    class MEMBVARSYM                * asMEMBVARSYM();
    class LOCVARSYM                 * asLOCVARSYM();
    class METHSYM                   * asMETHSYM();
    class FAKEMETHSYM               * asFAKEMETHSYM();
    class PROPSYM                   * asPROPSYM();
    class METHPROPSYM               * asMETHPROPSYM();
    class SCOPESYM                  * asSCOPESYM();
    class ANONSCOPESYM              * asANONSCOPESYM();
    class ARRAYSYM                  * asARRAYSYM();
    class PTRSYM                    * asPTRSYM();
    class NUBSYM                    * asNUBSYM();
    class TYVARSYM                  * asTYVARSYM();
    class AGGTYPESYM                * asAGGTYPESYM();
    class PINNEDSYM                 * asPINNEDSYM();
    class PARAMMODSYM               * asPARAMMODSYM();
    class MODOPTSYM                 * asMODOPTSYM();
    class MODOPTTYPESYM             * asMODOPTTYPESYM();
    class VOIDSYM                   * asVOIDSYM();
    class NULLSYM                   * asNULLSYM();
    class UNITSYM                   * asUNITSYM();
    class ANONMETHSYM               * asANONMETHSYM();
    class METHGRPSYM                * asMETHGRPSYM();
    class CACHESYM                  * asCACHESYM();
    class LABELSYM                  * asLABELSYM();
    class ERRORSYM                  * asERRORSYM();
    class ALIASSYM                  * asALIASSYM();
    class GLOBALATTRSYM             * asGLOBALATTRSYM();
    class EVENTSYM                  * asEVENTSYM();
    class XMLFILESYM                * asXMLFILESYM();
    class SYNTHINFILESYM            * asSYNTHINFILESYM();
    class MISCSYM                   * asMISCSYM();
    class EXTERNALIASSYM            * asEXTERNALIASSYM();

    // Define the ones for the abstract classes.
    class PARENTSYM                 * asPARENTSYM();
    class TYPESYM                   * asTYPESYM();
    class VARSYM                    * asVARSYM();

    // Define the ones which traverse subclass relationships:
    class METHSYM                   * asFMETHSYM();
    class INFILESYM                 * asANYINFILESYM();
        
    // type testing
    bool isBAGSYM();
    bool isDECLSYM();
    bool isNSSYM();
    bool isNSAIDSYM();
    bool isNSDECLSYM();
    bool isAGGSYM();
    bool isAGGDECLSYM();
    bool isFWDAGGSYM();
    bool isINFILESYM();
    bool isMODULESYM();
    bool isRESFILESYM();
    bool isOUTFILESYM();
    bool isMEMBVARSYM();
    bool isLOCVARSYM();
    bool isMETHSYM();
    bool isFAKEMETHSYM();
    bool isPROPSYM();
    bool isMETHPROPSYM();
    bool isSCOPESYM();
    bool isANONSCOPESYM();
    bool isARRAYSYM();
    bool isPTRSYM();
    bool isNUBSYM();
    bool isTYVARSYM();
    bool isAGGTYPESYM();
    bool isPINNEDSYM();
    bool isPARAMMODSYM();
    bool isMODOPTSYM();
    bool isMODOPTTYPESYM();
    bool isVOIDSYM();
    bool isNULLSYM();
    bool isUNITSYM();
    bool isANONMETHSYM();
    bool isMETHGRPSYM();
    bool isCACHESYM();
    bool isLABELSYM();
    bool isERRORSYM();
    bool isALIASSYM();
    bool isGLOBALATTRSYM();
    bool isEVENTSYM();
    bool isIFACEIMPLMETHSYM();
    bool isXMLFILESYM();
    bool isSYNTHINFILESYM();
    bool isMISCSYM();
    bool isEXTERNALIASSYM();

    // Define the ones for the abstract classes.
    bool isPARENTSYM();
    bool isTYPESYM();
    bool isVARSYM();

    // Define the ones which traverse subclass relationships:
    bool isFMETHSYM();
    bool isANYINFILESYM();

    SYMKIND getKind() { return (SYMKIND)this->kind; }
    void setKind(SYMKIND kind) { this->kind = kind; }
    
    // Allocate zeroed from a no-release allocator.
    void * operator new(size_t sz, NRHEAP * allocator, int * psidLast)
    {
        if (!psidLast)
            return allocator->AllocZero(sz);
        void ** ppv = (void **)allocator->AllocZero(sz + sizeof(void *));
        *ppv++ = (void *)(INT_PTR)++(*psidLast);
        return (void *)ppv;
    }

    symbmask_t mask() { return ((symbmask_t) 1) << kind; };

    void copyInto(SYM * sym) {
        sym->access         = this->access;
        sym->isBogus        = this->isBogus;
        sym->checkedBogus   = this->checkedBogus;
    }

    bool hasExternalAccess();
    bool hasExternalOrFriendAccess();

    // returns parse tree for classes, and class members
    BASENODE *      getParseTree();
    BASENODE *      GetSomeParseTree();
    INFILESYM *     getInputFile();
    INFILESYM *     GetSomeInputFile();
    MODULESYM * GetModule() const;
    IMetaDataImport2 * GetMetaImportV2(COMPILER *compiler);
    IMetaDataImport * GetMetaImport(COMPILER * compiler);
    int GetAssemblyID();
    bool InternalsVisibleTo(int aid);
    bool SameAssemOrFriend(SYM * sym);

    mdToken *       getTokenEmitPosition();
    mdToken         getTokenEmit();
    mdToken         getTokenImport();
    bool            isUserCallable();
    CorAttributeTargets getElementKind();

    // This one asserts if there is no decl.
    DECLSYM * containingDeclaration() {
        DECLSYM * decl = GetDecl();
        ASSERT(decl || isNSDECLSYM());
        return decl;
    }

    // This one returns NULL if there is no decl.
    DECLSYM * GetDecl();

    BASENODE *      getAttributesNode();
    bool            isContainedInDeprecated() const;
    bool            IsVirtual();
    bool            IsOverride();
    bool            IsHideByName();
    SYM *           SymBaseVirtual(); // Returns the virtual that this sym overrides (if its an override or explicit interface member impl), NULL otherwise.
    bool            checkBogus() const { ASSERT(this->checkedBogus); return this->isBogus; } // if this ASSERT fires then call COMPILER::CheckBogus() instead
    bool            getBogus() const { return this->isBogus; }
    bool            hasBogus() const { return this->checkedBogus; }
    void            setBogus(bool isBogus) { this->isBogus = isBogus; this->checkedBogus = true; }
    bool            isUnsafe();

    bool IsDeprecated();
    bool IsDeprecatedError();
    PCWSTR DeprecatedMessage();
    void SetDeprecated(bool fDep, bool fDepError, PCWSTR pszDep);
    void CopyDeprecatedFrom(SYM * symSrc)
        { SetDeprecated(symSrc->IsDeprecated(), symSrc->IsDeprecatedError(), symSrc->DeprecatedMessage()); }

    int GetSid() {
        ASSERT(fHasSid);
        return fHasSid ? (int)(INT_PTR)((void **)this)[-1] : 0;
    }

#ifdef DEBUG
    virtual void zDummy() {};
#endif
};


// Encapsulates a type list, including its size and metadata token.
class TypeArray
{
private:
    friend class TypeArrayTable; // So it can set fHasErrors and fUnres.
    uint aggState : 8; // This is for an optimization that avoids calling EnsureState on each item.
    uint fHasErrors: 1; // Whether any constituents have errors. This is immutable.
    uint fUnres: 1; // Whether any constituents are unresolved. This is immutable.

public:

    mdToken tok;
    int size; // Read only!

    AggStateEnum AggState() { return (AggStateEnum)aggState; }
    void SetAggState(AggStateEnum aggState) { this->aggState = (uint)aggState; }

    int static Size(TypeArray * pta) {
        return pta ? pta->size : 0;
    }

    bool HasErrors() { return fHasErrors; }
    bool IsUnresolved() { return fUnres; }

    TYPESYM * Item(int i) {
        ASSERT((unsigned)i < (unsigned)size);
        return items[i];
    }
    TYVARSYM * ItemAsTYVARSYM(int i) {
        ASSERT((unsigned)i < (unsigned)size);
        return ((SYM *)items[i])->asTYVARSYM();
    }

    void CopyItems(int i, int c, TYPESYM ** dest) {
        ASSERT(SizeAdd(i, c) <= (size_t)size);
        memcpy(dest, items + i, c * sizeof(items[0]));
    }

    void CopyItems(int i, int c, TYVARSYM ** dest) {
        ASSERT(SizeAdd(i, c) <= (size_t)size);
        memcpy(dest, items + i, c * sizeof(items[0]));
    }

    TYPESYM ** ItemPtr(int i) {
        ASSERT((uint)i <= (uint)size);
        return &items[i];
    }

#if DEBUG
    void AssertValid() {
        ASSERT(size >= 0);
        for (int i = 0; i < size; i++) {
            ASSERT(items[i]);
        }
    }
#endif

    static bool EqualRange(TypeArray * pta1, int i1, TypeArray * pta2, int i2, int c) {
        ASSERT(SizeAdd(i1, c) <= (size_t)pta1->size);
        ASSERT(SizeAdd(i2, c) <= (size_t)pta2->size);
        return !memcmp(pta1->items + i1, pta2->items + i2, c * sizeof(pta1->items[0]));
    }

    int Find(TYPESYM * type) {
        for (int i = 0; i < size; i++) {
            if (type == items[i])
                return i;
        }
        return -1;
    }

    bool Contains(TYPESYM * type) {
        for (int i = 0; i < size; i++) {
            if (type == items[i])
                return true;
        }
        return false;
    }

private:
    TYPESYM * items[];
};

/*
 * CACHESYM - a symbol which wraps other symbols so that
 * they can be cached in the local scope by name
 * LOCVARSYMs are never cached in the introducing scope
 */
namespace NameCacheFlags { enum _Enum {
    None = 0x00,
    TypeSameName = 0x01,
    NoTypeSameName = 0x02,
}; };
DECLARE_FLAGS_TYPE(NameCacheFlags)


class CACHESYM: public SYM {
public:
    IS_A(CACHESYM)

    PSYM sym; // The symbol this cache entry points to
    TypeArray * types; // The types containing the symbol(s)
    NameCacheFlagsEnum flags;
    PSCOPESYM scope;  // The scope in which this name is bound to that symbol

    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(MISCSYM)
        NOT_A(SCOPESYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};

/*
 * LABELSYM - a symbol representing a label.  
 */
class LABELSYM: public SYM {
public:
    IS_A(LABELSYM)

    class EXPRLABEL * labelExpr; // The corresponding label statement

    NOT_A(CACHESYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
        NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};


/*
 * PARENTSYM - a symbol that can contain other symbols as children.
 */
class PARENTSYM: public SYM {
public:
    IS_A(PARENTSYM)

    SYM * firstChild;       // List of all children of this symbol
    SYM ** psymAttachChild; // Where the next child will be linked in.

    // This adds the sym to the child list but doesn't associate it
    // in the symbol table.
    void AddToChildList(SYM * sym);
    void RemoveFromChildList(SYM * sym);

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};

#define FOREACHCHILD(_parent, _child)           \
{                                               \
    for (SYM * _child = (_parent)->firstChild;  \
         _child;                                \
         _child = _child->nextChild) {          \

#define ENDFOREACHCHILD                         \
    }                                           \
}

/*
 * PREDEFATTR - enum of predefined attributes
 */
enum PREDEFATTR
{

#define PREDEFATTRDEF(id,iPredef) id,
#include "predefattr.h"
#undef PREDEFATTRDEF

    PA_COUNT
};



/*
 * TYPESYM - a symbol that can be a type. Our handling of derived types
 * (like arrays and pointers) requires that all types extend PARENTSYM.
 */

class TYPESYM: public PARENTSYM {
public:
    IS_A(TYPESYM)

    PWSTR pszAssemblyQualifiedName;

private:
    friend class BSYMMGR; // So it can set fHasErrors and fUnres.
    uint aggState: 8; // The minimum of the states of the constituent AGGSYMs.
    uint fHasErrors: 1; // Whether any constituents have errors. This is immutable.
    uint fUnres: 1; // Whether any constituents are unresolved. This is immutable.

public:

    AggStateEnum AggState() { return (AggStateEnum)aggState; }
    void SetAggState(AggStateEnum aggState) { this->aggState = (uint)aggState; }
    bool IsPrepared() { return AggState() >= AggState::Prepared; }

    void InitFromParent();

    bool HasErrors() { return fHasErrors; }
    bool IsUnresolved() { return fUnres; }

    // Get the fundemental type of the symbol.
    FUNDTYPE fundType();
    TYPESYM * underlyingType(); // if the type is an enum, expose the type underlying it, else return the type
    TYPESYM * GetNakedType(bool fStripNub = false);   // Strip off ARRAYSYM, PTRSYM, PARAMMODSYM, PINNEDSYM and optionally NUBSYM down to the naked type.
    AGGSYM * GetNakedAgg(bool fStripNub = false);     // If GetNakedType() returns an AGGTYPESYM, this returns the associated AGGSYM. Otherwise, returns NULL.
    AGGSYM * getAggregate();    // Asserts that this is an AGGTYPESYM. Returns the associated AGGSYM.

    TYPESYM * StripNubs() {
        if (!this)
            return NULL;
        TYPESYM * type;
        for (type = this; type->isNUBSYM(); type = type->parent->asTYPESYM())
            ;
        return type;
    }

    TYPESYM * StripNubs(int * pcnub) {
        *pcnub = 0;
        if (!this)
            return NULL;
        TYPESYM * type;
        for (type = this; type->isNUBSYM(); type = type->parent->asTYPESYM())
            (*pcnub)++;
        return type;
    }

    TYPESYM * StripAllButOneNub() {
        if (!this || !this->isNUBSYM())
            return this;
        TYPESYM * type;
        for (type = this; type->parent->isNUBSYM(); type = type->parent->asTYPESYM())
            ;
        return type;
    }

    AGGTYPESYM * asATSorNUBSYM();

    bool isATSorNUBSYM() {
        return isNUBSYM() || isAGGTYPESYM();
    }

    bool     isDelegateType();
    bool     isSimpleType();
    bool     isSimpleOrEnum();
    bool     isSimpleOrEnumOrString();
    bool     isSimpleOrEnumOrStringOrPtr();
    bool     isSimpleOrEnumOrStringOrAnyPtr();
    bool     isPointerLike(); // any pointers plus IntPtr and UIntPtr
    bool     isQSimpleType();
    bool     isNumericType();
    bool     isStructOrEnum();  // struct or enum - AGGTYPESYM only
    bool     isStructType();    // struct - AGGTYPESYM only
    bool     isEnumType();      // enum - AGGTYPESYM only
    bool     isInterfaceType(); // interfact - AGGTYPESYM only
    bool     isClassType();     // class - AGGTYPESYM only
    AGGTYPESYM *underlyingEnumType();
    bool     isUnsigned();
    bool     isUnsafe();
    bool     isPredefType(PREDEFTYPE pt);
    bool     isPredefined();
    PREDEFTYPE getPredefType();
    bool     isSpecialByRefType();
    bool     isGenericInstance();
    bool     isSecurityAttribute();
    bool     isFabricated();
    bool     isStaticClass();

    // These check for AGGTYPESYMs, TYVARSYMs and others as appropriate.
    bool IsValType();
    bool IsNonNubValType();
    bool IsRefType();       // is this type known to be a reference type by the C# spec
    bool IsRefTypeInVerifier();  // is this type known to be a reference type by the JIT

    // A few types can be the same pointer value and not actually
    // be equivalent or convertible (like ANONMETHSYMs)
    bool IsNeverSameType() { return isANONMETHSYM() || isMETHGRPSYM() || (isERRORSYM() && !parent); }

    // Only some types can be used in const declarations
    bool CanBeConst() { return IsRefType() || isSimpleOrEnumOrString(); }
    bool CanBeVolatile ();

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
        NOT_A(MISCSYM)
};







/*
 * OUTFILESYM -- a symbol that represents an output file we are creating.
 * Its children all all input files that contribute. The symbol name is the
 * file name.
 */
class OUTFILESYM: public PARENTSYM {
private:
    int     aid;                    // The module id for the output module
                                    // Only set if this is a real output file with
                                    // real sources as INFILESYMs
public:
    IS_A(OUTFILESYM)

    bool    isDll: 1;               // A dll or an exe?
    bool    isResource: 1;          // Is this a resource file that we linked?
    bool    isManifest: 1;          // Whether to make this an assembly.
    bool    isConsoleApp: 1;        // A console application?
    bool    multiEntryReported: 1;  // Has the 'Multiple Entry Points' error been reported for this file?
    bool    makeResFile: 1;         // True if we autogenerate .RES file.  False if we use resourceFile
    bool    fHasSigningAttribute: 1;// True if this assembly will be strong named signed via an attribute (i.e. either KeyFile or DelaySign attributes)

    int     defaultCharSet;         // Default character set marshalling (from module level attribute)
    ULONGLONG imageBase;            // Image base (or 0 for default)
    ULONG   fileAlign;              // File Alignment (or 0 for default)
    union {
        PWSTR  resourceFile;           // resource file name. (valid iff makeResFile == false)
        PWSTR  iconFile;               // icon file name.     (valid iff makeResFile == true)
    };
    AGGSYM *globalClass;            // global class of which we hang native methodrefs
    PWSTR  entryClassName;         // User specified entryPoint Fully-Qualified Class name
    PMETHSYM entrySym;              // 'Main' method symbol (for EXEs only)
    mdFile  idFile;                 // MetaData token for the file
    mdModuleRef idModRef;           // Used for scoped TypeRefs
    PGLOBALATTRSYM attributes;      // Attributes for this module
    NAME * nameModule;              // The module name as it will appear in metadata
    NAME * simpleName;              // The assembly's simple name (only set if isManifest)
    BYTE * pbPublicKeyToken;        // Chached value of public key token (gotten from command-line arguments)
    DWORD  cbPublicKeyToken;        // size of public key token (-1 for un-inited, 0 for 'NULL')
    ULONG   cInputFiles;            // nummber of input files
    METHSYM * methodWithEmittedModuleInfo;

    PCWSTR pszPDBFile;             // if !null, then is the filename for the output file, specified by /pdb:filename.  This may be either a path or a file.
                                    // if this is not set, we default to the output filename as the name for the pdb.

    PINFILESYM  firstInfile() {
        PSYM sym;
        for( sym = firstChild; sym && !sym->isINFILESYM(); sym = sym->nextChild);
        return sym ? sym->asINFILESYM() : NULL; }
    PRESFILESYM firstResfile() { 
        PSYM sym;
        for( sym = firstChild; sym && !sym->isRESFILESYM(); sym = sym->nextChild);
        return sym ? sym->asRESFILESYM() : NULL; }
    POUTFILESYM nextOutfile() { return nextChild->asOUTFILESYM(); }
    bool isUnnamed() { return !wcscmp(name->text, L"?"); }

    void SetModuleID(int aid) { ASSERT(this->aid == 0); this->aid = aid; }
    int GetModuleID();

    bool hasNoPublicKeyToken() { return cbPublicKeyToken == 0;}

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};


/*
 * RESFILESYM - a symbol that represents a resource input file.
 * Its parent is the output file it contributes to, or Default
 * if it will be embeded in the Assembly file.
 * The symbol name is the resource Identifier.
 */
class RESFILESYM: public SYM {
public:
    IS_A(RESFILESYM)

    PWSTR  filename;
    bool    isVis;
    bool    isEmbedded;

    PRESFILESYM nextResfile() {
        PSYM sym;
        for( sym = nextChild; sym && !sym->isRESFILESYM(); sym = sym->nextChild);
        return sym ? sym->asRESFILESYM() : NULL; }
    POUTFILESYM getOutputFile() { return parent->asOUTFILESYM(); }

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};

template<typename T>
class CorTList
{
public:
    CComPtr<T> corObject;
    class CorTList<T> * next;
    bool HasCorObject(T * object)
    {
        for (CorTList<T> * p = this; p; p = p->next) 
        {
            if (p->corObject == object) return true;
        }
        return false;
    }
    void NullAll()
    {
        for (CorTList<T> * p = this; p; p = p->next)
        {
            p->corObject = NULL;
        }
    }
    static HRESULT Add(CorTList<T> ** list, NRHEAP * heap, T* newObject)
    {
        CorTList<T> * newOne = (CorTList<T> *) heap->AllocZero(sizeof(CorTList<T>));
        if (!newOne) return E_OUTOFMEMORY;
        newOne->corObject = newObject;
        newOne->next = *list;
        *list = newOne;
        return S_OK;
    }
    static HRESULT EnsurePresent(CorTList<T> ** list, NRHEAP * heap, T* newObject)
    {
        if ((*list)->HasCorObject(newObject)) return S_OK;
        return Add(list, heap, newObject);
    }
    T* FindAnyCorObject ()
    {
        for (CorTList<T> * p = this; p; p = p->next)
        {
            if (p->corObject) return p->corObject.p;
        }
        return NULL;
    }
};

typedef CorTList<ICorDebugModule> CorModuleList;
typedef CorTList<ICorDebugAssembly> CorAssemblyList;

/*
 * INFILESYM - a symbol that represents an input file, either source
 * code or meta-data, of a file we may read. Its parent is the output
 * file it contributes to. The symbol name is the file name.
 * Children include MODULESYMs.
 */
class INFILESYM: public PARENTSYM {
public:
    IS_A(INFILESYM)

private:
    // Which aliases this INFILE is in. For source INFILESYMs, only bits kaidThisAssembly and kaidGlobal
    // should be set.
    BitSet bsetFilter;
    int aid;

    BitSet bsetFriend; // Which assemblies this one grants rights to. May include kaidThisAssembly.
#ifdef DEBUG
    bool fUnionCalled;
#endif

public:
    void SetAssemblyID(int aid, NRHEAP * heap);
    int GetAssemblyID() {
        ASSERT(aid >= kaidThisAssembly);
        return aid;
    }

    void AddInternalsVisibleTo(int aid, NRHEAP * heap);
    bool InternalsVisibleTo(int aid) { return bsetFriend.TestBit(aid); }

    void AddToAlias(int aid, NRHEAP * heap);
    bool InAlias(int aid) {
        ASSERT(0 <= aid);
        return bsetFilter.TestBit(aid);
    }

    // Unions this INFILESYM's bsetFilter into the given destination bitset.
    // In DEBUG we assert if AddToAlias is ever called after this has been called.
    void UnionAliasFilter(BitSet & bsetDst, NRHEAP * heap);

    bool   isSource: 1;         // If true, source code, if false, metadata
    bool   isAddedModule: 1;
    bool   hasGlobalAttr: 1;
    bool   isDefined: 1;        // have symbols for this file been defined
    bool   isConstsEvaled: 1;   // have compile time constants been evaluated for this file
    bool   isBCL: 1;            // is this the infilesym for mscorlib.dll
    bool   hasModuleCLSattribute: 1;// We need to distinguish between CLSCompliantAttribute on the assembly
                                    // and on the module of added .netmodules
    bool fTooManyLinesReported: 1;  // For tracking if we've already reported too many lines for debug info on this file (only valid if isSource)
    bool fFriendAccessUsed: 1;

    NSDECLSYM * rootDeclaration;    // the top level declaration for this file


    // If metadata, then the following are available.
    int cscope;
    mdToken mdImpFile;
    IMetaDataAssemblyImport * assemimport; // assembly meta-data import interface.
    NAME * assemblyName;           // text version of assembly for attributes
    WORD   assemblyVersion[4];     // assembly version numbers (used for comparing 2 equivalent imports)
    mdAssemblyRef idLocalAssembly; // Assembly id for use in scoped TypeRefs.
    MODULESYM * moduleManifest;        // The main module - the one with the assembly manifest

    // If a source file, then the following are available.
    struct NAMESPACENODE * nspace;  // The top level namespace associated w/ the file
    CSourceData * pData; // Associated source module data
    ISymUnmanagedDocumentWriter * documentWriter;

    PINFILESYM nextInfile() {
        PSYM sym;
        for( sym = nextChild; sym && !sym->isINFILESYM(); sym = sym->nextChild);
        return sym ? sym->asINFILESYM() : NULL; }
    POUTFILESYM getOutputFile() { return ((parent->isOUTFILESYM()) ? parent->asOUTFILESYM() : parent->asINFILESYM()->getOutputFile()); }
    bool isSymbolDefined(NAME *symbol);
    int CompareVersions(INFILESYM * infile2) {
        int result = 0;
        for (unsigned int i = 0; result == 0 && i < lengthof(assemblyVersion); i++)
            result = assemblyVersion[i] - infile2->assemblyVersion[i];
        return result;
    }

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};

/***************************************************************************************************
    A MODULESYM has an array of these - one for each unloaded type def in the module. They are
    sorted by (tokRoot, tok). tokRoot is outermost (just inside the namespace) type containing tok.
***************************************************************************************************/
struct ModTypeInfo
{
    NSSYM * ns;
    mdTypeDef tokRoot;
    mdTypeDef tok;

    bool operator <(ModTypeInfo & mti) {
        return tokRoot < mti.tokRoot || tokRoot == mti.tokRoot && tok < mti.tok;
    }
    bool operator <=(ModTypeInfo & mti) {
        return tokRoot < mti.tokRoot || tokRoot == mti.tokRoot && tok <= mti.tok;
    }
};


// Represents an imported module. Parented by a metadata INFILESYM. Name is the module name.
// Parents MODOPTSYMs.
class MODULESYM : public PARENTSYM {
private:
    int aid;
    int iscope; // Needed to communicate back to Alink.
    IMetaDataAssemblyImport * assemimport;
    IMetaDataImport2 * metaimportV2;


public:
    // These are set and consumed by IMPORTER. The ModTypeInfo array contains
    // all type defs and their corresponding NSSYM (if it hasn't been loaded yet).
    // This is used to defer loading of types until the containing namespace is being
    // accessed for the first time.
    int cmti;
    ModTypeInfo * prgmti;

    void SetModuleID(int aid) { ASSERT(this->aid == 0); this->aid = aid; }
    int GetModuleID() {
        ASSERT(aid >= kaidStartAssigning);
        return aid;
    }

    INFILESYM * getInputFile() { return parent->asINFILESYM(); }

    void Clear();
    void Init(int iscope, IMetaDataImport2 * metaimport);

    IMetaDataImport * GetMetaImport(COMPILER *compiler);
    IMetaDataImport2 * GetMetaImportV2(COMPILER *compiler);
    IMetaDataAssemblyImport * GetAssemblyImport(COMPILER *compiler);
    int GetIndex() { return this->iscope; }
};

/* 
 * Same as above. Only used for #line directives
 */
class SYNTHINFILESYM : public INFILESYM {
public:
    ERRLOC *pErrlocChecksum;
    GUID    guidChecksumID;
    BYTE   *pbChecksumData;
    int     cbChecksumData;
};


// Base class for NSSYM and AGGSYM. Bags have DECLSYMs.
// Parent is another BAG. Children are other BAGs, members, type vars, etc.
class BAGSYM : public PARENTSYM {
public:
    DECLSYM * declFirst;
    DECLSYM ** pdeclAttach;

    void AddDecl(DECLSYM * decl, NRHEAP * heap);
    BAGSYM * BagPar() { return parent->asBAGSYM(); }
    DECLSYM * DeclFirst() { return declFirst; }

    bool InAlias(COMPILER * comp, int aid);
};


// Base class for NSDECLSYM and AGGDECLSYM. Parent is another DECL.
// Children are DECLs.
class DECLSYM : public PARENTSYM {
public:
    BAGSYM * bag;
    DECLSYM * declNext;

    void containingDeclaration() { } // Hides SYM::containingDeclaration. Use DeclPar instead.

    DECLSYM * DeclPar() { return parent->asDECLSYM(); }
    BAGSYM * Bag() { return bag; }
    DECLSYM * DeclNext() { return declNext; }
    NSDECLSYM * GetNsDecl();

    bool IsFirst() { return this == Bag()->DeclFirst(); }
};


/*
 * Namespaces, Namespace Declarations, and their members.
 *
 *
 * The parent, child, nextChild relationships are overloaded for namespaces.
 * The cause of all of this is that a namespace can be declared in multiple
 * places. This would not be a problem except that the using clauses(which
 * effect symbol lookup) are related to the namespace declaration not the
 * namespace itself. The result is that each namespace needs lists of all of
 * its declarations, and its members. Each namespace declaration needs a list
 * the declarations and types declared within it. Each member of a namespace
 * needs to access both the namespace it is contained in and the namespace
 * declaration it is contained in.
 */

/*
 * NSSYM - a symbol representing a name space. 
 * parent is the containing namespace.
 */
class NSSYM: public BAGSYM {
public:
    IS_A(NSSYM)

    // List of declarations of this namespace in source files
    NSSYM * BagPar() { return parent->asNSSYM(); }
    NSSYM * Parent() { return parent->asNSSYM(); }
    NSDECLSYM * DeclFirst()  { return declFirst->asNSDECLSYM(); }

    bool fHasPredefs : 1; // Does this namespace contain predefined classes?
    bool checkedForCLS        : 1; // Have we already checked children for CLS name clashes?
    bool checkingForCLS       : 1; // Have we added this NSSYM to it's parent's list?
    bool isDefinedInSource    : 1; // Is this namespace (or rather one of it's NSDECLS) defined in source?

private:
    bool fAnyTypesUnloaded: 1; // This is a cache of bsetNeedLoaded.TestAnyBits()

    // Which assemblies and extern aliases contain this namespace.
    BitSet bsetFilter;

    // Which assemblies contain unloaded types in this namespace.
    BitSet bsetNeedLoaded;

public:
    // Called by BAGSYM::AddDecl whenever an NSDECLSYM is added.
    // This allows the NSSYM to update its alias bitset.
    void DeclAdded(NSDECLSYM * decl, NRHEAP * heap);

    bool InAlias(class COMPILER * comp, int aid) {
        ASSERT(0 <= aid);
        return bsetFilter.TestBit(aid);
    }
    bool InAlias(class BSYMMGR * bsymmgr, int aid)
    {
        ASSERT(0 <= aid);
        return bsetFilter.TestBit(aid);
    }
    void SetTypesUnloaded(int aid, NRHEAP * heap) {
        ASSERT(kaidThisAssembly <= aid && aid < kaidMinModule);
        bsetNeedLoaded.SetBit(aid, heap);
        fAnyTypesUnloaded = true;
    }
    void ClearTypesUnloaded(int aid) {
        ASSERT(kaidThisAssembly <= aid && aid < kaidMinModule);
        bsetNeedLoaded.ClearBit(aid);
        fAnyTypesUnloaded = bsetNeedLoaded.TestAnyBits();
    }
    bool TypesUnloaded(int aid) {
        ASSERT(0 <= aid);
        return bsetNeedLoaded.TestBit(aid);
    }
    bool AnyTypesUnloaded(BitSet & bsetCheck) {
        ASSERT(!fAnyTypesUnloaded == !bsetNeedLoaded.TestAnyBits());
        return fAnyTypesUnloaded && bsetNeedLoaded.TestAnyBits(bsetCheck);
    }
    bool AnyTypesUnloaded() {
        ASSERT(!fAnyTypesUnloaded == !bsetNeedLoaded.TestAnyBits());
        return fAnyTypesUnloaded;
    }

    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};

// Parented by an NSSYM. Represents an NSSYM within an aid (assembly/alias id).
// The name is a form of the aid.
class NSAIDSYM : public PARENTSYM {
public:
    int aid;

    int GetAid() { return aid; }
    NSSYM * GetNS() { return parent->asNSSYM(); }
};

/*
 * NSDECLSYM - a symbol representing a declaration
 * of a namspace in the source. 
 *
 * firstChild/firstChild->nextChild enumerates the 
 * NSDECLs and AGGDECLs declared within this declaration.
 *
 * parent is the containing namespace declaration.
 *
 * Bag() is the namespace corresponding to this declaration.
 *
 * DeclNext() is the next declaration for the same namespace.
 */
class NSDECLSYM : public DECLSYM {
public:
    IS_A(NSDECLSYM)

    NSDECLSYM * DeclPar() { return parent->asNSDECLSYM(); }
    NSSYM * Bag() { return bag->asNSSYM(); }
    NSSYM * NameSpace() { return bag->asNSSYM(); }
    NSDECLSYM * DeclNext() { return declNext->asNSDECLSYM(); }
    void GetNsDecl() { } // Hides DECLSYM::GetNsDecl.

    INFILESYM *             inputfile;
    NAMESPACENODE *         parseTree;
    SYMLIST *               usingClauses;
    METHSYM * methodWithEmittedUsings;

    bool isDefined : 1;
    // True if the using clauses for this declaration have been resolved. This can occur between declare and
    // define steps in compilation.
    bool usingClausesResolved:1;

    // accessors
    bool isRootDeclaration() { return !parent; }
    int GetAssemblyID() { return inputfile->GetAssemblyID(); }
    bool InternalsVisibleTo(int aid) { return inputfile->InternalsVisibleTo(aid); }
    bool isDottedDeclaration();  // true for a&b in namespace a.b.c {}

    NOT_A(NSSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};

/*
 * AGGDECLSYM - represents a declaration of a aggregate type. With partial classes,
 * an aggregate type might be declared in multiple places.  This symbol represents
 * on of the declarations.
 *
 * parent is the containing DECLSYM.
 */
class AGGDECLSYM: public DECLSYM
{
public:
    IS_A(AGGDECLSYM)


    struct BASENODE *   parseTree;       // The parse tree for this class.  
                         // Either a CLASSNODE or a DELEGATENODE
    bool            isUnsafe: 1;                 // Either this aggdecl, or an containing one, is marked with "unsafe".
    bool            isPartial: 1;                // This declaration is marked with the partial modifier.

    AGGSYM * Bag() { return bag->asAGGSYM(); }
    AGGSYM * Agg() { return bag->asAGGSYM(); }
    AGGDECLSYM * DeclNext() { return declNext->asAGGDECLSYM(); }

    INFILESYM * getInputFile() { return GetNsDecl()->inputfile; }
    int GetAssemblyID() { return GetNsDecl()->GetAssemblyID(); }
    bool InternalsVisibleTo(int aid) { return GetNsDecl()->InternalsVisibleTo(aid); }
    BASENODE *      getAttributesNode();

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};

// Macros to loop over all the AGGDECLs associated with an AGGSYM.

#define FOREACHAGGDECL(_parent, _child)              \
{                                \
    for (AGGDECLSYM * _child = (_parent)->DeclFirst(); \
         _child;                         \
         _child = _child->DeclNext()) {

#define ENDFOREACHAGGDECL                    \
    }                                \
}

// Name used for AGGDECLs in the symbol table.
#define AGGDECLNAME L"##decl"

#define DECLARE_DEP_STR \
    private: PCWSTR m_pszDep; \
    public: void SetDepMsg(PCWSTR pszDep) { m_pszDep = pszDep; } \
    public: PCWSTR PszDepMsg() { return m_pszDep; }

/*
 * AGGSYM - a symbol representing an aggregate type. These are classes,
 * interfaces, and structs. Parent is a namespace or class. Children are methods,
 * properties, and member variables, and types (including its own AGGTYPESYMs).
 * 
 */
class AGGSYM: public BAGSYM {
    DECLARE_DEP_STR

private:
    // This INFILESYM is some infile for the assembly containing this AGGSYM.
    // It is used for fast access to the filter BitSet and assembly ID.
    INFILESYM * infile;

    // The instance type. Created when first needed.
    AGGTYPESYM * atsInst;

public:
    IS_A(AGGSYM)

    BAGSYM * Bag() { return parent->asBAGSYM(); }
    BAGSYM * Parent() { return parent->asBAGSYM(); }
    AGGDECLSYM * DeclFirst()  { return declFirst->asAGGDECLSYM(); }

    // This is allowed for enums, delegates, fabricated types and types imported from metadata,
    // all of which can have only one declaration.
    AGGDECLSYM * DeclOnly() {
        ASSERT(!hasParseTree || IsDelegate() || IsEnum() || isFabricated);
        ASSERT(DeclFirst() && DeclFirst()->DeclNext() == NULL);
        return DeclFirst();
    }

    void InitFromOuterDecl(DECLSYM * declOuter);

    /******************************************************************************
        Returns true iff this AGGSYM is in the given "alias". The aid is either
        an assembly ID or an extern alias ID (both come from BSYMMGR::AidAlloc()).
        The AGGSYM's infile has a BitSet (bsetFilter) indicating which "aliases"
        contain the INFILESYM (and the entire associated assembly).
    ******************************************************************************/
    bool InAlias(int aid) {
        ASSERT(infile);
        ASSERT(!DeclFirst() || DeclFirst()->GetAssemblyID() == infile->GetAssemblyID());
        ASSERT(0 <= aid);
        if (aid < kaidMinModule)
            return infile->InAlias(aid);
        return (aid == GetModuleID());
    }

    int GetModuleID() {
        return (isSource ? getOutputFile()->GetModuleID() : module->GetModuleID());
    }

    int GetAssemblyID() {
        ASSERT(infile);
        ASSERT(!DeclFirst() || DeclFirst()->GetAssemblyID() == infile->GetAssemblyID());
        return infile->GetAssemblyID();
    }

    bool InternalsVisibleTo(int aid) {
        ASSERT(infile);
        ASSERT(!DeclFirst() || DeclFirst()->GetAssemblyID() == infile->GetAssemblyID());
        return infile->InternalsVisibleTo(aid);
    }
    bool IsUnresolved() {
        return infile && infile->GetAssemblyID() == kaidUnresolved;
    }
    UNRESAGGSYM * AsUnresolved() {
        ASSERT(infile && infile->GetAssemblyID() == kaidUnresolved);
        return reinterpret_cast<UNRESAGGSYM *>(this);
    }

    bool IsCLRAmbigStruct();

private:
    uint state : 8;
public:
    AggStateEnum AggState() const { return (AggStateEnum)state; }
    void SetAggState(AggStateEnum aggState) { state = (uint)aggState; }

    bool IsResolvingBaseClasses() const { return AggState() == AggState::ResolvingInheritance; }
    bool HasResolvedBaseClasses() const { return AggState() >= AggState::Inheritance; }
    bool IsDefined() const { return AggState() >= AggState::DefinedMembers; }
    bool IsPreparing() const { return AggState() == AggState::Preparing; }
    bool IsPrepared() const { return AggState() >= AggState::Prepared; }
    bool IsCompiled() const { return AggState() >= AggState::Compiled; }

private:
    uint aggKind : 8;
public:
    AggKindEnum AggKind() const { return (AggKindEnum)aggKind; }
    void SetAggKind(AggKindEnum aggKind) { this->aggKind = (uint)aggKind; }

    bool IsClass() const { return AggKind() == AggKind::Class; }
    bool IsDelegate() const { return AggKind() == AggKind::Delegate; }
    bool IsInterface() const { return AggKind() == AggKind::Interface; }
    bool IsStruct() const { return AggKind() == AggKind::Struct; }
    bool IsEnum() const { return AggKind() == AggKind::Enum; }
    bool IsValueType() const { return AggKind() == AggKind::Struct || AggKind() == AggKind::Enum; }
    bool IsRefType() const { return AggKind() == AggKind::Class || AggKind() == AggKind::Interface || AggKind() == AggKind::Delegate; }

    // Predefined
    uint isPredefined: 1;    // A special predefined type.
    uint iPredef: 7;        // index of the predefined type, if isPredefined.

    uint isTypeDefEmitted: 1; // has type defs been emitted?
    uint isBasesEmitted: 1; // has the bases for the type def been emitted?
    uint isMemberDefsEmitted: 1; // have all member defs been emitted?

    // Set if we've previously checked the instance type of this struct for cycles.
    uint fLayoutChecked: 1;
    uint fLayoutError: 1;

    uint isSource: 1;        // This class is defined in source, although the source might not be being read during this compile.
    uint hasParseTree: 1;    // Do one or more parse trees exist for this class?  

    // Flags
    uint isAbstract: 1;      // Can it be instantiated?
    uint isSealed: 1;        // Can it be derived from?

    // returns true iff this is a static class.  A static class is defined to be both abstract and sealed.
    bool IsStatic() { 
        return (isAbstract && isSealed);
    }

    uint isMarshalByRef: 1;  // App-domain bound or context bound?

    // If the AGGSYM has non-zero arity and this is set, the arity is encoded in the metadata name (eg !2)
    uint isArityInName: 1;

    // Attribute
    uint isAttribute: 1;         // is this a class which derives from System.Attribute
    uint isSecurityAttribute: 1; // is this a class which derives from System.Security.CodeAccessPermission
    uint isMultipleAttribute: 1; // set if this class is an attribute class 
                                    // which can be applied multiple times to a single symbol
   
    uint hasConversion: 1;   // set if this type or any base type has user defined conversion operators

    uint isUnmanagedStruct: 1; // Set if the struct is known to be un-managed (for unsafe code). Set in FUNCBREC.
    uint isManagedStruct: 1; // Set if the struct is known to be managed (for unsafe code). Set during import.
    uint isMDPrivate: 1;     // Set if the type is known to be inaccessible to this assembly.
    uint isFixedBufferStruct: 1; // Set if the type is a fixed size buffer struct

    // Constructors
    uint hasNoArgCtor: 1;    // Whether it has an instance constructor taking no args
    uint hasPubNoArgCtor: 1; // Whether it has a public instance constructor taking no args
    uint hasUDStaticCtor: 1;       // Class has a user-defined static constructor

    uint hasExplicitImpl: 1;     // class has explicit impls (used only on structs)
    uint hasExternReference: 1;  // private struct members hsould not be checked for assignment or refencees

    // User defined operators
    uint fSelfCmpValid: 1; // below bits are valid
    uint fHasSelfEq: 1;    // has operator == defined on itself
    uint fHasSelfNe: 1;    // has operator != defined on itself

    uint fSkipUDOps: 1; // Never check for user defined operators on this type (eg, decimal, string, delegate).

    uint isComImport: 1;     // Does it have [ComImport]

    uint hasMethodImpl: 1;   // iface which is implemented by a method impl
    uint hasLinkDemand: 1;   // has a security attribute of type link demand

    uint fCheckedCondSymbols: 1;     // true iff conditionalSymbols already includes symbols from base aggregates


    TypeArray *typeVarsThis;    // The type variables for this generic class, as declarations.
    TypeArray *typeVarsAll;     // The type variables for this generic class and all containing classes.
    mdToken * toksEmitTypeVars; // The emitted tokens for all the type variables
    AGGTYPESYM * baseClass;     // For a class/struct/enum, the base class. For iface: unused.
    AGGTYPESYM * underlyingType;// For enum, the underlying type. For iface, the resolved CoClass. Not used for class/struct.
    TypeArray * ifaces;         // The explicit base interfaces for a class or interface.
    TypeArray * ifacesAll;      // Recursive closure of base interfaces ordered so an iface appears before all of its base ifaces.

    NAMELIST * conditionalSymbols;   // list of symbols which attributes are conditional on.

    class BSYMMGR * symmgr;     // This is so AGGTYPESYMs can instantiate their baseClass and ifacesAll members on demand.


    mdToken tokenImport;        // Meta-data token for imported class.
    MODULESYM * module;         // Meta-data module imported from

    mdExportedType tokenComType;    // The ComType token used for nested classes
    mdToken tokenEmit;          // Meta-data token (typeRef or typeDef) in the current output file.
    mdToken tokenEmitRef;       // If tokenEmit is a typeDef, this is the corresponding typeRef.

    // For abstract classes the list of all unoverriden abstract methods (inherited and new). These must be
    // in derived to base order.
    PSYMLIST abstractMethods;

    // First UD conversion operator. This chain is for this type only (not base types).
    // The hasConversion flag indicates whether this or any base types have UD conversions.
    METHSYM * convFirst;

    CorAttributeTargets attributeClass; // symbol type this type can be an attribute on. 
                                        // 0 == not an attribute class
                                        // -1 == unknown (not imported)

    PWSTR  comImportCoClass;   // If IsInterface() and class has ComImport and CoClass attributes, this is the unresolved CoClass string

    // See also AGGTYPESYM::GetBaseClass().
    AGGSYM * GetBaseAgg();

    // Returns true if agg is this AGGSYM or a base of this AGGSYM.
    // See also AGGTYPESYM::FindBaseType(AGGSYM * agg).
    bool FindBaseAgg(AGGSYM * agg);

    bool isNested() const { return parent && parent->isAGGSYM(); }
    AGGSYM * GetOuterAgg() const {
        return parent && parent->isAGGSYM() ? parent->asAGGSYM() : NULL;
    }

    unsigned        allowableMemberAccess();
    MODULESYM * GetModule() const { return this->module; }
    IMetaDataImport2 * GetMetaImportV2(COMPILER *compiler) { return this->GetModule()->GetMetaImportV2(compiler); }
    IMetaDataImport * GetMetaImport(COMPILER *compiler) { return this->GetModule()->GetMetaImport(compiler); }
    CorAttributeTargets     getElementKind();
    AGGTYPESYM *    getThisType();
    bool            isPredefAgg(PREDEFTYPE pt) { return this->isPredefined && (PREDEFTYPE)this->iPredef == pt; }

    // These methods (on SYM) return nothing useful for AGGSYMs. So make sure they aren't
    // called directly on an AGGSYM (they assert when called on a SYM.), we do this by declaration them as returning void.
    void      getParseTree() {}
    void      getInputFile() {}
    void      containingDeclaration() {}
    OUTFILESYM * getOutputFile();

    NOT_A(TYPESYM)
    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};

// SYM representing a forwarded type (one which previously existed in one assembly, but has been moved to a new assembly).  
// We need this sym in order to resolve typerefs which point to the old assembly, and should get forwarded to the new one.
class FWDAGGSYM : public SYM {
protected:
    // Actual AGGSYM this forwarder resolves to. Note that there may be a chain of forwarders.
    // This will NEVER by an unresolved AGGSYM. If the agg can't be resolved, moduleBreak and
    // tokBreak are set to where the resolution failed.
    AGGSYM * m_aggRes;

    // If resolution of the forwarder fails, these are set to the unresolvable assembly ref token and
    // the module containing the token.
    MODULESYM * m_moduleBreak;
    mdAssemblyRef m_tokBreak;
    bool m_fCycle;

    bool m_fResolving; // Used for cycle detection while resolving.


    void Resolve(COMPILER * compiler);

public:
    IS_A(FWDAGGSYM)

    int cvar;        // number of type parameters on this type.  This is derived from the name, since that is all we have to go on.
    mdToken tokenImport;        // Meta-data token used for importing.
    MODULESYM * module;         // Meta-data module imported from
    mdAssemblyRef tkAssemblyRef;      // Target assembly of this forwarder.  Instead of using this, you should just resolve this forwarder and use
                                // GetTargetAssemblyID() instead, that will point to the final assembly (if multiple forwarders are strung together).
    INFILESYM * infile;

    AGGSYM * GetAgg(COMPILER * compiler);

    // These are only valid after GetAgg has been called (and returned NULL).
    MODULESYM * GetModuleBreak() { return m_moduleBreak; }
    mdAssemblyRef GetAssemRefBreak() { return m_tokBreak; }
    bool FCycle() { return m_fCycle; }

    bool InAlias(int aid) {
        ASSERT(infile);
        ASSERT(0 <= aid);
        if (aid < kaidMinModule)
            return infile->InAlias(aid);
        return (aid == module->GetModuleID());
    }

    NOT_A(TYPESYM)
    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};

// A fabricated AGGSYM to represent an imported type that we couldn't resolve.
// Used for error reporting.
// In the EE this is used as a place holder until the real AGGSYM is created.
class UNRESAGGSYM: public AGGSYM {
public:
    // TypeRef information that couldn't be resolved.
    MODULESYM * moduleRef;
    mdTypeRef tokRef;

    // Module and typeref or assembly ref for error information. This may be
    // different than the above if there are type forwarders involved.
    MODULESYM * moduleErr;
    mdToken tokErr;

    // An error was issued when the UNRESAGGSYM was created, so don't issue one on use.
    // This is used to supress a bogus error when there is a type-forwarder cycle, for example.
    bool fSuppressError;

};


/*
 * PERMSETINFO - capability security on a symbol
 */
class PERMSETINFO
{
public:
    bool        isAnySet;
    bool        isGuid[10];
    STRCONST *  str[10];

    bool        isSet(int index)        { return str[index] != 0; }
};
typedef PERMSETINFO * PPERMSETINFO;

/*
 * AGGINFO - Additional information about an aggregate symbol that isn't
 * needed by other code binding against this aggregate. This structure
 * lives only when this particular aggregate is being compiled.
 */
class AGGINFO
{
public:
    bool hasStructLayout:1;
    bool hasExplicitLayout:1;
    bool hasUuid:1;     // COM classic attribute
    bool isComimport:1; // COM classic attribute
};
typedef AGGINFO * PAGGINFO;


// The pseudo-methods uses for accessing arrays (except in
// the optimized 1-d case.
enum ARRAYMETHOD {
    ARRAYMETH_LOAD,
    ARRAYMETH_LOADADDR,
    ARRAYMETH_STORE,
    ARRAYMETH_CTOR, 
    ARRAYMETH_GETAT,  // Keep these in this order!!!

    ARRAYMETH_COUNT
};

/*
 * ALIASSYM - a symbol representing an using alias clause
 *
 * Its parent is an NSDECLSYM, but it is not linked into the child list
 * or in the symbol table.
 */
class ALIASSYM: public SYM {
public:
    IS_A(ALIASSYM)

    bool hasBeenBound;
    bool fExtern;
    SYM * sym; // Can be NSAIDSYM or AGGTYPESYM.
    SYM * symDup; // Duplicate symbol - report error on use with . if this is non-null.
    BASENODE * parseTree;

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};

/*
 * ARRAYSYM - a symbol representing an array.
 */
class ARRAYSYM: public TYPESYM
{
public:
    IS_A(ARRAYSYM)

    int rank;               // rank of the array.                                                 
                            // zero means unknown rank int [?].
    PTYPESYM elementType()
        { return parent->asTYPESYM(); }  // parent is the element type.

    // Returns the first non-array type in the parent chain.
    PTYPESYM GetMostBaseType()
    {
        TYPESYM* type;
        for (type = parent->asTYPESYM(); type->isARRAYSYM(); type = type->parent->asTYPESYM())
            ;
        return type;
    }

    mdTypeRef  tokenEmit;                // Metadata token (typeRef) in the current output file.

    // Metadata tokens for ctor/load/loadaddr/store special methods.
    mdMemberRef  tokenEmitPseudoMethods[ARRAYMETH_COUNT];

    NOT_A(TYVARSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};


/*
 * PARAMMODSYM - a symbol representing parameter modifier -- either
 * out or ref.
 */
class PARAMMODSYM: public TYPESYM
{
public:
    IS_A(PARAMMODSYM)

    bool isRef: 1;            // One of these two bits must be set,
    bool isOut: 1;            // indication a ref or out parameter.

    PTYPESYM paramType()
        { return parent->asTYPESYM(); }  // parent is the parameter type.

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};
 

/*
 * MODOPTSYM - a symbol representing a modopt from an imported signature.
 * Parented by a MODULESYM. Contains the import token.
 * Caches the emit token.
 */
class MODOPTSYM: public SYM
{
public:
    IS_A(MODOPTSYM)

    MODULESYM * GetModule()
        { return parent->asMODULESYM(); }

    mdToken tokImport;
    mdToken tokEmit;
};


/*
 * MODOPTTYPESYM - a symbol representing a modopt modifying a type.
 * Parented by a TYPESYM. Contains a MODOPTSYM.
 */
class MODOPTTYPESYM: public TYPESYM
{
public:
    IS_A(MODOPTTYPESYM)

    TYPESYM * baseType()
        { return parent->asTYPESYM(); }  // parent is modified type.
    MODOPTSYM * opt;
};


/***************************************************************************************************
    Represents a generic constructed (or instantiated) type. Parent is the AGGSYM.
***************************************************************************************************/
class AGGTYPESYM: public TYPESYM {
public:
    IS_A(AGGTYPESYM)

private:
    AGGTYPESYM * baseType;  // This is the result of calling SubstTypeArray on the aggregate's baseClass.
    TypeArray * ifacesAll;  // This is the result of calling SubstTypeArray on the aggregate's ifacesAll.

public:

    AGGTYPESYM * GetBaseClass();
    TypeArray * GetIfacesAll();

    // This looks for the given AGGSYM among this type and its base types and returns the corresponding type.
    AGGTYPESYM * FindBaseType(AGGSYM * agg);
    bool FindBaseType(AGGTYPESYM * type) {
        return FindBaseType(type->getAggregate()) == type;
    }

    TypeArray *typeArgsThis;        // Array of arguments, e.g. "[String]" in "List<String>"
    TypeArray *typeArgsAll;         // includes args from outer types

    AGGTYPESYM *outerType;          // the outer type if this is a nested type
    mdToken tokenEmit;              // Metadata token (typeSpec) in the current output file.

    uint fConstraintsChecked: 1;    // Have the constraints been checked yet?
    uint fConstraintError: 1;       // Did the constraints check produce an error?

    // These two flags are used to track hiding within interfaces.
    // Their use and validity is always localized. See e.g. MemberLookup::LookupInInterfaces.
    uint fAllHidden: 1;             // All members are hidden by a derived interface member.
    uint fDiffHidden: 1;            // Members other than a specific kind are hidden by a derived interface member or class member.

    AGGSYM * getAggregate() const { return parent->asAGGSYM(); }
    bool IsInstType() { return getAggregate()->getThisType() == this; }
    AGGTYPESYM * GetInstType() { return getAggregate()->getThisType(); }

private:
    // The number of bits required to represent this type (for definite assignment) plus one.
    // It is zero if it hasn't yet been set.
    uint cbitDefAssg: 24;
public:
    bool FCbitDefAssgSet() { return cbitDefAssg > 0; }
    int GetCbitDefAssg() { ASSERT(cbitDefAssg > 0); return cbitDefAssg - 1; }
    void SetCbitDefAssg(int cbit) { ASSERT(cbit >= 0); cbitDefAssg = cbit + 1; }

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
};


// Special constraints.
namespace SpecCons { enum _Enum {
    None = 0x00,

    New = 0x01,
    Ref = 0x02,
    Val = 0x04,
}; };
DECLARE_FLAGS_TYPE(SpecCons);


/***************************************************************************************************
    Represents a type variable within an aggregate or method. Parent is the owning AGGSYM
    or METHSYM. There are canonical TYVARSYMs for each index used for normalization of emitted
    metadata.
***************************************************************************************************/
class TYVARSYM: public TYPESYM {
public:
    IS_A(TYVARSYM)

    // Special constraints.
    uint cons: 8;       // Special constraints.

    uint isMethTyVar: 1;        // true for method type variable, false for class type variable.
    uint seenWhere: 1;          // Used locally by DefineBounds.

    uint fResolving: 1;         // Use for recursion detection while computing ifacesAll (in ResolveBounds).
    uint fHasRefBnd: 1;         // Whether typeBaseAbs implies that this type variable is a reference type.
    uint fHasValBnd: 1;         // Whether typeBaseAbs implies that this type variable is a value type. Rarely set....


private:
    TypeArray * bnds;           // Bounds. Contains: class (0 or 1), type vars (0 or more), interfaces (0 or more).
    AGGTYPESYM * atsBaseCls;    // The effective base class.
    TYPESYM * typeBaseAbs;      // Most derived type bound. This is usually an AGGTYPESYM, but may be a NUBSYM or ARRAYSYM.
    TypeArray * ifacesAll;      // Recursive closure of the interface bounds. The effective interface list (and base ifaces).

public:
    TYPEBASENODE * parseTree;   // parse tree
    short index;                // no. of tyvar in declaration list
    short indexTotal;           // no. of tyvar starting at outer most type
    mdToken tokenEmit;          // Metadata token (typeSpec) in the current output file.  (this is only used for standard type variables)

    ATTRLIST *  attributeList;

    ATTRLIST ** attributeListTail;    

    AGGSYM * getClass() const { return parent->asAGGSYM(); }

    void SetBnds(TypeArray * bnds) {
        this->bnds = bnds;
        this->atsBaseCls = NULL;
        this->typeBaseAbs = NULL;
        this->ifacesAll = NULL;
        this->fHasRefBnd = false;
        this->fHasValBnd = false;
    }
    void SetBaseTypes(TYPESYM * typeBaseAbs, AGGTYPESYM * atsBaseCls);
    void SetIfacesAll(TypeArray * ifacesAll) { this->ifacesAll = ifacesAll; }

    TypeArray * GetBnds() { return bnds; }
    AGGTYPESYM * GetBaseCls() { return atsBaseCls; }
    TYPESYM * GetAbsoluteBaseType() { return typeBaseAbs; }
    TypeArray * GetIfacesAll() { return ifacesAll; }

    bool FResolved() {
        ASSERT(bnds || !typeBaseAbs);
        ASSERT(!typeBaseAbs == !atsBaseCls && !typeBaseAbs == !ifacesAll);
        return ifacesAll != NULL;
    }

    // Returns true iff the type variable must be a reference type.
    bool IsRefType() {
        return (cons & SpecCons::Ref) || fHasRefBnd;
    }
    bool IsValType() {
        return (cons & SpecCons::Val) || fHasValBnd;
    }
    bool IsNonNubValType() {
        return (cons & SpecCons::Val) || fHasValBnd && !typeBaseAbs->isNUBSYM();
    }

    bool FNewCon() { return !!(cons & SpecCons::New); }
    bool FRefCon() { return !!(cons & SpecCons::Ref); }
    bool FValCon() { return !!(cons & SpecCons::Val); }

    bool FCanNew() { return !!(cons & (SpecCons::New | SpecCons::Val)) || fHasValBnd; }

    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};


/*
 * PTRSYM - a symbol representing a pointer type
 */
class PTRSYM: public TYPESYM
{
public:
    IS_A(PTRSYM)

    mdTypeRef  tokenEmit;   // Metadata token (typeRef) in the current output file.

    PTYPESYM baseType()
        { return parent->asTYPESYM(); }  // parent is the base type.
    PTYPESYM GetMostBaseType() 
    {
        TYPESYM * type;
        for (type = parent->asTYPESYM(); type->isPTRSYM(); type = type->parent->asTYPESYM())
            ;
        return type;
    };

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};


/***************************************************************************************************
    A "derived" type representing Nullable<T>. The base type T is the parent.
***************************************************************************************************/
class NUBSYM: public TYPESYM
{
private:
    friend class BSYMMGR; // So it can set fHasErrors and fUnres.
    AGGTYPESYM * ats;
    class BSYMMGR * symmgr;

public:
    IS_A(NUBSYM)

    TYPESYM * baseType()
        { return parent->asTYPESYM(); }  // parent is the base type.
    AGGTYPESYM * GetAts();
};


/*
 * PINNEDSYM - a symbol representing a pinned type
 *      used only to communicate between ilgen & emitter
 */
class PINNEDSYM: public TYPESYM
{
public:
    IS_A(PINNEDSYM)

    PTYPESYM baseType()
        { return parent->asTYPESYM(); }  // parent is the base type.

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};

/*
 * VOIDSYM - represents the type "void".
 */
class VOIDSYM: public TYPESYM
{
public:
    IS_A(VOIDSYM)

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(NULLSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};

/*
 * NULLSYM - represents the null type -- the type of the "null constant".
 */
class NULLSYM: public TYPESYM
{
public:
    IS_A(NULLSYM)

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(ERRORSYM)
    NOT_A(AGGTYPESYM)
};

/***************************************************************************************************
    UNITSYM - a placeholder typesym used only in type argument lists for open types. There is
    exactly one of these.
***************************************************************************************************/
class UNITSYM: public TYPESYM
{
};

/***************************************************************************************************
    ANONMETHSYM - a placeholder typesym used only as the type of an anonymous method expression.
    There is exactly one of these.
***************************************************************************************************/
class ANONMETHSYM: public TYPESYM
{
};

/***************************************************************************************************
    METHGRPSYM - a placeholder typesym used only as the type of an method groupe expression.
    There is exactly one of these.
***************************************************************************************************/
class METHGRPSYM: public TYPESYM
{
};

/*
 * METHPROPSYM - abstract class representing a method or a property. There
 * are a bunch of algorithms in the compiler (e.g., override and overload resolution)
 * that want to treat methods and properties the same. This abstract base class
 * has the common parts. 
 *
 * Changed to a PARENTSYM to allow generic methods to parent their type
 * variables.
 */
class METHPROPSYM: public PARENTSYM {
    DECLARE_DEP_STR

public:
    IS_A(METHPROPSYM)

    uint modOptCount: 16;              // number of CMOD_OPTs in signature and return type

    uint hasParamsDefined: 1;       // params have been defined
    uint isStatic: 1;               // Static member?
    uint isOverride: 1;             // Overrides an inherited member. Only valid if isVirtual is set.
                                    // false implies that a new vtable slot is required for this method.

    uint isUnsafe: 1;               // member is unsafe (either marked as such, or is in an unsafe context)

    uint useMethInstead: 1;         // Only valid iff isBogus == TRUE && isPROPSYM().
                                    // If this is true then tell the user to call the accessors directly.

    uint isOperator: 1;             // a user defined operator (or default indexed property)

    uint isParamArray: 1;           // new style varargs

    uint isHideByName: 1;           // this property hides all below it regardless of signature

    uint fNeedsMethodImp: 1;        // Needs a method impl to swtSlot.

    // This indicates the base member that this member overrides or implements.
    // For an explicit interface member implementation, this is the interface member (and type)
    // that the member implements. For an override member, this is the base member that is
    // being overridden. This is not affected by implicit interface member implementation.
    // If this symbol is a property and an explicit interface member implementation, the swtSlot
    // may be an event. This is filled in during prepare.
    SymWithType swtSlot;

    ERRORSYM * errExpImpl;          // If name == NULL but swtExpImpl couldn't be resolved, this contains error information.

    mdToken     tokenImport;        // Meta-data token for imported method.

    mdToken     tokenEmit;          // Metadata token (memberRef or memberDef) in the current output file.

    PTYPESYM    retType;            // Return type.
    TypeArray * params;             // array of cParams parameter types.

    BASENODE *  parseTree;          // Valid only between define & prepare stages...

    AGGDECLSYM * declaration;       // containing declaration

    AGGSYM *    getClass() const { return parent->asAGGSYM(); }

    // Explicit interface member implementations are recognized by having no name.
    bool IsExpImpl() { return !name; }

    AGGDECLSYM * containingDeclaration() { return declaration; }

    void copyInto(METHPROPSYM * mpsDst, AGGTYPESYM * typeSrc, COMPILER * compiler);

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};
typedef METHPROPSYM * PMETHPROPSYM;

// At most one of these per SCOPESYM. The lifetime of this is limited to the rewrite phase.
// During this time the AnonMethInfo list is stable.
struct AnonScopeInfo {
    AnonMethInfo * pamiFirst;
    AGGSYM * aggHoist;   // The compiler generated class (for locals and instance methods)
    EXPRLOCAL * exprLoc; // a pointer to the "$local" local for this scope
};

namespace MethodKind { enum _Enum {
    None = 0,
    Ctor = 1,             // Ctor or static ctor
    Dtor = 2,
    PropAccessor = 3,
    EventAccessor = 4,
    ExplicitConv = 5,     // Explicit user defined conversion
    ImplicitConv = 6,     // Implicit user defined conversion
    Anonymous = 7,
    Invoke = 8,           // Invoke method of a delegate type
}; };
DECLARE_ENUM_TYPE(MethodKind);

/*
 * METHSYM - a symbol representing a method. Parent is a struct, interface
 * or class (aggregate). No children.
 */
class METHSYM: public METHPROPSYM {
public:
    IS_A(METHSYM)

    uint isExternal : 1;            // Has external definition.
    uint isSysNative :1;            // Has definition implemented by the runtime.
    uint isVirtual: 1;              // Virtual member?
    uint isMetadataVirtual: 1;      // Marked as virtual in the metadata (if mdVirtual + mdSealed, this will be true, but isVirtual will be false).
    uint isAbstract: 1;             // Abstract method?
    uint isIfaceImpl: 1;            // is really a IFACEIMPLMETHSYM
    uint checkedCondSymbols: 1;     // conditionalSymbols already includes parent symbols if override
    uint hasLinkDemand: 1;          // has a security attribute of type link demand
    uint isVarargs: 1;              // has varargs
    uint isNewSlot: 1;              // had or needs mdNewSlot bit set

#if USAGEHACK
    uint isUsed: 1;
#endif

private:
    uint methKind : 5; // An extra bit to prevent sign-extension

public:
    MethodKindEnum MethKind() const
        { return (MethodKindEnum)methKind; }
    bool isCtor() const             // Is a constructor or static constructor (depending on isStatic).
        { return methKind == MethodKind::Ctor; }
    bool isDtor() const             // Is a destructor
        { return methKind == MethodKind::Dtor; }
    bool isPropertyAccessor() const // true if this method is a property set or get method
        { return methKind == MethodKind::PropAccessor; }
    bool isEventAccessor() const    // true if this method is an event add/remove method 
        { return methKind == MethodKind::EventAccessor; }
    bool isExplicit() const         // is user defined explicit conversion operator
        { return methKind == MethodKind::ExplicitConv; }
    bool isImplicit() const         // is user defined implicit conversion operator
        { return methKind == MethodKind::ImplicitConv; }
    bool isAnonymous() const        // is an Anonymous Method
        { return methKind == MethodKind::Anonymous; }
    bool isInvoke() const           // Invoke method on a delegate - isn't user callable
        { return methKind == MethodKind::Invoke; }
    void SetMethKind(MethodKindEnum mk) {
        ASSERT(methKind == MethodKind::None || methKind == (uint)mk);
        methKind = mk;
    }

    enum CanInferState { cisMaybe = 0, cisYes = 1, cisNo = 2 };
    uint cisCanInfer: 3;   // Set to cisYes if all type variables are used in the method signature so inferencing might work.
                                    // Set to cisNo if either it has no type variables or not all type variables are used so inferencing will never work.
                                    // Set to cisMaybe if we haven't set this yet.
                                    // We use three bits for this so we don't have sign extension issues.

    TypeArray * typeVars;           // All the type variables for a generic method, as declarations.
    mdToken * toksEmitTypeVars;     // The tokens of the emitted type variables.

    NAMELIST *  conditionalSymbols; // set if a conditional symbols for method

private:
    union {
        METHSYM * m_convNext; // For linked list of conversion operators.
        PROPSYM * m_prop;     // For property accessors, this is the PROPSYM.
        EVENTSYM * m_evt;     // For event accessors, this is the EVENTSYM.
    };

public:
    METHSYM * ConvNext() {
        ASSERT(isImplicit() || isExplicit());
        return m_convNext;
    }
    void SetConvNext(METHSYM * conv) {
        ASSERT(isImplicit() || isExplicit());
        ASSERT(!conv || conv->isImplicit() || conv->isExplicit());
        m_convNext = conv;
    }
    PROPSYM *getProperty() {
        ASSERT(isPropertyAccessor());
        return m_prop;
    }
    void SetProperty(PROPSYM * prop) {
        ASSERT(isPropertyAccessor());
        m_prop = prop;
    }
    EVENTSYM *getEvent() {
        ASSERT(isEventAccessor());
        return m_evt;
    }
    void SetEvent(EVENTSYM * evt) {
        ASSERT(isEventAccessor());
        m_evt = evt;
    }

    bool isCompilerGeneratedCtor();

    // returns true if accessor is a get accessor. Only valid if isPropertyAcessor is true.
    bool isGetAccessor();
    bool isConversionOperator() { return (isExplicit() || isImplicit()); }
    bool isUserCallable()       { return !isOperator && !isAnyAccessor(); }
    bool isAnyAccessor()        { return isPropertyAccessor() || isEventAccessor(); }
    IFACEIMPLMETHSYM *asIFACEIMPLMETHSYM() { ASSERT(isIfaceImpl); return (IFACEIMPLMETHSYM *)this; }

    void copyInto(METHSYM * methDst, AGGTYPESYM * typeSrc, COMPILER * compiler) {
        METHPROPSYM::copyInto(methDst, typeSrc, compiler);
        methDst->isVirtual = isVirtual;
        methDst->isMetadataVirtual = isMetadataVirtual;
        methDst->isAbstract = isAbstract;
        methDst->isVarargs = isVarargs;
        methDst->methKind = (methKind == MethodKind::Ctor) ? MethodKind::Ctor : MethodKind::None;
        methDst->typeVars = typeVars;
    }

    BASENODE *  getAttributesNode();

    NOT_A(PROPSYM)
};

// Used for varargs.
class FAKEMETHSYM: public METHSYM {
public:
    IS_A(FAKEMETHSYM)

    METHSYM * parentMethSym;

    NOT_A(IFACEIMPLMETHSYM)
};

/*
 * an explicit method impl generated by the compiler
 * usef for CMOD_OPT interop
 */
class IFACEIMPLMETHSYM : public METHSYM {
public:
    IS_A(IFACEIMPLMETHSYM)

    METHSYM * implMethod;

    NOT_A(FAKEMETHSYM)
};

/* 
 * PARAMINFO - Additional information about a parameter symbol that isn't
 * needed by other code binding against this method. This structure
 * lives only when this particular method is being compiled.
 */
class PARAMINFO
{
private:
    NAME * name;

public:
    void SetName(NAME * name) {
        ASSERT(!this->name || this->name == name);
        this->name = name;
    }
    NAME * Name() { return name; }

    // Corresponding parse tree node if there is one.
    PARAMETERNODE * node;
    BASENODE * nodeAttr;

    // COM classic attributes
    bool isIn:1;
    bool isOut:1;
    bool isParamArray:1;

    mdToken         tokenEmit;
};

class IterInfo
{
public:
    AGGSYM * aggIter;       // This is the compiler generated class that implements everything
    EXPRSTMT * disposeBody; // This is where we hang the dispose method body statements
    bool fGeneric;          // Does the class implement the generic interfaces
    bool fEnumerable;       // Does the class implement the IEnumerable pattern or just the IEnumerator pattern
};

/* 
 * METHINFO - Additional information about an method symbol that isn't
 * needed by other code binding against this method. This structure
 * lives only when this particular method is being compiled.
 */
class METHINFO
{
public:
    // Typically a METHINFO is allocated with a STACK_ALLOC_ZERO(byte, Size(params->size)).
    static int Size(int cpin) { return sizeof(METHINFO) + sizeof(PARAMINFO) * (cpin - 1); }

    void InitFromIterInfo(METHINFO * infoSrc, METHSYM * meth, int cpinMax) {
        memset(this, 0, Size(cpinMax));

        this->outerScope = infoSrc->outerScope;
        this->piin = infoSrc->piin;
        this->hasYieldAsLeave = infoSrc->hasYieldAsLeave;
        this->yieldType = infoSrc->yieldType;
        this->meth = meth;
    }
    bool IsIterator() {
        return yieldType != NULL;
    }

    METHSYM * meth;
    SCOPESYM * outerScope;              // The arg scope of this method, if any...
    IterInfo * piin;                    // This is shared among multiple METHINFOs - hence the indirection.
    TYPESYM * yieldType;
    AnonMethInfo * pamiFirst;           // The forest of anonymous method info structs.

    EXPRSTMTNODE * nodeRet;             // First return node found in the method.
    bool isMagicImpl;                   // This is a "magic" method with run-time supplied implementation:
                                        //   e.g.: delegate Invoke or delegate ctor.

    bool hasRetAsLeave;                 // has a return inside of a try or catch
    bool hasYieldAsLeave;               // has a yield inside a try/finally

    bool noDebugInfo;                   // Don't generate debug information. Used for compiler-created methods.
    bool debuggerHidden;                // Should emit DebuggerHiddenAttribute (should also set noDebugInfo)

    bool isSynchronized;                // synchronized bit (only used for event accessor's; not settable from code).

    BASENODE * unsafeTree;              // First occurance of unsafe type or code block

    // The parse tree for the params (if there is one). This may not cover all of the params
    // (e.g. for a set accessor or a BeginInvoke method on a delegate type), or it may contain
    // some non-params (e.g. for an EndInvoke).
    BASENODE * nodeParams;

    // The attributes for the "method". This may come from a non-method node (eg delegate).
    BASENODE * nodeAttr;

    PARAMINFO returnValueInfo;

    int cpin; // number of PARAMINFOs. If meth is a old-style varargs, this will be one less than meth->params->size.
    PARAMINFO rgpin[1]; // Must be last!
};


/*
 * PROPSYM - a symbol representing a property. Parent is a struct, interface
 * or class (aggregate). No children.
 */
class PROPSYM: public METHPROPSYM {
public:
    IS_A(PROPSYM)

    bool        isEvent : 1;        // This field is the implementation for an event.
    bool        fHadAttributeError : 1;   // There was an error binding parameter attributes on the first accessor.  
                                          // If this flag is set we won't attempt to bind attributes on the second accessor.


    METHSYM    *methGet;            // Getter method (always has same parent)
    METHSYM    *methSet;            // Setter method (always has same parent)

    bool        isIndexer()     { return isOperator; }
    class INDEXERSYM * asINDEXERSYM() { ASSERT(isIndexer()); return (class INDEXERSYM*)this; }
    NAME *      getRealName();
    void copyInto(PROPSYM * propDst, AGGTYPESYM * typeSrc, COMPILER * compiler) {
        METHPROPSYM::copyInto(propDst, typeSrc, compiler);
    }
    EVENTSYM *getEvent(class BSYMMGR *);   // returns event. Only valid to call if isEvent is true
    BASENODE *getAttributesNode();

    NOT_A(METHSYM)
        NOT_A(FAKEMETHSYM)
};
typedef PROPSYM * PPROPSYM;

/*
 * INDEXERSYM - a symbol representing an indexed property. Parent is a struct, interface
 * or class (aggregate). No children.
 *
 * Has kind == SK_PROPSYM.
 */
class INDEXERSYM: public PROPSYM
{
public:
    IS_A(INDEXERSYM)

    NAME *      realName;       // the 'real' name of the indexer. All indexers have the same name.
};

/* 
 * PROPINFO - Additional information about an property symbol that isn't
 * needed by other code binding against this method. This structure
 * lives only when this particular method is being compiled.
 */
class PROPINFO
{
public:
    PARAMINFO * paramInfos;          // Parameter name info (has PROPSYM::cParams elements)
};
typedef PROPINFO * PPROPINFO;

/* 
 * VARSYM - a symbol representing a variable. Specific subclasses are 
 * used - MEMBVARSYM for member variables, LOCVARSYM for local variables
 * and formal parameters, 
 */
class VARSYM: public SYM {
public:
    IS_A(VARSYM)

    PTYPESYM    type;                       // Type of the field.

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
};
typedef VARSYM * PVARSYM;


/*
 * MEMBVARSYM - a symbol representing a member variable of a class. Parent
 * is a struct or class.
 */
class MEMBVARSYM: public VARSYM {
    DECLARE_DEP_STR

public:
    IS_A(MEMBVARSYM)

    bool isStatic: 1;               // Static member?
    bool isConst: 1;                // Is a compile-time constant; see constVal for value.
    bool isReadOnly : 1;            // Can only be changed from within constructor.
    bool isEvent : 1;               // This field is the implementation for an event.
    bool isVolatile : 1;            // This fields is marked volatile

    bool isUnevaled: 1;             // This has an unevaluated constant value
    bool isReferenced:1;            // Has this been referenced by the user?
    bool isAssigned:1;              // Has this ever been assigned by the user?
    bool isUnsafe: 1;               // member is unsafe (either marked as such, or is in an unsafe context)
    bool isHoistedParameter: 1;     // Member has 'original' copy of hoisted parameter

    // Set if the field's ibit (for definite assignment checking) varies depending on the generic
    // instantiation of the containing type. For example:
    //    struct S<T> { T x; int y; }
    // The ibit value for y depends on what T is bound to. For S<Point>, y's ibit is 2. For S<int>, y's
    // ibit is 1. This flag is set the first time a calculated ibit for the member is found to not
    // match the return result of GetIbitInst().
    bool fIbitVaries: 1;

    CONSTVAL constVal;              // If isConst is set, a constant value.
                                    // If fixedAgg is non-NULL, the constant of the fixed buffer length

    struct BASENODE * parseTree;    // parse tree, could be a VARDECLNODE or a ENUMMEMBRNODE

    mdToken tokenImport;            // Meta-data token for imported variable.

    mdToken tokenEmit;              // Metadata token (memberRef or memberDef) in the current output file.

    int iIteratorLocal;             // index into iterator local array
private:
    union
    {
        // 1-based bit index for definite assignment. Zero means it hasn't been set yet.
        // This is applicable only when used within the instance type - not other generic
        // instantiations.
        int jbit;

        MEMBVARSYM *membPreviousEnumerator; // used for enumerator values only
                                            // pointer to previous enumerator in enum declaration
        MEMBVARSYM *membOriginalCopy;       // pointer to member holding the original copy of the hoisted parameter
                                            // only valid in fabircated classes when isHoistedParameter is set
    };

public:


    AGGDECLSYM * declaration;           // containing declaration
    AGGSYM *    fixedAgg;               // This is the nested struct the compiler creates for fixed sized buffers

    AGGSYM *    getClass() const { return parent->asAGGSYM(); }
    BASENODE *  getBaseExprTree();    // returns the base of the expression tree for this initializer
    BASENODE *  getConstExprTree();   // returns the constant expression tree(after the =) or null
    EVENTSYM *  getEvent(class BSYMMGR *);   // returns event. Only valid to call if isEvent is true
    BASENODE *  getAttributesNode();
    AGGDECLSYM * containingDeclaration() { return declaration; }

    // Ibit is the bit offset for this field within its parent instance type.
    void SetIbitInst(int ibit) { 
        ASSERT(getClass()->IsStruct());
        ASSERT(!jbit);
        ASSERT(ibit >= 0);
        this->jbit = ibit + 1;
    }
    int GetIbitInst() {
        ASSERT(jbit > 0);
        return jbit - 1;
    }

    MEMBVARSYM * GetPreviousEnumerator() { 
        ASSERT(getClass()->IsEnum());
        return membPreviousEnumerator; 
    }
    void SetPreviousEnumerator(MEMBVARSYM * membPreviousEnumerator) { 
        ASSERT(getClass()->IsEnum());
        this->membPreviousEnumerator = membPreviousEnumerator; 
    }

    MEMBVARSYM * GetOriginalCopy() { 
        ASSERT(getClass()->isFabricated && isHoistedParameter && !getClass()->IsStruct() && !getClass()->IsEnum());
        return membOriginalCopy; 
    }
    void SetOriginalCopy(MEMBVARSYM * membOriginalCopy) { 
        ASSERT(getClass()->isFabricated && !getClass()->IsStruct() && !getClass()->IsEnum()); 
        isHoistedParameter = true; 
        this->membOriginalCopy = membOriginalCopy; 
    }
};

/*
 * MEMBVARINFO - Additional information about an meber variable symbol that isn't
 * needed by other code binding against this variable. This structure
 * lives only when this particular variable is being compiled.
 */
class MEMBVARINFO
{
public:
    bool        foundOffset;
};
typedef MEMBVARINFO * PMEMBVARINFO;

/* 
 * EVENTSYM - a symbol representing an event. The symbol points to the AddOn and RemoveOn methods
 * that handle adding and removing delegates to the event. If the event wasn't imported, it
 * also points to the "implementation" of the event -- a field or property symbol that is always
 * private.
 */
class EVENTSYM: public SYM
{
    DECLARE_DEP_STR

public:
    IS_A(EVENTSYM)

    bool        isStatic: 1;        // Static member?

    bool useMethInstead: 1;         // Only valid iff isBogus == TRUE.
                                    // If this is true then tell the user to call the accessors directly.

    bool isUnsafe: 1;               // event is unsafe (either marked as such, or is in an unsafe context)
    bool isOverride: 1;

    PTYPESYM    type;               // Type of the event.

    METHSYM    *methAdd;            // Adder method (always has same parent)
    METHSYM    *methRemove;         // Remover method (always has same parent)

    SYM        *implementation;     // underlying field or property that implements the event.

    // For an explicit impl, this is the base event we're implementing. For an override, it's the
    // base virtual/abstract event.
    EventWithType ewtSlot;
    ERRORSYM * errExpImpl;          // For an explicit impl, if ewtSlot couldn't be resolved, this contains error information.

    mdToken     tokenImport;        // Meta-data token for imported event.
    mdToken     tokenEmit;          // Metadata token (memberRef or memberDef) in the current output file.

    struct BASENODE * parseTree;    // parse tree, could be a VARDECLNODE or a PROPDECLNODE

    AGGDECLSYM * declaration;       // containing declaration

    AGGSYM *    getClass() const { return parent->asAGGSYM(); }
    BASENODE * getAttributesNode();
    unsigned int getParseFlags();

    bool IsExpImpl() { return !name; }

    AGGDECLSYM * containingDeclaration() { return declaration; }

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
    NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
};
typedef EVENTSYM * PEVENTSYM;

/* 
 * EVENTINFO - Additional information about an event symbol that isn't needed by other code
 * binding against this event. This structure lives only when this particular variable is
 * being compiled.
 */
class EVENTINFO
{
public:
};
typedef EVENTINFO * PEVENTINFO;


//////////////////////////////////////////////////////////////////////////////////////////////

enum TEMP_KIND
{
    TK_SHORTLIVED,
    TK_RETURN,
    TK_LOCK,
    TK_USING,
    TK_DURABLE,
    TK_FOREACH_GETENUM,
    TK_FOREACH_ARRAY,
    TK_FOREACH_ARRAYINDEX_0,       
    // NOTE: this must be the last one. 
    // NOTE: additional kinds are created based on the rank of the foreached array
    TK_FOREACH_ARRAYLIMIT_0 = TK_FOREACH_ARRAYINDEX_0 + 256,
    // NOTE: OK, I lied. we need two extendible kinds of temps, so limit the arrayindexes in EnC to 256
    TK_FIXED_STRING_0 = TK_FOREACH_ARRAYLIMIT_0 + 256,
};

class LOCSLOTINFO {
public:
    PTYPESYM type;
private:
    uint islot; // Used for definite assignment tracking and again (independently) for code gen
    bool hasIndex:1;  // Is islot a valid slot #
    bool fUsed:1;
public:
    bool isParam:1;
    bool isRefParam:1; // also set if outparam...
    bool isTemporary:1;
    bool hasInit:1;
    bool isReferenced:1;
    bool isReferencedAssg:1;
    bool mustBePinned:1; // this is set when the variable is declared
    bool isPinned:1; // and this is set when it is first assigned to
    bool aliasPossible:1; // cannot be verifed to be unaliased
    // The following fields apply to temporaries only:
    bool isTaken:1;
    TEMP_KIND tempKind;
#if DEBUG
    PCSTR lastFile;
    unsigned lastLine;
#endif

    void SetJbitDefAssg(int jbit) {
        ASSERT(!hasIndex);
        islot = jbit;
    }
    int JbitDefAssg() {
        ASSERT(!hasIndex);
        return islot;
    }
    void SetUsed(bool fUsed) {
        this->fUsed = fUsed;
    }
    bool IsUsed() {
        return fUsed;
    }
    void SetIndex(uint islot) {
        this->islot = islot;
        this->hasIndex = true;
    }
    bool HasIndex() {
        return hasIndex;
    }
    uint Index() {
        ASSERT(hasIndex || !islot);
        return islot;
    }
};


/*
 * GLOBALATTRSYM - a symbol representing a global attribute on an assembly or module
 */
class GLOBALATTRSYM : public SYM
{
public:
    IS_A(GLOBALATTRSYM)

    struct BASENODE *   parseTree;
    CorAttributeTargets elementKind;
    GLOBALATTRSYM *     nextAttr;

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(XMLFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
        NOT_A(MISCSYM)
    NOT_A(EVENTSYM)
};

/*
 * LOCVARSYM - a symbol representing a local variable or parameter. Parent
 * is a scope.
 */
class LOCVARSYM: public VARSYM {
public:
    IS_A(LOCVARSYM)

    LOCSLOTINFO slot;
    int isLockOrDisposeTargetCount;

    bool isConst : 1;
    bool isNonWriteable : 1;    // used for catch variables, and fixed variables
    bool isThis : 1;            // Is this the one and only <this> pointer?
    bool fIsIteratorLocal : 1;  // This local has been hoisted for an interator
                                // movedToField should have iIteratorLocal set appropriately
    bool fIsCompilerGenerated : 1; // The local is compiler generated and has a managled name
    bool fUsedInAnonMeth : 1;   // Set if the local is ever used in an anon method
    bool fHoistForAnonMeth : 1; // Set if the local is used in an anon method and the anon method should be emitted

    // For better lvalue errors
    bool isForeach : 1;
    bool isUsing : 1;
    bool isFixed : 1;
    bool isCatch : 1;

    CONSTVAL constVal;
    POSDATA firstUsed; // line of decl...
    struct BBLOCK * debugBlockFirstUsed;  // If debug info on: IL location of first use
    unsigned        debugOffsetFirstUsed;
    struct BASENODE * declTree;
    MEMBVARSYM * movedToField;   // Indicates the field that a local was hoisted to
    struct BASENODE * nodeAddrTaken;      // The first place where this local had it's address taken (for error reporting)
    struct BASENODE * nodeAnonMethUse;    // The first place where this local was used inside an anonymous method (for error reporting)
    SCOPESYM * declarationScope() { return parent->asSCOPESYM(); }

    bool IsAssumedPinned() { ASSERT(nodeAnonMethUse == NULL || nodeAddrTaken == NULL); return (nodeAddrTaken != NULL); }

    NOT_A(MEMBVARSYM)
};

// Flags which any scope may have
enum SCOPEFLAGS {
    SF_NONE                =   0x00,
    SF_CATCHSCOPE          =   0x01,
    SF_TRYSCOPE            =   0x02,
    SF_SWITCHSCOPE         =   0x04,
    SF_FINALLYSCOPE        =   0x08,
    SF_DELEGATESCOPE       =   0x10,
    SF_KINDMASK            =   0x1F,
    SF_ARGSCOPE            =   0x40,  // special scope for base (or this) call args
    SF_HASVARS             =   0x80,  // this, or a child scope, has locals of interest
    SF_LAZYFINALLY         =  0x200,  // this finally does little work in its finally clause (always terminates quickly)
    SF_HASYIELDBREAK       =  0x400,  // has a yield break; statement (set and used during rewrite)
    SF_HASYIELDRETURN      =  0x800,  // has a yield return <expr>; statement (set during post bind, set and used during rewrite)
    SF_DISPOSESCOPE        = 0x2000,  // Scope has been copied (at least partially) into dispose method of iterators
};

/*
 * SCOPESYM - a symbol represent a scope that holds other symbols. Typically
 * unnamed.
 */
class SCOPESYM: public PARENTSYM {
public:
    IS_A(SCOPESYM)

    unsigned nestingOrder;  // the nesting order of this scopes. outermost == 0
private:
    union {
        class EXPRBLOCK * block; // the associated block... (not for try scopes)
        SCOPESYM * finallyScope; // for try scopes only...
    };
public:
    AnonScopeInfo * pasi;
    BASENODE * tree; // last statement in this scope...
    struct BBLOCK * debugBlockStart;  // If debug info on: location of first IL instruction in scope
    struct BBLOCK * debugBlockEnd;    // If debug info on: location of first IL instruction after scope
    unsigned        debugOffsetStart;
    unsigned        debugOffsetEnd;
    int scopeFlags;

    void SetBlock(EXPRBLOCK* in) 
    { 
        ASSERT(!GetFinallyScope());
        ASSERT(!(scopeFlags & SF_TRYSCOPE));
        block = in;
    }

    void SetFinallyScope(SCOPESYM* in)
    {
        ASSERT(!GetBlock());
        ASSERT(scopeFlags & SF_TRYSCOPE);
        finallyScope = in;
    }

    EXPRBLOCK* GetBlock()
    { 
        return (scopeFlags & SF_TRYSCOPE) ? NULL : block; 
    }

    SCOPESYM* GetFinallyScope()
    { 
        return (scopeFlags & SF_TRYSCOPE) ? finallyScope : NULL; 
    }


    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(MISCSYM)
};


class ANONSCOPESYM : public SYM {
public:
    SCOPESYM * scope;
};


/*
 * ERRORSYM - a symbol representing an error that has been reported.
 */
class ERRORSYM: public TYPESYM {
public:
    IS_A(ERRORSYM)

    NAME * nameText;
    TypeArray * typeArgs;
    mdToken tokenEmit;     // Type ref - doesn't include type args and parent.
    mdToken tokenEmitSpec; // Type spec - includes type args and parent.

    NOT_A(TYVARSYM)
    NOT_A(ARRAYSYM)
    NOT_A(PTRSYM)
    NOT_A(PINNEDSYM)
    NOT_A(PARAMMODSYM)
    NOT_A(VOIDSYM)
    NOT_A(NULLSYM)
    NOT_A(AGGTYPESYM)
};

/*
 * XMLFILESYM - a symbol representing an XML file that has been included
 * via the <include> element in a DocComment
 */
class XMLFILESYM: public SYM {
public:
    IS_A(XMLFILESYM)


    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
        NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
};

class SORTEDLIST {
public:
    SYM** symlist; // This list is NULL terminated
};



class MISCSYM : public SYM {
public:
    IS_A(MISCSYM)

    enum TYPE {
    };

    void Init(TYPE typ) {
        miscKind = typ;
    }
    TYPE miscKind;


private:
public:

    NOT_A(NSSYM)
    NOT_A(NSDECLSYM)
    NOT_A(INFILESYM)
        NOT_A(SYNTHINFILESYM)
    NOT_A(OUTFILESYM)
    NOT_A(AGGDECLSYM)
    NOT_A(AGGSYM)
    NOT_A(TYPESYM)
        NOT_A(TYVARSYM)
        NOT_A(ARRAYSYM)
        NOT_A(PTRSYM)
        NOT_A(PINNEDSYM)
        NOT_A(PARAMMODSYM)
        NOT_A(VOIDSYM)
        NOT_A(NULLSYM)
        NOT_A(ERRORSYM)
        NOT_A(AGGTYPESYM)
    NOT_A(METHPROPSYM)
        NOT_A(METHSYM)
            NOT_A(FAKEMETHSYM)
        NOT_A(PROPSYM)
    NOT_A(VARSYM)
        NOT_A(MEMBVARSYM)
        NOT_A(LOCVARSYM)
    NOT_A(SCOPESYM)
};


/*
 * EXTERNALISSYM - a symbol representing an alias for a referenced file
 */
class EXTERNALIASSYM: public SYM {
public:
    IS_A(EXTERNALIASSYM)

    int aid;
    NSAIDSYM * nsa; // Cache of "Foo::" (global namespace in aid). 

    SYMLIST * infileList;
    SYMLIST ** infileListTail;

    // The assembly ids of all contained assemblies.
    BitSet bsetAssemblies;

    int GetAssemblyID() { return aid; }

    // Doesn't have an in-source representation, so prevent some bad calls
    // returns parse tree for classes, and class members
    void            getParseTree() {}
    void            getInputFile() {}
    void            GetModule() const {}
    void            GetMetaImport(COMPILER *compiler) {}
    void            containingDeclaration() {}
    void            getAttributesNode() {}
    void            isContainedInDeprecated() const {}
    void            IsVirtual() {}
    void            IsOverride() {}

    NOT_A(CACHESYM)
    NOT_A(LABELSYM)
    NOT_A(ALIASSYM)
    NOT_A(RESFILESYM)
    NOT_A(PARENTSYM)
        NOT_A(NSSYM)
        NOT_A(NSDECLSYM)
        NOT_A(INFILESYM)
            NOT_A(SYNTHINFILESYM)
        NOT_A(OUTFILESYM)
        NOT_A(AGGDECLSYM)
        NOT_A(AGGSYM)
        NOT_A(TYPESYM)
            NOT_A(TYVARSYM)
            NOT_A(ARRAYSYM)
            NOT_A(PTRSYM)
            NOT_A(PINNEDSYM)
            NOT_A(PARAMMODSYM)
            NOT_A(VOIDSYM)
            NOT_A(NULLSYM)
            NOT_A(ERRORSYM)
            NOT_A(AGGTYPESYM)
        NOT_A(METHPROPSYM)
            NOT_A(METHSYM)
                NOT_A(FAKEMETHSYM)
            NOT_A(PROPSYM)
                NOT_A(FAKEPROPSYM)
        NOT_A(VARSYM)
            NOT_A(MEMBVARSYM)
            NOT_A(LOCVARSYM)
        NOT_A(SCOPESYM)
        NOT_A(MISCSYM)
    NOT_A(GLOBALATTRSYM)
    NOT_A(EVENTSYM)
    NOT_A(XMLFILESYM)
};


/*
 * We have member functions here to do casts that, in DEBUG, check the 
 * symbol kind to make sure it is right. For example, the casting method
 * for METHODSYM is called "asMETHODSYM". In retail builds, these 
 * methods optimize away to nothing.
 */

// Define all the concrete kinds here.
#define SYMBOLDEF(k, global, local) \
    __forceinline bool SYM::is ## k () {   \
        return (this->kind == SK_ ## k);  \
    }
#define SYMBOLDEF_EXTRA(kind, global, local)
#include "symkinds.h"

// Define all the concrete kinds here.
#define SYMBOLDEF(k, global, local) \
    __forceinline k * SYM::as ## k () {   \
        RETAILVERIFY(this == NULL || this->is ## k());  \
        return static_cast<k *>(this);     \
    }
#define SYMBOLDEF_EXTRA(kind, global, local)
#include "symkinds.h"

// Define the symbol casting functions for the abstract symbol kinds.

__forceinline bool SYM::isBAGSYM()
{
    return this->isAGGSYM() || this->isNSSYM();
}

__forceinline BAGSYM * SYM::asBAGSYM() 
{
    RETAILVERIFY(this == NULL || this->isBAGSYM());
    return static_cast<BAGSYM *>(this);
}

__forceinline bool SYM::isDECLSYM()
{
    return this->isAGGDECLSYM() || this->isNSDECLSYM();
}

__forceinline DECLSYM * SYM::asDECLSYM() 
{
    RETAILVERIFY(this == NULL || this->isDECLSYM());
    return static_cast<DECLSYM *>(this);
}

__forceinline bool SYM::isTYPESYM()
{
    switch (this->getKind()) {
    case SK_AGGTYPESYM:
    case SK_ARRAYSYM:
    case SK_VOIDSYM:
    case SK_PARAMMODSYM:
    case SK_TYVARSYM:
    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_NULLSYM:
    case SK_ERRORSYM:
    case SK_MODOPTTYPESYM:
    case SK_ANONMETHSYM:
    case SK_METHGRPSYM:
    case SK_UNITSYM:
        return true;
    default:
        return false;
    }
}

__forceinline TYPESYM * SYM::asTYPESYM() 
{
    RETAILVERIFY(this == NULL || this->isTYPESYM());
    return static_cast<TYPESYM *>(this);
}

__forceinline bool SYM::isVARSYM()
{
    return
        this->isMEMBVARSYM() || 
        this->isLOCVARSYM();
}

__forceinline VARSYM * SYM::asVARSYM() 
{
    RETAILVERIFY(this == NULL || this->isVARSYM());
    return static_cast<VARSYM *>(this);
}

__forceinline bool SYM::isMETHPROPSYM()
{
    return
        this->isMETHSYM() || 
        this->isPROPSYM() ||
        this->isFAKEMETHSYM();
}

__forceinline METHPROPSYM * SYM::asMETHPROPSYM() 
{
    RETAILVERIFY(this == NULL || this->isMETHPROPSYM());
    return static_cast<METHPROPSYM *>(this);
}

__forceinline bool SYM::isFMETHSYM()
{
    return
        this->isMETHSYM() || 
        this->isFAKEMETHSYM();
}

__forceinline METHSYM * SYM::asFMETHSYM()
{
    RETAILVERIFY(this == NULL || this->isFMETHSYM());
    return static_cast<METHSYM *>(this);
}

__forceinline bool SYM::isANYINFILESYM()
{
    return
        this->isINFILESYM() || 
        this->isSYNTHINFILESYM();
}

__forceinline INFILESYM * SYM::asANYINFILESYM()
{
    RETAILVERIFY(this == NULL || this->isANYINFILESYM());
    return static_cast<INFILESYM *>(this);
}

__forceinline bool SYM::isPARENTSYM()
{
    return
        this->isBAGSYM() ||
        this->isDECLSYM() ||
        this->isTYPESYM() ||
        this->isNSAIDSYM() ||
        this->isSCOPESYM() ||
        this->isOUTFILESYM() ||
        this->isARRAYSYM() ||
        this->isMETHSYM() ||
        this->isFAKEMETHSYM();
}

__forceinline PARENTSYM * SYM::asPARENTSYM() 
{
    RETAILVERIFY(this == NULL || this->isPARENTSYM());
    return static_cast<PARENTSYM *>(this);
}

__forceinline bool BAGSYM::InAlias(COMPILER * comp, int aid)
{
    ASSERT(this->isAGGSYM() || this->isNSSYM());
    switch (this->getKind()) {
    case SK_AGGSYM:
        return this->asAGGSYM()->InAlias(aid);
    case SK_NSSYM:
        return this->asNSSYM()->InAlias(comp, aid);
    default:
        RETAILVERIFY(false);
        return false;
    }
}

__forceinline bool TYPESYM::isStructOrEnum()
{
    return (isAGGTYPESYM() && (getAggregate()->IsStruct() || getAggregate()->IsEnum())) || isNUBSYM();
}

__forceinline bool TYPESYM::isEnumType()
{
    return (isAGGTYPESYM() && getAggregate()->IsEnum());
}

__forceinline bool TYPESYM::IsRefType()
{
    switch (this->getKind()) {
    case SK_ARRAYSYM:
    case SK_NULLSYM:
        return true;
    case SK_TYVARSYM:
        return this->asTYVARSYM()->IsRefType();
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsRefType();
    default:
        return false;
    }
}

__forceinline bool TYPESYM::IsRefTypeInVerifier()
{
    // NOTE: type variables are never considered reference types by the Verifier
    switch (this->getKind()) {
    case SK_ARRAYSYM:
    case SK_NULLSYM:
        return true;
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsRefType();
    default:
        return false;
    }
}

__forceinline bool TYPESYM::IsValType()
{
    switch (this->getKind()) {
    case SK_TYVARSYM:
        return this->asTYVARSYM()->IsValType();
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsValueType();
    case SK_NUBSYM:
        return true;
    default:
        return false;
    }
}

__forceinline bool TYPESYM::IsNonNubValType()
{
    switch (this->getKind()) {
    case SK_TYVARSYM:
        return this->asTYVARSYM()->IsNonNubValType();
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsValueType();
    case SK_NUBSYM:
        return false;
    default:
        return false;
    }
}

__forceinline AGGTYPESYM *TYPESYM::underlyingEnumType()
{
    ASSERT(isEnumType());
    return getAggregate()->underlyingType;
}

__forceinline bool TYPESYM::isPredefined()
{
    return this->isAGGTYPESYM() && this->getAggregate()->isPredefined;
}

__forceinline PREDEFTYPE TYPESYM::getPredefType()
{
    ASSERT(isPredefined());
    return (PREDEFTYPE) this->getAggregate()->iPredef;
}

__forceinline bool TYPESYM::isGenericInstance()
{
    return isAGGTYPESYM() && getAggregate()->typeVarsThis->size != 0;
}

__forceinline bool TYPESYM::isSecurityAttribute()
{
    return isAGGTYPESYM() && getAggregate()->isSecurityAttribute;
}

__forceinline AGGSYM * TYPESYM::getAggregate()
{
    ASSERT(isAGGTYPESYM());
    return asAGGTYPESYM()->getAggregate();
}

inline bool TYPESYM::isStaticClass()
{
    if (!this)
        return false;

    AGGSYM * agg = this->GetNakedAgg();
    if (!agg)
        return false;
    
    if (!agg->IsStatic())
        return false;

    return true;
}

inline bool SYM::hasExternalAccess()
{
    ASSERT(parent || isNSSYM() || isNSDECLSYM());
    if (isAGGDECLSYM())
        return this->asAGGDECLSYM()->Agg()->hasExternalAccess();

    return isNSSYM() || isNSDECLSYM() || access >= ACC_PROTECTED && parent->hasExternalAccess();
}

inline bool SYM::hasExternalOrFriendAccess()
{
    ASSERT(parent || isNSSYM() || isNSDECLSYM());
    if (isAGGDECLSYM())
        return this->asAGGDECLSYM()->Agg()->hasExternalOrFriendAccess();

    return isNSSYM() || isNSDECLSYM() || access >= ACC_INTERNAL && parent->hasExternalOrFriendAccess();
}

__forceinline TYPESYM * TYPESYM::underlyingType() 
{
    if (this->isAGGTYPESYM() && getAggregate()->IsEnum()) return getAggregate()->underlyingType;
    return this;
}

__forceinline bool TYPESYM::isUnsafe()
{
    // Pointer types are the only unsafe types.
    // Note that generics may not be instantiated with pointer types
    return (this != NULL && (this->isPTRSYM() || (this->isARRAYSYM() && this->asARRAYSYM()->elementType()->isUnsafe())));
}

__forceinline bool TYPESYM::isFabricated()
{
    return this != NULL && this->isAGGTYPESYM() && this->getAggregate()->isFabricated;
}

__forceinline AGGTYPESYM * TYPESYM::asATSorNUBSYM() {
    if (isNUBSYM()) {
        return asNUBSYM()->GetAts();
    } else {
        return asAGGTYPESYM();
    }
}


__forceinline NAME *PROPSYM::getRealName()
{
    if (isIndexer()) {
        return this->asINDEXERSYM()->realName;
    } else {
        return name;
    }
}

__forceinline AGGSYM * AGGSYM::GetBaseAgg()
{
    return !baseClass ? NULL : baseClass->getAggregate();
}

__forceinline AGGSYM * TYPESYM::GetNakedAgg(bool fStripNub)
{
    TYPESYM * type = GetNakedType(fStripNub);
    if (type && type->isAGGTYPESYM())
        return type->asAGGTYPESYM()->getAggregate();
    return NULL;
}

__forceinline NSDECLSYM * DECLSYM::GetNsDecl()
{
    for (DECLSYM * decl = this; ; decl = decl->DeclPar()) {
        if (decl->isNSDECLSYM())
            return decl->asNSDECLSYM();
    }
}

__forceinline int OUTFILESYM::GetModuleID()
{
    ASSERT(firstInfile()->GetAssemblyID() == kaidThisAssembly);
    return aid;
}


// nb. includes the input type
#define BASE_CLASS_TYPES_LOOP(typ, baseTypeId) { AGGTYPESYM *tmp_type = typ; while (tmp_type) { AGGTYPESYM *baseTypeId = tmp_type; {
#define END_BASE_CLASS_TYPES_LOOP } tmp_type = tmp_type->GetBaseClass(); } }

// nb. includes the input type
#define BASE_CLASSES_LOOP(typ, baseClassId) { AGGSYM *tmp_agg = typ; while (tmp_agg) { AGGSYM *baseClassId = tmp_agg; {
#define END_BASE_CLASSES_LOOP } tmp_agg = tmp_agg->GetBaseAgg(); } }

#endif //__symbol_h__

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
//
// siginfo.hpp
//
#ifndef _H_TYPECTXT
#define _H_TYPECTXT

//------------------------------------------------------------------------
// A signature type context gives the information necessary to interpret 
// the ELEMENET_TYPE_VAR and ELEMENT_TYPE_MVAR elements of a regular
// metadata signature.  These are usually stack allocated at appropriate
// points where the SigPointer objects are created, or are allocated
// inside a MetaSig (which are themselves normally stack allocated)
//
// They are normally passed as "const SigTypeContext *".
//------------------------------------------------------------------------

class SigTypeContext
{
public:
    DWORD m_classInstCount;
    TypeHandle *m_classInst;
    DWORD m_methInstCount;
    TypeHandle *m_methInst;

    // Default constructor for non-generic code
    inline SigTypeContext()
    { WRAPPER_CONTRACT; InitTypeContext(this); }


    // Initialize a type context given instantiations.
    inline SigTypeContext(DWORD classInstCount,TypeHandle *classInst,DWORD methInstCount,TypeHandle *methInst)
    { WRAPPER_CONTRACT; InitTypeContext(classInstCount,classInst,methInstCount,methInst,this); }


    // Initialize a type context from a MethodDesc.  If this is a MethodDesc that gets
    // shared between generic instantiations (e.g. one being jitted by a code-sharing JIT)
    // and a null declaring Type is passed then the type context will 
    // be a representative context, not an exact one.
    // This is sufficient for most purposes, e.g. GC and field layout, because
    // these operations are "parametric", i.e. behave the same for all shared types.
    //
    // If declaringType is non-null, then the MethodDesc is assumed to be
    // shared between generic classes, and the type handle is used to give the 
    // exact type context.  The method should be one of the methods supported by the
    // given type handle.
    //
    // If the method is a method in an array type then the type context will
    // contain one item in the class instantiation corresponding to the 
    // element type of the array.
    //
    // Finally, exactMethodInst should be specified if md might represent a generic method definition,
    // as type parameters are not always available off the method desc for generic method definitions without
    // forcing a load. Typically the caller will use MethodDesc::LoadMethodInstantiation.
    inline SigTypeContext(MethodDesc *md)                 
    { WRAPPER_CONTRACT; InitTypeContext(md,this); }

    inline SigTypeContext(MethodDesc *md, TypeHandle declaringType)                               
    { WRAPPER_CONTRACT; InitTypeContext(md,declaringType,this); }

    inline SigTypeContext(MethodDesc *md, TypeHandle declaringType, TypeHandle *exactMethodInst)
    { WRAPPER_CONTRACT; InitTypeContext(md,declaringType,exactMethodInst,this); }

    // This is similar to the one above except that exact 
    // instantiations are provided explicitly.
    // This will only normally be used when the code is shared
    // between generic instantiations and after fetching the 
    // exact instantiations from the stack.
    //
    inline SigTypeContext(MethodDesc *md, TypeHandle *exactClassInst, TypeHandle *exactMethodInst) 
    { WRAPPER_CONTRACT; InitTypeContext(md,exactClassInst,exactMethodInst,this); }

    // Initialize a type context from a type handle.  This is used when 
    // generating the type context for a
    // any of the metadata in the class covered by the type handle apart from
    // the metadata for any generic methods in the class.
    // If the type handle satisfies th.IsNull() then the created type context
    // will be empty.
    inline SigTypeContext(TypeHandle th)
    { WRAPPER_CONTRACT; InitTypeContext(th,this); }

    inline SigTypeContext(FieldDesc *pFD, TypeHandle declaringType = TypeHandle())
    { WRAPPER_CONTRACT; InitTypeContext(pFD,declaringType,this); }

    // Copy contructor - try not to use this.  The C++ compiler is not doing a good job
    // of copy-constructor based code, and we've had perf regressions when using this too 
    // much for this simple objects.  Use an explicit call to InitTypeContext instead,
    // or use GetOptionalTypeContext.
    inline SigTypeContext(const SigTypeContext &c)
    { WRAPPER_CONTRACT; InitTypeContext(&c,this); }

    // Copy contructor from a possibly-NULL pointer.
    inline SigTypeContext(const SigTypeContext *c)
    { WRAPPER_CONTRACT; InitTypeContext(c,this); }

    static void InitTypeContext(MethodDesc *md, SigTypeContext *pRes);
    static void InitTypeContext(MethodDesc *md, TypeHandle declaringType, SigTypeContext *pRes);
    static void InitTypeContext(MethodDesc *md, TypeHandle declaringType, TypeHandle *exactMethodInst, SigTypeContext *pRes);
    static void InitTypeContext(MethodDesc *md, TypeHandle *exactClassInst, TypeHandle *exactMethodInst, SigTypeContext *pRes);
    static void InitTypeContext(TypeHandle th, SigTypeContext *pRes);
    static void InitTypeContext(FieldDesc *pFD, TypeHandle declaringType, SigTypeContext *pRes);
    inline static void InitTypeContext(DWORD classInstCount,TypeHandle *classInst,DWORD methInstCount,TypeHandle *methInst, SigTypeContext *pRes);
    inline static void InitTypeContext(SigTypeContext *);
    inline static void InitTypeContext(const SigTypeContext *c, SigTypeContext *pRes);

    // These are allowed to return NULL if an empty type context is generated.  The NULL value
    // can then be passed around to represent the empty type context.
    // pRes must be non-null.
    // pRes must initially be zero-initialized, e.g. by the default SigTypeContext constructor.
    static const SigTypeContext * GetOptionalTypeContext(MethodDesc *md, TypeHandle declaringType, SigTypeContext *pRes);
    static const SigTypeContext * GetOptionalTypeContext(TypeHandle th, SigTypeContext *pRes);

    // SigTypeContexts are used as part of keys for various data structures indiexed by instantiation
    static BOOL Equal(const SigTypeContext *pCtx1, const SigTypeContext *pCtx2);
    BOOL IsNull() const { LEAF_CONTRACT; return m_classInstCount == 0 && m_methInstCount == 0; }

};

inline void SigTypeContext::InitTypeContext(SigTypeContext *pRes)
{
    LEAF_CONTRACT;
    pRes->m_classInstCount = 0;
    pRes->m_classInst = NULL;
    pRes->m_methInstCount = 0;
    pRes->m_methInst = NULL;
}

inline void SigTypeContext::InitTypeContext(DWORD classInstCount,
                                            TypeHandle *classInst,
                                            DWORD methInstCount,
                                            TypeHandle *methInst,
                                            SigTypeContext *pRes)
{
    LEAF_CONTRACT;
    pRes->m_classInstCount = classInstCount;
    pRes->m_classInst = classInst;
    pRes->m_methInstCount = methInstCount;
    pRes->m_methInst = methInst;
}


// Copy contructor from a possibly-NULL pointer.
inline void SigTypeContext::InitTypeContext(const SigTypeContext *c,SigTypeContext *pRes)
{
    LEAF_CONTRACT;
    if (c)
    {
        pRes->m_classInstCount = c->m_classInstCount;
        pRes->m_classInst = c->m_classInst;
        pRes->m_methInstCount = c->m_methInstCount;
        pRes->m_methInst = c->m_methInst;
    }
    else
    {
        pRes->m_classInstCount = 0;
        pRes->m_classInst = NULL;
        pRes->m_methInstCount = 0;
        pRes->m_methInst = NULL;
    }
}


#endif

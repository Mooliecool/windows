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
#include "excep.h"
#include "comvariant.h"
#include "olevariant.h"
#include "comdatetime.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "fieldmarshaler.h"
#include "ml.h"
#include "stublink.h"
#include "mlgen.h"
#include "comdelegate.h"
#include "mlinfo.h"
#include "marshalnative.h"
#include "log.h"
#include "comvarargs.h"
#include "frames.h"
#include "util.hpp"
#include "interoputil.h"
#include "dllimport.h"
#include "wrappers.h"

//  Marshaler
//  |
//  |-- ReferenceMarshaler
//  |   |-- BSTRMarshaler
//  |   |-- WSTRMarshaler
//  |   |-- CSTRMarshaler
//  |   |-- AnsiBSTRMarshaler
//  |   |-- WSTRBufferMarshaler
//  |   |-- CSTRBufferMarshaler
//  |   |-- InterfaceMarshaler
//  |   |-- SafeArrayMarshaler
//  |   |-- NativeArrayMarshaler
//  |   |-- AsAnyAMarshaler
//  |   |-- AsAnyWMarshaler
//  |   |-- DelegateMarshaler
//  |   |-- BlittablePtrMarshaler
//  |   |-- LayoutClassPtrMarshaler
//  |
//  |-- StandardMarshaler
//  |   |-- WinBoolMarshaler
//  |   |-- VtBoolMarshaler
//  |   |-- CBoolMarshaler
//  |   |-- AnsiCharMarshaler
//  |   |-- CurrencyMarshaler
//  |   |-- OleColorMarshaler
//  |   |-- ValueClassPtrMarshaler
//  |   |-- DateMarshaler
//  |   |-- ArgIteratorMarshaler
//  |
//  |-- CopyMarshaler
//  |   |-- FloatMarshaler
//  |   |-- DoubleMarshaler
//  |
//  |-- CustomMarshaler
//  |   |-- ReferenceCustomMarshaler
//  |
//  |-- ObjectMarshaler
//  |-- VBByValStrMarshaler
//  |-- VBByValStrWMarshaler
//  |-- ArrayWithOffsetMarshaler
//  |-- BlittableValueClassMarshaler
//  |-- BlittableValueClassWithCopyCtorMarshaler
//  |-- ValueClassMarshaler
//  |-- HandleRefMarshaler

/* ------------------------------------------------------------------------- *
 * Marshalers
 * ------------------------------------------------------------------------- */

class Marshaler
{
  public:
    Marshaler(CleanupWorkList *pList,
              BYTE nativeSize, BYTE CLRSize,
              BYTE fReturnsNativeByref, BYTE fReturnsCLRByref) :
        m_cbCLR(StackElemSize(CLRSize)),
        m_cbNative(StackElemSize(nativeSize)),
        m_fReturnsCLRByref(fReturnsCLRByref),
        m_fReturnsNativeByref(fReturnsNativeByref),
        m_pList(pList),
        m_pMarshalerCleanupNode(NULL)
    {
        LEAF_CONTRACT;
    }

    //
    // A marshaler should be created in space local to the stack frame being
    // marshaled.  Marshal() is called before the call is made, and Unmarshal()
    // afterward.  (Or Prereturn() and Return() for return values.)
    //
    // Note that marshalers rely on static overriding & template instantiation,
    // rather than virtual functions.  This is strictly for reasons of efficiency,
    // since the subclass-specialized functions are typically very simple
    // functions that ought to be inlined.
    // (Actually a really smart compiler ought to be able to generate similar code
    // from virtual functions, but our compiler is not that smart.)
    //
    // NOTE ON NAMING:  (this is a bit confusing...)
    //   "Native" means unmanaged - e.g. classic COM
    //   "CLR" means managed.
    //
    // Definitions:
    //
    // "SPACE" & "CONTENTS"
    // A value being marshaled is divided into 2 levels - its "space" and
    // its "contents".
    // The distinction is made as follows:
    //      an "in" parameter has both valid space & contents.
    //      an "non-in" parameter has valid space, but not valid contents.
    //      a byref "non-in" parameter has neither valid space nor contents.
    //
    // For instance, for an array type, the space is the array itself,
    // while the contents is the elements in the array.
    //
    // Note that only container types have a useful definition "space" vs. "contents",
    // primitive types simply ignore the concept & just have contents.
    //
    // "HOME"
    // A marshaler has 2 "homes" - one for native values and one for managed values.
    // The main function of the marshaler is to copy values in and out of the homes,
    // and convert between the two homes for marshaling purposes.
    //
    // A home has 3 states:
    //      empty: empty
    //      allocated: valid space but not contents
    //      full: valid space & contents.
    //
    // In order to clean up after a marshaling, the output home should be
    // emptied.
    //
    // A marshaler also has a "dest" pointer, which is a copy of a byref parameter's
    // input destination.
    //
    // Marshalers also define 4 constants:
    //  c_nativeSize - size of the native value on the stack
    //  c_CLRSize - size of the native value on the stack
    //  c_fReturnsNativeByref - whether a native return value is byref or not
    //      (on x86 this means a pointer to the value is returned)
    //  c_fReturnsCLRByref - whether a CLR return value is byref or not
    //      (this means that the return value appears as a byref parameter)
    //  c_fArgNativeByref - native parameter is passed by reference
    //  c_fArgCLRByref - CLR parameter is passed by reference
    //

    //
    // Type primitives:
    //
    // These small routines form the primitive from which the marshaling
    // routines are built.  There are many of these routines, they
    // are mostly intended for use by the marshaling templates rather
    // than being called directly.  Because of the way the templates
    // are instantiated they will usually be inlined so the fact that
    // there are a lot of little routines won't be a performance hit.
    //

    //
    // InputStack : Copies a value from the stack into a home.
    //  START: the home is empty
    //  END: Home is allocated, but may or may not be full
    //      (depending on whether it's an [in] parameter)
    //

    void InputNativeStack(void *pStack)
    {
        LEAF_CONTRACT;
    }

    void InputCLRStack(void *pStack)
    {
        LEAF_CONTRACT;
    }

    //
    // InputRef : Copies the value referred to by the dest pointer into a home.
    //  START: the home is empty
    //  END: the home is full
    //
    void InputNativeRef(void *pStack)
    {
        LEAF_CONTRACT;
    }

    void InputCLRRef(void *pStack)
    {
        LEAF_CONTRACT;
    }


    //
    // InputDest : Copies a reference from the stack into the marshal's dest pointer.
    //
    void InputDest(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_pDest = *(void**)pStack;
    }

    //
    // InputCLRField
    // OutputCLRField : Copies the CLR home to and from the given object
    // field.
    //
    void InputCLRField(void *pField)
    {
        LEAF_CONTRACT;
        _ASSERTE(!"NYI");
    }

    void OutputCLRField(void *pField)
    {
        LEAF_CONTRACT;
        _ASSERTE(!"NYI");
    };

    //
    // ConvertSpace: Converts the "space" layer from one home to another.
    // Temp version used when native buffer exists only over the call.
    //  START: dest home is empty
    //  END: dest home is allocated
    //
    void ConvertSpaceNativeToCLR()
    {
        LEAF_CONTRACT;
    }

    void ConvertSpaceCLRToNative()
    {
        LEAF_CONTRACT;
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        LEAF_CONTRACT;
    }


    //
    // ConvertSpace: Converts the "contents" layer from one home to another
    //  START: dest home is allocated
    //  END: dest home is full
    //
    void ConvertContentsNativeToCLR()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsCLRToNative()
    {
        LEAF_CONTRACT;
    }


    //
    // ClearSpace: Clears the "space" and "contents" layer in a home.
    // Temp version used when native buffer exists only over the call.
    //  START: dest home is allocated
    //  END: dest home is empty
    //
    void ClearNative()
    {
        LEAF_CONTRACT;
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ClearCLR()
    {
        LEAF_CONTRACT;
    }


    // ReInitNative: Reinitializes the "space" to a safe value for deallocating
    // (normally a "NULL" pointer). Used to overwrite dangling pointers left
    // after a ClearNative(). Note that this method need not do anything if
    // the datatype isn't allocated or lacks allocated subparts.
    //   START: dest home is empty
    //   END:   dest home is full (and set to safe value)
    void ReInitNative()
    {
        LEAF_CONTRACT;
    }

    //
    // ClearContents: Clears the "contents" layer in a home.
    //  START: dest home is full
    //  END: dest home is allocated
    //
    void ClearNativeContents()
    {
        LEAF_CONTRACT;
    }

    void ClearCLRContents()
    {
        LEAF_CONTRACT;
    }

    //
    // OutputStack copies a home's value onto the stack, possibly
    // performing type promotion in the process.
    //  START: home is full
    //  END: home is empty
    //
    void OutputNativeStack(void *pStack)
    {
        LEAF_CONTRACT;
    }

    void OutputCLRStack(void *pStack)
    {
        LEAF_CONTRACT;
    }

    //
    // OutputRef copies a pointer to a home, onto the stack
    //
    void OutputNativeRef(void *pStack)
    {
        LEAF_CONTRACT;
    }

    void OutputCLRRef(void *pStack)
    {
        LEAF_CONTRACT;
    }

    //
    // OutputDest copies a home's value into the location pointed
    // to by the dest pointer.
    //  START: home is full
    //  END: home is empty
    //
    void OutputNativeDest()
    {
        LEAF_CONTRACT;
    }

    void OutputCLRDest()
    {
        LEAF_CONTRACT;
    }

    //
    // Virtual functions, to be overridden in leaf classes
    // (usually by a simple instantiation of each of the above templates.
    //
    // !!! this may be too much code - perhaps we should parameterize some
    // of these with in/out flags rather than having 3 separate routines.
    //

    virtual void GcScanRoots(promote_func *fn, ScanContext* sc)
    {
        LEAF_CONTRACT;
    }


    virtual size_t GetNativeToCLRSrcStackSize() = 0;
    virtual size_t GetNativeToCLRDestStackSize() = 0;
    virtual void MarshalNativeToCLR(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalNativeToCLROut(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalNativeToCLRByref(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalNativeToCLRByrefOut(void *pInStack, void *pOutStack) = 0;
    virtual void UnmarshalNativeToCLRIn() = 0;
    virtual void UnmarshalNativeToCLROut() = 0;
    virtual void UnmarshalNativeToCLRInOut() = 0;
    virtual void UnmarshalNativeToCLRByrefIn() = 0;
    virtual void UnmarshalNativeToCLRByrefOut() = 0;
    virtual void UnmarshalNativeToCLRByrefInOut() = 0;

    virtual size_t GetCLRToNativeSrcStackSize() = 0;
    virtual size_t GetCLRToNativeDestStackSize() = 0;
    virtual void MarshalCLRToNative(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalCLRToNativeOut(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalCLRToNativeByref(void *pInStack, void *pOutStack) = 0;
    virtual void MarshalCLRToNativeByrefOut(void *pInStack, void *pOutStack) = 0;
    virtual void UnmarshalCLRToNativeIn() = 0;
    virtual void UnmarshalCLRToNativeOut() = 0;
    virtual void UnmarshalCLRToNativeInOut() = 0;
    virtual void UnmarshalCLRToNativeByrefIn() = 0;
    virtual void UnmarshalCLRToNativeByrefOut() = 0;
    virtual void UnmarshalCLRToNativeByrefInOut() = 0;

    virtual void PrereturnNativeFromCLR(void *pInStack, void *pOutStack) = 0;
    virtual void PrereturnNativeFromCLRRetval(void *pInStack, void *pOutStack) = 0;
    virtual void ReturnNativeFromCLR(void *pInReturn, void *pOutReturn) = 0;
    virtual void ReturnNativeFromCLRRetval(void *pInReturn, void *pOutReturn) = 0;

    virtual void PrereturnCLRFromNative(void *pInStack, void *pOutStack) = 0;
    virtual void PrereturnCLRFromNativeRetval(void *pInStack, void *pOutStack) = 0;
    virtual void ReturnCLRFromNative(void *pInReturn, void *pOutReturn) = 0;
    virtual void ReturnCLRFromNativeRetval(void *pInReturn, void *pOutReturn) = 0;

    virtual void SetCLR(void *pInReturn, void *pField) = 0;

    virtual void PregetCLRRetval(void *pInStack, void *pField) = 0;

    virtual void DoExceptionCleanup() = 0;
    virtual void DoExceptionReInit() = 0;

    void CancelCleanup()
    {
        WRAPPER_CONTRACT;

        if (m_pMarshalerCleanupNode)
            m_pMarshalerCleanupNode->CancelCleanup();
    }

    // A marshaler can override this to override the normal ML code generation.
    // We use this mechanism for two purposes:
    //
    //  - To implement types such as PIS's "asany", which work in only one
    //    direction and doesn't fit into the normal marshaler scheme.
    //
    //  - To implement stack allocation & pinning optimizations for
    //    the CLR->Native calling case.
    //
    //
    // Returns:
    //   HANDLEASNORMAL, OVERRIDDEN or DISALLOWED
    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        LEAF_CONTRACT;
        return HANDLEASNORMAL;
    }


    // Similar to ArgumentMLOverride but for return values.
    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                     InteropStubLinker *pslPost,
                                                     BOOL        CLRToNative,
                                                     BOOL        fThruBuffer,
                                                     OverrideProcArgs *pargs,
                                                     UINT       *pResID)
    {
        LEAF_CONTRACT;
        return HANDLEASNORMAL;
    }


    BYTE                m_cbCLR;
    BYTE                m_cbNative;
    BYTE                m_fReturnsCLRByref;
    BYTE                m_fReturnsNativeByref;
    CleanupWorkList*    m_pList;
    void*               m_pDest;
    CleanupWorkList::MarshalerCleanupNode* m_pMarshalerCleanupNode;
};

//
// Templates:
// These templates build marshaling routines using the above primitives.
// They are typically instantiated in subclasses of marshaler to implement
// the virtual marshaling opcode routines.
//

template < class MARSHAL_BASE >
class DefaultMarshalOverrides : public MARSHAL_BASE
{
public:
    DefaultMarshalOverrides(CleanupWorkList *pList)
      : MARSHAL_BASE(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

    virtual size_t GetNativeToCLRSrcStackSize();
    virtual size_t GetNativeToCLRDestStackSize();
    virtual void MarshalNativeToCLR(void *pInStack, void *pOutStack);
    virtual void MarshalNativeToCLROut(void *pInStack, void *pOutStack);
    virtual void MarshalNativeToCLRByref(void *pInStack, void *pOutStack);
    virtual void MarshalNativeToCLRByrefOut(void *pInStack, void *pOutStack);
    virtual void UnmarshalNativeToCLRIn();
    virtual void UnmarshalNativeToCLROut();
    virtual void UnmarshalNativeToCLRInOut();
    virtual void UnmarshalNativeToCLRByrefIn();
    virtual void UnmarshalNativeToCLRByrefOut();
    virtual void UnmarshalNativeToCLRByrefInOut();

    virtual size_t GetCLRToNativeSrcStackSize();
    virtual size_t GetCLRToNativeDestStackSize();
    virtual void MarshalCLRToNative(void *pInStack, void *pOutStack);
    virtual void MarshalCLRToNativeOut(void *pInStack, void *pOutStack);
    virtual void MarshalCLRToNativeByref(void *pInStack, void *pOutStack);
    virtual void MarshalCLRToNativeByrefOut(void *pInStack, void *pOutStack);
    virtual void UnmarshalCLRToNativeIn();
    virtual void UnmarshalCLRToNativeOut();
    virtual void UnmarshalCLRToNativeInOut();
    virtual void UnmarshalCLRToNativeByrefIn();
    virtual void UnmarshalCLRToNativeByrefOut();
    virtual void UnmarshalCLRToNativeByrefInOut();

    virtual void PrereturnNativeFromCLR(void *pInStack, void *pOutStack);
    virtual void PrereturnNativeFromCLRRetval(void *pInStack, void *pOutStack);
    virtual void ReturnNativeFromCLR(void *pInReturn, void *pOutReturn);
    virtual void ReturnNativeFromCLRRetval(void *pInReturn, void *pOutReturn);

    virtual void PrereturnCLRFromNative(void *pInStack, void *pOutStack);
    virtual void PrereturnCLRFromNativeRetval(void *pInStack, void *pOutStack);
    virtual void ReturnCLRFromNative(void *pInReturn, void *pOutReturn);
    virtual void ReturnCLRFromNativeRetval(void *pInReturn, void *pOutReturn);

    virtual void SetCLR(void *pInReturn, void *pField);

    virtual void PregetCLRRetval(void *pInStack, void *pField);

    virtual void DoExceptionCleanup();
    virtual void DoExceptionReInit();
};

//
// Native to CLR Marshaling
//

template < class MARSHAL_BASE >
size_t DefaultMarshalOverrides<MARSHAL_BASE>::GetNativeToCLRSrcStackSize()
{
    LEAF_CONTRACT;

    if (c_fArgNativeByref)
        return sizeof(void*);
    else
        return m_cbNative;
}

template < class MARSHAL_BASE >
size_t DefaultMarshalOverrides<MARSHAL_BASE>::GetNativeToCLRDestStackSize()
{
    LEAF_CONTRACT;

    if (c_fArgCLRByref)
        return sizeof(void*);
    else
        return m_cbCLR;
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalNativeToCLR(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    if (c_fArgNativeByref)
        InputNativeRef(pInStack);
    else
        InputNativeStack(pInStack);

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();

    if (c_fArgCLRByref)
        OutputCLRRef(pOutStack);
    else
        OutputCLRStack(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalNativeToCLROut(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputNativeStack(pInStack);
    ConvertSpaceNativeToCLR();
    OutputCLRStack(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalNativeToCLRByref(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputDest(pInStack);

    if (!*(void**)pInStack)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

    InputNativeRef(pInStack);
    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();
    OutputCLRRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalNativeToCLRByrefOut(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputDest(pInStack);
    OutputCLRRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLRIn()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLROut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ConvertContentsCLRToNative();
    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLRInOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNativeContents();
    ConvertContentsCLRToNative();
    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLRByrefIn()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLRByrefInOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNative();
    ReInitNative();

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();
    OutputNativeDest();

    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalNativeToCLRByrefOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();
    OutputNativeDest();
    ClearCLR();
}

//
// CLR to Native marshaling
//

template < class MARSHAL_BASE >
size_t DefaultMarshalOverrides<MARSHAL_BASE>::GetCLRToNativeSrcStackSize()
{
    LEAF_CONTRACT;

    if (c_fArgCLRByref)
        return sizeof(void*);
    else
        return m_cbCLR;
}

template < class MARSHAL_BASE >
size_t DefaultMarshalOverrides<MARSHAL_BASE>::GetCLRToNativeDestStackSize()
{
    LEAF_CONTRACT;

    if (c_fArgNativeByref)
        return sizeof(void*);
    else
        return m_cbNative;
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNative(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    if (c_fArgCLRByref)
        InputCLRRef(pInStack);
    else
        InputCLRStack(pInStack);

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();

    if (c_fArgNativeByref)
        OutputNativeRef(pOutStack);
    else
        OutputNativeStack(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNativeOut(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputCLRStack(pInStack);
    ConvertSpaceCLRToNative();
    OutputNativeStack(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNativeByref(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputDest(pInStack);
    InputCLRRef(pInStack);
    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();
    OutputNativeRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNativeByrefOut(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputDest(pInStack);
    ReInitNative();
    if (m_pList)
        m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);

    OutputNativeRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeIn()
{
    CONTRACTL
{
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNative();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeOut()
{
    CONTRACTL
{
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ConvertContentsNativeToCLR();
    CancelCleanup();
    ClearNative();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeInOut()
{
    CONTRACTL
{
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLRContents();
    ConvertContentsNativeToCLR();
    CancelCleanup();
    ClearNative();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeByrefIn()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNative();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeByrefInOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLR();

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();

    CancelCleanup();
    ClearNative();

    OutputCLRDest();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeByrefOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();
    CancelCleanup();
    ClearNative();
    OutputCLRDest();
}

//
// Return Native from CLR
//

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::PrereturnNativeFromCLR(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    if (c_fReturnsCLRByref)
        OutputCLRRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::PrereturnNativeFromCLRRetval(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputDest(pInStack);
    if (c_fReturnsCLRByref)
        OutputCLRRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::ReturnNativeFromCLR(void *pInReturn, void *pOutReturn)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInReturn));
        PRECONDITION(CheckPointer(pOutReturn));
    }
    CONTRACTL_END;

    if (!c_fReturnsCLRByref)
        InputCLRStack(pOutReturn);

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();

    if (c_fReturnsNativeByref)
        OutputNativeRef(pInReturn);
    else
        OutputNativeStack(pInReturn);

    ClearCLR();
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::ReturnNativeFromCLRRetval(void *pInReturn, void *pOutReturn)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInReturn));
        PRECONDITION(CheckPointer(pOutReturn));
    }
    CONTRACTL_END;

    if (!c_fReturnsCLRByref)
        InputCLRStack(pOutReturn);

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();
    OutputNativeDest();
    ClearCLR();
}

//
// Return CLR from Native
//

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::PrereturnCLRFromNative(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack, NULL_OK));
    }
    CONTRACTL_END;

    if (c_fReturnsCLRByref)
        InputDest(pInStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::PrereturnCLRFromNativeRetval(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;

        // NULL from DoMLPrereturnC2N
        PRECONDITION(CheckPointer(pInStack, NULL_OK));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    if (c_fReturnsCLRByref)
        InputDest(pInStack);

    ReInitNative();
    if (m_pList)
    {
        m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
    }
    OutputNativeRef(pOutStack);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::ReturnCLRFromNative(void *pInReturn, void *pOutReturn)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInReturn));
        PRECONDITION(CheckPointer(pOutReturn));
    }
    CONTRACTL_END;

    if (c_fReturnsNativeByref)
    {
        if (!*(void**)pOutReturn)
            COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

        InputNativeRef(pOutReturn);
    }
    else
        InputNativeStack(pOutReturn);

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();

    CancelCleanup();
    ClearNative();

    if (c_fReturnsCLRByref)
        OutputCLRDest();
    else
        OutputCLRStack(pInReturn);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::ReturnCLRFromNativeRetval(void *pInReturn, void *pOutReturn)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInReturn));

        // NULL from DoMLReturnC2NRetVal
        PRECONDITION(CheckPointer(pOutReturn, NULL_OK));
    }
    CONTRACTL_END;

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();

    // ClearNative can trigger GC
    CancelCleanup();
    ClearNative();

    // No GC after this
    if (c_fReturnsCLRByref)
        OutputCLRDest();
    else
        OutputCLRStack(pInReturn);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::SetCLR(void *pInStack, void *pField)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pField));
    }
    CONTRACTL_END;

    InputNativeStack(pInStack);
    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();

    OutputCLRField(pField);
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::PregetCLRRetval(void *pInStack, void *pField)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pField));
    }
    CONTRACTL_END;

    InputDest(pInStack);

    InputCLRField(pField);

    ConvertSpaceCLRToNative();
    ConvertContentsCLRToNative();
    OutputNativeDest();
}


template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::DoExceptionCleanup()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearNative();
    ReInitNative(); // this is necessary in case the parameter
                    // was pass "[in,out] byref" - as the caller
                    // may still legally attempt cleanup on m_native.
}

template < class MARSHAL_BASE >
void DefaultMarshalOverrides<MARSHAL_BASE>::DoExceptionReInit()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ReInitNative(); // this is necessary in case the parameter
                    // was pass "[in,out] byref" - as the caller
                    // may still legally attempt cleanup on m_native.
}

// Alternative templates used when marshaling/unmarshaling from CLR to
// native and we wish to distinguish between native buffers allocated on a
// temporary basis as opposed to those given out permanently to native code.
//
// When marshaling/unmarshaling from CLR to native using a temporary native
// buffer we wish to avoid using the default marshaling overriddes and
// instead use some which distinguish between allocating native buffers
// which have unbounded lifetimes versus those which will exist for just the
// duration of the marshal/unmarshal (we can heavily optimize buffer
// allocation in the latter case). To this end we create two new helper
// functions, ConvertSpaceCLRToNativeTemp and ClearNativeTemp, which will
// perform the lightweight allocation/deallocation.

template < class MARSHAL_BASE >
class FastAllocMarshalOverrides : public DefaultMarshalOverrides<MARSHAL_BASE>
{
public:
    FastAllocMarshalOverrides(CleanupWorkList *pList)
      : DefaultMarshalOverrides<MARSHAL_BASE>(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

    virtual void MarshalCLRToNative(void *pInStack, void *pOutStack);
    virtual void MarshalCLRToNativeOut(void *pInStack, void *pOutStack);
    virtual void UnmarshalCLRToNativeIn();
    virtual void UnmarshalCLRToNativeOut();
    virtual void UnmarshalCLRToNativeInOut();
    virtual void UnmarshalCLRToNativeByrefIn();
    virtual void UnmarshalCLRToNativeByrefInOut();
};

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNative(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    if (c_fArgCLRByref)
        InputCLRRef(pInStack);
    else
        InputCLRStack(pInStack);

    ConvertSpaceCLRToNativeTemp();
    ConvertContentsCLRToNative();

    if (c_fArgNativeByref)
        OutputNativeRef(pOutStack);
    else
        OutputNativeStack(pOutStack);
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::MarshalCLRToNativeOut(void *pInStack, void *pOutStack)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pInStack));
        PRECONDITION(CheckPointer(pOutStack));
    }
    CONTRACTL_END;

    InputCLRStack(pInStack);
    ConvertSpaceCLRToNativeTemp();
    OutputNativeStack(pOutStack);
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeIn()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNativeTemp();
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeOut()
{
    CONTRACTL
{
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ConvertContentsNativeToCLR();
    CancelCleanup();
    ClearNativeTemp();
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeInOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLRContents();
    ConvertContentsNativeToCLR();
    CancelCleanup();
    ClearNativeTemp();
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeByrefIn()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    CancelCleanup();
    ClearNativeTemp();
}

template < class MARSHAL_BASE >
void FastAllocMarshalOverrides<MARSHAL_BASE>::UnmarshalCLRToNativeByrefInOut()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    ClearCLR();

    ConvertSpaceNativeToCLR();
    ConvertContentsNativeToCLR();
    OutputCLRDest();

    CancelCleanup();
    ClearNativeTemp();
}

//
// Macros to determine unnecessary unmarshaling
// conditions
//

#define NEEDS_UNMARSHAL_NATIVE_TO_CLR_IN(c) \
    c::c_fNeedsClearCLR

#define NEEDS_UNMARSHAL_NATIVE_TO_CLR_OUT(c) \
    (c::c_fNeedsConvertContents \
     || c::c_fNeedsClearCLR)

#define NEEDS_UNMARSHAL_NATIVE_TO_CLR_IN_OUT(c) \
    (c::c_fNeedsClearNativeContents \
     || c::c_fNeedsConvertContents \
     || c::c_fNeedsClearCLR)

#define NEEDS_UNMARSHAL_NATIVE_TO_CLR_BYREF_IN(c) \
    c::c_fNeedsClearCLR

#define NEEDS_UNMARSHAL_CLR_TO_NATIVE_IN(c) \
    c::c_fNeedsClearNative

#define NEEDS_UNMARSHAL_CLR_TO_NATIVE_OUT(c) \
    (c::c_fNeedsConvertContents \
     || c::c_fNeedsClearNative)

#define NEEDS_UNMARSHAL_CLR_TO_NATIVE_IN_OUT(c) \
    (c::c_fNeedsClearCLRContents \
     || c::c_fNeedsConvertContents \
     || c::c_fNeedsClearNative)

#define NEEDS_UNMARSHAL_CLR_TO_NATIVE_BYREF_IN(c) \
    c::c_fNeedsClearNative

#define NEEDS_UNMARSHAL_CLR_TO_NATIVE_BYREF_IN(c) \
    c::c_fNeedsClearNative


/* ------------------------------------------------------------------------- *
 * Primitive type marshallers
 * ------------------------------------------------------------------------- */

//
// CopyMarshal handles marshaling of primitive types (with
// compatible layouts.)
//

template < class ELEMENT, class PROMOTED_ELEMENT, BOOL RETURNS_CLR_BYREF >
class CopyMarshalerBase : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = sizeof(PROMOTED_ELEMENT),
        c_CLRSize = sizeof(PROMOTED_ELEMENT),

        c_fReturnsNativeByref = sizeof(ELEMENT)>8,
        c_fReturnsCLRByref = RETURNS_CLR_BYREF,

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        c_fArgNativeByref = sizeof(ELEMENT) > ENREGISTERED_PARAMTYPE_MAXSIZE,
        c_fArgCLRByref    = sizeof(ELEMENT) > ENREGISTERED_PARAMTYPE_MAXSIZE,
#else
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
#endif

        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,

        c_fInOnly = TRUE,
    };

    CopyMarshalerBase(CleanupWorkList *pList)
      : Marshaler(pList, c_nativeSize, c_CLRSize, c_fReturnsNativeByref, c_fReturnsCLRByref)
    {
        WRAPPER_CONTRACT;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_home = *(PROMOTED_ELEMENT*)pStack;
    }

    void InputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_home = *(PROMOTED_ELEMENT*)pStack;
    }

    void InputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(ELEMENT**)pStack));
        }
        CONTRACTL_END;

        m_home = **(ELEMENT**)pStack;
    }

    void InputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(ELEMENT**)pStack));
        }
        CONTRACTL_END;

        m_home = **(ELEMENT**)pStack;
    }

    void OutputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(PROMOTED_ELEMENT*)pStack = m_home;
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(PROMOTED_ELEMENT*)pStack = m_home;
    }

    void OutputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(ELEMENT **)pStack = &m_home;
    }

    void OutputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(ELEMENT **)pStack = &m_home;
    }

    void OutputNativeDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
        }
        CONTRACTL_END;

        *(ELEMENT *)m_pDest = m_home;
    }

    void OutputCLRDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
        }
        CONTRACTL_END;

        *(ELEMENT *)m_pDest = m_home;
    }

    void InputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        m_home = *(ELEMENT*)pField;
    }

    void OutputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        *(ELEMENT*)pField = m_home;
    }

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker*    psl,
                                                      InteropStubLinker*    pslPost,
                                                      BOOL                  byref,
                                                      BOOL                  fin,
                                                      BOOL                  fout,
                                                      BOOL                  CLRToNative,
                                                      OverrideProcArgs*     pargs,
                                                      UINT*                 pResID,
                                                      UINT                  argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;


        if (byref)
        {
            psl->MLEmit(ML_COPYPINNEDGCREF);
            return OVERRIDDEN;
        }
        else
        {
            return HANDLEASNORMAL;
        }
    }

    // We only need one home, since the types are identical.  This makes
    // conversion a noop

    ELEMENT m_home;
};

typedef DefaultMarshalOverrides< CopyMarshalerBase<INT8,INT_PTR,FALSE> > CopyMarshaler1;
typedef DefaultMarshalOverrides< CopyMarshalerBase<UINT8,UINT_PTR,FALSE> > CopyMarshalerU1;
typedef DefaultMarshalOverrides< CopyMarshalerBase<INT16,INT_PTR,FALSE> > CopyMarshaler2;
typedef DefaultMarshalOverrides< CopyMarshalerBase<UINT16,UINT_PTR,FALSE> > CopyMarshalerU2;
typedef DefaultMarshalOverrides< CopyMarshalerBase<INT32,INT_PTR,FALSE> > CopyMarshaler4;
typedef DefaultMarshalOverrides< CopyMarshalerBase<UINT32,INT_PTR,FALSE> > CopyMarshalerU4;
typedef DefaultMarshalOverrides< CopyMarshalerBase<INT64,INT64,FALSE> > CopyMarshaler8;
typedef DefaultMarshalOverrides< CopyMarshalerBase<DECIMAL,DECIMAL,TRUE> > DecimalMarshaler;
typedef DefaultMarshalOverrides< CopyMarshalerBase<GUID,GUID,TRUE> > GuidMarshaler;


typedef DefaultMarshalOverrides< CopyMarshalerBase<float, float, FALSE> > FloatMarshaler;
typedef DefaultMarshalOverrides< CopyMarshalerBase<double, double, FALSE> > DoubleMarshaler;



/* ------------------------------------------------------------------------- *
 * Standard Marshaler template (for when there is no stack promotion.)
 * ------------------------------------------------------------------------- */

template < class NATIVE_TYPE, class CLR_TYPE, BOOL RETURNS_NATIVE_BYREF, BOOL RETURNS_CLR_BYREF >
class StandardMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = sizeof(NATIVE_TYPE),
        c_CLRSize = sizeof(CLR_TYPE),
        c_fReturnsNativeByref = RETURNS_NATIVE_BYREF,
        c_fReturnsCLRByref = RETURNS_CLR_BYREF,

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        c_fArgNativeByref = sizeof(NATIVE_TYPE) > ENREGISTERED_PARAMTYPE_MAXSIZE,
        c_fArgCLRByref    = sizeof(CLR_TYPE)    > ENREGISTERED_PARAMTYPE_MAXSIZE,
#else
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
#endif
    };

    StandardMarshaler(CleanupWorkList *pList)
      : Marshaler(pList, c_nativeSize, c_CLRSize, c_fReturnsNativeByref, c_fReturnsCLRByref)
    {
        WRAPPER_CONTRACT;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_native = *(NATIVE_TYPE*)pStack;
    }

    void InputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_managed = *(CLR_TYPE*)pStack;
    }

    void InputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(NATIVE_TYPE**)pStack));
        }
        CONTRACTL_END;

        m_native = **(NATIVE_TYPE**)pStack;
    }

    void InputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(CLR_TYPE**)pStack));
        }
        CONTRACTL_END;

        m_managed = **(CLR_TYPE**)pStack;
    }

    void OutputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(NATIVE_TYPE*)pStack = m_native;
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(CLR_TYPE*)pStack = m_managed;
    }

    void OutputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(NATIVE_TYPE**)pStack = &m_native;
    }

    void OutputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(CLR_TYPE**)pStack = &m_managed;
    }

    void OutputNativeDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
        }
        CONTRACTL_END;

        *(NATIVE_TYPE*) m_pDest = m_native;
    }

    void OutputCLRDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
        }
        CONTRACTL_END;

        *(CLR_TYPE*) m_pDest = m_managed;
    }

    void InputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        m_managed = *(CLR_TYPE*)pField;
    }

    void OutputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        *(CLR_TYPE*)pField = m_managed;
    }

    NATIVE_TYPE     m_native;
    CLR_TYPE        m_managed;
};




/* ------------------------------------------------------------------------- *
 * WinBool marshaller (32-bit Win32 BOOL)
 * ------------------------------------------------------------------------- */

class WinBoolMarshalerBase : public StandardMarshaler<BOOL, INT8, FALSE, FALSE>
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    WinBoolMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<BOOL, INT8, FALSE, FALSE>(pList)
    {
        WRAPPER_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        LEAF_CONTRACT;
        m_managed = m_native ? 1 : 0;
    }

    void ConvertContentsCLRToNative()
    {
        LEAF_CONTRACT;
        m_native = (BOOL)m_managed;
    }

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;


        if (CLRToNative && !byref)
        {
#ifdef WRONGCALLINGCONVENTIONHACK
            psl->MLEmit(ML_COPY4);
#else
            psl->MLEmit(ML_COPYU1);
#endif
            return OVERRIDDEN;
        }
        else
        {
            return HANDLEASNORMAL;
        }
    }
};
typedef DefaultMarshalOverrides<WinBoolMarshalerBase> WinBoolMarshaler;




/* ------------------------------------------------------------------------- *
 * CBoolMarshaler marshaller (BYTE)
 * ------------------------------------------------------------------------- */
class CBoolMarshalerBase : public StandardMarshaler<BYTE, INT8, FALSE, FALSE>
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    CBoolMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<BYTE, INT8, FALSE, FALSE>(pList)
    {
        WRAPPER_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        LEAF_CONTRACT;
        m_managed = m_native ? 1 : 0;
    }

    void ConvertContentsCLRToNative()
    {
        LEAF_CONTRACT;
        m_native = m_managed ? 1 : 0;
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(StackElemType*)pStack = (StackElemType)m_managed;
    }
};
typedef DefaultMarshalOverrides<CBoolMarshalerBase> CBoolMarshaler;


/* ------------------------------------------------------------------------- *
 * AnsiChar marshaller
 * ------------------------------------------------------------------------- */

class AnsiCharMarshalerBase : public StandardMarshaler<UINT8, UINT16, FALSE, FALSE>
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    AnsiCharMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<UINT8, UINT16, FALSE, FALSE>(pList)
    {
        WRAPPER_CONTRACT;
        m_BestFitMap = TRUE;
        m_ThrowOnUnmappableChar = FALSE;
    }

    void ConvertContentsNativeToCLR()
    {
        WRAPPER_CONTRACT;
        
        MultiByteToWideChar(CP_ACP, 0, (LPSTR)&m_native, 1, (LPWSTR)&m_managed, 1);
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        InternalWideToAnsi((LPWSTR)&m_managed,
                           1,
                           (LPSTR)&m_native,
                           1,
                           m_BestFitMap,
                           m_ThrowOnUnmappableChar);
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(StackElemType*)pStack = (StackElemType)m_managed;
    }

    void SetBestFitMap(BOOL BestFit)
    {
        LEAF_CONTRACT;
        m_BestFitMap = BestFit;
    }

    void SetThrowOnUnmappableChar(BOOL ThrowOnUnmapChar)
    {
        LEAF_CONTRACT;
        m_ThrowOnUnmappableChar = ThrowOnUnmapChar;
    }

    BOOL  m_BestFitMap;
    BOOL  m_ThrowOnUnmappableChar;
};
typedef DefaultMarshalOverrides<AnsiCharMarshalerBase> AnsiCharMarshaler;


/* ------------------------------------------------------------------------- *
 * Currency marshaler.
 * ------------------------------------------------------------------------- */

class CurrencyMarshalerBase : public StandardMarshaler<CURRENCY, DECIMAL, TRUE, TRUE>
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    CurrencyMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<CURRENCY, DECIMAL, TRUE, TRUE>(pList)
    {
        WRAPPER_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        HRESULT hr = VarDecFromCy(m_native, &m_managed);
        IfFailThrow(hr);

        if (FAILED(DecimalCanonicalize(&m_managed)))
            COMPlusThrow(kOverflowException, L"Overflow_Currency");
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        HRESULT hr = VarCyFromDec(&m_managed, &m_native);
        IfFailThrow(hr);
    }
};
typedef DefaultMarshalOverrides<CurrencyMarshalerBase> CurrencyMarshaler;




/* ------------------------------------------------------------------------- *
 * Value class marshallers
 * ------------------------------------------------------------------------- */

//
// ValueClassPtrMarshal handles marshaling of value class types (with
// compatible layouts), which are represented on the native side by ptrs
// and in CLR by value.
//

template < class ELEMENT, BOOL RETURNS_CLR_BYREF >
class ValueClassPtrMarshalerBase : public StandardMarshaler<ELEMENT *, ELEMENT, FALSE, RETURNS_CLR_BYREF>
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    ValueClassPtrMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<ELEMENT *, ELEMENT, FALSE, RETURNS_CLR_BYREF>(pList)
    {
        WRAPPER_CONTRACT;
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        m_native = (ELEMENT *) CoTaskMemAlloc(sizeof(ELEMENT));
        if (m_native == NULL)
            ThrowOutOfMemory();

        m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        m_native = (ELEMENT *) GetThread()->m_MarshalAlloc.Alloc(sizeof(ELEMENT));
    }

    void ConvertContentsNativeToCLR()
    {
        LEAF_CONTRACT;

        if (m_native != NULL)
            m_managed = *m_native;
    }

    void ConvertContentsCLRToNative()
    {
        LEAF_CONTRACT;

        if (m_native != NULL)
            *m_native = m_managed;
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
            CoTaskMemFree(m_native);
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ReInitNative()
    {
        LEAF_CONTRACT;
        m_native = NULL;
    }
};

typedef ValueClassPtrMarshalerBase<DECIMAL, IA64_ONLY(FALSE) NOT_IA64(TRUE)> DecimalPtrMarshalerBase;
typedef ValueClassPtrMarshalerBase<GUID,    IA64_ONLY(FALSE) NOT_IA64(TRUE)> GuidPtrMarshalerBase;

typedef FastAllocMarshalOverrides< DecimalPtrMarshalerBase > DecimalPtrMarshaler;
typedef FastAllocMarshalOverrides< GuidPtrMarshalerBase > GuidPtrMarshaler;

/* ------------------------------------------------------------------------- *
 * Date marshallers
 * ------------------------------------------------------------------------- */

class DateMarshalerBase : public StandardMarshaler<DATE, INT64, FALSE, TRUE>
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    DateMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<DATE, INT64, FALSE, TRUE>(pList)
    {
        WRAPPER_CONTRACT;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        {
            m_native = *(DATE*)pStack;
        }
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        m_managed = (INT64)0;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        m_managed = COMDateTime::DoubleDateToTicks(m_native);
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        m_native = (double)0.0;
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        m_native = (double)0.0;        
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        m_native = COMDateTime::TicksToDoubleDate(m_managed);
    }

};
typedef DefaultMarshalOverrides<DateMarshalerBase> DateMarshaler;






/* ------------------------------------------------------------------------- *
 * Reference type abstract marshaler
 * ------------------------------------------------------------------------- */
class ReferenceMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = sizeof(void *),
        c_CLRSize = sizeof(OBJECTREF),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
    };

    ReferenceMarshaler(CleanupWorkList *pList)
      : Marshaler(pList, c_nativeSize, c_CLRSize, c_fReturnsNativeByref, c_fReturnsCLRByref)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
        
        m_managed = pList->NewProtectedObjectRef(NULL);
        m_native = NULL;
    }

    void InputDest(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_pDest = pStack;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_native = *(void **)pStack;
    }

    void InputCLRStack(void *pStack)
    {
        CONTRACTL
    {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(*(Object **)pStack), GetAppDomain());
    }

    void InputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(void**)pStack));
    }
        CONTRACTL_END;

        m_native = **(void ***)pStack;
    }

    void InputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(void**)pStack));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(**(Object ***)pStack), GetAppDomain());
    }

    void OutputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(void **)pStack = m_native;
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(OBJECTREF*)pStack = *m_managed;
    }

    void OutputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(void ***)pStack = &m_native;
    }

    void OutputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(OBJECTREF**)pStack = m_managed;
    }

    void OutputNativeDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
            PRECONDITION(CheckPointer(*(void**)m_pDest));
        }
        CONTRACTL_END;

        **(void ***)m_pDest = m_native;
    }

    void OutputCLRDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(m_pDest));
        }
        CONTRACTL_END;

        SetObjectReference(*(OBJECTREF**)m_pDest, *m_managed, GetAppDomain());
    }

    void InputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(*(Object**)pField), GetAppDomain());
    }

    void OutputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pField));
        }
        CONTRACTL_END;

        SetObjectReference((OBJECTREF*)pField, *m_managed, GetAppDomain());
    }

    void ReInitNative()
    {
        LEAF_CONTRACT;
        m_native = NULL;
    }

    OBJECTREF           *m_managed;
    void                *m_native;
};


/* ------------------------------------------------------------------------- *
 * String marshallers
 * ------------------------------------------------------------------------- */

class WSTRMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    WSTRMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            INT32 length = (INT32)wcslen((LPWSTR)m_native);
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            SetObjectReference(m_managed, (OBJECTREF) COMString::NewString(length), GetAppDomain());
        }
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            SIZE_T length = stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = (LPWSTR) CoTaskMemAlloc((length+1) * sizeof(WCHAR));
            if (m_native == NULL)
                ThrowOutOfMemory();

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
            CoTaskMemFree((LPWSTR)m_native);
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 length = (UINT32)stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = (LPWSTR) GetThread()->m_MarshalAlloc.Alloc((length+1) * sizeof(WCHAR));
        }
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        LPWSTR str = (LPWSTR) m_native;

        if (str != NULL)
        {
            STRINGREF stringRef = (STRINGREF) *m_managed;

            SIZE_T length = wcslen((LPWSTR) m_native) + 1;
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            memcpyNoGCRefs(stringRef->GetBuffer(), str, length * sizeof(WCHAR));
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef != NULL)
        {
            SIZE_T length = stringRef->GetStringLength() + 1;
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            memcpyNoGCRefs((LPWSTR) m_native, stringRef->GetBuffer(), length * sizeof(WCHAR));
        }
    }

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && !fout)
        {
            psl->MLEmit(ML_PINNEDUNISTR_C2N);
            return OVERRIDDEN;
        }
        else
        {
            return HANDLEASNORMAL;
        }
    }
};
typedef FastAllocMarshalOverrides<WSTRMarshalerBase> WSTRMarshaler;


class CSTRMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    CSTRMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        m_BestFitMap = TRUE;
        m_ThrowOnUnmappableChar = FALSE;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            // The length returned by MultiByteToWideChar includes the null terminator
            // so we need to substract one to obtain the length of the actual string.
            UINT32 length = (UINT32)MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
                                                       (LPSTR)m_native, -1,
                                                        NULL, 0) - 1;
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            if (length == ((UINT32)(-1)))
                COMPlusThrowWin32();

            SetObjectReference(m_managed, (OBJECTREF) COMString::NewString(length), GetAppDomain());
        }
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 length = (UINT32)stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = CoTaskMemAlloc((length * GetMaxDBCSCharByteSize()) + 1);
            if (m_native == NULL)
                ThrowOutOfMemory();

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 length = (UINT32)stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = GetThread()->m_MarshalAlloc.Alloc((length+3) * GetMaxDBCSCharByteSize());
            if (m_native == NULL)
                ThrowOutOfMemory();

            ((char*)m_native)[length+2] = '\0';
            
#ifdef _DEBUG
            FillMemory(m_native, (length+1)*sizeof(char), 0xcc);
#endif
        }
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
            CoTaskMemFree(m_native);
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        LPSTR str = (LPSTR) m_native;

        if (str != NULL)
        {
            STRINGREF stringRef = (STRINGREF) *m_managed;

            UINT32 length = (UINT32)stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            length++;

            if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPSTR) m_native, -1,
                                    stringRef->GetBuffer(), length) == 0)
            {
                COMPlusThrowWin32();
            }
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGREF stringRef = (STRINGREF) *m_managed;

        if (stringRef != NULL)
        {
            UINT32 length = (UINT32)stringRef->GetStringLength();
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            DWORD mblength = 0;
            mblength = InternalWideToAnsi(stringRef->GetBuffer(),
                                          length,
                                          (LPSTR)m_native,
                                          (length * GetMaxDBCSCharByteSize()) + 1,
                                          m_BestFitMap,
                                          m_ThrowOnUnmappableChar);

            ((CHAR*)m_native)[mblength] = '\0';
        }
    }


    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && !fout)
        {
            psl->MLEmit(ML_CSTR_C2N);
            psl->Emit8(pargs->m_pMarshalInfo->GetBestFitMapping());
            psl->Emit8(pargs->m_pMarshalInfo->GetThrowOnUnmappableChar());
            psl->MLNewLocal(sizeof(ML_CSTR_C2N_SR));
            return OVERRIDDEN;
        }
        else
        {
            return HANDLEASNORMAL;
        }
    }

    void SetBestFitMap(BOOL BestFit)
    {
        LEAF_CONTRACT;
        m_BestFitMap = BestFit;
    }

    void SetThrowOnUnmappableChar(BOOL ThrowOnUnmapChar)
    {
        LEAF_CONTRACT;
        m_ThrowOnUnmappableChar = ThrowOnUnmapChar;
    }

    BOOL  m_BestFitMap;
    BOOL  m_ThrowOnUnmappableChar;
};
typedef FastAllocMarshalOverrides<CSTRMarshalerBase> CSTRMarshaler;



/* ------------------------------------------------------------------------- *
 * StringBuffer marshallers
 * ------------------------------------------------------------------------- */

class WSTRBufferMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    WSTRBufferMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            UINT32 length = (UINT32)wcslen((LPWSTR)m_native);
            SetObjectReference(m_managed, (OBJECTREF) COMStringBuffer::NewStringBuffer(length), GetAppDomain());
        }
    }

#ifndef DACCESS_COMPILE

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            SIZE_T capacity = COMStringBuffer::NativeGetCapacity(stringRef);
            if (capacity > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = CoTaskMemAlloc((capacity+2) * sizeof(WCHAR));
            if (m_native == NULL)
                ThrowOutOfMemory();

            ((WCHAR*)m_native)[capacity+1] = L'\0';

#ifdef _DEBUG
            FillMemory(m_native, (capacity+1)*sizeof(WCHAR), 0xcc);
#endif

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
            CoTaskMemFree(m_native);
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 capacity = (UINT32)COMStringBuffer::NativeGetCapacity(stringRef);
            if (capacity > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            m_native = GetThread()->m_MarshalAlloc.Alloc((capacity+2) * sizeof(WCHAR));
            
            ((WCHAR*)m_native)[capacity+1] = L'\0';

#ifdef _DEBUG
            FillMemory(m_native, (capacity+1)*sizeof(WCHAR), 0xcc);
#endif
        }
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        LPWSTR str = (LPWSTR) m_native;
        if (str != NULL)
        {
            COMStringBuffer::ReplaceBuffer((STRINGBUFFERREF *) m_managed, str, (INT32)wcslen(str));
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef != NULL)
        {
            SIZE_T length = COMStringBuffer::NativeGetLength(stringRef);
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            memcpyNoGCRefs((WCHAR *) m_native, COMStringBuffer::NativeGetBuffer(stringRef), length * sizeof(WCHAR));

            ((WCHAR*)m_native)[length] = 0;
        }
    }

#endif // #ifndef DACCESS_COMPILE

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && fout)
        {
            _ASSERTE(NULL != psl);
            _ASSERTE(NULL != pslPost);
            psl->MLEmit(ML_WSTRBUILDER_C2N);
            pslPost->MLEmit(ML_WSTRBUILDER_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_WSTRBUILDER_C2N_SR)));
            return OVERRIDDEN;
        }
        return HANDLEASNORMAL;
    }
};
typedef FastAllocMarshalOverrides<WSTRBufferMarshalerBase> WSTRBufferMarshaler;


class CSTRBufferMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    CSTRBufferMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        m_BestFitMap = TRUE;
        m_ThrowOnUnmappableChar = FALSE;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            UINT32 length = (UINT32)strlen((LPSTR)m_native);
            SetObjectReference(m_managed, (OBJECTREF) COMStringBuffer::NewStringBuffer(length), GetAppDomain());
        }
    }

#ifndef DACCESS_COMPILE

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            SIZE_T capacity = COMStringBuffer::NativeGetCapacity(stringRef);
            if (capacity > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);


            // capacity is the count of wide chars, allocate buffer big enough for maximum
            // conversion to DBCS.
            m_native = CoTaskMemAlloc((capacity * GetMaxDBCSCharByteSize()) + 4);
            if (m_native == NULL)
                ThrowOutOfMemory();

            ((CHAR*)m_native)[capacity+1] = '\0';
            ((CHAR*)m_native)[capacity+2] = '\0';
            ((CHAR*)m_native)[capacity+3] = '\0';

#ifdef _DEBUG
            FillMemory(m_native, (capacity+1) * sizeof(CHAR), 0xcc);
#endif
            // Due to some Windows APIs returning non zero-terminated strings
            //  on win9x DBCS systems, we'll eagerly fill the string with zeros
            //  to catch this case.
            if (RunningOnWin95())
                FillMemory(m_native, (capacity+1) * sizeof(CHAR), 0x00);
            
            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;
        if (m_native != NULL)
            CoTaskMemFree(m_native);
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 capacity = (UINT32)COMStringBuffer::NativeGetCapacity(stringRef);
            if (capacity > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);


            // capacity is the count of wide chars, allocate buffer big enough for maximum
            // conversion to DBCS.
            m_native = GetThread()->m_MarshalAlloc.Alloc((capacity * GetMaxDBCSCharByteSize()) + 4);

            ((CHAR*)m_native)[capacity+1] = '\0';
            ((CHAR*)m_native)[capacity+2] = '\0';
            ((CHAR*)m_native)[capacity+3] = '\0';

#ifdef _DEBUG
            FillMemory(m_native, (capacity+1) * sizeof(CHAR), 0xcc);
#endif
            // Due to some Windows APIs returning non zero-terminated strings
            //  on win9x DBCS systems, we'll eagerly fill the string with zeros
            //  to catch this case.
            if (RunningOnWin95())
                FillMemory(m_native, (capacity+1) * sizeof(CHAR), 0x00);
        }
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        LPSTR str = (LPSTR) m_native;
        if (str != NULL)
        {
            COMStringBuffer::ReplaceBufferAnsi((STRINGBUFFERREF *) m_managed, str, (INT32)strlen(str));
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        STRINGBUFFERREF stringRef = (STRINGBUFFERREF) *m_managed;

        if (stringRef != NULL)
        {
            UINT32 length = (UINT32)COMStringBuffer::NativeGetLength(stringRef);
            if (length > 0x7ffffff0)
                COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

            DWORD mblength = 0;
            if (length)
            {
                UINT32 capacity = (UINT32)COMStringBuffer::NativeGetCapacity(stringRef);
                if (capacity > 0x7ffffff0)
                    COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

                mblength = InternalWideToAnsi(COMStringBuffer::NativeGetBuffer(stringRef),
                                              length,
                                              (LPSTR) m_native,
                                              (capacity * GetMaxDBCSCharByteSize()) + 4,
                                              m_BestFitMap,
                                              m_ThrowOnUnmappableChar);
            }
            ((CHAR*)m_native)[mblength] = '\0';
        }
    }

#endif // #ifndef DACCESS_COMPILE

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && fout)
        {
            psl->MLEmit(ML_CSTRBUILDER_C2N);
            psl->Emit8(pargs->m_pMarshalInfo->GetBestFitMapping());
            psl->Emit8(pargs->m_pMarshalInfo->GetThrowOnUnmappableChar());
            pslPost->MLEmit(ML_CSTRBUILDER_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_CSTRBUILDER_C2N_SR)));
            return OVERRIDDEN;
        }
        return HANDLEASNORMAL;
    }

    void SetBestFitMap(BOOL BestFit)
    {
        LEAF_CONTRACT;
        m_BestFitMap = BestFit;
    }

    void SetThrowOnUnmappableChar(BOOL ThrowOnUnmapChar)\
    {
        LEAF_CONTRACT;
        m_ThrowOnUnmappableChar = ThrowOnUnmapChar;
    }

    BOOL  m_BestFitMap;
    BOOL  m_ThrowOnUnmappableChar;
};
typedef FastAllocMarshalOverrides<CSTRBufferMarshalerBase> CSTRBufferMarshaler;


/* ------------------------------------------------------------------------- *
 * Interface marshaller
 * ------------------------------------------------------------------------- */
class InterfaceMarshalerBase : public ReferenceMarshaler
{
  protected:
    InterfaceMarshalerBase(CleanupWorkList *pList)
      : ReferenceMarshaler(pList),
        m_dwFlags(0)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

  public:
    void SetClassMT(MethodTable *pClassMT)
    {
        LEAF_CONTRACT;
        m_pClassMT = pClassMT;
    }

    void SetItfMT(MethodTable *pItfMT)
    {
        LEAF_CONTRACT;
        m_pItfMT = pItfMT;
    }

    void SetUseBasicItf(BOOL bUseBasicItf)
    {
        LEAF_CONTRACT;

        if (bUseBasicItf)
        {
            m_dwFlags |= ItfMarshalInfo::ITF_MARSHAL_USE_BASIC_ITF;
        }
    }

    void SetIsDispatch(BOOL bDispatch)
    {
        LEAF_CONTRACT;
        
        if (bDispatch)
        {
            m_dwFlags |= ItfMarshalInfo::ITF_MARSHAL_DISP_ITF;
        }
    }

    void SetClassIsHint(BOOL bClassIsHint)
    {
        LEAF_CONTRACT;
        if (bClassIsHint)
        {
            m_dwFlags |= ItfMarshalInfo::ITF_MARSHAL_CLASS_IS_HINT;
        }
    }

    void ClearNative()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (m_native != NULL)
        {
            GCX_PREEMP();

            ULONG cbRef = SafeRelease((IUnknown *)m_native);
            LogInteropRelease((IUnknown *)m_native, cbRef, "InterfaceMarshalerBase::ClearNative: In/Out release");
        }
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (*m_managed == NULL)
        {
            m_native = NULL;
        }
        else
        {
            EnsureComStarted();

            // Convert the ObjectRef to a COM IP.
            SafeComHolder<IUnknown> pUnk;

            pUnk = MarshalObjectToInterface(m_managed, m_pItfMT, m_pClassMT, m_dwFlags);

            if (pUnk != NULL)
                m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);

            m_native = pUnk;
            pUnk.SuppressRelease();
        }
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            EnsureComStarted();
            UnmarshalObjectFromInterface(m_managed, (IUnknown*)m_native, m_pItfMT, m_pClassMT, m_dwFlags);
        }
    }

    MethodTable     *m_pClassMT;
    MethodTable     *m_pItfMT;
    DWORD           m_dwFlags;
};


class InterfaceMarshaler : public DefaultMarshalOverrides<InterfaceMarshalerBase>
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,
        c_fInOnly = TRUE,
    };

    InterfaceMarshaler(CleanupWorkList *pList)
      : DefaultMarshalOverrides<InterfaceMarshalerBase>(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }

};





/* ------------------------------------------------------------------------- *
 * Native array marshallers
 * ------------------------------------------------------------------------- */
class NativeArrayMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = TRUE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    NativeArrayMarshalerBase(CleanupWorkList *pList)
      : ReferenceMarshaler(pList), 
        m_Array()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        Init();
    }

    void Init()
    {
        SetBestFitMap(TRUE);
        SetThrowOnUnmappableChar(FALSE);
        m_elementCount = 0;
        m_pElementMT = NULL;
        m_vt = VT_EMPTY;
    }

    void SetElementCount(INT32 count)
    {
        LEAF_CONTRACT;
        m_elementCount = count;
    }

    void SetElementMethodTable(MethodTable *pElementMT)
    {
        LEAF_CONTRACT;
        m_pElementMT = pElementMT;
    }

    void SetElementType(VARTYPE vt)
    {
        LEAF_CONTRACT;
        m_vt = vt;
    }

    void SetBestFitMap(BOOL BestFit)
    {
        LEAF_CONTRACT;
        m_BestFitMap = BestFit;
    }

    void SetThrowOnUnmappableChar(BOOL ThrowOnUnmapChar)
    {
        LEAF_CONTRACT;
        m_ThrowOnUnmappableChar = ThrowOnUnmapChar;
    }

    void SetMOPS(ML_CREATE_MARSHALER_CARRAY_OPERANDS* pmops)
    {
        LEAF_CONTRACT;
        m_mops = *pmops;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        ReferenceMarshaler::InputNativeStack(pStack);
        DetermineElementCount(pStack);
    }

    void InputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        ReferenceMarshaler::InputNativeRef(pStack);

        void* pRefStack = *(void**)pStack;
        DetermineElementCount(pRefStack);
    }

    void InputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        ReferenceMarshaler::InputCLRStack(pStack);
    }

    void InputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        ReferenceMarshaler::InputCLRRef(pStack);

        void* pRefStack = *(void**)pStack;
        DetermineElementCount(pRefStack);
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        BASEARRAYREF arrayRef = (BASEARRAYREF) *m_managed;

        if (arrayRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            SIZE_T cElements = arrayRef->GetNumComponents();
            SIZE_T cbElement = OleVariant::GetElementSizeForVarType(m_vt, m_pElementMT);

            if (cbElement == 0)
                COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);

            SIZE_T cbArray = cElements;
            if ( (!SafeMulSIZE_T(&cbArray, cbElement)) || cbArray > 0x7ffffff0)
                COMPlusThrow(kArgumentException, IDS_EE_STRUCTARRAYTOOLARGE);

            m_native = CoTaskMemAlloc(cbArray);
            if (m_native == NULL)
                ThrowOutOfMemory();

            // initialize the array
            FillMemory(m_native, cbArray, 0);

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        BASEARRAYREF arrayRef = (BASEARRAYREF) *m_managed;

        if (arrayRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            UINT32 cElements = arrayRef->GetNumComponents();
            UINT32 cbElement = (UINT32)OleVariant::GetElementSizeForVarType(m_vt, m_pElementMT);

            if (cbElement == 0)
                COMPlusThrow(kArgumentException, IDS_EE_COM_UNSUPPORTED_SIG);

            SIZE_T cbArray = cElements;
            if ( (!SafeMulSIZE_T(&cbArray, cbElement)) || cbArray > 0x7ffffff0)
                COMPlusThrow(kArgumentException, IDS_EE_STRUCTARRAYTOOLARGE);

            m_native = GetThread()->m_MarshalAlloc.Alloc((UINT32)cbArray);
            if (m_native == NULL)
                ThrowOutOfMemory();

            // initialize the array
            FillMemory(m_native, cbArray, 0);
        }
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            if (m_Array.IsNull())
            {
                // Get proper array class name & type
                m_Array = OleVariant::GetArrayForVarType(m_vt, TypeHandle(m_pElementMT));
                if (m_Array.IsNull())
                    COMPlusThrow(kTypeLoadException);
            }
            //
            // Allocate array
            //
            SetObjectReference(m_managed, AllocateArrayEx(m_Array, &m_elementCount, 1), GetAppDomain());
        }
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        if (m_native != NULL)
        {
            const OleVariant::Marshaler *pMarshaler = OleVariant::GetMarshalerForVarType(m_vt, TRUE);

            BASEARRAYREF *pArrayRef = (BASEARRAYREF *) m_managed;

            if (pMarshaler == NULL || pMarshaler->OleToComArray == NULL)
            {
                SIZE_T cElements = (*pArrayRef)->GetNumComponents();
                SIZE_T cbArray = cElements;
                if ( (!SafeMulSIZE_T(&cbArray, OleVariant::GetElementSizeForVarType(m_vt, m_pElementMT))) || cbArray > 0x7ffffff0)
                    COMPlusThrow(kArgumentException, IDS_EE_STRUCTARRAYTOOLARGE);

                    // If we are copying variants, strings, etc, we need to use write barrier
                _ASSERTE(!GetTypeHandleForCVType(OleVariant::GetCVTypeForVarType(m_vt)).GetMethodTable()->ContainsPointers());
                memcpyNoGCRefs((*pArrayRef)->GetDataPtr(), m_native, cbArray );
            }
            else
            {
                pMarshaler->OleToComArray(m_native, pArrayRef, m_pElementMT);
            }
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        BASEARRAYREF *pArrayRef = (BASEARRAYREF *) m_managed;

        if (*pArrayRef != NULL)
        {
            const OleVariant::Marshaler *pMarshaler = OleVariant::GetMarshalerForVarType(m_vt, TRUE);

            if (pMarshaler == NULL || pMarshaler->ComToOleArray == NULL)
            {
                SIZE_T cElements = (*pArrayRef)->GetNumComponents();
                SIZE_T cbArray = cElements;
                if ( (!SafeMulSIZE_T(&cbArray, OleVariant::GetElementSizeForVarType(m_vt, m_pElementMT))) || cbArray > 0x7ffffff0)
                    COMPlusThrow(kArgumentException, IDS_EE_STRUCTARRAYTOOLARGE);

                _ASSERTE(!GetTypeHandleForCVType(OleVariant::GetCVTypeForVarType(m_vt)).GetMethodTable()->ContainsPointers());
                memcpyNoGCRefs(m_native, (*pArrayRef)->GetDataPtr(), cbArray);
            }
            else
            {
                pMarshaler->ComToOleArray(pArrayRef, m_native, m_pElementMT, m_BestFitMap, m_ThrowOnUnmappableChar);
            }
        }
    }

    void ClearNative()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native != NULL)
        {
            ClearNativeContents();
            CoTaskMemFree(m_native);
        }
    }

    void ClearNativeTemp()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native != NULL)
            ClearNativeContents();
    }

    void ClearNativeContents()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native != NULL)
        {
            const OleVariant::Marshaler *pMarshaler = OleVariant::GetMarshalerForVarType(m_vt, FALSE);

            BASEARRAYREF *pArrayRef = (BASEARRAYREF *) m_managed;

            if (pMarshaler != NULL && pMarshaler->ClearOleArray != NULL)
            {
                SIZE_T cElements = (*pArrayRef)->GetNumComponents();
                pMarshaler->ClearOleArray(m_native, cElements, m_pElementMT);
            }
        }
    }

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        VARTYPE vt = pargs->na.m_vt;

/*
        if (vt == VTHACK_ANSICHAR && (byref || !CLRToNative))
        {
            *pResID = IDS_EE_BADMARSHAL_CHARARRAYRESTRICTION;
            return DISALLOWED;
        }
*/
        if ( (!byref) && CLRToNative && NULL == OleVariant::GetMarshalerForVarType(vt, TRUE) )
        {
            // The base offset should only be 0 for System.Array parameters for which
            // OleVariant::GetMarshalerForVarType(vt) should never return NULL.
            _ASSERTE(pargs->na.m_optionalbaseoffset != 0);

            psl->MLEmit(ML_PINNEDISOMORPHICARRAY_C2N_EXPRESS);
            psl->Emit16(pargs->na.m_optionalbaseoffset);
            return OVERRIDDEN;
        }

        return HANDLEASNORMAL;
    }

private:
    void DetermineElementCount(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        DWORD numelems = m_mops.additive;
        if (m_mops.multiplier != 0 && m_mops.countSize != 0) // don't dereference ofsbump+psrc if countsize is zero!!!
        {
            const BYTE* pCount = m_mops.offsetbump + (const BYTE *)pStack;
            switch (m_mops.countSize)
            {
                case 1: numelems += m_mops.multiplier * (DWORD)*((UINT8*)pCount); break;
                case 2: numelems += m_mops.multiplier * (DWORD)*((UINT16*)pCount); break;
                case 4: numelems += m_mops.multiplier * (DWORD)*((UINT32*)pCount); break;
                case 8: numelems += m_mops.multiplier * (DWORD)*((UINT64*)pCount); break;
                default:
                    _ASSERTE(0);
            }
        }

        SetElementCount(numelems);
    }

    VARTYPE                 m_vt;
    MethodTable            *m_pElementMT;
    TypeHandle              m_Array;
    INT32                   m_elementCount;
    BOOL                    m_BestFitMap;
    BOOL                    m_ThrowOnUnmappableChar;
    ML_CREATE_MARSHALER_CARRAY_OPERANDS m_mops;
};
typedef FastAllocMarshalOverrides<NativeArrayMarshalerBase> NativeArrayMarshaler;


/* ------------------------------------------------------------------------- *
 * AsAnyAArray marshaller (implements the PIS "asany" - Ansi mode)
 * ------------------------------------------------------------------------- */
class AsAnyAMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = TRUE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    AsAnyAMarshalerBase(CleanupWorkList *pList)
      : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }


    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref)
        {
            psl->MLEmit(ML_OBJECT_C2N);
            psl->Emit8( (fin ? ML_IN : 0) | (fout ? ML_OUT : 0) );
            psl->Emit8(pargs->m_pMarshalInfo->GetBestFitMapping());
            psl->Emit8(pargs->m_pMarshalInfo->GetThrowOnUnmappableChar());
            psl->Emit8(1 /*isansi*/);
            pslPost->MLEmit(ML_OBJECT_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(SizeOfML_OBJECT_C2N_SR()));
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_ASANYRESTRICTION;
            return DISALLOWED;
        }
    }

    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                     InteropStubLinker *pslPost,
                                                     BOOL        CLRToNative,
                                                     BOOL        fThruBuffer,
                                                     OverrideProcArgs *pargs,
                                                     UINT       *pResID)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        *pResID = IDS_EE_BADMARSHAL_ASANYRESTRICTION;
        return DISALLOWED;        
    }
};
typedef DefaultMarshalOverrides<AsAnyAMarshalerBase> AsAnyAMarshaler;





/* ------------------------------------------------------------------------- *
 * AsAnyWArray marshaller (implements the PIS "asany" - Unicode mode)
 * ------------------------------------------------------------------------- */
class AsAnyWMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = TRUE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    AsAnyWMarshalerBase(CleanupWorkList *pList)
      : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;
    }


    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref)
        {
            psl->MLEmit(ML_OBJECT_C2N);
            psl->Emit8( (fin ? ML_IN : 0) | (fout ? ML_OUT : 0) );
            psl->Emit8(pargs->m_pMarshalInfo->GetBestFitMapping());
            psl->Emit8(pargs->m_pMarshalInfo->GetThrowOnUnmappableChar());
            psl->Emit8(0 /* !isansi */);
            pslPost->MLEmit(ML_OBJECT_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(SizeOfML_OBJECT_C2N_SR()));
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_ASANYRESTRICTION;
            return DISALLOWED;
        }
    }

    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                     InteropStubLinker *pslPost,
                                                     BOOL        CLRToNative,
                                                     BOOL        fThruBuffer,
                                                     OverrideProcArgs *pargs,
                                                     UINT       *pResID)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        *pResID = IDS_EE_BADMARSHAL_ASANYRESTRICTION;
        return DISALLOWED;        
    }
};
typedef DefaultMarshalOverrides<AsAnyWMarshalerBase> AsAnyWMarshaler;





/* ------------------------------------------------------------------------- *
 * Delegate marshaller
 * ------------------------------------------------------------------------- */

class DelegateMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,

        c_fInOnly = TRUE,
    };

    DelegateMarshalerBase(CleanupWorkList *pList)
      : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        m_pMT = 0;
    }


    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        OBJECTREF objectRef = *m_managed;

        if (objectRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            m_native = (void*) COMDelegate::ConvertToCallback(objectRef);
        }
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
            SetObjectReference(m_managed, NULL, GetAppDomain());
        else
            SetObjectReference(m_managed, COMDelegate::ConvertToDelegate(m_native, m_pMT), GetAppDomain());
    }

    void SetMT(MethodTable* pMT)
    {
        LEAF_CONTRACT;
        m_pMT = pMT;
    }

    MethodTable*  m_pMT;
};
typedef DefaultMarshalOverrides<DelegateMarshalerBase> DelegateMarshaler;



class BlittablePtrMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    BlittablePtrMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        m_pMT = 0;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            SetObjectReference(m_managed, m_pMT->Allocate(), GetAppDomain());
        }
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        OBJECTREF obj = *m_managed;

        if (obj == NULL)
        {
            m_native = NULL;
        }
        else
        {
            m_native = CoTaskMemAlloc(m_pMT->GetNativeSize());
            if (m_native == NULL)
                ThrowOutOfMemory();

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ClearNative()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (m_native != NULL)
            CoTaskMemFree(m_native);
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        OBJECTREF obj = *m_managed;

        if (obj == NULL)
            m_native = NULL;
        else
            m_native = GetThread()->m_MarshalAlloc.Alloc(m_pMT->GetNativeSize());
    }

    void ClearNativeTemp()
    {
        LEAF_CONTRACT;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native != NULL)
        {
            OBJECTREF obj = *m_managed;
            _ASSERTE(!m_pMT->ContainsPointers());

            memcpyNoGCRefs(obj->GetData(), m_native, m_pMT->GetNativeSize());
        }
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        OBJECTREF obj = *m_managed;

        _ASSERTE(!m_pMT->ContainsPointers());
        if (obj != NULL)
        {
            memcpyNoGCRefs(m_native, obj->GetData(), m_pMT->GetNativeSize());
        }
    }


    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref /*isomorphic so no need to check in/out */)
        {
            psl->MLEmit(ML_BLITTABLELAYOUTCLASS_C2N);
            return OVERRIDDEN;
        }
        else
        {
            return HANDLEASNORMAL;
        }
    }

    void SetMT(MethodTable *pMT)
    {
        m_pMT = pMT;
    }

  private:
    MethodTable *m_pMT;  //method table
};
typedef FastAllocMarshalOverrides<BlittablePtrMarshalerBase> BlittablePtrMarshaler;





class LayoutClassPtrMarshalerBase : public ReferenceMarshaler
{
  public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = TRUE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    LayoutClassPtrMarshalerBase(CleanupWorkList *pList) : ReferenceMarshaler(pList)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pList));
        }
        CONTRACTL_END;

        m_pMT = 0;
    }

    void ConvertSpaceNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native == NULL)
        {
            SetObjectReference(m_managed, NULL, GetAppDomain());
        }
        else
        {
            SetObjectReference(m_managed, m_pMT->Allocate(), GetAppDomain());
        }
    }

    void ConvertSpaceCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;

        OBJECTREF objRef = *m_managed;

        if (objRef == NULL)
        {
            m_native = NULL;
        }
        else
        {
            m_native = CoTaskMemAlloc(m_pMT->GetNativeSize());
            if (m_native == NULL)
                ThrowOutOfMemory();

            m_pMarshalerCleanupNode = m_pList->ScheduleMarshalerCleanupOnException(this);
        }
    }

    void ClearNative()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
        {
            LayoutDestroyNative(m_native, m_pMT);
            CoTaskMemFree(m_native);
        }
    }

    void ConvertSpaceCLRToNativeTemp()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        OBJECTREF obj = *m_managed;
        if (obj == NULL)
            m_native = NULL;
        else
            m_native = GetThread()->m_MarshalAlloc.Alloc(m_pMT->GetNativeSize());
    }

    void ClearNativeTemp()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
            LayoutDestroyNative(m_native, m_pMT);
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_native != NULL)
            FmtClassUpdateCLR( (OBJECTREF*)m_managed, (LPBYTE)m_native, FALSE );
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        OBJECTREF objRef = *m_managed;

        if (objRef != NULL)
        {
            //Gotta do this first so an error halfway thru doesn't leave things in a bad state.
            FillMemory(m_native, m_pMT->GetNativeSize(), 0);
            FmtClassUpdateNative( (OBJECTREF*)m_managed, (LPBYTE)m_native, m_pList);
        }
    }

    void ClearNativeContents()
    {
        WRAPPER_CONTRACT;

        if (m_native != NULL)
        {
            LayoutDestroyNative(m_native, m_pMT);
        }
    }

    void SetMT(MethodTable *pMT)
    {
        m_pMT = pMT;
    }

  private:
    MethodTable *m_pMT;  //method table
};
typedef FastAllocMarshalOverrides<LayoutClassPtrMarshalerBase> LayoutClassPtrMarshaler;


class ArrayWithOffsetMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = sizeof(LPVOID),
        c_CLRSize = sizeof(ArrayWithOffsetData),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,

        c_fArgNativeByref = FALSE,
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        c_fArgCLRByref = sizeof(ArrayWithOffsetData) > ENREGISTERED_PARAMTYPE_MAXSIZE,
#else
        c_fArgCLRByref = FALSE,
#endif

        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = TRUE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    ArrayWithOffsetMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && fout)
        {
            psl->MLEmit(ML_ARRAYWITHOFFSET_C2N);
            pslPost->MLEmit(ML_ARRAYWITHOFFSET_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_ARRAYWITHOFFSET_C2N_SR)));
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_AWORESTRICTION;
            return DISALLOWED;
        }
    }
};



class BlittableValueClassMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = VARIABLESIZE,
        c_CLRSize = VARIABLESIZE,
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,
        c_fInOnly = TRUE,
    };

    BlittableValueClassMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        if (pargs->m_pMT->GetNativeSize() > ENREGISTERED_PARAMTYPE_MAXSIZE)
            byref = TRUE;
#endif

        if (CLRToNative && !byref && fin && !fout)
        {
            psl->MLEmit(ML_BLITTABLEVALUECLASS_C2N);
            psl->Emit32(pargs->m_pMT->GetNativeSize());
            return OVERRIDDEN;
        }
        else if (CLRToNative && byref)
        {
            psl->MLEmit(ML_COPYPINNEDGCREF);
            return OVERRIDDEN;
        }
        else if (!CLRToNative && !byref && fin && !fout)
        {
            psl->MLEmit(ML_BLITTABLEVALUECLASS_N2C);
            psl->Emit32(pargs->m_pMT->GetNativeSize());
            return OVERRIDDEN;
        }
        else if (!CLRToNative && byref)
        {
            psl->MLEmit( sizeof(LPVOID) == 4 ? ML_COPY4 : ML_COPY8);
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_BVCRESTRICTION;
            return DISALLOWED;
        }
    }
};



class BlittableValueClassWithCopyCtorMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = VARIABLESIZE,
        c_CLRSize = sizeof(OBJECTREF),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,
        c_fInOnly = TRUE,
    };

    BlittableValueClassWithCopyCtorMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (!byref && CLRToNative)
        {
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            if (pargs->mm.m_pMT->GetNativeSize() > ENREGISTERED_PARAMTYPE_MAXSIZE)
            {
                psl->MLNewLocal(StackElemSize(pargs->mm.m_pMT->GetNativeSize()));
            }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

            psl->MLEmit(ML_COPYCTOR_C2N);
            psl->EmitPtr(pargs->mm.m_pMT);
            psl->EmitPtr(pargs->mm.m_pCopyCtor);
            psl->EmitPtr(pargs->mm.m_pDtor);
            return OVERRIDDEN;
        } else  if (!byref && !CLRToNative) {
            psl->MLEmit(ML_COPYCTOR_N2C);
            psl->EmitPtr(pargs->mm.m_pMT);
            return OVERRIDDEN;
        } else {
            *pResID = IDS_EE_BADMARSHAL_COPYCTORRESTRICTION;
            return DISALLOWED;
        }
    }
};





class ValueClassMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = VARIABLESIZE,
        c_CLRSize = VARIABLESIZE,
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = FALSE,

        c_fInOnly = TRUE,
    };

    ValueClassMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref && fin && !fout)
        {
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            BOOL fByRefNative = MetaSig::IsArgPassedByRef(pargs->m_pMT->GetNativeSize());
            BOOL fByRefCLR    = MetaSig::IsArgPassedByRef(pargs->m_pMT->GetAlignedNumInstanceFieldBytes());

            if (fByRefNative && fByRefCLR)
            {
                psl->MLNewLocal(sizeof(ML_REFVALUECLASS_C2N_SR));
                psl->MLEmit(ML_REFVALUECLASS_C2N);
                psl->Emit8(ML_IN);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else if (fByRefNative)
            {
                psl->MLEmit(ML_VALUECLASS_C2BYREFN);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else if (fByRefCLR)
            {
                psl->MLEmit(ML_VALUECLASS_BYREFC2N);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
            {
                psl->MLEmit(ML_VALUECLASS_C2N);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
        }
        else if (CLRToNative && byref)
        {
            psl->MLEmit(ML_REFVALUECLASS_C2N);
            psl->Emit8( (fin ? ML_IN : 0) | (fout ? ML_OUT : 0) );
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_REFVALUECLASS_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFVALUECLASS_C2N_SR)));
            return OVERRIDDEN;
        }
        else if (!CLRToNative && !byref && fin && !fout)
        {
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            BOOL fByRefNative = MetaSig::IsArgPassedByRef(pargs->m_pMT->GetNativeSize());
            BOOL fByRefCLR    = MetaSig::IsArgPassedByRef(pargs->m_pMT->GetAlignedNumInstanceFieldBytes());

            if (fByRefNative && fByRefCLR)
            {
                psl->MLNewLocal(sizeof(ML_REFVALUECLASS_N2C_SR));
                psl->MLEmit(ML_REFVALUECLASS_N2C);
                psl->Emit8(ML_IN);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else if (fByRefNative)
            {
                psl->MLEmit(ML_VALUECLASS_BYREFN2C);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else if (fByRefCLR)
            {
                psl->MLNewLocal(sizeof(ML_REFVALUECLASS_N2C_SR));
                psl->MLEmit(ML_VALUECLASS_N2BYREFC);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
            else
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
            {
                psl->MLEmit(ML_VALUECLASS_N2C);
                psl->EmitPtr(pargs->m_pMT);
                return OVERRIDDEN;
            }
        }
        else if (!CLRToNative && byref)
        {
            psl->MLEmit(ML_REFVALUECLASS_N2C);
            psl->Emit8( (fin ? ML_IN : 0) | (fout ? ML_OUT : 0) );
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_REFVALUECLASS_N2C_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFVALUECLASS_N2C_SR)));
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_VCRESTRICTION;

            return DISALLOWED;
        }
    }
};


/* ------------------------------------------------------------------------- *
 * Custom Marshaler.
 * ------------------------------------------------------------------------- */

class CustomMarshaler : public Marshaler
{
public:
    enum
    {
        c_fNeedsClearNative = TRUE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = TRUE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fInOnly = FALSE,
    };

    CustomMarshaler(CleanupWorkList *pList) :
        Marshaler(pList, 0, 0, c_fReturnsNativeByref, c_fReturnsCLRByref),
        m_managed(pList->NewProtectedObjectRef(NULL)),
        m_native(NULL),
        m_pCMHelper(NULL)
    {
        WRAPPER_CONTRACT;
    }

    void InputDest(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_pDest = pStack;
    }

    void ConvertContentsNativeToCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, m_pCMHelper->InvokeMarshalNativeToManagedMeth(m_native), GetAppDomain());
    }

    void ConvertContentsCLRToNative()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        m_native = m_pCMHelper->InvokeMarshalManagedToNativeMeth(*m_managed);
    }

    void ClearNative()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;
        
        m_pCMHelper->InvokeCleanUpNativeMeth(m_native);
    }

    void ClearCLR()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        m_pCMHelper->InvokeCleanUpManagedMeth(*m_managed);
    }

    void ReInitNative()
    {
        LEAF_CONTRACT;
        m_native = NULL;
    }

    void SetCMHelper(CustomMarshalerHelper *pCMHelper)
    {
        WRAPPER_CONTRACT;
        m_pCMHelper = pCMHelper;
        m_cbCLR = StackElemSize((BYTE)pCMHelper->GetManagedSize());
        m_cbNative = StackElemSize((BYTE)pCMHelper->GetNativeSize());
    }

    OBJECTREF               *m_managed;
    void                    *m_native;
    CustomMarshalerHelper   *m_pCMHelper;
};



class ReferenceCustomMarshalerBase : public CustomMarshaler
{
public:
    enum
    {
        c_nativeSize = sizeof(OBJECTREF),
        c_CLRSize = sizeof(void *),
    };

    ReferenceCustomMarshalerBase(CleanupWorkList *pList)
    : CustomMarshaler(pList)
    {
        WRAPPER_CONTRACT;
    }

    void InputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        m_native = *(void**)pStack;
    }

    void InputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(*(Object**)pStack), GetAppDomain());
    }

    void InputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(void***)pStack));
        }
        CONTRACTL_END;

        m_native = **(void ***)pStack;
    }

    void InputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(*(Object***)pStack));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(**(Object ***)pStack), GetAppDomain());
    }

    void OutputNativeStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(void **)pStack = m_native;
    }

    void OutputCLRStack(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        *(OBJECTREF*)pStack = *m_managed;
    }

    void OutputNativeRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(void **)pStack = &m_native;
    }

    void OutputCLRRef(void *pStack)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pStack));
        }
        CONTRACTL_END;

        *(OBJECTREF**)pStack = m_managed;
    }

    void OutputNativeDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(m_pDest));
            PRECONDITION(CheckPointer(*(void***)m_pDest));
        }
        CONTRACTL_END;

        **(void ***)m_pDest = m_native;
    }

    void OutputCLRDest()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(m_pDest));
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        SetObjectReference(*(OBJECTREF**)m_pDest, *m_managed, GetAppDomain());
    }

    void InputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pField));
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        SetObjectReference(m_managed, ObjectToOBJECTREF(*(Object**)pField), GetAppDomain());
    }

    void OutputCLRField(void *pField)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pField));
            PRECONDITION(CheckPointer(m_managed));
        }
        CONTRACTL_END;

        SetObjectReference((OBJECTREF*)pField, *m_managed, GetAppDomain());
    }
};
typedef DefaultMarshalOverrides<ReferenceCustomMarshalerBase> ReferenceCustomMarshaler;


/* ------------------------------------------------------------------------- *
 * ArgIterator marshaller
 * ------------------------------------------------------------------------- */

class ArgIteratorMarshalerBase : public StandardMarshaler<va_list, VARARGS*, TRUE, TRUE>
{
  public:
    enum
    {
        c_CLRSize = sizeof(VARARGS),
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = TRUE,
    };

    ArgIteratorMarshalerBase(CleanupWorkList *pList)
      : StandardMarshaler<va_list, VARARGS*, TRUE, TRUE>(pList)
    {
        LEAF_CONTRACT;
    }

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref)
        {
            psl->MLEmit(ML_ARGITERATOR_C2N);
            return OVERRIDDEN;
        }
        else if (!CLRToNative && !byref)
        {
            psl->MLEmit(ML_ARGITERATOR_N2C);
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_ARGITERATORRESTRICTION;
            return DISALLOWED;
        }
    }
};
typedef DefaultMarshalOverrides<ArgIteratorMarshalerBase> ArgIteratorMarshaler;


/* ------------------------------------------------------------------------- *
 * HandleRef marshaller
 * ------------------------------------------------------------------------- */
class HandleRefMarshaler : public Marshaler
{
  public:
    enum
    {
        c_nativeSize = sizeof(LPVOID),
        c_CLRSize = sizeof(HANDLEREF),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    HandleRefMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        if (CLRToNative && !byref)
        {
            psl->MLEmit(ML_HANDLEREF_C2N);
            return OVERRIDDEN;
        }
        else
        {
            *pResID = IDS_EE_BADMARSHAL_HANDLEREFRESTRICTION;
            return DISALLOWED;
        }
    }

    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                     InteropStubLinker *pslPost,
                                                     BOOL        CLRToNative,
                                                     BOOL        fThruBuffer,
                                                     OverrideProcArgs *pargs,
                                                     UINT       *pResID)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;

        *pResID = IDS_EE_BADMARSHAL_HANDLEREFRESTRICTION;
        return DISALLOWED;        
    }
};

/* ------------------------------------------------------------------------- *
 * SafeHandle marshaller
 * ------------------------------------------------------------------------- */
class SafeHandleMarshaler : public Marshaler
{
public:
    enum
    {
        c_nativeSize = sizeof(LPVOID),
        c_CLRSize = sizeof(SAFEHANDLE),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };

    SafeHandleMarshaler(CleanupWorkList *pList);

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(psl));
            PRECONDITION(CheckPointer(pslPost));
            PRECONDITION(CheckPointer(pargs));
            PRECONDITION(CheckPointer(pResID));
        }
        CONTRACTL_END;

        if (!CLRToNative)
        {
            *pResID = IDS_EE_BADMARSHAL_SAFEHANDLENATIVETOCOM;
            return DISALLOWED;
        }

        if (byref)
        {
            // Ref and out SafeHandle parameters must not be abstract.
            if (fout && pargs->m_pMT->IsAbstract())
            {
                *pResID = IDS_EE_BADMARSHAL_ABSTRACTOUTSAFEHANDLE;
                return DISALLOWED;
            }

            psl->MLEmit(ML_REFSAFEHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            psl->Emit8((fin ? ML_IN : 0) | (fout ? ML_OUT : 0));
            pslPost->MLEmit(ML_REFSAFEHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFSAFEHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
        else
        {
            psl->MLEmit(ML_SAFEHANDLE_C2N);
            return OVERRIDDEN;
        }
    }

    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                    InteropStubLinker *pslPost,
                                                    BOOL        CLRToNative,
                                                    BOOL        fThruBuffer,
                                                    OverrideProcArgs *pargs,
                                                    UINT       *pResID)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(psl, NULL_OK));
            PRECONDITION(CheckPointer(pslPost, NULL_OK));
            PRECONDITION(CheckPointer(pargs));
            PRECONDITION(CheckPointer(pResID));
        }
        CONTRACTL_END;

        if (!CLRToNative)
        {
            *pResID = IDS_EE_BADMARSHAL_RETURNSHCOMTONATIVE;
            return DISALLOWED;
        }

        // Returned SafeHandle parameters must not be abstract.
        if (pargs->m_pMT->IsAbstract())
        {
            *pResID = IDS_EE_BADMARSHAL_ABSTRACTRETSAFEHANDLE;
            return DISALLOWED;
        }

        if (psl == NULL)
        {
            _ASSERTE(pslPost == NULL);
            return OVERRIDDEN;
        }

        if (fThruBuffer)
        {
            psl->MLEmit(ML_RETVALSAFEHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_RETVALSAFEHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFSAFEHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
        else
        {
            psl->MLEmit(ML_RETSAFEHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_RETSAFEHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_RETSAFEHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
    }
};


//----------------------------------------------------------------------
// An image of the record placed on LOCAL array by the ML_OBJECT_C2N
// instruction.
//----------------------------------------------------------------------
struct ML_OBJECT_C2N_SR
{
    public:
        LPVOID DoConversion(OBJECTREF       *ppProtectedObjectRef,  
                            BYTE             inout,
                            BYTE             fIsAnsi,
                            BYTE             fBestFitMapping,
                            BYTE             fThrowOnUnmappableChar,
                        CleanupWorkList *pCleanup);

        VOID BackPropagate(BOOL *pfDeferredException);

    private:             
        enum BackPropType
        {
            BP_NONE,
            BP_UNMARSHAL,
        };


        // DoConversion() delegates to some other conversion code
        // depending on the runtime type of the parameter.
        // Then it saves it away here so that BackPropagate can delegate
        // to the appropriate propagater code without redoing the type
        // analysis.
        UINT8      m_backproptype;
        BYTE       m_inout;
        BYTE       m_fIsAnsi;

        union
        {
            // Use void* to ensure normal alignment for the marshalers.
            void*                     m_marshaler;
            BYTE                      m_cstrmarshaler[sizeof(CSTRMarshaler)];
            BYTE                      m_wstrmarshaler[sizeof(WSTRMarshaler)];
            BYTE                      m_cstrbuffermarshaler[sizeof(CSTRBufferMarshaler)];
            BYTE                      m_wstrbuffermarshaler[sizeof(WSTRBufferMarshaler)];
            BYTE                      m_nativearraymarshaler[sizeof(NativeArrayMarshaler)];
            BYTE                      m_layoutclassptrmarshaler[sizeof(LayoutClassPtrMarshaler)];
        };
};




/* ------------------------------------------------------------------------- *
 * CriticalHandle marshaller
 * ------------------------------------------------------------------------- */
class CriticalHandleMarshaler : public Marshaler
{
public:
    enum
    {
        c_nativeSize = sizeof(LPVOID),
        c_CLRSize = sizeof(CRITICALHANDLE),
        c_fReturnsNativeByref = FALSE,
        c_fReturnsCLRByref = FALSE,
        c_fArgNativeByref = FALSE,
        c_fArgCLRByref = FALSE,
        c_fNeedsClearNative = FALSE,
        c_fNeedsClearNativeContents = FALSE,
        c_fNeedsClearCLR = FALSE,
        c_fNeedsClearCLRContents = FALSE,
        c_fNeedsConvertContents = TRUE,
        c_fInOnly = FALSE,
    };
        
    CriticalHandleMarshaler(CleanupWorkList *pList); 

    static MarshalerOverrideStatus ArgumentMLOverride(InteropStubLinker* psl,
                                                      InteropStubLinker* pslPost,
                                                      BOOL               byref,
                                                      BOOL               fin,
                                                      BOOL               fout,
                                                      BOOL               CLRToNative,
                                                      OverrideProcArgs*  pargs,
                                                      UINT*              pResID,
                                                      UINT               argidx)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(psl));
            PRECONDITION(CheckPointer(pslPost));
            PRECONDITION(CheckPointer(pargs));
            PRECONDITION(CheckPointer(pResID));            
        }
        CONTRACTL_END;

        if (!CLRToNative)
        {
            *pResID = IDS_EE_BADMARSHAL_CRITICALHANDLENATIVETOCOM;
            return DISALLOWED;
        }

        if (byref)
        {
            // Ref and out CriticalHandle parameters must not be abstract.
            if (fout && pargs->m_pMT->IsAbstract())
            {
                *pResID = IDS_EE_BADMARSHAL_ABSTRACTOUTCRITICALHANDLE;
                return DISALLOWED;
            }

            psl->MLEmit(ML_REFCRITICALHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            psl->Emit8((fin ? ML_IN : 0) | (fout ? ML_OUT : 0));
            pslPost->MLEmit(ML_REFCRITICALHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFCRITICALHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
        else
        {
            psl->MLEmit(ML_CRITICALHANDLE_C2N);
            return OVERRIDDEN;
        }
    }

    static MarshalerOverrideStatus ReturnMLOverride(InteropStubLinker *psl,
                                                    InteropStubLinker *pslPost,
                                                    BOOL        CLRToNative,
                                                    BOOL        fThruBuffer,
                                                    OverrideProcArgs *pargs,
                                                    UINT       *pResID)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(psl, NULL_OK));
            PRECONDITION(CheckPointer(pslPost, NULL_OK));
            PRECONDITION(CheckPointer(pargs));
            PRECONDITION(CheckPointer(pResID));            
        }
        CONTRACTL_END;

        if (!CLRToNative)
        {
            *pResID = IDS_EE_BADMARSHAL_RETURNSHCOMTONATIVE;
            return DISALLOWED;
        }

        // Returned CriticalHandle parameters must not be abstract.
        if (pargs->m_pMT->IsAbstract())
        {
            *pResID = IDS_EE_BADMARSHAL_ABSTRACTRETCRITICALHANDLE;
            return DISALLOWED;
        }

        if (psl == NULL)
        {
            _ASSERTE(pslPost == NULL);
            return OVERRIDDEN;
        }

        if (fThruBuffer)
        {
            psl->MLEmit(ML_RETVALCRITICALHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_RETVALCRITICALHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_REFCRITICALHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
        else
        {
            psl->MLEmit(ML_RETCRITICALHANDLE_C2N);
            psl->EmitPtr(pargs->m_pMT);
            pslPost->MLEmit(ML_RETCRITICALHANDLE_C2N_POST);
            pslPost->Emit16(psl->MLNewLocal(sizeof(ML_RETCRITICALHANDLE_C2N_SR)));
            return OVERRIDDEN;
        }
    }
};

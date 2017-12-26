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

#include "common.h"
#include "sigformat.h"
#include "typedesc.h"

SigFormat::SigFormat()
{
    WRAPPER_CONTRACT; // THROWS;GC_TRIGGERS;INJECT_FAULT(ThrowOM)
    _size = SIG_INC;
    _pos = 0;
    _fmtSig = new char[_size];
}

SigFormat::SigFormat(MetaSig &metaSig, LPCUTF8 szMemberName, LPCUTF8 szClassName, LPCUTF8 szNameSpace)
{
    WRAPPER_CONTRACT;
    FormatSig(metaSig, szMemberName, szClassName, szNameSpace);
}


// SigFormat::SigFormat()
// This constructor will create the string representation of a
//  method.
SigFormat::SigFormat(MethodDesc* pMeth, TypeHandle owner, BOOL fIgnoreMethodName)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END
    MetaSig sig(pMeth, owner);
    if (fIgnoreMethodName)
    {
        FormatSig(sig, NULL);
    }
    else
    {
        FormatSig(sig, pMeth->GetName());
    }
}


SigFormat::~SigFormat()
{
    LEAF_CONTRACT;

    if (_fmtSig)
        delete [] _fmtSig;
}

const char * SigFormat::GetCString()
{
    LEAF_CONTRACT;
    return _fmtSig;
}

const char * SigFormat::GetCStringParmsOnly()
{
    LEAF_CONTRACT;
     // _fmtSig looks like: "void Put (byte[], int, int)".
     // Skip to the '('.
     int skip;
     for(skip=0; _fmtSig[skip]!='('; skip++)
            ;
     return _fmtSig + skip;
}

void SigFormat::AddString(LPCUTF8 s)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    size_t len = strlen(s);
    // Allocate on overflow
    size_t requiredSize = _pos + len + 1;

    if (requiredSize <= _pos) { // check for integer overflow in previous calc
    #ifndef DACCESS_COMPILE
        COMPlusThrowOM();
    #else
        DacError(E_OUTOFMEMORY);
    #endif
    }

    if (requiredSize > _size) {
        size_t newSize = (_size+SIG_INC > requiredSize) ? _size+SIG_INC : requiredSize+SIG_INC;
        char* temp = new char[newSize];
        memcpy(temp,_fmtSig,_size);
        delete [] _fmtSig;
        _fmtSig = temp;
        _size=newSize;
    }
    strcpy_s(&_fmtSig[_pos],_size - (&_fmtSig[_pos] - _fmtSig), s);
    _pos += len;
}


//------------------------------------------------------------------------
// Replacement for SigFormat::AddType that avoids class loading
// and copes with formal type parameters
//------------------------------------------------------------------------
void SigFormat::AddTypeString(Module* pModule, SigPointer sig, const SigTypeContext *pTypeContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    LPCUTF8     szcName;
    LPCUTF8     szcNameSpace;
    /*
    ULONG       cArgs;
    VOID       *pEnum;
    ULONG       i;
    */

    CorElementType type;
    IfFailThrow(sig.GetElemType(&type));

    // Format the output
    switch (type)
    {
    case ELEMENT_TYPE_VOID:     AddString("Void"); break;
    case ELEMENT_TYPE_BOOLEAN:  AddString("Boolean"); break;
    case ELEMENT_TYPE_I1:       AddString("SByte"); break;
    case ELEMENT_TYPE_U1:       AddString("Byte"); break;
    case ELEMENT_TYPE_I2:       AddString("Int16"); break;
    case ELEMENT_TYPE_U2:       AddString("UInt16"); break;
    case ELEMENT_TYPE_CHAR:     AddString("Char"); break;
    case ELEMENT_TYPE_I:        AddString("IntPtr"); break;
    case ELEMENT_TYPE_U:        AddString("UIntPtr"); break;
    case ELEMENT_TYPE_I4:       AddString("Int32"); break;
    case ELEMENT_TYPE_U4:       AddString("UInt32"); break;
    case ELEMENT_TYPE_I8:       AddString("Int64"); break;
    case ELEMENT_TYPE_U8:       AddString("UInt64"); break;
    case ELEMENT_TYPE_R4:       AddString("Single"); break;
    case ELEMENT_TYPE_R8:       AddString("Double"); break;
    case ELEMENT_TYPE_OBJECT:   AddString(g_ObjectClassName); break;
    case ELEMENT_TYPE_STRING:   AddString(g_StringClassName); break;

    // For Value Classes we fall through unless the pVMC is an Array Class,
    // If its an array class we need to get the name of the underlying type from
    // it.
    case ELEMENT_TYPE_VALUETYPE:
    case ELEMENT_TYPE_CLASS:
        {
            IMDInternalImport *pInternalImport = pModule->GetMDImport();
            mdToken token;
            IfFailThrow(sig.GetToken(&token));

            if (TypeFromToken(token) == mdtTypeDef)
            {
                pInternalImport->GetNameOfTypeDef(token, &szcName, &szcNameSpace);
            }
            else if (TypeFromToken(token) == mdtTypeRef)
            {
                pInternalImport->GetNameOfTypeRef(token, &szcNameSpace, &szcName);
            }
            else
                break;

            if (*szcNameSpace)
            {
                AddString(szcNameSpace);
                AddString(".");
            }
            AddString(szcName);
            break;
        }
    case ELEMENT_TYPE_INTERNAL:
        {
            TypeHandle hType;
            CorSigUncompressPointer(sig.GetPtr(), (void**)&hType);
            _ASSERTE(!hType.IsNull());
            MethodTable *pMT = hType.GetMethodTable();
            _ASSERTE(pMT);
            mdToken token = pMT->GetCl();
            pMT->GetMDImport()->GetNameOfTypeDef(token, &szcName, &szcNameSpace);
            if (*szcNameSpace)
            {
                AddString(szcNameSpace);
                AddString(".");
            }
            AddString(szcName);
            break;
        }
    case ELEMENT_TYPE_TYPEDBYREF:
        {
            AddString("TypedReference");
            break;
        }

    case ELEMENT_TYPE_BYREF:
        {
            AddTypeString(pModule, sig, pTypeContext);
            AddString(" ByRef");
        }
        break;

    case ELEMENT_TYPE_MVAR :
        {
            DWORD ix;
            IfFailThrow(sig.GetData(&ix));
            if (pTypeContext && pTypeContext->m_methInst && ix >= 0 && ix < pTypeContext->m_methInstCount)
            {
                AddType(pTypeContext->m_methInst[ix]);
            }
            else
            {
                char smallbuf[20];
                sprintf_s(smallbuf, COUNTOF(smallbuf), "!!%d", ix);
                AddString(smallbuf);
            }
        }
      break;

    case ELEMENT_TYPE_VAR :
        {
            DWORD ix;
            IfFailThrow(sig.GetData(&ix));

            if (pTypeContext && pTypeContext->m_classInst && ix >= 0 && ix < pTypeContext->m_classInstCount)
            {
                AddType(pTypeContext->m_classInst[ix]);
            }
            else
            {
                char smallbuf[20];
                sprintf_s(smallbuf, COUNTOF(smallbuf), "!%d", ix);
                AddString(smallbuf);
            }
        }
        break;

    case ELEMENT_TYPE_GENERICINST :
        {
            AddTypeString(pModule, sig, pTypeContext);

            IfFailThrow(sig.SkipExactlyOne());
            DWORD n;
            IfFailThrow(sig.GetData(&n));

            AddString("<");
            for (DWORD i = 0; i < n; i++)
            {
                if (i > 0)
                    AddString(",");
                AddTypeString(pModule,sig, pTypeContext);
                IfFailThrow(sig.SkipExactlyOne());
            }
            AddString(">");

            break;
        }

    case ELEMENT_TYPE_SZARRAY:      // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:        // General Array
        {
            AddTypeString(pModule, sig, pTypeContext);
            IfFailThrow(sig.SkipExactlyOne());
            if (type == ELEMENT_TYPE_ARRAY)
            {
                AddString("[");
                ULONG len;
                IfFailThrow(sig.GetData(&len));

                for (ULONG i=0;i<len-1;i++)
                    AddString(",");

                AddString("]");
            }
            else
            {
                AddString("[]");
            }
        }
        break;

    case ELEMENT_TYPE_PTR:
        {
            // This will pop up on methods that take a pointer to a block of unmanaged memory.
            AddTypeString(pModule, sig, pTypeContext);
            AddString("*");
            break;
        }

    case ELEMENT_TYPE_FNPTR:
        {
            DWORD callConv;
            IfFailThrow(sig.GetData(&callConv));
        
            ULONG cArgs;
            IfFailThrow(sig.GetData(&cArgs));

            AddTypeString(pModule, sig, pTypeContext);
            IfFailThrow(sig.SkipExactlyOne());
            AddString(" (");
            ULONG i;
            for (i = 0; i < cArgs; i++) {
                AddTypeString(pModule, sig, pTypeContext);
                IfFailThrow(sig.SkipExactlyOne());
                if (i != (cArgs - 1))
                    AddString(", ");
            }
            if ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_VARARG)
            {
                if (cArgs)
                    AddString(", ");
                AddString("...");
            }
            AddString(")");
            break;
        }

    default:
        AddString("**UNKNOWN TYPE**");

    }
}

void SigFormat::FormatSig(MetaSig &sig, LPCUTF8 szMemberName, LPCUTF8 szClassName, LPCUTF8 szNameSpace)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    UINT            cArgs;

    _size = SIG_INC;
    _pos = 0;
    _fmtSig = new char[_size];

    AddTypeString(sig.GetModule(), sig.GetReturnProps(), sig.GetSigTypeContext());

    AddString(" ");
    if (szNameSpace != NULL)
    {
        AddString(szNameSpace);
        AddString(".");
    }
    if (szClassName != NULL)
    {
        AddString(szClassName);
        AddString(".");
    }
    if (szMemberName != NULL)
    {
        AddString(szMemberName);
    }

    cArgs = sig.NumFixedArgs();
    sig.Reset();

    AddString("(");

    // Loop through all of the args
    for (UINT i=0;i<cArgs;i++) {
        sig.NextArg();
       AddTypeString(sig.GetModule(), sig.GetArgProps(), sig.GetSigTypeContext());
       if (i != cArgs-1)
           AddString(", ");
    }

    // Display vararg signature at end
    if (sig.IsVarArg())
    {
        if (cArgs)
            AddString(", ");
        AddString("...");
    }

    AddString(")");
}

void SigFormat::AddType(TypeHandle th)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    LPCUTF8     szcName;
    LPCUTF8     szcNameSpace;
    ULONG       cArgs;
    ULONG       i;

    if (th.IsNull()) {
        AddString("**UNKNOWN TYPE**");
        return;
    }

    CorElementType type = th.GetSignatureCorElementType();

    // Format the output
    switch (type)
    {
    case ELEMENT_TYPE_VOID:     AddString("Void"); break;
    case ELEMENT_TYPE_BOOLEAN:  AddString("Boolean"); break;
    case ELEMENT_TYPE_I1:       AddString("SByte"); break;
    case ELEMENT_TYPE_U1:       AddString("Byte"); break;
    case ELEMENT_TYPE_I2:       AddString("Int16"); break;
    case ELEMENT_TYPE_U2:       AddString("UInt16"); break;
    case ELEMENT_TYPE_CHAR:     AddString("Char"); break;
    case ELEMENT_TYPE_I:        AddString("IntPtr"); break;
    case ELEMENT_TYPE_U:        AddString("UIntPtr"); break;
    case ELEMENT_TYPE_I4:       AddString("Int32"); break;
    case ELEMENT_TYPE_U4:       AddString("UInt32"); break;
    case ELEMENT_TYPE_I8:       AddString("Int64"); break;
    case ELEMENT_TYPE_U8:       AddString("UInt64"); break;
    case ELEMENT_TYPE_R4:       AddString("Single"); break;
    case ELEMENT_TYPE_R8:       AddString("Double"); break;
    case ELEMENT_TYPE_OBJECT:   AddString(g_ObjectClassName); break;
    case ELEMENT_TYPE_STRING:   AddString(g_StringClassName); break;

    // For Value Classes we fall through unless the pVMC is an Array Class,
    // If its an array class we need to get the name of the underlying type from
    // it.
    case ELEMENT_TYPE_VALUETYPE:
    case ELEMENT_TYPE_CLASS:
        {
            th.GetMethodTable()->GetMDImport()->GetNameOfTypeDef(th.GetCl(), &szcName, &szcNameSpace);

            if (*szcNameSpace)
            {
                AddString(szcNameSpace);
                AddString(".");
            }
            AddString(szcName);

            if (th.HasInstantiation())
            {
                TypeHandle *inst = th.GetInstantiation();

                if(inst != NULL)
                {
                    AddString("<");
                    for (DWORD j = 0; j < th.GetNumGenericArgs(); j++)
                    {
                        if (j > 0)
                        AddString(",");

                        AddType(inst[j]);
                    }
                    AddString(">");
                }
            }

            break;
        }
    case ELEMENT_TYPE_TYPEDBYREF:
        {
            AddString("TypedReference");
            break;
        }

    case ELEMENT_TYPE_BYREF:
        {
            TypeHandle h = th.AsTypeDesc()->GetTypeParam();
            AddType(h);
            AddString(" ByRef");
        }
        break;

    case ELEMENT_TYPE_SZARRAY:      // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:        // General Array
        {
            ArrayTypeDesc* aTD = th.AsArray();
            AddType(aTD->GetArrayElementTypeHandle());

            if (type == ELEMENT_TYPE_ARRAY)
            {
                AddString("[");
                int len = aTD->GetRank();

                for (int j=0;j<len-1;j++)

                    AddString(",");
                AddString("]");
            }
            else
            {
                AddString("[]");
            }
        }
        break;

    case ELEMENT_TYPE_PTR:
        {
            // This will pop up on methods that take a pointer to a block of unmanaged memory.
            TypeHandle h = th.AsTypeDesc()->GetTypeParam();
            AddType(h);
            AddString("*");
            break;
        }
    case ELEMENT_TYPE_FNPTR:
        {
            FnPtrTypeDesc* pTD = th.AsFnPtrType();

            TypeHandle *pRetAndArgTypes = pTD->GetRetAndArgTypes();
            AddType(pRetAndArgTypes[0]);
            AddString(" (");

            cArgs = pTD->GetNumArgs();

            for (i = 0; i < cArgs; i++)
            {
                AddType(pRetAndArgTypes[i+1]);

                if (i != (cArgs - 1))
                AddString(", ");
            }
            if ((pTD->GetCallConv() & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_VARARG)
            {
                AddString(", ");

                AddString("...");
            }
            AddString(")");

            break;
        }
    default:
        AddString("**UNKNOWN TYPE**");
    }
}


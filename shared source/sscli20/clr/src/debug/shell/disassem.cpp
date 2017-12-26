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
/* ------------------------------------------------------------------------- *
 * disassem.cpp
 * ------------------------------------------------------------------------- */

#include "stdafx.h"
#include "palclr.h"

#include "dshell.h"
#include <limits.h>

/* ------------------------------------------------------------------------- *
 * Opcode tables
 * ------------------------------------------------------------------------- */
#define OLD_OPCODE_FORMAT 0     // Fix remove after 8/1/99

#include "openum.h"

#define DECLARE_DATA 1

#undef OPDEF
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) L##s,
static const PCWSTR opcodeName[] =
{
#include "opcode.def"
};

#undef OPDEF
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) args,

static const OPCODE_FORMAT operandDescription[] =
{
#include "opcode.def"
};

enum OPCODE_CONTROL_TYPE
{
    OPCODE_CONTROL_NEXT,
    OPCODE_CONTROL_BRANCH,
    OPCODE_CONTROL_COND_BRANCH,
    OPCODE_CONTROL_CALL,
    OPCODE_CONTROL_RETURN,
    OPCODE_CONTROL_BREAK,
    OPCODE_CONTROL_THROW,
    OPCODE_CONTROL_META,
    OPCODE_CONTROL_PHI
};

#undef OPDEF
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl)      OPCODE_CONTROL_ ## ctrl,

static const OPCODE_CONTROL_TYPE controlTypes[] =
{
#include "opcode.def"
};


//
// table of opcode argument sizes
//

static const BYTE argSizes[] =
{
    #define InlineNone_size           0
    #define ShortInlineVar_size       1
    #define InlineVar_size            2
    #define ShortInlineI_size         1
    #define InlineI_size              4
    #define InlineI8_size             8
    #define ShortInlineR_size         4
    #define InlineR_size              8
    #define ShortInlineBrTarget_size  1
    #define InlineBrTarget_size       4
    #define InlineMethod_size         4
    #define InlineField_size          4
    #define InlineType_size           4
    #define InlineString_size         4
    #define InlineSig_size            4
    #define InlineRVA_size            4
    #define InlineTok_size            4
    #define InlineSwitch_size         ((BYTE) -1)
    #define InlinePhi_size            ((BYTE) -2)
    #define InlineVarTok_size         0       // remove

    #undef OPDEF
    #define OPDEF(name,string,pop,push,oprType,opcType,l,s1,s2,ctrl) oprType ## _size ,
    #include "opcode.def"

    #undef InlineNone_size
    #undef ShortInlineVar_size
    #undef InlineVar_size
    #undef ShortInlineI_size
    #undef InlineI_size
    #undef InlineI8_size
    #undef ShortInlineR_size
    #undef InlineR_size
    #undef ShortInlineBrTarget_size
    #undef InlineBrTarget_size
    #undef InlineMethod_size
    #undef InlineField_size
    #undef InlineType_size
    #undef InlineString_size
    #undef InlineSig_size
    #undef InlineRVA_size
    #undef InlineTok_size
    #undef InlineSwitch_size
    #undef InlinePhi_size
};

/* ------------------------------------------------------------------------- *
 * Useful macros
 * ------------------------------------------------------------------------- */

#define POP_STACK_VALUE(p, t)   (p += sizeof(t), *(t*)(p-sizeof(t)))

/* ------------------------------------------------------------------------- *
 * Disassembly routines
 * ------------------------------------------------------------------------- */

//
// ReadNextOpcode reads the next opcode from the stream and the current
// opcode maps.
// ip           -> instruction pointer to get opcode from.
//  result      <- decoded opcode which should be executed.
//              <- pointer past opcode, to opcode's argument
//
const BYTE *ReadNextOpcode(const BYTE *ip,  DWORD *result)
{
    const BYTE *nextIP = ip;
    DWORD opcode = *nextIP++;

    switch (opcode)
    {
    case CEE_PREFIX1:
        opcode = OPCODE(*nextIP++ + 256);
        break;

    default:
        // !!! error on macro?
        break;
    }

    *result = opcode;
    return (nextIP);
}

//
// SkipIP skips past the opcode argument & returns the address of the
// next instruction in the instruction stream.  Note that this is
// not necessarily the next instruction which will be executed.
//
// ip   -> address of argument for the opcode specified in
//          the instruction stream.  Note that this is not an
//          instruction boundary, it is past the opcode.
//      <- returns a pointer to the  next instruction in the
//          stream.
//

const BYTE *SkipIP(const BYTE *ip, DWORD opcode)
{
    if (opcode == CEE_SWITCH)
    {
        unsigned int numcases = GET_UNALIGNED_VAL32(ip);
        ip += 4;

        return (ip + (numcases*4));
    }
    else
        return (ip + argSizes[opcode]);
}

void DisassembleToken(IMetaDataImport *i,
                      DWORD token,
                      __inout_z WCHAR *buffer)
{
    HRESULT hr;

    switch (TypeFromToken(token))
    {
    default:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"<unknown token type %08x>", TypeFromToken(token));
        break;

    case mdtTypeDef:
        {
            ULONG cLen;
            WCHAR szName[50];

            hr = i->GetTypeDefProps(token, szName, 49, &cLen, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szName, L"<unknown type def>");

            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s", szName);
        }
        break;

    case mdtTypeRef:
        {
            ULONG cLen;
            WCHAR szName[50];

            hr = i->GetTypeRefProps(token, NULL, szName, 49, &cLen);

            if (FAILED(hr))
                wcscpy(szName, L"<unknown type ref>");

            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s", szName);
        }
        break;

    case mdtFieldDef:
        {
            ULONG cLen;
            WCHAR szFieldName[50];
            WCHAR szClassName[50];
            mdTypeDef mdClass;

            hr = i->GetFieldProps(token, &mdClass, szFieldName, 49, &cLen,
                                  NULL, NULL, NULL, NULL, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szFieldName, L"<unknown field def>");

            hr = i->GetTypeDefProps(mdClass, szClassName, 49, &cLen,
                                    NULL, NULL);

            if (FAILED(hr))
                wcscpy(szClassName, L"<unknown type def>");

            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s::%s", szClassName, szFieldName);
        }
        break;

    case mdtMethodDef:
        {
            ULONG cLen;
            WCHAR szFieldName[50];
            WCHAR szClassName[50];
            mdTypeDef mdClass;

            hr = i->GetMethodProps(token, &mdClass, szFieldName, 49, &cLen,
                                   NULL, NULL, NULL, NULL, NULL);

            if (FAILED(hr))
                wcscpy(szFieldName, L"<unknown method def>");

            hr = i->GetTypeDefProps(mdClass, szClassName, 49, &cLen,
                                    NULL, NULL);

            if (FAILED(hr))
                wcscpy(szClassName, L"<unknown type def>");

            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s::%s", szClassName, szFieldName);
        }
        break;

    case mdtMemberRef:
        {
            mdTypeRef cr = mdTypeRefNil;
            LPWSTR pMemberName;
            WCHAR memberName[50];
            ULONG memberNameLen;

            hr = i->GetMemberRefProps(token, &cr, memberName, 49,
                                      &memberNameLen, NULL, NULL);

            if (FAILED(hr))
            {
                pMemberName = L"<unknown member ref>";
            }
            else
                pMemberName = memberName;

            ULONG cLen;
            WCHAR szName[50];

            if(TypeFromToken(cr) == mdtTypeRef)
            {
                if (FAILED(i->GetTypeRefProps(cr, NULL, szName, 50, &cLen)))
                {
                    wcscpy(szName, L"<unknown type ref>");
                }
            }
            else if(TypeFromToken(cr) == mdtTypeDef)
            {
                if (FAILED(i->GetTypeDefProps(cr, szName, 49, &cLen,
                                              NULL, NULL)))
                {
                    wcscpy(szName, L"<unknown type def>");
                }
            }
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s::%s ", szName, pMemberName);
            buffer += wcslen(buffer);
        }
        break;
    }
}

void DisassembleArgument(BYTE *ip,
                         DWORD address,
                         int type,
                         __inout_z WCHAR *buffer)
{
    /*
     * !!! this code isn't processor portable.
     */

    switch (type)
    {
    case InlineNone:
        *buffer = L'\0';
        break;

    case ShortInlineI:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%d", *(char *)ip);
        break;

    case ShortInlineVar:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%u", *(unsigned char *)ip);
        break;

    case InlineVar:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%u", GET_UNALIGNED_VAL16(ip));
        break;

    case InlineI:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%d", GET_UNALIGNED_VAL32(ip));
        break;

    case InlineI8:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%I64d", GET_UNALIGNED_VAL64(ip));
        break;

    case ShortInlineR:
        {
            __int32 Value = GET_UNALIGNED_VAL32(ip);
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%g", (float &)Value);
        }
        break;

    case InlineR:
        {
            __int64 Value = GET_UNALIGNED_VAL64(ip);
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%g", (double &) Value);
        }
        break;

    case ShortInlineBrTarget:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"[%.4x]", address + 1 + *(char *)ip);
        break;

    case InlineBrTarget:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"[%.4x]", address + 4 + GET_UNALIGNED_VAL32(ip));
        break;

    case InlineSwitch:
        {
            DWORD caseCount = GET_UNALIGNED_VAL32(ip);
            ip += 4;

            address += caseCount*4 + 4;

            DWORD index = 0;
            while (index < caseCount)
            {
                int offset = GET_UNALIGNED_VAL32(ip);
                buffer += wcslen(buffer);
                _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%d:[%.4x] ", index, address + offset);
                index++;
                ip += 4;
            }
        }
        break;

    case InlinePhi:
        {
            DWORD caseCount = *(unsigned char*)ip;
            ip += 1;

            DWORD index = 0;
            while (index < caseCount)
            {
                buffer += wcslen(buffer);
                _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%d:[%.4x] ", index, GET_UNALIGNED_VAL16(ip));
                index++;
                ip += 2;
            }
        }
        break;

    case InlineTok:
    case InlineMethod:
    case InlineField:
    case InlineType:
    case InlineSig:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%d", GET_UNALIGNED_VAL32(ip));
        break;

    case InlineString:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%08x", GET_UNALIGNED_VAL32(ip));
        break;

    default:
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"<unknown type %d>", type);
    }
}


/*static*/ SIZE_T DebuggerFunction::WalkInstruction(BOOL native,
                                                    SIZE_T offset,
                                                    BYTE *codeStart,
                                                    BYTE *codeEnd)
{
    BYTE *code = codeStart + offset;

    if (!native)
    {
        DWORD opcode;
        code = (BYTE *) ReadNextOpcode(code, &opcode);
        code = (BYTE *) SkipIP(code, opcode);

        return (code - codeStart);
    }
    else
    {
        return 0xffff;
    }
}

/*static*/ SIZE_T DebuggerFunction::Disassemble(BOOL native,
                                                SIZE_T offset,
                                                BYTE *codeStart,
                                                BYTE *codeEnd,
                                                __inout_z WCHAR *buffer,
                                                BOOL noAddress,
                                                DebuggerModule *module,
                                                BYTE *ilCode)
{
    SIZE_T ret;

    if (!native)
    {
        //
        // Write the address
        //
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"[IL:%.4x] ", offset);
        buffer += wcslen(buffer);

        //
        // Read next opcode
        //
        BYTE *ip = codeStart + offset;
        DWORD opcode;
        BYTE *prevIP = ip;
        ip = (BYTE *) ReadNextOpcode(ip, &opcode);

        //
        // Get the end of the instruction
        //
        BYTE *nextIP = (BYTE *) SkipIP(ip, opcode);

        BYTE *bytes = prevIP;

        //
        // Dump the raw value of the stream
        //
        while (bytes < ip)
        {
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%.2x", *bytes++);
            buffer += wcslen(buffer);
        }
        *buffer++ = ':';
        while (bytes < nextIP)
        {
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%.2x", *bytes++);
            buffer += wcslen(buffer);
        }

        while (bytes++ - prevIP < 8)
        {
            *buffer++ = L' ';
            *buffer++ = L' ';
        }

        //
        // Print the opcode
        //
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"%s\t", opcodeName[opcode]);
        buffer += wcslen(buffer);

        int tokenType = operandDescription[opcode];

        if (tokenType == InlineSwitch)
        {
            *buffer++ = L'\n';

            DWORD caseCount = GET_UNALIGNED_VAL32(ip);
            ip += 4;

            DWORD index = 0;
            while (index < caseCount)
            {
                _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"\t\t\t%.5d:[%.4x]\n", index,
                        GET_UNALIGNED_VAL16(ip));
                buffer += wcslen(buffer);
                index++;
                ip += 4;
            }
        }
        else if (tokenType == InlinePhi)
        {
            *buffer++ = L'\n';

            DWORD caseCount = *(unsigned char*)ip;
            ip += 1;

            DWORD index = 0;
            while (index < caseCount)
            {
                _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"\t\t\t%.5d: [%.4x]\n", index, GET_UNALIGNED_VAL16(ip));
                buffer += wcslen(buffer);
                index++;
                ip += 2;
            }
        }
        else if (tokenType == InlineTok || tokenType == InlineType ||
                 tokenType == InlineField ||  tokenType == InlineMethod)
        {
            DisassembleToken(module->GetMetaData(), GET_UNALIGNED_VAL32(ip), buffer);
            buffer += wcslen(buffer);
        }
        else
        {
            DisassembleArgument(ip, ip - ilCode, tokenType, buffer);
            buffer += wcslen(buffer);
        }

        ret = nextIP - ilCode;
    }
    else
    {
        // Write the address
        if (!noAddress)
        {
            _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"[%.4x] ", offset);
            buffer += wcslen(buffer);
        }

        ret = 0xffff;
        goto Done;
    }

Done:
    // If there's a failure, mark something.
    if (ret == 0xffff)
    {
        _snwprintf(buffer, INT_MAX/sizeof(WCHAR), L"????");
        buffer += wcslen(buffer);
    }

    *buffer++ = L'\n';
    *buffer = L'\0';

    return (ret);
}



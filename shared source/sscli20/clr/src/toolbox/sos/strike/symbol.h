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
#ifndef __symbol_h__
#define __symbol_h__

struct SYM_OFFSET
{
    char *name;
    ULONG offset;
};
    
/* Fill a member of a class if the offset for the symbol exists. */
#define FILLCLASSMEMBER(symOffset, symCount, member, addr)        \
{                                                                 \
    size_t n;                                                     \
    for (n = 0; n < symCount; n ++)                               \
    {                                                             \
        if (strcmp (#member, symOffset[n].name) == 0)             \
        {                                                         \
            if (symOffset[n].offset == -1)                        \
            {                                                     \
                /*dprintf ("offset not exist for %s\n", #member);*/   \
                break;                                            \
            }                                                     \
            memset(&member,0,sizeof(member));                                           \
            move (member, addr+symOffset[n].offset);              \
            break;                                                \
        }                                                         \
    }                                                             \
                                                                  \
    if (n == symCount)                                            \
    {                                                             \
        dprintf ("offset not found for %s\n", #member);           \
        /*return;*/                                               \
    }                                                             \
}

/* Fill a member of a class if the offset for the symbol exists. */
#define FILLCLASSBITMEMBER(symOffset, symCount, preBit, member, addr, size) \
{                                                                 \
    size_t n;                                                     \
    for (n = 0; n < symCount; n ++)                               \
    {                                                             \
        if (strcmp (#member, symOffset[n].name) == 0)             \
        {                                                         \
            if (symOffset[n].offset == -1)                        \
            {                                                     \
                dprintf ("offset not exist for %s\n", #member);   \
                break;                                            \
            }                                                     \
            int csize = size/8;                                   \
            if ((size % 8) != 0) {                                \
                 csize += 1;                                      \
            }                                                     \
            memset ((BYTE*)&preBit+sizeof(preBit),0,csize);       \
            g_ExtData->ReadVirtual(                               \
                (ULONG64)addr+symOffset[n].offset,                \
                (BYTE*)&preBit+sizeof(preBit),                    \
                csize, NULL);                                     \
            break;                                                \
        }                                                         \
    }                                                             \
                                                                  \
    if (n == symCount)                                            \
    {                                                             \
        dprintf ("offset not found for %s\n", #member);           \
        /*return;*/                                               \
    }                                                             \
}

#define FILLPARENT(parent)                                        \
{                                                                 \
    DWORD_PTR dwAddr = dwStartAddr;                               \
    parent::FillFromPDB(dwAddr);                                  \
    if (!CallStatus)                                              \
        return;                                                   \
}

DWORD_PTR GetSymbolType (const char* name, SYM_OFFSET *offset, int count);
ULONG Get1DArrayLength (const char *name);

// Get Name in a enum type for a constant.
// Will allocate buffer in EnumName if succeeds
void NameForEnumValue (const char *EnumType, DWORD_PTR EnumValue, __out char ** EnumName);
#endif

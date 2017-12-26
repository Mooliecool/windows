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
#include "strike.h"
#include "util.h"
#include "disasm.h"
#include <dbghelp.h>
#include <limits.h>

PVOID
GenOpenMapping(
    PCSTR FilePath,
    PULONG Size
    )
{
    HANDLE hFile;
    HANDLE hMappedFile;
    PVOID MappedFile;


    hFile = CreateFileA(
                FilePath,
                GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );

    *Size = GetFileSize(hFile, NULL);
    if (*Size == -1) {
        CloseHandle( hFile );
        return FALSE;
    }
    
    hMappedFile = CreateFileMapping (
                        hFile,
                        NULL,
                        PAGE_READONLY,
                        0,
                        0,
                        NULL
                        );

    if ( !hMappedFile ) {
        CloseHandle ( hFile );
        return FALSE;
    }

    MappedFile = MapViewOfFile (
                        hMappedFile,
                        FILE_MAP_READ,
                        0,
                        0,
                        0
                        );

    CloseHandle (hMappedFile);
    CloseHandle (hFile);

    return MappedFile;
}

char* PrintOneLine (__in __in_z char *begin, __in __in_z char *limit)
{
    if (begin == NULL || begin >= limit) {
        return NULL;
    }
    char line[128];
    size_t length;
    char *end;
    while (1) {
        if (IsInterrupt())
            return NULL;
        length = strlen (begin);
        end = strstr (begin, "\r\xa");
        if (end == NULL) {
            ExtOut ("%s", begin);
            end = begin+length+1;
            if (end >= limit) {
                return NULL;
            }
        }
        else {
            end += 2;
            length = end-begin;
            while (length) {
                if (IsInterrupt())
                    return NULL;
                size_t n = length;
                if (n > 127) {
                    n = 127;
                }
                strncpy_s (line,_countof(line), begin, n);
                line[n] = '\0';
                ExtOut ("%s", line);
                begin += n;
                length -= n;
            }
            return end;
        }
    }
}

void UnassemblyUnmanaged(DWORD_PTR IP)
{
    char            filename[MAX_PATH+1];
    char            line[256];
    int             lcount          = 10;

    ReloadSymbolWithLineInfo();

    ULONG linenum;
    ULONG64 Displacement;
    BOOL fLineAvailable;
    ULONG64 vIP;
    
    fLineAvailable = SUCCEEDED (g_ExtSymbols->GetLineByOffset (IP, &linenum,
                                                               filename,
                                                               MAX_PATH+1,
                                                               NULL,
                                                               &Displacement));
    ULONG FileLines = 0;
    ULONG64* Buffer = NULL;
    ToDestroyCxxArray<ULONG64> des0(&Buffer);

    if (fLineAvailable)
    {
        g_ExtSymbols->GetSourceFileLineOffsets (filename, NULL, 0, &FileLines);
        if (FileLines == 0xFFFFFFFF || FileLines == 0)
            fLineAvailable = FALSE;
    }

    if (fLineAvailable)
    {
        Buffer = new ULONG64[FileLines];
        if (Buffer == NULL)
            fLineAvailable = FALSE;
    }
    
    if (!fLineAvailable)
    {
        vIP = IP;
        // There is no line info.  Just disasm the code.
        while (lcount-- > 0)
        {
            if (IsInterrupt())
                return;
            g_ExtControl->Disassemble (vIP, 0, line, 256, NULL, &vIP);
            ExtOut (line);
        }
        return;
    }

    g_ExtSymbols->GetSourceFileLineOffsets (filename, Buffer, FileLines, NULL);
    
    int beginLine = 0;
    int endLine = 0;
    int lastLine;
    linenum --;
    for (lastLine = linenum; lastLine >= 0; lastLine --) {
        if (IsInterrupt())
            return;
        if (Buffer[lastLine] != DEBUG_INVALID_OFFSET) {
            g_ExtSymbols->GetNameByOffset(Buffer[lastLine],NULL,0,NULL,&Displacement);
            if (Displacement == 0) {
                beginLine = lastLine;
                break;
            }
        }
    }
    if (lastLine < 0) {
        int n = lcount / 2;
        lastLine = linenum-1;
        beginLine = lastLine;
        while (lastLine >= 0) {
            if (IsInterrupt())
                return;
            if (Buffer[lastLine] != DEBUG_INVALID_OFFSET) {
                beginLine = lastLine;
                n --;
                if (n == 0) {
                    break;
                }
            }
            lastLine --;
        }
    }
    while (beginLine > 0 && Buffer[beginLine-1] == DEBUG_INVALID_OFFSET) {
        if (IsInterrupt())
            return;
        beginLine --;
    }
    int endOfFunc = 0;
    for (lastLine = linenum+1; (ULONG)lastLine < FileLines; lastLine ++) {
        if (IsInterrupt())
            return;
        if (Buffer[lastLine] != DEBUG_INVALID_OFFSET) {
            g_ExtSymbols->GetNameByOffset(Buffer[lastLine],NULL,0,NULL,&Displacement);
            if (Displacement == 0) {
                endLine = lastLine;
                break;
            }
            endOfFunc = lastLine;
        }
    }
    if ((ULONG)lastLine == FileLines) {
        int n = lcount / 2;
        lastLine = linenum+1;
        endLine = lastLine;
        while ((ULONG)lastLine < FileLines) {
            if (IsInterrupt())
                return;
            if (Buffer[lastLine] != DEBUG_INVALID_OFFSET) {
                endLine = lastLine;
                n --;
                if (n == 0) {
                    break;
                }
            }
            lastLine ++;
        }
    }

    PVOID MappedBase = NULL;
    ULONG MappedSize = 0;

    class ToUnmap
    {
        PVOID *m_Base;
    public:
        ToUnmap (PVOID *base)
        :m_Base(base)
        {}
        ~ToUnmap ()
        {
            if (*m_Base) {
                UnmapViewOfFile (*m_Base);
                *m_Base = NULL;
            }
        }
    };
    ToUnmap toUnmap(&MappedBase);

#define MAX_SOURCE_PATH 1024
    char Found[MAX_SOURCE_PATH];
    char *pFile;
    if (g_ExtSymbols->FindSourceFile(0, filename,
                       DEBUG_FIND_SOURCE_BEST_MATCH |
                       DEBUG_FIND_SOURCE_FULL_PATH,
                       NULL, Found, sizeof(Found), NULL) != S_OK)
    {
        pFile = filename;
    }
    else
    {
        MappedBase = GenOpenMapping ( Found, &MappedSize );
        pFile = Found;
    }
    
    lastLine = beginLine;
    char *pFileCh = (char*)MappedBase;
    if (MappedBase) {
        ExtOut ("%s\n", pFile);
        int n = beginLine;
        while (n > 0) {
            while (!(pFileCh[0] == '\r' && pFileCh[1] == 0xa)) {
                if (IsInterrupt())
                    return;
                pFileCh ++;
            }
            pFileCh += 2;
            n --;
        }
    }
    
    char filename1[MAX_PATH+1];
    for (lastLine = beginLine; lastLine < endLine; lastLine ++) {
        if (IsInterrupt())
            return;
        if (MappedBase) {
            ExtOut ("%4d ", lastLine+1);
            pFileCh = PrintOneLine (pFileCh, (char*)MappedBase+MappedSize);
        }
        if (Buffer[lastLine] != DEBUG_INVALID_OFFSET) {
            if (MappedBase == 0) {
                ExtOut (">>> %s:%d\n", pFile, lastLine+1);
            }
            vIP = Buffer[lastLine];
            ULONG64 vNextLineIP;
            int i;
            for (i = lastLine + 1; (ULONG)i < FileLines && Buffer[i] == DEBUG_INVALID_OFFSET; i ++) {
                if (IsInterrupt())
                    return;
            }
            if ((ULONG)i == FileLines) {
                vNextLineIP = 0;
            }
            else
                vNextLineIP = Buffer[i];
            while (1) {
                if (IsInterrupt())
                    return;
                g_ExtControl->Disassemble (vIP, 0, line, 256, NULL, &vIP);
                ExtOut (line);
                if (vIP > vNextLineIP || vNextLineIP - vIP > 40) {
                    if (FAILED (g_ExtSymbols->GetLineByOffset (vIP, &linenum,
                                                               filename1,
                                                               MAX_PATH+1,
                                                               NULL,
                                                               &Displacement))) {
                        if (lastLine != endOfFunc) {
                            break;
                        }
                        if (strstr (line, "ret") || strstr (line, "jmp")) {
                            break;
                        }
                    }

                    if (linenum != (ULONG)lastLine+1 || strcmp (filename, filename1)) {
                        break;
                    }
                }
                else if (vIP == vNextLineIP) {
                    break;
                }
            }
        }
    }
        
}


void DisasmAndClean (DWORD_PTR &IP, __out_ecount (length) __out_opt char *line, ULONG length)
{
    ULONG64 vIP = IP;
    g_ExtControl->Disassemble (vIP, 0, line, length, NULL, &vIP);
    IP = (DWORD_PTR)vIP;
    // remove the ending '\n'
    char *ptr = strrchr (line, '\n');
    if (ptr != NULL)
        ptr[0] = '\0';
}

// If byref, move to pass the byref prefix
BOOL IsByRef (__deref_inout_z char *& ptr)
{
    BOOL bByRef = FALSE;
    if (ptr[0] == '[')
    {
        bByRef = TRUE;
        ptr ++;
    }
    else if (!strncmp (ptr,
                       "dword ptr [", 11
                       ))
    {
        bByRef = TRUE;
        ptr += 11;
    }
    return bByRef;
}

BOOL IsTermSep (char ch)
{
    return (ch == '\0' || isspace (ch) || ch == ',' || ch == '\n'
            );
}

// Find next term. A term is seperated by space or ,
void NextTerm (__deref_inout_z char *& ptr)
{
    // If we have a byref, skip to ']'
    if (IsByRef (ptr))
    {
        while (ptr[0] != ']' && ptr[0] != '\0')
        {
            if (IsInterrupt())
                return;
            ptr ++;
        }
        if (ptr[0] == ']')
            ptr ++;
    }
    
    while (!IsTermSep (ptr[0]))
    {
        if (IsInterrupt())
            return;
        ptr ++;
    }

    while (IsTermSep(ptr[0]) && (*ptr != '\0'))
    {
        if (IsInterrupt())
            return;
        ptr ++;
    }
}


// Parses something like 6e24d310.  On 64-bit, also parses things like
// 000006fb`f9b70f50 and 000006fbf9b70f50.
INT_PTR ParseHexNumber (__in __in_z char *ptr, __out char **endptr)
{
    char *endptr1;
    INT_PTR value1 = strtoul(ptr, &endptr1, 16);


    *endptr = endptr1;
    return value1;
}


// only handle pure value, or memory address
INT_PTR GetValueFromExpr(__in __in_z char *ptr, INT_PTR &value)
{
    BOOL bNegative = FALSE;
    value = 0;
    char *myPtr = ptr;
    BOOL bByRef = IsByRef (myPtr);

    if (myPtr[0] == '-')
    {
        myPtr ++;
        bNegative = TRUE;
    }
    if (!strncmp (myPtr, "0x", 2) || isxdigit (myPtr[0]))
    {
        char *endptr;
        value = ParseHexNumber(myPtr, &endptr);
        if ((IsTermSep (endptr[0]) && !bByRef)
            || (endptr[0] == ']' && bByRef))
        {
            if (bNegative)
                value = -value;
            ptr = endptr;
            if (bByRef)
            {
                ptr += 1;
                SafeReadMemory (value, &value, 4, NULL);
            }
            return ptr - myPtr;
        }
    }

    // handle mscorlib+0xed310 (6e24d310)
    if (!bByRef)
    {
        ptr = myPtr;
        while (ptr[0] != ' ' && ptr[0] != '+' && ptr[0] != '\0')
        {
            if (IsInterrupt())
                return 0;
            ptr ++;
        }
        if (ptr[0] == '+')
        {
            NextTerm (ptr);
            if (ptr[0] == '(')
            {
                ptr ++;
                char *endptr;
                value = ParseHexNumber(ptr, &endptr);
                if (endptr[0] == ')')
                {
                    ptr ++;
                    return ptr - myPtr;
                }
            }
        }
    }
    if (bByRef)
    {
        // handle dword [mscorlib+0x2bd788 (02ead788)]
        ptr = myPtr;
        while (ptr[0] != '(' && ptr[0] != '\0')
        {
            if (IsInterrupt())
                return 0;
            ptr ++;
        }
        if (ptr[0] == '(')
        {
            ptr ++;
            char *endptr;
            value = ParseHexNumber(ptr, &endptr);
            if (endptr[0] == ')' && endptr[1] == ']')
            {
                ptr = endptr + 2;
                SafeReadMemory (value, &value, 4, NULL);
                return ptr - myPtr;
            }
        }
    }


    return 0;
}


const char * HelperFuncName (size_t IP)
{
    static char s_szHelperName[100];
    if (S_OK == DacpJitHelperFunctionData::GetName(g_clrData, IP, sizeof(s_szHelperName), &s_szHelperName[0]))
        return &s_szHelperName[0];
    else
        return NULL;
}

void SOSEHInfo::FormatForDisassembly(CLRDATA_ADDRESS offSet)
{                
    for (UINT i=0; i < EHCount; i++)
    {
        DACEHInfo *pCur = m_pInfos + i;

        if (pCur->isDuplicateClause)
        {
            // Don't print anything for duplicate clauses
            continue;
        }
        
        if (offSet == pCur->tryStartOffset)
        {
            ExtOut ("EHHandler %d: %s", i, EHTypeName(pCur->clauseType));
            if ((pCur->clauseType == EHTyped) && pCur->isCatchAllHandler)
            {                     
                ExtOut (" catch(...)");                     
            }
            ExtOut (" CLAUSE BEGIN\n");
        }

        if (offSet == pCur->tryEndOffset)
        {
            ExtOut ("EHHandler %d: %s CLAUSE END\n", i, EHTypeName(pCur->clauseType));
        }

        if (offSet == pCur->handlerStartOffset)
        {
            ExtOut ("EHHandler %d: %s HANDLER BEGIN\n", i, EHTypeName(pCur->clauseType));
        }

        if (offSet == pCur->handlerEndOffset)
        {
            ExtOut ("EHHandler %d: %s HANDLER END\n", i, EHTypeName(pCur->clauseType));
        }

        if ((pCur->clauseType == EHFilter) &&
            (offSet == pCur->filterOffset))
        {
            ExtOut ("EHHandler %d: %s FILTER BEGIN\n",i, EHTypeName(pCur->clauseType));
        }
    }
}



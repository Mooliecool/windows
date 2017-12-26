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
// File: IniFile.CPP
// 
// A simple API for reading/writing .ini files
//
// It is not designed to be 100% replacement for Win32's Read[Private]ProfileString/
// Write[Private]ProfileString.
//
// It does not provide any smart caching nor locking. It is up to the caller
// to provide it. Also, the grammar for the .ini files probably differs 
// in corner cases.
// ===========================================================================

#include "rotor_palrt.h"

#define CONFIG_FILE_NAME   L"\\rotor.ini"

// The underlaying datastructure is a DOM-like tree
struct Node
{
    Node*   pNext;
    Node*   pChild;
    DWORD   dwSize;
    BYTE    Value[1];
};

// Smart buffer
class CBlobBuilder
{
    BYTE* m_pData; // the data storage
    ULONG m_nSize; // the current size of the string
    ULONG m_nData; // the size of the allocated data storage
    bool  m_fError; // an out-of-memory error occured

private:
    BYTE* Ensure(DWORD nNewData)
    {
        if (m_fError)
            return NULL;

        // grow 2x for smaller sizes, 1.25x for bigger sizes
        DWORD n = m_nData;
        n = min(2 * n, n + n / 4 + 0x10000);

        // don't allocate tiny chunks
        n = max(n, 0x80);

        // compare with the hard limit
        nNewData = max(n, m_nSize + nNewData);

        BYTE * pNewData = (BYTE*)realloc(m_pData, nNewData);
        if (pNewData == NULL)
        {
            m_fError = true;
            return pNewData;
        }

        m_nData = nNewData;
        m_pData = pNewData;
        return pNewData;
    }

    static const WCHAR s_newline[2];

public:
    void Init()
    {
        m_pData = 0;
        m_nSize = 0;
        m_nData = 0;
        m_fError = false;
    }

    void Destroy()
    {
        free(m_pData);
    }

    void Append(LPCVOID pData, ULONG nSize)
    {
        ULONG nNewSize = m_nSize + nSize;

        if (nNewSize > m_nData)
        {
            if (!Ensure(nNewSize))
                return;
        }

        BYTE *p = m_pData + m_nSize;
        m_nSize = nNewSize;
        memcpy(p, pData, nSize);
    }

    void AppendChar(WCHAR ch)
    {
        Append(&ch, sizeof(WCHAR));
    }

    void AppendString(const WCHAR* pString, DWORD dwLength)
    {
        Append((LPCVOID)pString, dwLength * sizeof(WCHAR));
    }

    void AppendNewLine()
    {
        Append(s_newline, sizeof(s_newline));
    }

    void AppendNode(const Node* pNode)
    {
        Append((LPCVOID)&pNode->Value, pNode->dwSize);
    }

    bool IsError() const
        { return m_fError; }

    PVOID GetBuffer()
        { return m_pData; }

    ULONG GetSize()
        { return m_nSize; }
};

const WCHAR CBlobBuilder::s_newline[2] = { '\r', '\n' };

// The ini file implementation

class IniFile
{
    enum Action
    {
        CREATE_NODE,
        DELETE_NODE,
        FIND_NODE,
    };

    // the root node
    Node* m_pRoot;
    BOOL m_fDirty;

protected:
    // create a free floating node with the given value
    Node* CreateNode(LPCVOID pValue, DWORD dwSize)
    {
        Node* pNode = (Node*)malloc(FIELD_OFFSET(Node, Value) + dwSize);
        if (pNode == NULL)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }
        m_fDirty = TRUE;

        pNode->pNext = NULL;
        pNode->pChild = NULL;
        pNode->dwSize = dwSize;

        memcpy(&pNode->Value, pValue, dwSize);

        return pNode;
    }

    // destroy node and all its childs
    void DestroyNode(Node* pNode)
    {
        while (pNode != NULL)
        {
            if (pNode->pChild != NULL)
                DestroyNode(pNode->pChild);

            Node* pTmp = pNode;
            pNode = pNode->pNext;
            free(pTmp);
        }
    }

    // does a node hold the given value?
    static BOOL Equal(const Node* pNode, LPCVOID pValue, DWORD dwSize)
    {
        return (pNode->dwSize == dwSize) && 
            (memcmp(&pNode->Value, pValue, dwSize) == 0);
    }

    // find a node with the given value, create one if it does not exist
    //  and fCreate is specified
    Node* GetNode(Node** ppNode, LPCVOID pValue, DWORD dwSize, Action action)
    {
        Node* pNode = *ppNode;

        while (pNode != NULL)
        {
            if (Equal(pNode, pValue, dwSize))
                break;
            ppNode = &pNode->pNext;
            pNode = *ppNode;
        }

        switch (action)
        {
        case CREATE_NODE:
            if (pNode == NULL)
            {
                pNode = CreateNode(pValue, dwSize);
                *ppNode = pNode;
            }
            break;

        case DELETE_NODE:
            if (pNode != NULL)
            {
                *ppNode = pNode->pNext;
                pNode->pNext = NULL;
                DestroyNode(pNode);
            }
            break;

        default:
            _ASSERTE(action == FIND_NODE);
        }

        return pNode;
    }

    // find a node with the given string value, create one if it does not exist
    //  and fCreate is specified
    Node* GetNode(Node** ppNode, LPCWSTR pszValue, Action action)
    {
        return GetNode(ppNode, (LPCVOID)pszValue, sizeof(WCHAR) * wcslen(pszValue), action);
    }

public:
    IniFile()
    {
        m_pRoot = NULL;
        m_fDirty = FALSE;
    }

    ~IniFile()
    {
        DestroyNode(m_pRoot);
    }

    inline static bool IsJunk(int c)
        { return c <= 0x20; }

    inline static bool IsEndOfLine(int c)
        { return (c == '\n') || (c == '\r'); }

    BOOL Load(LPCWSTR pszFileName)
    {
        DWORD dwSize, dwRead;
        DWORD dwSizeHigh;
        BOOL fRet = FALSE;
        PVOID pReadBuffer = NULL;
        WCHAR* pBuffer = NULL;
        Node* pSection = NULL;
        Node* pKey;
        WCHAR* p;
        WCHAR* pEnd;

        // read the ini file
        HANDLE hFile = CreateFileW(pszFileName, 
            GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        dwSize = GetFileSize(hFile, &dwSizeHigh);
        if ((dwSizeHigh == INVALID_FILE_SIZE) && GetLastError() != NO_ERROR)
            goto Error;
        if (dwSizeHigh != 0)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Error;
        }

        pReadBuffer = malloc(dwSize);
        if (pReadBuffer == NULL)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            goto Error;
        }

        if (!ReadFile(hFile, pReadBuffer, dwSize, &dwRead, NULL))
            goto Error;
        if (dwRead != dwSize)
        {
            SetLastError(ERROR_READ_FAULT);
            goto Error;
        }

        pBuffer = (WCHAR*)malloc(dwSize * sizeof(WCHAR));
        if (pBuffer == NULL)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            goto Error;
        }

        dwSize = MultiByteToWideChar(CP_UTF8, 0, 
            (LPCSTR)pReadBuffer, dwSize, pBuffer, dwSize);

        p = pBuffer;
        pEnd = pBuffer + dwSize;
        for (;;)
        {
            WCHAR *pValueStart;
            WCHAR *pValueEnd;

#define TEST(cond) ((p < pEnd) && (cond))
#define PARSE_UNTIL(ch) while (TEST((*p != ch) && !IsJunk(*p))) p++
#define PARSE_WHILE(pred) while (TEST(pred(*p))) p++

            PARSE_WHILE(IsJunk);

            // are we done?
            if (p >= pEnd)
                break;

            if (TEST(*p == '['))
            {
                p++;
                PARSE_WHILE(IsJunk);

                // we've got the section name
                pValueStart = p;
                PARSE_UNTIL(']');
                pValueEnd = p;

                PARSE_WHILE(IsJunk);

                // better have closing bracket now
                if (!TEST(*p == ']'))
                    goto ParseError;
                p++;

                pSection = GetNode(&m_pRoot, 
                    (LPCVOID)pValueStart, (BYTE*)pValueEnd - (BYTE*)pValueStart, CREATE_NODE);
                if (pSection == NULL)
                    goto Error;

                continue;
            }
            else
            if (TEST(*p == ';') || TEST(*p == '#') || TEST(*p == '!'))
            {
                p++;
                // everything up to the end of the line is comment - ignored
                PARSE_WHILE(!IsEndOfLine);
                continue;
            }
            else
            {                
                // we've got the key value pair
                pValueStart = p;
                PARSE_UNTIL('=');
                pValueEnd = p;

                PARSE_WHILE(IsJunk);

                // better have the equal sign now
                if (!TEST(*p == '='))
                    goto ParseError;
                p++;

                // no section to throw this in
                if (pSection == NULL)
                    goto ParseError;

                pKey = GetNode(&(pSection->pChild), 
                    (LPCVOID)pValueStart, (BYTE*)pValueEnd - (BYTE*)pValueStart, CREATE_NODE);
                if (pKey == NULL)
                    goto Error;

                // the value is everything till end of the line
                pValueStart = p;
                PARSE_WHILE(!IsEndOfLine);
                pValueEnd = p;

                // duplicate node
                if (pKey->pChild != NULL)
                    goto ParseError;

                pKey->pChild = CreateNode((LPCVOID)pValueStart, (BYTE*)pValueEnd - (BYTE*)pValueStart);
                if (pKey->pChild == NULL)
                    goto Error;

                continue;
            }

        ParseError:
            // We don't support more graceful recovery by skipping the current
            // line. Instead we bail out at the first instance of parse error.
            SetLastError(ERROR_FILE_INVALID);
            goto Error;
        }

        m_fDirty = FALSE;

        fRet = TRUE;

    Error:
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        free(pReadBuffer);
        free(pBuffer);

        return fRet;
    }

    BOOL Save(LPCWSTR pszFileName, BOOL fForce)
    {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        CBlobBuilder builder;
        BOOL fRet = FALSE;
        PVOID pBuffer = NULL;
        DWORD dwSize, dwWritten;

        if (!fForce && !m_fDirty)
            return TRUE;

        builder.Init();

        // for all sections
        Node* pSection = m_pRoot;
        while (pSection != NULL)
        {
            // append the section name
            builder.AppendChar('[');
            builder.AppendNode(pSection);
            builder.AppendChar(']');
            builder.AppendNewLine();

            // for all keys
            Node* pKey = pSection->pChild;
            while (pKey != NULL)
            {
                // append the key-value pair
                builder.AppendNode(pKey);
                builder.AppendChar('=');

                Node* pValue = pKey->pChild;
                if (pValue != NULL)
                {
                    builder.AppendNode(pValue);
                    builder.AppendNewLine();
                }
                pKey = pKey->pNext;
            }

            pSection = pSection->pNext;
            if (pSection != NULL)
                builder.AppendNewLine();
        }

        // did we run out of memory?
        if (builder.IsError())
            goto Error;

        // convert to UTF-8 (one Unicode char can be up to 3 UTF-8 bytes)
        dwSize = 3 * (builder.GetSize() / sizeof(WCHAR));
        pBuffer = malloc(dwSize);
        if (pBuffer == NULL)
            goto Error;

        dwSize = WideCharToMultiByte(CP_UTF8, 0,
            (LPCWSTR)builder.GetBuffer(), builder.GetSize() / sizeof(WCHAR),
            (LPSTR)pBuffer, dwSize, 0, 0);

        // write it to the disk
        hFile = CreateFileW(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            goto Error;

        if (!WriteFile(hFile, pBuffer, dwSize, &dwWritten, NULL))
            goto Error;

        if (dwWritten != dwSize)
        {
            SetLastError(ERROR_WRITE_FAULT);
            goto Error;
        }

        m_fDirty = FALSE;

        fRet = TRUE;

    Error:
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        free(pBuffer);
        builder.Destroy();

        return fRet;
    }

    DWORD ReadString(
        LPCWSTR pszSection,
        LPCWSTR pszKey,
        LPWSTR pszValue,
        DWORD dwBufLength)
    {
        Node* pNode;

        if (pszSection == NULL)
        {
            pNode = m_pRoot;
            goto SetOfValues;
        }

        pNode = GetNode(&m_pRoot, pszSection, FIND_NODE);
        if (pNode == NULL)
            goto SetOfValues;

        if (pszKey == NULL)
        {
            pNode = pNode->pChild;
            goto SetOfValues;
        }

        pNode = GetNode(&pNode->pChild, pszKey, FIND_NODE);
        if (pNode == NULL)
            goto SetOfValues;

        pNode = pNode->pChild;

SetOfValues:
        DWORD dwTotalLength = 0;
        BOOL fMultiple = (pszSection == NULL) || (pszKey == NULL) || (pNode == NULL);

        // Save space for the terminating '\0'
        if (fMultiple)
        {
            if (dwBufLength == 0)
                return 0;
            dwBufLength--;
        }

        while (pNode != NULL)
        {
            DWORD dwLength = pNode->dwSize / sizeof(WCHAR);

            if (dwLength >= dwBufLength)
            {
                if (dwBufLength == 0)
                    break;

                dwLength = dwBufLength - 1;
            }

            memcpy(pszValue, &pNode->Value, dwLength * sizeof(WCHAR));
            *(pszValue + dwLength) = '\0';

            pszValue += (dwLength + 1);
            dwBufLength -= (dwLength + 1);

            dwTotalLength += dwLength;

            if (!fMultiple)
                break;

            // count the '\0' separator
            dwTotalLength++;

            pNode = pNode->pNext;
        }

        if (fMultiple)
        {
            // terminating '\0'
            *pszValue = '\0';
        }

        return dwTotalLength;
    }

    BOOL WriteString(
        LPCWSTR pszSection,
        LPCWSTR pszKey,
        LPCWSTR pszValue)
    {
        Node* pSection = GetNode(&m_pRoot, pszSection, (pszKey != NULL) ? CREATE_NODE : DELETE_NODE);
        if (pszKey == NULL)
            return TRUE;
        if (pSection == NULL)
            return FALSE;

        Node* pKey = GetNode(&pSection->pChild, pszKey, (pszValue != NULL) ? CREATE_NODE : DELETE_NODE);
        if (pszValue == NULL)
            return TRUE;
        if (pKey == NULL)
            return FALSE;

        Node* pValue = pKey->pChild;
        DWORD dwSize = sizeof(WCHAR) * wcslen(pszValue);
        if (pValue)
        {
            if (Equal(pValue, (LPCVOID)pszValue, dwSize))
                return TRUE;
            DestroyNode(pValue);
        }

        pKey->pChild = CreateNode((LPCVOID)pszValue, dwSize);
        if (pKey->pChild == NULL)
            return FALSE;

        return TRUE;
    }

    BOOL GetString(
        LPCWSTR pszSection,
        LPCWSTR pszKey,
        LPWSTR pszValue,
        DWORD dwBufLength)
    {
        Node* pNode;

        pNode = GetNode(&m_pRoot, pszSection, FIND_NODE);
        if (pNode == NULL)
            goto NotFound;

        pNode = GetNode(&pNode->pChild, pszKey, FIND_NODE);
        if (pNode == NULL)
            goto NotFound;

        pNode = pNode->pChild;
        if (pNode == NULL)
        {
    NotFound:
            SetLastError(NOERROR);
            return FALSE;
        }

        DWORD dwLength = pNode->dwSize / sizeof(WCHAR);

        if (dwLength >= dwBufLength)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }

        memcpy(pszValue, &pNode->Value, dwLength * sizeof(WCHAR));
        *(pszValue + dwLength) = '\0';
        return TRUE;
    }
};

// PAL API Wrappers

HINI PALAPI PAL_IniCreate()
{
    IniFile* pIniFile = new IniFile();

    if (pIniFile == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return NULL;
    }

    return (HINI)pIniFile;
}

BOOL PALAPI PAL_IniClose(HINI hIni)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    delete (IniFile*)hIni;
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
PAL_IniLoad(
    HINI hIni, 
    LPCWSTR pszFileName)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((IniFile*)hIni)->Load(pszFileName);
}

EXTERN_C
BOOL
PALAPI
PAL_IniSave(
    HINI hIni, 
    LPCWSTR pszFileName,
    BOOL fForce)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((IniFile*)hIni)->Save(pszFileName, fForce);
}

EXTERN_C
DWORD
PALAPI
PAL_IniReadString(
    HINI hIni,
    LPCWSTR pszSection,
    LPCWSTR pszKey,
    LPWSTR pszValue,
    DWORD nSize)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((IniFile*)hIni)->ReadString(pszSection, pszKey, pszValue, nSize);
}

EXTERN_C
BOOL
PALAPI
PAL_IniWriteString(
    HINI hIni,
    LPCWSTR pszSection,
    LPCWSTR pszKey,
    LPCWSTR pszValue)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((IniFile*)hIni)->WriteString(pszSection, pszKey, pszValue);
}

EXTERN_C
BOOL 
PALAPI
PAL_IniGetString(
     HINI hIni,
     LPCWSTR pszSection,
     LPCWSTR pszKey,
     LPWSTR pszValue,
     DWORD dwBufLength)
{
    if (hIni == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((IniFile*)hIni)->GetString(pszSection, pszKey, pszValue, dwBufLength);
}


EXTERN_C
BOOL
PALAPI
PAL_FetchConfigurationStringW(
                    IN BOOL bPerMachine,
                    IN LPCWSTR lpParameterName,
                    OUT LPWSTR lpParameterValue,
                    IN UINT cbParameterValueLength)
{
    BOOL b;
    HINI hIni = NULL;
    WCHAR ConfigFileName[_MAX_PATH];
    DWORD cch = sizeof(ConfigFileName)/sizeof(ConfigFileName[0]);

    if (bPerMachine) {
        b = PAL_GetMachineConfigurationDirectoryW(ConfigFileName, cch);
    } else {
        b = PAL_GetUserConfigurationDirectoryW(ConfigFileName, cch);
    }

    if (!b) {
        goto LExit;
    }

    if (wcslen(ConfigFileName)+sizeof(CONFIG_FILE_NAME)/sizeof(WCHAR) >=
        _MAX_PATH) {
        // buffer would overflow
        b = FALSE;
        goto LExit;
    }
    wcscat(ConfigFileName, CONFIG_FILE_NAME);

    hIni = PAL_IniCreate();
    if (!hIni) {
        b = FALSE;
        goto LExit;
    }

    b = PAL_IniLoad(hIni, ConfigFileName);
    if (b == FALSE) {
        goto LExit;
    }

    b = PAL_IniGetString(hIni,
                         L"Rotor",
                         lpParameterName,
                         lpParameterValue,
                         cbParameterValueLength);

LExit:
    if (hIni) {
        PAL_IniClose(hIni);
    }
    return b;
}

EXTERN_C
BOOL
PALAPI
PAL_SetConfigurationStringW(
                    IN BOOL bPerMachine,
                    IN LPCWSTR lpParameterName,
                    IN LPCWSTR lpParameterValue)
{
    BOOL b;
    HINI hIni = NULL;
    WCHAR ConfigFileName[_MAX_PATH];
    DWORD cch = sizeof(ConfigFileName)/sizeof(ConfigFileName[0]);

    if (bPerMachine) {
        b = PAL_GetMachineConfigurationDirectoryW(ConfigFileName, cch);
    } else {
        b = PAL_GetUserConfigurationDirectoryW(ConfigFileName, cch);
    }

    if (!b) {
        goto LExit;
    }

    if (wcslen(ConfigFileName)+sizeof(CONFIG_FILE_NAME)/sizeof(WCHAR) >=
        _MAX_PATH) {
        // buffer would overflow
        b = FALSE;
        goto LExit;
    }
    wcscat(ConfigFileName, CONFIG_FILE_NAME);

    hIni = PAL_IniCreate();
    if (!hIni) {
        b = FALSE;
        goto LExit;
    }

    if (GetFileAttributesW(ConfigFileName) != 0xffffffff) {
        b = PAL_IniLoad(hIni, ConfigFileName);
        if (b == FALSE) {
            goto LExit;
        }
    }

    b = PAL_IniWriteString(hIni,
                         L"Rotor",
                         lpParameterName,
                         lpParameterValue);
        
        if (b == FALSE) {
        goto LExit;
    }
    
    b = PAL_IniSave(hIni, ConfigFileName, FALSE);

LExit:
    if (hIni) {
        PAL_IniClose(hIni);
    }
    return b;
}


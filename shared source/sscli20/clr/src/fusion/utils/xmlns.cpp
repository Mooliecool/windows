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
#include "fusionp.h"
#include "xmlns.h"
#include "helpers.h"
#include "util.h"

//
// CNamespaceMapNode
//

CNamespaceMapNode::CNamespaceMapNode()
: _pwzPrefix(NULL)
, _pwzURI(NULL)
, _dwDepth(0)
{
}

CNamespaceMapNode::~CNamespaceMapNode()
{
    SAFEDELETEARRAY(_pwzPrefix);
    SAFEDELETEARRAY(_pwzURI);
}

HRESULT CNamespaceMapNode::Create(LPCWSTR pwzPrefix, LPCWSTR pwzURI,
                                  DWORD dwCurDepth, CNamespaceMapNode **ppMapNode)
{
    HRESULT                                      hr = S_OK;
    CNamespaceMapNode                           *pMapNode = NULL;

    if (!pwzURI || !ppMapNode) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppMapNode = NULL;

    pMapNode = NEW(CNamespaceMapNode);
    if (!pMapNode) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if (pwzPrefix) {
        pMapNode->_pwzPrefix = WSTRDupDynamic(pwzPrefix);
        if (!pMapNode->_pwzPrefix) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    pMapNode->_pwzURI = WSTRDupDynamic(pwzURI);
    if (!pMapNode->_pwzURI) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pMapNode->_dwDepth = dwCurDepth;

    *ppMapNode = pMapNode;

Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pMapNode);
    }

    return hr;
}

//
// CNamespaceManager
//

CNamespaceManager::CNamespaceManager()
: _dwCurDepth(0)
, _hrResult(S_OK)
{
}

CNamespaceManager::~CNamespaceManager()
{
    LISTNODE                                      pos;
    LISTNODE                                      posStack;
    CNamespaceMapNode                            *pMapNode;
    NamespaceStack                               *pStack;
    int                                           i;

    // Default namespace stack cleanup

    pos = _stackDefNamespace.GetHeadPosition();
    while (pos) {
        // Should be empty, if successful

        _ASSERTE(FAILED(_hrResult));

        pMapNode = _stackDefNamespace.GetNext(pos);
        _ASSERTE(pMapNode);

        SAFEDELETE(pMapNode);
    }

    _stackDefNamespace.RemoveAll();

    // Prefix stack clean up

    for (i = 0; i < NAMESPACE_HASH_TABLE_SIZE; i++) {
        pos = _listMap[i].GetHeadPosition();

        // Table should be empty, if successful

        while (pos) {
            _ASSERTE(FAILED(_hrResult));

            pStack = _listMap[i].GetNext(pos);
            _ASSERTE(pStack);

            // Clean up the stack entries
            
            posStack = pStack->GetHeadPosition();
            _ASSERTE(posStack);

            while (posStack) {
                // We didn't get cleaned up properly!
                
                pMapNode = pStack->GetNext(posStack);
                _ASSERTE(pMapNode);

                SAFEDELETE(pMapNode);
            }

            pStack->RemoveAll();

            // Clean up the stack

            SAFEDELETE(pStack)
        }
    }
}

HRESULT CNamespaceManager::OnCreateNode(IXMLNodeSource __RPC_FAR *pSource,
                                        PVOID pNodeParent, USHORT cNumRecs,
                                        XML_NODE_INFO __RPC_FAR **aNodeInfo)
{
    HRESULT                                     hr = S_OK;
    LPWSTR                                      pwzURI = NULL;
    CNamespaceMapNode                          *pMapNode = NULL;
    LPWSTR                                      pwzPrefix = NULL;
    BOOL                                        bFound;
    DWORD                                       dwHash;
    LISTNODE                                    pos;
    LISTNODE                                    posStack;
    NamespaceStack                             *pStack = NULL;
    NamespaceStack                             *pStackCur = NULL;
    CNamespaceMapNode                          *pMapNodeCur = NULL;
    int                                         iLen;
    USHORT                                      idx = 1;

    if (aNodeInfo[0]->dwType == XML_ELEMENT) {
        _dwCurDepth++;
    }

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            if (aNodeInfo[idx]->ulLen == XML_NAMESPACE_TAG_LEN &&
                !FusionCompareStringN(aNodeInfo[idx]->pwcText, XML_NAMESPACE_TAG, XML_NAMESPACE_TAG_LEN)) {

                // This is in the default namespace

                hr = ::ExtractXMLAttribute(&pwzURI, aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }

                hr = CNamespaceMapNode::Create(NULL, (pwzURI) ? (pwzURI) : (L""), _dwCurDepth, &pMapNode);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (!_stackDefNamespace.AddHead(pMapNode))
                {
                    hr = E_OUTOFMEMORY;
                    SAFEDELETE(pMapNode);
                    goto Exit;
                }

                SAFEDELETEARRAY(pwzURI);
                pMapNode = NULL;

                continue;
            }
            else if (aNodeInfo[idx]->ulLen >= XML_NAMESPACE_TAG_LEN &&
                     !FusionCompareStringN(aNodeInfo[idx]->pwcText, XML_NAMESPACE_PREFIX_TAG, XML_NAMESPACE_PREFIX_TAG_LEN)) {

                // This is a namespace prefix

                iLen = aNodeInfo[idx]->ulLen - XML_NAMESPACE_PREFIX_TAG_LEN;
                _ASSERTE(iLen > 0);

                pwzPrefix = NEW(WCHAR[iLen + 1]);
                if (!pwzPrefix) {
                    hr = E_OUTOFMEMORY;
                    SAFEDELETEARRAY(pwzURI);
                    goto Exit;
                }

                hr = StringCchCopyN(pwzPrefix, iLen + 1, aNodeInfo[idx]->pwcText + XML_NAMESPACE_PREFIX_TAG_LEN, iLen);
                if (FAILED(hr)) {
                    goto Exit;
                }
                
                hr = ::ExtractXMLAttribute(&pwzURI, aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (!pwzURI || !lstrlenW(pwzURI)) {
                    // It is illegal to have the form:
                    //    <tag xmlns:foo="">
                    // Error out in this case

                    hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                    goto Exit;
                }

                hr = CNamespaceMapNode::Create(pwzPrefix, pwzURI, _dwCurDepth, &pMapNode);
                if (FAILED(hr)) {
                    goto Exit;
                }

                dwHash = HashString(pwzPrefix, 0, NAMESPACE_HASH_TABLE_SIZE, TRUE);

                pos = _listMap[dwHash].GetHeadPosition();
                if (!pos) {
                    // No entries at this hash table location. Make a stack
                    // at this location, and add the node.

                    pStack = NEW(NamespaceStack);
                    if (!pStack) {
                        hr = E_OUTOFMEMORY;
                        SAFEDELETE(pMapNode);
                        goto Exit;
                    }

                    if (!pStack->AddHead(pMapNode))
                    {
                        hr = E_OUTOFMEMORY;
                        SAFEDELETE(pStack);
                        SAFEDELETE(pMapNode);
                        goto Exit;
                    }

                    if (!_listMap[dwHash].AddHead(pStack))
                    {
                        hr = E_OUTOFMEMORY;
                        SAFEDELETE(pMapNode);
                        SAFEDELETE(pStack);
                        goto Exit;
                    }
                }
                else {
                    // Each node here represents a hash collision.
                    // Every node is a stack for a particular prefix. Find
                    // the prefix we want, and add to the stack, or add
                    // a new node.

                    bFound = FALSE;
                    while (pos) {
                        // Get the stack

                        pStackCur = _listMap[dwHash].GetNext(pos);
                        _ASSERTE(pStackCur);

                        // Get the first entry in the stack

                        posStack = pStackCur->GetHeadPosition();
                        _ASSERTE(posStack);
                        if (!posStack) {
                            continue;
                        }

                        // Get the head of the stack

                        pMapNodeCur = pStackCur->GetAt(posStack);
                        _ASSERTE(pMapNodeCur);

                        // See if the node at the head of the stack has the
                        // prefix we're interested in

                        if (!FusionCompareString(pMapNodeCur->_pwzPrefix, pwzPrefix)) {
                            // We found the right stack. Push node onto stack.

                            if (!pStackCur->AddHead(pMapNode))
                            {
                                hr = E_OUTOFMEMORY;
                                delete pMapNode;
                                goto Exit;
                            }

                            bFound = TRUE;
                            break;
                        }
                    }

                    if (!bFound) {
                        // We had a hash collision on the prefix,
                        // although the stack for this prefix hasn't been
                        // created yet.

                        pStack = NEW(NamespaceStack);
                        if (!pStack) {
                            hr = E_OUTOFMEMORY;
                            SAFEDELETE(pMapNode);
                            goto Exit;
                        }
                        
                        if (!pStack->AddHead(pMapNode))
                        {
                            hr = E_OUTOFMEMORY;
                            SAFEDELETE(pStack);
                            SAFEDELETE(pMapNode);
                            goto Exit;
                        }

                        if (!_listMap[dwHash].AddHead(pStack))
                        {
                            hr = E_OUTOFMEMORY;
                            SAFEDELETE(pMapNode);
                            SAFEDELETE(pStack);
                            goto Exit;
                        }   
                    }
                }

                SAFEDELETEARRAY(pwzPrefix);
                SAFEDELETEARRAY(pwzURI);

                pMapNode = NULL;
                continue;
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    SAFEDELETEARRAY(pwzPrefix);
    SAFEDELETEARRAY(pwzURI);

    if (FAILED(hr)) {
        _hrResult = hr;
    }

    return hr;
}

HRESULT CNamespaceManager::OnEndChildren()
{
    HRESULT                                          hr = S_OK;
    LISTNODE                                         pos;
    LISTNODE                                         curPos;
    LISTNODE                                         posStack;
    CNamespaceMapNode                               *pMapNode;
    NamespaceStack                                  *pStack;
    int                                              i;
    
    // Pop stack for default namespace

    pos = _stackDefNamespace.GetHeadPosition();
    if (pos) {
        pMapNode = _stackDefNamespace.GetAt(pos);
        _ASSERTE(pMapNode);

        if (pMapNode->_dwDepth == _dwCurDepth) {
            // Match found. Pop the stack.

            _stackDefNamespace.RemoveAt(pos);
            SAFEDELETE(pMapNode);
        }
    }

    // Pop stack for namespace prefixes

    // Walk each entry in the hash table.

    for (i = 0; i < NAMESPACE_HASH_TABLE_SIZE; i++) {
        pos = _listMap[i].GetHeadPosition();

        while (pos) {
            // For each entry in the hash table, look at the list of
            // stacks.

            curPos = pos;
            pStack = _listMap[i].GetNext(pos);
            _ASSERTE(pStack);

            // See if the head of the stack is at the depth we're unwinding.

            posStack = pStack->GetHeadPosition();
            if (posStack) {
                pMapNode = pStack->GetAt(posStack);
                _ASSERTE(pMapNode);
    
                if (pMapNode->_dwDepth == _dwCurDepth) {
                    pStack->RemoveAt(posStack);
    
                    SAFEDELETE(pMapNode);
                }

                if (!pStack->GetHeadPosition()) {
                    SAFEDELETE(pStack);
                    _listMap[i].RemoveAt(curPos);
                }
            }
        }
    }

    // Decrease depth
    
    _dwCurDepth--;

    return hr;
}

HRESULT CNamespaceManager::Map(LPCWSTR pwzAttribute, 
                               DWORD dwAttrLen, 
                               __out_ecount_opt(*pdwSize) LPWSTR pwzQualified, 
                               __inout LPDWORD pdwSize, 
                               DWORD dwFlags)
{
    HRESULT                                       hr = S_OK;
    LPWSTR                                        pwzPrefix = NULL;
    LPWSTR                                        pwzCur = NULL;
    DWORD                                         dwLen;
    DWORD                                         dwLenURI;
    DWORD                                         dwHash;
    DWORD                                         dwSize;
    LISTNODE                                      pos;
    LISTNODE                                      posStack;
    NamespaceStack                               *pStack;
    CNamespaceMapNode                            *pMapNode;
    BOOL                                          bAppliedNS = FALSE;
    DWORD                                         i;

    _ASSERTE(pwzAttribute && pwzQualified && pdwSize);

    // See if there is a colon in the name

    for (i = 0, pwzCur = (LPWSTR)pwzAttribute; i < dwAttrLen && (*pwzCur != L':'); i++, pwzCur++);
    if (i == dwAttrLen) {
        pwzCur = NULL;
    }
        
    if (!pwzCur) {
        // No colon in name. Apply default name space, if applicable.
        if (dwFlags & XMLNS_FLAGS_APPLY_DEFAULT_NAMESPACE) {
            pos = _stackDefNamespace.GetHeadPosition();
            if (pos) {
                pMapNode = _stackDefNamespace.GetAt(pos);
                _ASSERTE(pMapNode && pMapNode->_pwzURI);
    
                dwLenURI = lstrlenW(pMapNode->_pwzURI);
                if (dwLenURI) {
                    dwLen = dwLenURI + dwAttrLen + 2;
                    if (*pdwSize < dwLen) {
                        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                    }
                    else {
                        hr = StringCchPrintf(pwzQualified, *pdwSize, L"%ws^", pMapNode->_pwzURI);
                        if (FAILED(hr)) {
                            goto Exit;
                        }

                        dwSize = lstrlenW(pwzQualified);
                        pwzCur = pwzQualified + dwSize;

                        hr = StringCchCopyN(pwzCur, *pdwSize - dwSize, pwzAttribute, dwAttrLen);
                        if (FAILED(hr)) {
                            goto Exit;
                        }
                    }
                    *pdwSize = dwLen;
                    bAppliedNS = TRUE;
                }
            }
        }

        if (!bAppliedNS) {
            dwLen = dwAttrLen+1;
            if (*pdwSize < dwLen) {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
            else {
                hr = StringCchCopyN(pwzQualified, *pdwSize, pwzAttribute, dwLen-1);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
            *pdwSize = dwLen;   
        }

        goto Exit;
    }
    else {
        // slow path 
        //
        // Colon found in name. Apply mapping
        
        // Anchor NULL char so pwzPrefix points to the prefix.
        pwzPrefix = NEW(WCHAR[dwAttrLen+1]);
        if (!pwzPrefix) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        
        hr = StringCchCopyN(pwzPrefix, dwAttrLen+1, pwzAttribute, dwAttrLen);
        if (FAILED(hr)) {
            goto Exit;
        }

        pwzCur = StrChr(pwzPrefix, L':');
        _ASSERTE(pwzCur);
        PREFIX_ASSUME(pwzCur != NULL);

        *pwzCur = L'\0';
        pwzCur++;

        dwHash = HashString(pwzPrefix, 0, NAMESPACE_HASH_TABLE_SIZE, TRUE);

        pos = _listMap[dwHash].GetHeadPosition();
        if (!pos) {
            // Miss in hash table. Thus, we do not have a prefix.
            dwLen = dwAttrLen+1;
            if (*pdwSize < dwLen) {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
            else {
                hr = StringCchCopy(pwzQualified, *pdwSize, pwzPrefix);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
            *pdwSize = dwLen;
            goto Exit;
        }
        else {
            // Hit in the hash table. Find the right stack, if any.

            while (pos) {
                pStack = _listMap[dwHash].GetNext(pos);
                _ASSERTE(pStack);

                posStack = pStack->GetHeadPosition();
                _ASSERTE(posStack);

                pMapNode = pStack->GetAt(posStack);
                _ASSERTE(pMapNode);

                if (!FusionCompareString(pMapNode->_pwzPrefix, pwzPrefix)) {
                    // Hit found. Apply the mapping.
                    
                    _ASSERTE(pMapNode->_pwzURI);

                    dwLen = lstrlenW(pMapNode->_pwzURI) + lstrlenW(pwzCur) + 2;
                    if (*pdwSize < dwLen) {
                        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                    }
                    else {
                        hr = StringCchPrintf(pwzQualified, *pdwSize, L"%ws^%ws", pMapNode->_pwzURI, pwzCur);
                        if (FAILED(hr)) {
                            goto Exit;
                        }
                    }
                    *pdwSize = dwLen;
                    goto Exit;
                }
            }

            // We collided in the hash table, but didn't find a hit.
            // This must be an error because we hit something of the form
            // <a f:z="foo"> where "f" was not previously defined!

            hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(pwzPrefix);

    if (FAILED(hr)) {
        _hrResult = hr;
    }

    return hr;
}


void CNamespaceManager::Error(HRESULT hrError)
{
    _hrResult = hrError;
}


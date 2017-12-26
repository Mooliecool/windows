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
#include "stdafx.h"
#include <log.h>

#include "internalonly.h"

#define BUF_SIZE 256

// The Shell project is for internal testing purposes only at this point.
// We are therefore not going to scrub the code.  It should be moved out
// of the dev project longer term.

/* ------------------------------------------------------------------------- *
 * Debugger base class
 * ------------------------------------------------------------------------- */




DebuggerHashTable::~DebuggerHashTable()
{
    HASHFIND    find;

    for (DebuggerHashEntry *entry = (DebuggerHashEntry *) FindFirstEntry(&find);
        entry != NULL;
        entry = (DebuggerHashEntry *) FindNextEntry(&find))
        delete entry->pBase;
}

HRESULT DebuggerHashTable::AddBase(DebuggerBase *pBase)
{
    if (!m_initialized)
    {
        HRESULT hr = NewInit(m_iBuckets,
                             sizeof(DebuggerHashEntry), 0xffff);
        if (hr != S_OK)
            return (hr);

        m_initialized = true;
    }

    DebuggerHashEntry *entry = (DebuggerHashEntry *) Add(HASH(pBase->GetToken()));

    if (entry == NULL)
        return (E_FAIL);
    else
        entry->pBase = pBase;

    return (S_OK);
}

DebuggerBase *DebuggerHashTable::GetBase(ULONG_PTR id)
{
    if (!m_initialized)
        return (NULL);

    DebuggerHashEntry *entry
    = (DebuggerHashEntry *) Find(HASH(id), KEY(id));
    if (entry == NULL)
        return (NULL);
    else
        return (entry->pBase);
}

BOOL DebuggerHashTable::RemoveBase(ULONG_PTR id)
{
    if (!m_initialized)
        return (FALSE);

    DebuggerHashEntry *entry
    = (DebuggerHashEntry *) Find(HASH(id), KEY(id));

    if (entry == NULL)
        return (FALSE);

    DebuggerBase *base = entry->pBase;

    Delete(HASH(id), (HASHENTRY *) entry);
    delete base;

    return (TRUE);
}

void DebuggerHashTable::RemoveAll()
{
    HASHFIND    find;

    for (DebuggerHashEntry *entry = (DebuggerHashEntry *) FindFirstEntry(&find);
        entry != NULL;
        entry = (DebuggerHashEntry *) FindNextEntry(&find))
        delete entry->pBase;

    Clear();
}

DebuggerBase *DebuggerHashTable::FindFirst(HASHFIND *find)
{
    DebuggerHashEntry *entry = (DebuggerHashEntry *) FindFirstEntry(find);
    if (entry == NULL)
        return (NULL);
    else
        return (entry->pBase);
}

DebuggerBase *DebuggerHashTable::FindNext(HASHFIND *find)
{
    DebuggerHashEntry *entry = (DebuggerHashEntry *) FindNextEntry(find);
    if (entry == NULL)
        return (NULL);
    else
        return (entry->pBase);
}

/* ------------------------------------------------------------------------- *
 * DebuggerVarInfo
 * ------------------------------------------------------------------------- */
void DebuggerVarInfo::SetName(LPCSTR szName)
{
    if (szName != NULL)
    {
        MAKE_WIDEPTR_FROMUTF8(wArgName, szName); // sanity check
    }

    m_name = szName;
}



/* ------------------------------------------------------------------------- *
 * DebuggerClass
 * ------------------------------------------------------------------------- */
DebuggerClass::DebuggerClass (ICorDebugClass *pClass)
    : DebuggerBase ((ULONG_PTR)pClass),
        m_szName (NULL),
        m_szNamespace (NULL)
{
}

DebuggerClass::~DebuggerClass ()
{
    delete m_szName;
    delete m_szNamespace;
}

void DebuggerClass::SetName(__in_z __in_opt WCHAR *pszName,
                            __in_z __in_opt WCHAR *pszNamespace)
{
    if (pszName != NULL)
    {
        int iLength = (int)wcslen (pszName);
        if ((m_szName = new WCHAR [iLength+1]) != NULL)
            wcscpy (m_szName, pszName);
    }

    if (pszNamespace != NULL)
    {
        int iLength = (int)wcslen (pszNamespace);
        if ((m_szNamespace = new WCHAR [iLength+1]) != NULL)
            wcscpy (m_szNamespace, pszNamespace);
    }
}

WCHAR *DebuggerClass::GetName (void)
{
    return m_szName;
}

WCHAR *DebuggerClass::GetNamespace (void)
{
    return m_szNamespace;
}

/* ------------------------------------------------------------------------- *
 * DebuggerModule
 * ------------------------------------------------------------------------- */

DebuggerModule::DebuggerModule(ICorDebugModule* imodule)
    : DebuggerBase((ULONG_PTR)imodule),
      m_pIMetaDataImport2(NULL),
      m_pISymUnmanagedReader(NULL),
      m_sourceFiles(11),
      m_functions(37),
      m_functionsByIF(11),
      m_loadedClasses(11),
      m_breakpoints(NULL)
{
    _ASSERTE(imodule != NULL);

    // Also initialize the source file array
    for (int i=0; i<MAX_SF_BUCKETS; i++)
        m_pModSourceFile [i] = NULL;
    // Indicate that source file names are not yet loaded
    m_fSFNamesLoaded = false;

    imodule->AddRef();

    m_EnCLastUpdated = 0;

    // Cache the m_pIAppDomain
    {
        m_pIAppDomain = NULL;

        // Get the ICorDebugAppDomain that this module lives in.
        ICorDebugAssembly * pAssembly = NULL;

        imodule->GetAssembly(&pAssembly);
        // don't release module here because our getter didn't addref.

        if (pAssembly != NULL)
        {
            pAssembly->GetAppDomain(&m_pIAppDomain);
            pAssembly->Release();
        } // assembly
    }

}

HRESULT DebuggerModule::Init(const WCHAR *pSearchPath)
{
    // Get the necessary metadata interfaces now...
    HRESULT hr = GetICorDebugModule()->GetMetaDataInterface(
                                           IID_IMetaDataImport,
                                           (IUnknown**)&m_pIMetaDataImport);

    if (FAILED(hr))
        return hr;

    hr = GetICorDebugModule()->GetMetaDataInterface(
                                           IID_IMetaDataImport2,
                                           (IUnknown**)&m_pIMetaDataImport2);

    // Down-level runtimes may not support IMetaDataImport2
    if (FAILED(hr))
        m_pIMetaDataImport2 = NULL;

    // Get the module name
    WCHAR moduleName[MAX_PATH];
    ULONG32 nameLen;

    hr = GetICorDebugModule()->GetName(MAX_PATH, &nameLen, moduleName);
    _ASSERTE(nameLen <= MAX_PATH);

    // Don't get a reader if its a dynamic module... syms for those
    // come in on update later.
    BOOL isDynamic = FALSE;
    hr = GetICorDebugModule()->IsDynamic(&isDynamic);
    _ASSERTE(SUCCEEDED(hr));

    BOOL isInMemory = FALSE;
    hr = GetICorDebugModule()->IsInMemory(&isInMemory);
    _ASSERTE(SUCCEEDED(hr));

    if (isDynamic || isInMemory)
        return hr;

    // Get a symbol binder.
    ISymUnmanagedBinder *binder;

    hr = CoCreateInstance(CLSID_CorSymBinder_SxS, NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_ISymUnmanagedBinder,
                          (void**)&binder);

    if (FAILED(hr))
    {
        g_pShell->Write(L"Error: couldn't get a CorSymBinder for "
                        L"symbol loading. HR was :%x\n", hr);
        return S_FALSE;
    }

    // Ask the binder for a reader for this module.
    m_pISymUnmanagedReader = NULL;

    hr = binder->GetReaderForFile(m_pIMetaDataImport,
                                  moduleName,
                                  pSearchPath, // use global search path
                                  &m_pISymUnmanagedReader);

    // Release the binder
    binder->Release();

    if (FAILED(hr))
    {
        g_pShell->Write(L"Warning: couldn't load symbols for %s\n",
                        moduleName);

        // Oddly enough, sometimes GetReaderForFile actually gives us back a reader even if it couldn't load the symbols
        // for the module. In this case, we release the reader and set the value to NULL. NULL indicates to the rest of
        // cordbg that there are no syms available for this module.
        if (m_pISymUnmanagedReader != NULL)
        {
            m_pISymUnmanagedReader->Release();
            m_pISymUnmanagedReader = NULL;
        }

        return S_OK;
    }

    return hr;
}

DebuggerModule::~DebuggerModule()
{
    while (m_breakpoints != NULL)
    {
        DebuggerCodeBreakpoint *bp = m_breakpoints->m_next;
        delete m_breakpoints;
        m_breakpoints = bp;
    }

    if (g_pShell != NULL)
    {
        DebuggerBreakpoint *bp = g_pShell->m_breakpoints;

        while (bp != NULL)
        {
            if (bp->IsBoundToModule(this))
                bp->RemoveBoundModule(this);

            bp = bp->m_next;
        }
    }

    m_pIAppDomain->Release();

    GetICorDebugModule()->Release();

    if (m_pIMetaDataImport)
        m_pIMetaDataImport->Release();

    if (m_pIMetaDataImport2)
        m_pIMetaDataImport2->Release();

    if (m_pISymUnmanagedReader)
        m_pISymUnmanagedReader->Release();


    DeleteModuleSourceFiles();

    // Need to get rid of all functions associated with this module.
    m_functions.RemoveAll();
}

DebuggerSourceFile *DebuggerModule::LookupSourceFile(const WCHAR *name)
{
    GUID g = {0};
    ISymUnmanagedDocument *doc;
    DebuggerSourceFile *pRet = NULL;

    if (!m_pISymUnmanagedReader)
        return NULL;

    // Get the doc corresponding to the name
    HRESULT hr = m_pISymUnmanagedReader->GetDocument((WCHAR*)name, g, g, g, &doc);

    if (SUCCEEDED(hr) && doc!=NULL)
    {
        // Try and resolve the document to a DebuggerSourceFile
        pRet = ResolveSourceFile(doc);
        _ASSERTE(pRet);

        // Release the doc
        doc->Release();
    }

    return (pRet);
}

DebuggerSourceFile *DebuggerModule::ResolveSourceFile(ISymUnmanagedDocument *doc)
{
    // Not addref'd
    doc = FindDuplicateDocumentByURL(doc);

    DebuggerSourceFile *file =
        (DebuggerSourceFile*)m_sourceFiles.GetBase((ULONG_PTR)doc);

    if (file == NULL)
    {
        file = new DebuggerSourceFile(this, doc);
        _ASSERTE(file != NULL);

        if (file != NULL)
            m_sourceFiles.AddBase(file);
    }

    return (file);
}

DebuggerFunction *DebuggerModule::ResolveFunction(mdMethodDef md,
                                                  ICorDebugFunction *iFunction)
{
    // Make sure we don't have obviously invalid arguments
    _ASSERTE((md != mdMethodDefNil) || (iFunction != NULL));

    // What will be returned
    DebuggerFunction *function;

    // Get a pointer to the DebuggerFunction object
    if (md != mdMethodDefNil)
        function = (DebuggerFunction *)m_functions.GetBase(md);
    else
        function = (DebuggerFunction *)m_functionsByIF.GetBase((ULONG_PTR)iFunction);

    // Has not been created yet
    if (function == NULL)
    {
        // Create a new object
        function = new DebuggerFunction(this, md, iFunction);
        _ASSERTE(function != NULL);

        // Out of memory
        if (function == NULL)
        {
            g_pShell->ReportError(E_OUTOFMEMORY);
            return NULL;
        }

        // Init the DebuggerFunction object
        HRESULT hr = function->Init();

        // Error check
        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            delete function;
            return NULL;
        }

        // Add the DebuggerFunction object to the appropriate collection
        if (md != mdMethodDefNil)
            m_functions.AddBase(function);
        else
            m_functionsByIF.AddBase(function);
    }

    // Return DebuggerFunction pointer
    return (function);
}

DebuggerFunction *DebuggerModule::ResolveFunction(ISymUnmanagedMethod *method,
                                                  ICorDebugFunction *iFunction)
{
    HRESULT hr = S_OK;

    mdMethodDef tk;

    hr = method->GetToken(&tk);

    if (FAILED(hr))
        return NULL;
    else
        return ResolveFunction(tk, iFunction);
}

DebuggerModule *DebuggerModule::FromCorDebug(ICorDebugModule *module)
{
    // Return the DebuggerModule object
    return (g_pShell->ResolveModule(module));
}

DebuggerModule *DebuggerShell::ModuleOfType(ICorDebugType *itype)
{
    // Snagg the object's class.
    ICorDebugClass *iclass = NULL;
    HRESULT hr = itype->GetClass(&iclass);

    if (FAILED(hr)) {
        g_pShell->ReportError(hr);
        return NULL;
    }

    // Get the module from this class
    // Keep the class around for later...
    ICorDebugModule *imodule;
    iclass->GetModule(&imodule);

    if (FAILED(hr))
    {
        RELEASE(iclass);
        g_pShell->ReportError(hr);
        return NULL;
    }
    RELEASE(iclass);

    DebuggerModule *dm = DebuggerModule::FromCorDebug(imodule);
    _ASSERTE(dm != NULL);
    RELEASE(imodule);
    return dm;
}

mdTypeDef DebuggerShell::TokenOfType(ICorDebugType *itype)
{
    // Snagg the object's class.
    ICorDebugClass *iclass = NULL;
    HRESULT hr = itype->GetClass(&iclass);

    if (FAILED(hr)) {
        g_pShell->ReportError(hr);
        return NULL;
    }
    // Get the token for the class
    mdTypeDef tdClass;
    _ASSERTE(iclass != NULL);
    hr = iclass->GetToken(&tdClass);
    RELEASE(iclass);
    return tdClass;
}


HRESULT DebuggerModule::LoadSourceFileNames (void)
{
    HRESULT hr = S_OK;
    int     iBucket;

    if (m_fSFNamesLoaded
       )
        return S_OK;

    // Get all of the source files within this module.
    ULONG32 docCount;
    ISymUnmanagedDocument **allDocs;

    if (!m_pISymUnmanagedReader)
        return S_OK;

    // How many documents?
    hr = m_pISymUnmanagedReader->GetDocuments(0, &docCount, NULL);

    if (FAILED(hr))
        return hr;

    // Allocate space for the documents.
    allDocs = (ISymUnmanagedDocument**)_alloca(docCount * sizeof(ISymUnmanagedDocument*));
    _ASSERTE(allDocs != NULL);

    hr = m_pISymUnmanagedReader->GetDocuments(docCount, &docCount, allDocs);

    if (FAILED(hr))
        return hr;

    // Loop over the documents, setting up each's name and path
    // accordingly.
    for (ULONG32 i = 0; i < docCount; i++)
    {
        WCHAR docName[256];
        ULONG32 s;

        hr = allDocs[i]->GetURL(256, &s, docName);

        if (FAILED(hr))
            break;

        // This will actually take a long time, esp. for things like mscorlib.dll
        // so we'll only go looking for previous versions if the module's been
        // EnC'd (ie, the first time we just assume everything's new).
        if (
            SearchForDocByString(docName))
            continue; //we've already got a pointer to this, so don't re-create it.

        MAKE_ANSIPTR_FROMWIDE(docNameA, docName);

        int iLen;
        iLen = (int)strlen(docNameA);

        if (iLen)
        {
            // strip the path and store just the lowercase file name
            CHAR        rcFile[MAX_PATH];
            _splitpath(docNameA, NULL, NULL, rcFile, NULL);

            // make the file name lowercase
            int j=0;
            while (rcFile [j] != '\0')
            {
                rcFile[j] = tolower(rcFile[j]);
                j++;
            }

            // Based on the stripped file name, decide the bucket in
            // which it should go
            if (rcFile [0] < 'a')
                iBucket = 0;
            else if (rcFile [0] > 'z')
                iBucket = MAX_SF_BUCKETS - 1;
            else
                iBucket = (rcFile [0] - 'a') % MAX_SF_BUCKETS;

            // Allocate a new ModuleSourceFile object
            ModuleSourceFile *pmsf = new ModuleSourceFile;

            if (pmsf)
            {
                if (!pmsf->SetFullFileName(allDocs[i], docNameA))
                {
                    hr = E_FAIL;
                    delete pmsf;
                    break;
                }

                // Add this ModuleSourceFile object to the cache
                pmsf->SetNext (m_pModSourceFile [iBucket]);
                m_pModSourceFile [iBucket] = pmsf;
            }
            else
            {
                // out of memory
                hr = E_OUTOFMEMORY;
                break;
            }
        }

        RELEASE(allDocs[i]);
    }

    // Indicate that the module's source files have been cached
    if (SUCCEEDED (hr))
    {
        m_fSFNamesLoaded = true;
    }

    return hr;
}

void DebuggerModule::DeleteModuleSourceFiles(void)
{
    // Go through all the buckets and release the cached ModuleSourceFiles
    for (int i=0; i<MAX_SF_BUCKETS; i++)
    {
        ModuleSourceFile *pMod = m_pModSourceFile [i];
        ModuleSourceFile *pTemp = NULL;

        while (pMod)
        {
            pTemp = pMod;
            pMod = pTemp->GetNext();
            delete pTemp;
        }

        m_pModSourceFile [i] = NULL;
    }
}

HRESULT DebuggerModule::MatchStrippedFNameInModule(
    __in_z WCHAR *pstrFileName, // file name to look for (assumed to have been converted to  lower case)
    __deref_inout_ecount(*piCount) WCHAR **ppstrMatchedNames, // returned array containing full paths of matched filenames
    ISymUnmanagedDocument **ppDocs, // returned aray containing documents for source file
    int *piCount // number of files which matched the given filename
    )
{
    HRESULT hr;

    *piCount = 0;
    _ASSERTE (pstrFileName);

    // The filename length should be > 0
    if (!wcslen (pstrFileName))
        return (E_INVALIDARG);

    if ((hr = LoadSourceFileNames ()) != S_OK)
        return hr;

    hr = E_FAIL;

    // first, extract strip the path+file name  down to just the "file.ext" name
    WCHAR   rcFile[_MAX_FNAME];
    WCHAR   rcExt[_MAX_EXT];

    // _wsplitpath has a bug where it corrupts the stack if a ( is in the pstFilename
    if (wcsstr(pstrFileName, L"("))
    {
        return E_FAIL;
    }
    _wsplitpath(pstrFileName, NULL, NULL, rcFile, rcExt);
    wcscat(rcFile, rcExt);

    // get the bucket in which this file should be searched for
    int iBucketIndex;

    if (rcFile [0] < 'a')
        iBucketIndex = 0;
    else if (rcFile [0] > 'z')
        iBucketIndex = MAX_SF_BUCKETS - 1;
    else
        iBucketIndex = (rcFile [0] - 'a') % MAX_SF_BUCKETS;

    ModuleSourceFile *pmsf = m_pModSourceFile [iBucketIndex];

    // Search the whole list to find the files names which match
    while (pmsf)
    {
        WCHAR   *pStrippedFileName = pmsf->GetStrippedFileName();
        WCHAR   strTemp [MAX_PATH];

        // convert the name to lowercase before comparing
        wcscpy (strTemp, pStrippedFileName);

        int i=0;
        while (strTemp [i] != L'\0')
        {
            strTemp [i] = towlower (strTemp [i]);
            i++;
        }


        if (!wcscmp (strTemp, rcFile))
        {
            _ASSERTE (*piCount < MAX_FILE_MATCHES_PER_MODULE);
            // found a match
            ppstrMatchedNames [*piCount] = pmsf->GetFullFileName();
            ppDocs [*piCount] = pmsf->GetDocument();
            (*piCount)++;
            hr = S_OK;
        }

        pmsf = pmsf->GetNext();
    }

    return hr;
}

//***********************************************************************************************
// This will search through all of the debugger's ModuleSourceFile entries and find
// the stored ISymUnmanagedDocument pointer that has the same URL as the document
// passed in.  This is necessary due to a deficiency in the ISym* interface not being
// able to return the same ISymUnmanagedDocument pointer for the same document.
//***********************************************************************************************

ISymUnmanagedDocument *DebuggerModule::FindDuplicateDocumentByURL(ISymUnmanagedDocument *pDoc)
{
    _ASSERTE(pDoc);
    HRESULT  hr;

    /////////////////////////////////////////////////////////
    // Make sure all the source documents have been loaded

    hr = LoadSourceFileNames();
    _ASSERTE(SUCCEEDED(hr));

    if (FAILED(hr))
        return (NULL);

    ///////////////////////////////////////////
    // Get the URL of the document passed in

    ULONG32  ccUrl;
    WCHAR   *szUrl;

    // Find out how long the URL is
    hr = pDoc->GetURL(0, &ccUrl, NULL);
    _ASSERTE(SUCCEEDED(hr));

    // Allocate the memory for it
    szUrl = (WCHAR *)_alloca((ccUrl + 1) * sizeof(WCHAR));

    // Get the URL
    hr = pDoc->GetURL(ccUrl + 1, &ccUrl, szUrl);
    _ASSERTE(SUCCEEDED(hr));

    ////////////////////////////////////////////////////////////////////
    // Whip through all the ModuleSourceFile entries and find a match

    return SearchForDocByString(szUrl);
}

ISymUnmanagedDocument *DebuggerModule::SearchForDocByString(__in_z WCHAR *szUrl)
{
    HRESULT hr = S_OK;
    WCHAR      *szUrlCur = NULL;
    ULONG32     cbAllocatedBufferSize = 0;

    for (int i = 0; i < MAX_SF_BUCKETS; i++)
    {
        ModuleSourceFile *pMSF = m_pModSourceFile[i];

        while (pMSF)
        {
            /////////////////////////////////////////
            // Get the URL of the current document

            ULONG32  ccUrlCur;
            ULONG32  cbNeededBufferSize;
            ISymUnmanagedDocument *pDocCur = pMSF->GetDocument();
            _ASSERTE(pDocCur);

            // Find out how long the URL is
            hr = pDocCur->GetURL(0, &ccUrlCur, NULL);
            _ASSERTE(SUCCEEDED(hr));

            // Allocate the memory for it
            cbNeededBufferSize = (ccUrlCur + 1) * sizeof(WCHAR);
            if ( cbAllocatedBufferSize < cbNeededBufferSize )
            {
                if ( szUrlCur != NULL )
                {
                    free ( szUrlCur );
                    szUrlCur = NULL;
                }
                cbAllocatedBufferSize *= 2;
                if ( cbAllocatedBufferSize < cbNeededBufferSize )
                {
                    cbAllocatedBufferSize = cbNeededBufferSize;
                }
                szUrlCur = (WCHAR *) malloc ( cbAllocatedBufferSize );
            }
            _ASSERTE(szUrlCur);

            // Get the URL
            hr = pDocCur->GetURL(ccUrlCur + 1, &ccUrlCur, szUrlCur);
            _ASSERTE(SUCCEEDED(hr));

            ///////////////////////////////////////////////////////////////////////
            // Is this document a match?  If so, go ahead and return the pointer

            if (wcscmp(szUrl, szUrlCur) == 0)
            {
                if ( szUrlCur != NULL )
                {
                    free ( szUrlCur );
                }
                return pDocCur;
            }

            ///////////////////////////////////////////////
            // Get the next document and continue search

            pMSF = pMSF->GetNext();
        }
    }

    if ( szUrlCur != NULL )
    {
        free ( szUrlCur );
    }

    //////////////////////
    // Indicate failure

    return (NULL);
}


HRESULT     DebuggerModule::MatchFullFileNameInModule (__in_z WCHAR *pstrFileName,
                                                       ISymUnmanagedDocument **pDoc)
{

    int iBucketIndex;

    HRESULT hr;

    _ASSERTE (pstrFileName);

    // The filename length should be > 0
    if (!wcslen (pstrFileName))
        return (E_INVALIDARG);

    if ((hr = LoadSourceFileNames ()) != S_OK)
        return hr;

    hr = E_FAIL; // assume we won't find it in this module.

    // first, extract strip the path+file name  down to just the "file.ext" name

    WCHAR   rcFile[_MAX_FNAME];
    WCHAR   rcExt[_MAX_EXT];
    WCHAR   buf[1024];

    // _wsplitpath has a bug where it corrupts the stack if a ( is in the pstFilename
    if (wcsstr(pstrFileName, L"("))
    {
        return E_FAIL;
    }
    _wsplitpath(pstrFileName, buf, buf, rcFile, rcExt);
    wcscat(rcFile, rcExt);

    // get the bucket in which this file should be searched for
    if (rcFile [0] < 'a')
        iBucketIndex = 0;
    else if (rcFile [0] > 'z')
        iBucketIndex = MAX_SF_BUCKETS - 1;
    else
        iBucketIndex = (rcFile [0] - 'a') % MAX_SF_BUCKETS;

    ModuleSourceFile *pmsf = m_pModSourceFile [iBucketIndex];

    // Search the whole list to find the files name which matches
    while (pmsf)
    {
        WCHAR   *pFullFileName = pmsf->GetFullFileName();

        if (!wcscmp (pFullFileName, pstrFileName))
        {
            // found a match
            *pDoc = pmsf->GetDocument();
            hr = S_OK;
            break;
        }

        pmsf = pmsf->GetNext();
        if (!pmsf)
        {
            hr = E_FAIL;
            break;
        }
    }

    return hr;
}


void DebuggerModule::SetName (__in_z __in_opt WCHAR *pszFullName)
{
    if (pszFullName != NULL)
    {
        if (!m_szName.CopyFrom(pszFullName))
            return;

        // Calculate & cache the short name.
        WCHAR szShortName[_MAX_PATH];
        WCHAR fname[_MAX_FNAME];
        WCHAR ext[_MAX_EXT];

        _wsplitpath(pszFullName, NULL, NULL, fname, ext);
        _wmakepath(szShortName, NULL, NULL, fname, ext);

        m_szShortName.CopyFrom(szShortName);
    }
}

//
// Update the symbols for this module. Creates the syms if they aren't
// already created.
//
HRESULT DebuggerModule::UpdateSymbols(IStream *pStream)
{
    HRESULT hr = S_OK;

    // If we don't already have a reader, create one.
    if (m_pISymUnmanagedReader == NULL)
    {
        ISymUnmanagedBinder *pBinder = NULL;

        hr = CoCreateInstance(CLSID_CorSymBinder_SxS, NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_ISymUnmanagedBinder,
                              (void**)&pBinder);

        if (SUCCEEDED(hr))
        {
            hr = pBinder->GetReaderFromStream(m_pIMetaDataImport,
                                              pStream,
                                              &m_pISymUnmanagedReader);

            if (FAILED(hr))
                m_pISymUnmanagedReader = NULL;

            pBinder->Release();
        }
    }
    else
    {
        // We already have a reader, so just replace the symbols. We
        // replace instead of update because we are doing this only
        // for dynamic modules and the syms are cumulative.
        hr = m_pISymUnmanagedReader->ReplaceSymbolStore(NULL, pStream);
    }

    // Reset symbol caches so that we pick up new documents.
    m_fSFNamesLoaded = false;

    return hr;
}

void DebuggerShell::PrintGlobalVariable (mdFieldDef mdTok,
                                         __in_z WCHAR  *wszName,
                                         DebuggerModule *dm)
{
    ICorDebugModule *mod = dm->GetICorDebugModule();
    ICorDebugValue *val = NULL;
    HRESULT hr = S_OK;

    _ASSERTE( mod != NULL );

    hr = mod->GetGlobalVariableValue(mdTok, &val);
    if (FAILED(hr))
        return;

    this->PrintVariable(wszName, val, 0, TRUE);

    Write(L"\n");
}

#define PRINT_ALL       1
#define MATCH_N_CHARS   2
#define MATCH_ALL_CHARS 3

BOOL DebuggerModule::PrintGlobalVariables(const WCHAR *szSearchString,
                                          const WCHAR* szModName,
                                          DebuggerModule *dm)
{
    BOOL fWildCard = FALSE;
    int iMatchKind;
    ULONG iLength = (ULONG)wcslen(szSearchString);
    char szSearchName [MAX_SYMBOL_NAME_LENGTH];

    ULONG i;
    for (i=0; i<iLength; i++)
    {
        if (szSearchString [i] == L'*')
        {
            fWildCard = TRUE;
            iLength = i;
            break;
        }
    }

    if (iLength==0)
    {
        // print all symbols
        iMatchKind = PRINT_ALL;
    }
    else
    {
        WCHAR* buf = reinterpret_cast< WCHAR* >(_alloca((iLength + 1) * sizeof(WCHAR)));
        memcpy(buf, szSearchString, iLength * sizeof(WCHAR));
        buf[iLength] = L'\0';

        MAKE_UTF8PTR_FROMWIDE (szSearchName1, buf);

        strcpy (szSearchName, szSearchName1);

        if (fWildCard)
        {
            // match 'iLength' characters only
            iMatchKind = MATCH_N_CHARS;
        }
        else
        {
            // match the whole string
            iMatchKind = MATCH_ALL_CHARS;
        }
    }

    // first, look for the global functions
    HCORENUM phEnum = 0;
    mdMethodDef rTokens[100];
    ULONG count;
    HRESULT hr;
    MDUTF8CSTR name;
    MDUTF8STR  u_name;
    MDUTF8STR  szMDName;
    bool anythingPrinted = false;

    u_name = new char[MAX_CLASSNAME_LENGTH];

    do
    {
        hr = m_pIMetaDataImport->EnumFields(&phEnum, NULL, &rTokens[0], 100, &count);

        if (!SUCCEEDED(hr))
        {
            g_pShell->ReportError(hr);
            goto ErrExit;
        }

        for (i = 0; i < count; i++)
        {
            hr = m_pIMetaDataImport->GetNameFromToken(rTokens[i], &name);

            if (name == NULL)
                continue;

            MAKE_WIDEPTR_FROMUTF8( wszName, name );

            szMDName = (MDUTF8STR) name;

            if (iMatchKind == PRINT_ALL)
            {
                g_pShell->PrintGlobalVariable (rTokens[i],
                                               wszName,
                                               dm);
                anythingPrinted = true;
            }
            else
            {
                if (iMatchKind == MATCH_N_CHARS)
                {
                    if (!strncmp (szMDName, szSearchName, iLength))
                    {
                        g_pShell->PrintGlobalVariable (rTokens[i],
                                                       wszName,
                                                       dm);
                        anythingPrinted = true;
                    }
                }
                else
                {
                    if (!strcmp (szMDName, szSearchName))
                    {
                        g_pShell->PrintGlobalVariable (rTokens[i],
                                                       wszName,
                                                       dm);
                        anythingPrinted = true;
                    }
                }
            }
        }
    }
    while (count > 0);

ErrExit:
    delete[] u_name;

    if (!anythingPrinted)
        return FALSE;
    return TRUE;
}

BOOL DebuggerModule::PrintMatchingSymbols(const WCHAR *szSearchString, const WCHAR *szModName)
{
    BOOL fWildCard = FALSE;
    int iMatchKind;
    ULONG iLength = (ULONG)wcslen (szSearchString);
    char szSearchName[MAX_SYMBOL_NAME_LENGTH];

    ULONG i;
    for (i=0; i<iLength; i++)
    {
        if (szSearchString [i] == L'*')
        {
            fWildCard = TRUE;
            iLength = i;
            break;
        }
    }

    if (iLength==0)
    {
        // print all symbols
        iMatchKind = PRINT_ALL;
    }
    else
    {
        WCHAR* buf = reinterpret_cast< WCHAR *>(_alloca((iLength + 1) * sizeof(WCHAR)));
        memcpy(buf, szSearchString, iLength * sizeof(WCHAR));
        buf[iLength] = L'\0';

        MAKE_UTF8PTR_FROMWIDE (szSearchName1, buf);

        strncpy(szSearchName, szSearchName1, MAX_SYMBOL_NAME_LENGTH);
        szSearchName[MAX_SYMBOL_NAME_LENGTH - 1] = '\0';

        if (iLength >= MAX_SYMBOL_NAME_LENGTH)
        {
            iLength = MAX_SYMBOL_NAME_LENGTH - 1;
        }

        if (fWildCard)
        {
            // match 'iLength' characters only
            iMatchKind = MATCH_N_CHARS;
        }
        else
        {
            // match the whole string
            iMatchKind = MATCH_ALL_CHARS;
        }
    }

    // first, look for the global functions
    HCORENUM phEnum = 0;
    mdMethodDef rTokens[100];
    mdTypeDef rClassTokens [100];
    ULONG count;
    HRESULT hr;
    MDUTF8CSTR name;
    MDUTF8CSTR name1;
    MDUTF8STR  u_name;
    MDUTF8STR  szMDName;
    bool anythingPrinted = false;
    char *szMethod = NULL;
    int iLengthOfMethod = 0;

    u_name = new char[MAX_CLASSNAME_LENGTH];

    do
    {
        hr = m_pIMetaDataImport->EnumMethods(&phEnum, NULL, &rTokens[0], 100, &count);

        if (!SUCCEEDED(hr))
        {
            g_pShell->ReportError(hr);
            goto ErrExit;
        }

        for (i = 0; i < count; i++)
        {
            hr = m_pIMetaDataImport->GetNameFromToken(rTokens[i], &name);

            if (name == NULL)
                continue;

            szMDName = (MDUTF8STR) name;

            if (iMatchKind == PRINT_ALL)
            {
                g_pShell->Write (L"%s!::%S\n", szModName, szMDName);
                anythingPrinted = true;
            }
            else
            {
                if (iMatchKind == MATCH_N_CHARS)
                {
                    if (!strncmp (szMDName, szSearchName, iLength))
                    {
                        g_pShell->Write (L"%s!::%S\n", szModName, szMDName);
                        anythingPrinted = true;
                    }
                }
                else
                {
                    if (!strcmp (szMDName, szSearchName))
                    {
                        g_pShell->Write (L"%s!::%S\n", szModName, szMDName);
                        anythingPrinted = true;
                    }
                }
            }
        }
    }
    while (count > 0);

    // We might be looking for a class::method, in which case we want to match
    // the class part, then the method part.
    szMethod= strstr(szSearchName, "::");
    iLengthOfMethod = 0;

    if (szMethod != NULL)
    {
        _ASSERTE((szMethod - szSearchName) >= 0);
        _ASSERTE(':' == szMethod[0]);
        _ASSERTE(':' == szMethod[1]);
        szMethod += 2;

        iLengthOfMethod = (int)(iLength - (szMethod - szSearchName));
        iLength -= (iLengthOfMethod +2); //Don't match the "::"
    }


    // now enumerate all the classes...
    phEnum = 0;
    do
    {
        hr = m_pIMetaDataImport->EnumTypeDefs (&phEnum, &rClassTokens[0], 100, &count);

        if (!SUCCEEDED(hr))
        {
            g_pShell->ReportError(hr);
            goto ErrExit;
        }

        for (i = 0; i < count; i++)
        {
            BOOL fMatchFound = FALSE;
            WCHAR wszClassName [MAX_SYMBOL_NAME_LENGTH];
            ULONG ulClassNameLen;
            DWORD dwTypeDefFlags;
            mdToken tkExtends;

            wszClassName [0] = L'\0';

            hr = m_pIMetaDataImport->GetTypeDefProps (rClassTokens [i],
                                                      wszClassName,
                                                      MAX_SYMBOL_NAME_LENGTH-1,
                                                      &ulClassNameLen,
                                                      &dwTypeDefFlags,
                                                      &tkExtends);

            if (wcslen (wszClassName) == 0)
                continue;

            MAKE_UTF8PTR_FROMWIDE (szMDClassName, wszClassName);

            if (iMatchKind == PRINT_ALL)
            {
                fMatchFound = TRUE;
            }
            else
            {
                if (0 == strncmp(szMDClassName, szSearchName, iLength))
                {
                    fMatchFound = TRUE;
                }
            }

            if (fMatchFound)
            {
                ULONG ulMethodCount;
                HCORENUM phMethodEnum = 0;
                do
                {
                    ulMethodCount = 0;
                    hr = m_pIMetaDataImport->EnumMethods(&phMethodEnum, rClassTokens [i], &rTokens[0], 100, &ulMethodCount);

                    if (!SUCCEEDED(hr))
                    {
                        g_pShell->ReportError(hr);
                        goto ErrExit;
                    }

                    for (ULONG j = 0; j < ulMethodCount; j++)
                    {
                        name1 = NULL;
                        hr = m_pIMetaDataImport->GetNameFromToken(rTokens[j], &name1);

                        if ((name1 == NULL)||(strlen (name1) == 0))
                            continue;

                        fMatchFound = FALSE;

                        if (iMatchKind == PRINT_ALL)
                        {
                            fMatchFound = TRUE;
                        }
                        else
                        {
                            if (iMatchKind == MATCH_N_CHARS)
                            {
                                if (szMethod == NULL)
                                {
                                    // We matched all the characters we were given,
                                    // so print anything herein out.
                                    fMatchFound = TRUE;
                                }
                                else if (!strncmp (szMethod, name1, iLengthOfMethod))
                                {
                                    fMatchFound = TRUE;
                                }
                            }
                            else
                            {
                                if (szMethod != NULL &&
                                    !strcmp (szMethod, name1))
                                {
                                    fMatchFound = TRUE;
                                }
                            }
                        }

                        if (fMatchFound == TRUE)
                        {
                            szMDName = (MDUTF8STR)name1;

                            g_pShell->Write (L"%s!%S::%S\n", szModName, szMDClassName, szMDName);
                            anythingPrinted = true;
                        }
                    }
                }
                while (ulMethodCount > 0);
            }
        }

    }while (count > 0);

ErrExit:
    delete[] u_name;

    if (!anythingPrinted)
        return FALSE;
    return TRUE;
}


/* ------------------------------------------------------------------------- *
 * DebuggerSourceFile
 * ------------------------------------------------------------------------- */

DebuggerSourceFile::DebuggerSourceFile(DebuggerModule *module,
                                       ISymUnmanagedDocument *doc)

    : DebuggerBase((LONG_PTR)doc), m_module(module),
      m_totalLines(0), m_lineStarts(NULL),
      m_sourceTextLoaded(false),
      m_allBlocksLoaded(false),
      m_sourceNotFound(FALSE)
{
    _ASSERTE(doc);

    ULONG32 nameLen = 0;

    // Keep a hold of the document pointer
    m_doc = doc;
    m_doc->AddRef();

    // Get the length of the name first
    HRESULT hr = doc->GetURL(0, &nameLen, NULL);

    // Allocate space for the name
    if (SUCCEEDED(hr))
    {
        if (m_name.Allocate(nameLen))
        {
            // Now, copy the name for real.
            hr = doc->GetURL(nameLen + 1, &nameLen, m_name.GetData());
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    // If unsuccessful, don't initialize the source name
    if (FAILED(hr))
    {
        g_pShell->Write(L"Error loading source file info from symbol "
                        L"store.\n");
        g_pShell->ReportError(hr);

        m_name.Clear();
    }
}

DebuggerSourceFile::~DebuggerSourceFile()
{
    _ASSERTE(m_doc);
    m_doc->Release();
    m_doc = NULL;

    delete [] m_lineStarts;
}

// LoadText loads the text of a source file and builds a table of pointers
// to the start of each line.
//
//
BOOL DebuggerSourceFile::LoadText(const WCHAR* path, bool bChangeOfName)
{
    BOOLEAN fRetVal = FALSE;
    char *sourceA = NULL;
    int size = 0;
    WCHAR* ptr = NULL;
    unsigned int i = 0;
    int ilen = 0;

    if (m_sourceTextLoaded)
        return (TRUE);

    // Where to store fully qualified name
    char fullyQualifiedName[MAX_PATH];

    // Must convert to ANSI for Win9x users
    MAKE_ANSIPTR_FROMWIDE(pathA, path);
    _ASSERTE(pathA != NULL);


    HRESULT hr;

    // Let the shell see if it can resolve the source location.
    if ((hr = g_pShell->ResolveSourceFile(this, pathA, fullyQualifiedName, MAX_PATH, bChangeOfName)) != S_OK)
    {
        if (!m_sourceNotFound)
        {
            g_pShell->Write(L"\nError loading source file '%s': File not found\n", (m_name == NULL) ? L"unknown" : m_name.GetData());
            m_sourceNotFound = TRUE;
        }

        return (FALSE);

    }


    m_sourceNotFound = FALSE;

    // Read the source file into memory.
    HANDLE hFile = CreateFileA(fullyQualifiedName, GENERIC_READ,
                               FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        HRESULT localHR = HRESULT_FROM_WIN32(GetLastError());

        // No luck, report error.
        g_pShell->ReportError(localHR);
        return (FALSE);
    }

    DWORD sizeA = SafeGetFileSize(hFile, NULL);

    if (sizeA == 0xFFFFFFFF)
    {
        g_pShell->ReportError(HRESULT_FROM_WIN32(GetLastError()));
        CloseHandle(hFile);
        return (FALSE);
    }

    sourceA = new char[ (sizeA * sizeof(char)) ];
    if (sourceA == NULL)
    {
        g_pShell->Write( L"Insufficient memory to load file '%s'\n",m_name.GetData());
        CloseHandle(hFile);
        return (FALSE);
    }

    DWORD sizeReadA;
    BOOL ret = ReadFile(hFile, (void*) sourceA, sizeA, &sizeReadA, NULL);

    CloseHandle(hFile);

    if ((ret == FALSE) || (sizeA != sizeReadA))
    {
        if (ret == FALSE)
            g_pShell->ReportError(HRESULT_FROM_WIN32(GetLastError()));

        goto LExit;
    }

    // In unicode files, the first two bytes are 0xFF and 0xFE
    if ((BYTE)(sourceA[0]) == 0xff && (BYTE)(sourceA[1]) == 0xfe)
    {
        // Calculate the number of unicode chars
        _ASSERTE(sizeA%2 == 0);
        size = sizeA/2;

        // Skip the first unicode char
        size--;

        // Alocate the memory
        m_source.CopyNFrom((WCHAR*) (sourceA+2), size);
    }

    else
    {
        // Pick the code page to use in translation
        UINT codePage = CP_ACP;  // Default to ANSI
        SIZE_T bytesToSkip = 0;  // Don't skip any chars by default

        // UTF-8 files can start with 0xef 0xbb 0xbf
        if ((BYTE)(sourceA[0]) == 0xef &&
            (BYTE)(sourceA[1]) == 0xbb &&
            (BYTE)(sourceA[2]) == 0xbf)
        {
            codePage = CP_UTF8;
            bytesToSkip = 3;
        }

        // Translate the source file into UNICODE
        size = WszMultiByteToWideChar(
            codePage, 0, sourceA + bytesToSkip,
            (int)(sizeA - bytesToSkip), NULL, 0);

        if (!m_source.Allocate(size))
        {
            goto LExit;
        }

        WszMultiByteToWideChar(
            codePage, 0, sourceA + bytesToSkip,
            (int)(sizeA - bytesToSkip), m_source.GetData(), size);

        // Null terminate the wide string array
        m_source [size] = L'\0';
    }

    //
    // Figure out how many lines are in this file.
    //
    ptr = m_source.GetData();
    m_totalLines = 0;

    while (ptr < (m_source.GetData() + size))
    {
        if (*ptr++ == L'\n')
            m_totalLines++;
    }

    // account for the fact that the last line of the
    // file may contain text. So number of lines needs
    // to be incremented if this is the case.
    if ((size > 0) && (*(ptr-1) != L'\n'))
        m_totalLines++;

    // Build an array to point to the beginning of each line of the file.
    // Chop up the file into separate strings while we're at it.
    if (m_totalLines == 0)
        goto LExit;


    m_lineStarts = new WCHAR*[m_totalLines];
    _ASSERTE(m_lineStarts != NULL);

    if (m_lineStarts == NULL)
    {
        m_source.Clear();
        goto LExit;
    }

    ptr = m_source.GetData();
    m_lineStarts[i++] = ptr;

    while (ptr < (m_source.GetData() + size))
    {
        if (*ptr++ == '\n')
        {
            *(ptr - 1) = '\0';

            if ((ptr > m_source.GetData() + 1) && (*(ptr - 2) == '\r'))
                *(ptr - 2) = '\0';

            if (i < m_totalLines)
                m_lineStarts[i++] = ptr;
        }
    }

    m_sourceTextLoaded = TRUE;

    ilen = MultiByteToWideChar(CP_ACP, 0, fullyQualifiedName, -1, NULL, 0);
    if (m_path.Allocate(ilen))
    {
        MultiByteToWideChar(CP_ACP, 0, fullyQualifiedName, -1, m_path.GetData(), ilen+1);
    }
    fRetVal =  (TRUE);
 LExit:
    if (sourceA != NULL )
    {
        delete [] sourceA;
        sourceA = NULL;
    }

    return fRetVal;
}

BOOL DebuggerSourceFile::ReloadText(const WCHAR *path, bool bChangeOfName)
{
    m_sourceTextLoaded = FALSE;

    if (m_lineStarts != NULL)
    {
        delete m_lineStarts;
        m_lineStarts = NULL;
    }

    m_totalLines = 0;

    return (LoadText(path, bChangeOfName));
}

unsigned int DebuggerSourceFile::FindClosestLine(unsigned int line,
                                                 bool silently)
{
    HRESULT hr = S_OK;

    // Find and return the closest line in this document.
    ULONG32 closeLine;
    hr = GetDocument()->FindClosestLine(line, &closeLine);

    if (SUCCEEDED(hr))
    {
        if ((hr == S_FALSE) ||
            ((closeLine > (line + 3)) || (closeLine < (line - 3))))
            hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
        return closeLine;
    else
    {
        if (!silently)
            g_pShell->Write(L"Warning: source line may not have code. "
                            L"Breakpoint binding may fail.\n");

        return line;
    }
}

/* ------------------------------------------------------------------------- *
 * DebuggerFunction
 * ------------------------------------------------------------------------- */

//
// _skipTypeInSignature -- skip past a type in a given signature.
// Returns the number of bytes used by the type in the signature.
//
static ULONG _skipTypeInSignature(PCCOR_SIGNATURE sig)
{
    ULONG cb = 0;
    ULONG elementType = ELEMENT_TYPE_END;
    ULONG cBytes;

    cb = CorSigUncompressData(&sig[cb], &elementType);

    if (cb == (ULONG)-1)
    {
        return (ULONG)-1;
    }

    // CorSigUncompressData should have overwritten the elementType
    _ASSERTE(elementType != ELEMENT_TYPE_END);

    if ((elementType == ELEMENT_TYPE_CLASS) ||
        (elementType == ELEMENT_TYPE_VALUETYPE))
    {
        // Skip over typeref.
        mdToken typeRef;
        cBytes = CorSigUncompressToken(&sig[cb], &typeRef);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;
    }
    else if ((elementType == ELEMENT_TYPE_PTR) ||
             (elementType == ELEMENT_TYPE_BYREF) ||
             (elementType == ELEMENT_TYPE_PINNED) ||
             (elementType == ELEMENT_TYPE_SZARRAY))
    {
        // Skip over extra embedded type.
        cBytes = _skipTypeInSignature(&sig[cb]);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;
    }
    else if (elementType == ELEMENT_TYPE_ARRAY)
    {
        // Skip over extra embedded type.
        cBytes = _skipTypeInSignature(&sig[cb]);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;

        // Skip over rank
        ULONG rank;
        cBytes = CorSigUncompressData(&sig[cb], &rank);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;

        if (rank > 0)
        {
            // how many sizes?
            ULONG sizes;
            cBytes = CorSigUncompressData(&sig[cb], &sizes);

            if (cBytes == (ULONG)-1)
            {
                return (ULONG)-1;
            }

            cb += cBytes;

            // read out all the sizes
            unsigned int i;

            for (i = 0; i < sizes; i++)
            {
                ULONG dimSize;
                cBytes = CorSigUncompressData(&sig[cb], &dimSize);

                if (cBytes == (ULONG)-1)
                {
                    return (ULONG)-1;
                }

                cb += cBytes;
            }

            // how many lower bounds?
            ULONG lowers;
            cBytes = CorSigUncompressData(&sig[cb], &lowers);

            if (cBytes == (ULONG)-1)
            {
                return (ULONG)-1;
            }

            cb += cBytes;

            // read out all the lower bounds.
            for (i = 0; i < lowers; i++)
            {
                int lowerBound;
                cBytes = CorSigUncompressSignedInt(&sig[cb], &lowerBound);

                if (cBytes == (ULONG)-1)
                {
                    return (ULONG)-1;
                }

                cb += cBytes;
            }
        }
    }
    else if (elementType == ELEMENT_TYPE_GENERICINST)
    {
        cBytes = _skipTypeInSignature(&sig[cb]);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;

        ULONG argCnt;
        cBytes = CorSigUncompressData(&sig[cb], &argCnt); // Get number of parameters

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;
        while (argCnt--)
        {
            cBytes = _skipTypeInSignature(&sig[cb]);        // Skip the parameters

            if (cBytes == (ULONG)-1)
            {
                return (ULONG)-1;
            }

            cb += cBytes;
        }
    }
    else if ((elementType == ELEMENT_TYPE_VAR) ||
             (elementType == ELEMENT_TYPE_MVAR))
    {
        ULONG tyvarnum;
        cBytes = CorSigUncompressData(&sig[cb], &tyvarnum);

        if (cBytes == (ULONG)-1)
        {
            return (ULONG)-1;
        }

        cb += cBytes;
    }

    return (cb);
}

//
// Static, global byte used to represent a signature for a class. A pointer
// to this is used as the signature for the "this" argument to methods
//
static const BYTE g_ObjectSignature = ELEMENT_TYPE_CLASS;


namespace
{
    //
    // Certain wacky compilers out there (we won't name any names) emit "fake" sequence points that do not map to any
    // source. These sequence points are marked by their source line, which is some sentinel value.
    //
    // This function returns true if and only if the given source line maps to a "real" sequence point.
    //
    BOOL MapsToSource(ULONG32 sourceLine)
    {
        return (!((0xfeefee == sourceLine) || (0xfeefed == sourceLine)));
    }
} // anon namespace


DebuggerFunctionILRange::DebuggerFunctionILRange(const COR_DEBUG_STEP_RANGE& range,
                                                 ISymUnmanagedDocument* document, ULONG32 sourceLine) :
        m_range(range), m_document(document), m_sourceLine(sourceLine)
{
    if (NULL != m_document)
    {
        m_document->AddRef();
    }
}

DebuggerFunctionILRange::~DebuggerFunctionILRange()
{
    if (NULL != m_document)
    {
        m_document->Release();
    }
}

COR_DEBUG_STEP_RANGE
DebuggerFunctionILRange::GetRange() const
{
    return m_range;
}

ISymUnmanagedDocument*
DebuggerFunctionILRange::GetDocument()
{
    return m_document;
}

ULONG32
DebuggerFunctionILRange::GetSourceLine() const
{
    return m_sourceLine;
}

BOOL
DebuggerFunctionILRange::Contains(UINT_PTR ip) const
{
    return ((m_range.startOffset <= ip) && (ip < m_range.endOffset));
}



//
// DebuggerFunction
//
DebuggerFunction::DebuggerFunction(DebuggerModule *m, mdMethodDef md,
                                   ICorDebugFunction *iFunction)
    : DebuggerBase(md), m_module(m), m_class(0), m_ifunction(iFunction),
      m_isStatic(false),  m_allBlocksLoaded(false), m_allScopesLoaded(false),
      m_signature(NULL),
      m_VCHack(FALSE), m_arguments(NULL), m_argCount(0),
      m_returnType(NULL),
      m_ilCode(NULL), m_ilCodeSize(0), m_nativeCode(NULL), m_nativeCodeSize(0),
      m_sourcefulRanges(NULL), m_sourcefulRangeCount(0),
      m_sourcelessRanges(NULL), m_sourcelessRangeCount(0)
{
    if (md == mdMethodDefNil)
    {
        m_token = (ULONG_PTR) iFunction;
        iFunction->AddRef();
        m_VCHack = TRUE;
    }
}

namespace
{
    // Cleanup helper for CacheSequencePoints
    void CleanupDocuments(ISymUnmanagedDocument** documents, const ULONG32 count)
    {
        for (ULONG32 i = 0; i < count; ++i)
        {
            documents[i]->Release();
        }
    }
}

//
// Obtain & Cache sequence point information
//
HRESULT DebuggerFunction::CacheSequencePoints(void)
{
    HRESULT hr = S_OK;
    ISymUnmanagedDocument* pLastDuplicateDocumentFound = NULL;
    ISymUnmanagedDocument* pLastOriginalDocumentFound = NULL;
    ULONG32* offsets;
    ULONG32* lines;
    ISymUnmanagedDocument** documents = NULL;

    this->CleanupRanges();

    if (m_symMethod == NULL)
    {
        return hr;
    }

    // Now, load up the sequence points for this function since we
    // know that we'll need them later.

    ULONG32 sequencePointCount = 0;

    hr = m_symMethod->GetSequencePointCount(&sequencePointCount);

    TESTANDRETURNHR(hr);

    if (sequencePointCount <= 0)
    {
        return hr;
    }

    m_sourcefulRanges = new DebuggerFunctionILRange*[sequencePointCount];
    if (NULL == m_sourcefulRanges)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }
    m_sourcelessRanges = new DebuggerFunctionILRange*[sequencePointCount];

    if (NULL == m_sourcelessRanges)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }

    ULONG32 actualCount;

    offsets = reinterpret_cast< ULONG32* >(_alloca((sequencePointCount+1) * sizeof(ULONG32)));
    lines = reinterpret_cast< ULONG32* >(_alloca(sequencePointCount * sizeof(ULONG32)));
    documents = reinterpret_cast< ISymUnmanagedDocument** >(_alloca(sequencePointCount * sizeof(ISymUnmanagedDocument*)));

    hr = m_symMethod->GetSequencePoints(sequencePointCount, &actualCount, offsets,
                                        documents, lines, NULL, NULL, NULL);
    _ASSERTE(actualCount == sequencePointCount);
    if (FAILED(hr))
    {
        // If the call to GetSequencePoints fails, I don't need to Release the documents.
        documents = NULL;
        goto Cleanup;
    }


    // As a catch all, we're going to assume that the IL range that begins with the last sequence point "ends" off in
    // deep space.
    offsets[sequencePointCount] = 0xffffffff;

    m_sourcelessRangeCount = m_sourcefulRangeCount = 0;
    for (ULONG32 i = 0; i < sequencePointCount; ++i)
    {
        // As it is the implementation of GetSequencepoints doesn't find existing docs and just addref them, it creates
        // a brand new ISymUnmgdDoc object every time we call things like GetSequencePoints, so we need to do the work
        // to resolve all these ISymUnmgdDoc pointers to a common one.
        //
        // This is to search the existing documents of the function's module to find its ISymUnmanagedDocument that
        // matches the ones passed in the documents array, Only do a full search if this pointer's not the same as the
        // previous (In most cases, the array of ISymUnmgdDocs will all be pointers to the same object)
        if (documents[i] != pLastDuplicateDocumentFound)
        {
            // go looking for the new document

            pLastDuplicateDocumentFound = documents[i];
            pLastOriginalDocumentFound = m_module->FindDuplicateDocumentByURL(documents[i]);
            _ASSERTE(NULL != pLastOriginalDocumentFound);
        }


        COR_DEBUG_STEP_RANGE range =
        {
             offsets[i], offsets[i+1]
        };
        DebuggerFunctionILRange* pRange =
                new DebuggerFunctionILRange(range, pLastOriginalDocumentFound, lines[i]);
        if (NULL == pRange)
        {
            hr = E_OUTOFMEMORY;
            goto Cleanup;
        }

        // We need to separate those sequence points which do map to source from those that do not.
        if (MapsToSource(lines[i]))
        {
            m_sourcefulRanges[m_sourcefulRangeCount] = pRange;
            ++m_sourcefulRangeCount;
        }
        else
        {
            m_sourcelessRanges[m_sourcelessRangeCount] =  pRange;
            ++m_sourcelessRangeCount;
        }
    }

    // make sure I didn't "lose" any seqeunce points by accident
    _ASSERTE(sequencePointCount == m_sourcelessRangeCount + m_sourcefulRangeCount);

Cleanup:

    if (FAILED(hr))
    {
        this->CleanupRanges();
    }

    if (NULL != documents)
    {
        CleanupDocuments(documents, sequencePointCount);
    }

    return hr;
}




//
// Initialize a DebuggerFunction object.
//
HRESULT DebuggerFunction::Init(void)
{
    if (m_VCHack)
        return (S_OK);

    HRESULT hr = S_OK;

    m_nEditAndContinueLastSynched = g_pShell->m_cEditAndContinues;

    // Get the symbol reader method for this method.
    if (GetModule()->GetSymbolReader() != NULL)
    {
        hr = GetModule()->GetSymbolReader()->GetMethod((ULONG)GetToken(), &m_symMethod);

        if (hr != S_OK)
            m_symMethod.Clear();
    }
    else
        m_symMethod.Clear();

    CacheSequencePoints();

    //
    // Get properties of the method.
    //
    mdTypeDef   classToken = mdTypeDefNil;
    WCHAR       methodName[BUF_SIZE];
    ULONG       methodNameLength = 0;
    PCCOR_SIGNATURE sigBlob = NULL;
    ULONG       sigBlobSize = 0;
    DWORD       methodAttr = 0;
    ULONG       cb = 0;
    ULONG       cBytes;

    hr = m_module->GetMetaData()->GetMethodProps((ULONG)GetToken(),
                                                 &classToken,
                                                 methodName,
                                                 BUF_SIZE,
                                                 &methodNameLength,
                                                 &methodAttr,
                                                 &sigBlob,
                                                 &sigBlobSize,
                                                 NULL, NULL);

    TESTANDRETURNHR(hr);

    m_signature = sigBlob;
    m_class = classToken;
    if (!m_name.CopyNFrom(methodName, methodNameLength))
    {
        return E_OUTOFMEMORY;
    }


    //
    // Get properites of this method's class. (Mostly for the class name.)
    //
    if (m_class != mdTypeDefNil)
    {
        WCHAR       fullName[MAX_CLASSNAME_LENGTH];
        ULONG       fullNameSize = 0;
        WCHAR       *Namespace;
        ULONG       NamespaceSize=0;
        WCHAR       *className;
        ULONG       classNameSize = 0;

        hr = m_module->GetMetaData()->GetTypeDefProps(classToken,
                                                      fullName, MAX_CLASSNAME_LENGTH,
                                                      &fullNameSize,
                                                      NULL, NULL);
        TESTANDRETURNHR(hr);

        Namespace = fullName;
        className  = wcsrchr(fullName, L'.');
        if (className)
            *className++ = 0;
        else
        {
            Namespace = L"";
            className = fullName;
        }

        NamespaceSize = (ULONG)(wcslen(Namespace) + 1);
        classNameSize = (ULONG)(wcslen(className) + 1);

        if (!m_namespaceName.Allocate(NamespaceSize+1)) // +1 for the '.' at the end.
        {
            return (E_OUTOFMEMORY);
        }
        memcpy(m_namespaceName.GetData(), Namespace, NamespaceSize * sizeof(WCHAR));

        // Append a '.' for any non-empty string.
        if (wcslen(Namespace))
            wcscat (m_namespaceName.GetData(), L".");
        else
            m_namespaceName[0] = L'\0';


        if (!m_className.CopyFrom(className))
        {
            return (E_OUTOFMEMORY);
        }
    }
    else
    {
        if (!m_className.CopyFrom(L""))
        {
            return (E_OUTOFMEMORY);
        }

        if (!m_namespaceName.CopyFrom(L""))
        {
            return (E_OUTOFMEMORY);
        }
    }

    //
    // Make sure we have a method signature.
    //
    ULONG callConv;
    cBytes = CorSigUncompressData(&sigBlob[cb], &callConv);

    if (cBytes == (ULONG)-1)
    {
        return E_FAIL;
    }

    cb += cBytes;

    _ASSERTE(callConv != IMAGE_CEE_CS_CALLCONV_FIELD);


    //
    // Grab the type argument count.
    //
    if (callConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
    {
      ULONG tyArgCount;
      cBytes = CorSigUncompressData(&sigBlob[cb], &tyArgCount);

      if (cBytes == (ULONG)-1)
      {
          return E_FAIL;
      }

      cb += cBytes;
    }

    //
    // Grab the argument count.
    //
    ULONG argCount;
    cBytes = CorSigUncompressData(&sigBlob[cb], &argCount);

    if (cBytes == (ULONG)-1)
    {
        return E_FAIL;
    }

    cb += cBytes;

    m_argCount = argCount;

    //
    // Have the return type point directly to the return type in the
    // method signature.
    //
    m_returnType = &sigBlob[cb];

    //
    // Skip past the return type.
    //
    cBytes = _skipTypeInSignature(&sigBlob[cb]);

    if (cBytes == (ULONG)-1)
    {
        return E_FAIL;
    }

    cb += cBytes;

    m_isStatic = (methodAttr & mdStatic) != 0;


    if (!m_isStatic)
        m_argCount++;

    if (m_argCount)
    {
        unsigned int i;

        m_arguments = new DebuggerVarInfo[m_argCount];

        if (m_arguments == NULL)
        {
            m_argCount = 0;
            return (S_OK);
        }

        if (!m_isStatic)
        {
            char *newName = new char[5];
            _ASSERTE(newName != NULL);
            strcpy(newName, "this");
            m_arguments[0].SetName(newName);
            m_arguments[0].sig = (PCCOR_SIGNATURE) &g_ObjectSignature; // never really used...
            m_arguments[0].varNumber = 0;

            i = 1;
        }
        else
            i = 0;

        for (; i < m_argCount; i++)
        {
            m_arguments[i].SetName(NULL);
            m_arguments[i].sig = &sigBlob[cb];
            m_arguments[i].varNumber = i;

            cBytes = _skipTypeInSignature(&sigBlob[cb]);

            if (cBytes == (ULONG)-1)
            {
                return E_FAIL;
            }

            cb += cBytes;
        }
    }

    hr = m_module->GetICorDebugModule()->GetFunctionFromToken((ULONG)GetToken(),
                                                              &m_ifunction);

    if( FAILED(hr) )
        return hr;

    // Now, load any argument names.
    if (m_argCount > 0)
    {
        HCORENUM paramEnum = NULL;
        bool fCloseEnum = false;
        IMetaDataImport *pIMI = m_module->GetMetaData();

        while (TRUE)
        {
            mdParamDef param[1];
            ULONG      numParams = 0;

            hr = pIMI->EnumParams(&paramEnum,
                                  (ULONG)GetToken(),
                                  param, 1, &numParams);

            if (SUCCEEDED(hr) && (numParams == 0))
            {
                fCloseEnum = true;
                hr = S_OK;
                break;
            }

            if (FAILED(hr))
                break;

            fCloseEnum = true;

            WCHAR name[BUF_SIZE];
            ULONG nameLen;
            ULONG seq;

            hr = pIMI->GetParamProps(param[0], 0,
                                     &seq, name, BUF_SIZE, &nameLen,
                                     NULL, NULL, NULL, NULL);

            if (SUCCEEDED(hr))
            {
                // If we have a name, then assign it now.
                // Else, we'll let this variable pick up a default name below.
                if (nameLen > 0)
                {
                    char* newName = new char[nameLen];
                    _ASSERTE(newName != NULL);

                    unsigned int i;

                    for (i = 0; i < nameLen; i++)
                        newName[i] = (char)(name[i]);

                    if (m_isStatic)
                        seq--;

                    if (seq < m_argCount)
                        m_arguments[seq].SetName(newName);
                    else
                        delete [] newName;
                }
            }
            else
                break;
        }

        if (fCloseEnum)
            pIMI->CloseEnum(paramEnum);

        // Give any unnamed aguments names now...
        for (unsigned int i = 0; i < m_argCount; i++)
        {
            if (m_arguments[i].GetName() == NULL)
            {
                char *newName = new char[07];
                _ASSERTE(newName != NULL);
                sprintf(newName, "Arg%d", i);
                m_arguments[i].SetName(newName);
            }
        }
    }

    if (m_module->GetMetaData2() == NULL)
    {
        _ASSERTE(!"NYI");
        return E_NOINTERFACE;
    }

    m_typeArgCount = 0;
    {
            HCORENUM enumClassTyPars = 0;
            mdGenericParam classTyPar[8];
            ULONG cClassTyPars;
            do {
                hr = m_module->GetMetaData2()->EnumGenericParams(&enumClassTyPars, classToken, classTyPar, 8, &cClassTyPars);
                if( FAILED(hr) )
                    return hr;
                m_typeArgCount+= cClassTyPars;
            } while (cClassTyPars > 0);
            m_module->GetMetaData2()->CloseEnum(enumClassTyPars);

        // Add the method genericArgs....
        HCORENUM enumMethTyPars = 0;
        mdGenericParam methTyPar[8];
        ULONG cMethTyPars;
        do {
            hr = m_module->GetMetaData2()->EnumGenericParams(&enumMethTyPars, (ULONG)GetToken(), methTyPar, 8, &cMethTyPars);
            if( FAILED(hr) )
                return hr;
            m_typeArgCount+= cMethTyPars;
        } while (cMethTyPars > 0);
        m_module->GetMetaData2()->CloseEnum(enumMethTyPars);
    }

    // Set the sequence numbers...
    if (m_typeArgCount)
    {
        m_typeArguments = new DebuggerVarInfo[m_typeArgCount];

        if (m_typeArguments == NULL)
        {
            m_typeArgCount = 0;
            return (S_OK);
        }

        for (unsigned int i = 0; i < m_typeArgCount; i++)
        {
            m_typeArguments[i].SetName(NULL);
            m_typeArguments[i].sig = NULL;
            m_typeArguments[i].varNumber = i;
        }
    }


    // Now, load any type argument names.
    if (m_typeArgCount > 0)
    {
        // Add the class type parameters if the method is not static
            HCORENUM enumClassTyPars = 0;
            mdGenericParam classTyPar[1];
            ULONG cClassTyPars;
            ULONG typarNum = 0;
            do {
                hr = m_module->GetMetaData2()->EnumGenericParams(&enumClassTyPars, classToken, classTyPar, 1, &cClassTyPars);
                if( FAILED(hr) )
                    return hr;
                if (cClassTyPars == 0)
                    break;
                WCHAR name[BUF_SIZE];
                ULONG nameLen;
                ULONG seq;
                hr = m_module->GetMetaData2()->GetGenericParamProps(classTyPar[0],
                        &seq,NULL/*flags*/,NULL/*parent*/,NULL/*kind*/,
                        name,BUF_SIZE,&nameLen);
                if( FAILED(hr) )
                    return hr;
                char* newName = new char[nameLen];
                _ASSERTE(newName != NULL);
                for (unsigned int i = 0; i < nameLen; i++)
                    newName[i] = (char)(name[i]);
                if (typarNum < m_typeArgCount)
                    m_typeArguments[typarNum].SetName(newName);
                typarNum++;
            } while (true);
            m_module->GetMetaData2()->CloseEnum(enumClassTyPars);

        // Add the method genericArgs....
        HCORENUM enumMethTyPars = 0;
        mdGenericParam methTyPar[1];
        ULONG cMethTyPars;
        do {
            hr = m_module->GetMetaData2()->EnumGenericParams(&enumMethTyPars, (ULONG)GetToken(), methTyPar, 1, &cMethTyPars);
            if( FAILED(hr) )
                return hr;
            if (cMethTyPars == 0)
                break;
            WCHAR name[BUF_SIZE];
            ULONG nameLen;
            ULONG seq;
            hr = m_module->GetMetaData2()->GetGenericParamProps(methTyPar[0],
                &seq,NULL/*flags*/,NULL/*parent*/,NULL/*kind*/,
                name,BUF_SIZE,&nameLen);
            if( FAILED(hr) )
                return hr;
            char* newName = new char[nameLen];
            _ASSERTE(newName != NULL);
            for (unsigned int i = 0; i < nameLen; i++)
                newName[i] = (char)(name[i]);
            if (typarNum < m_typeArgCount)
                m_typeArguments[typarNum].SetName(newName);
            typarNum++;
        } while (true);
        m_module->GetMetaData2()->CloseEnum(enumMethTyPars);

        _ASSERTE(typarNum == m_typeArgCount);
    }

    return (hr);
}

void
DebuggerFunction::CleanupRanges()
{
    if (NULL != m_sourcefulRanges)
    {
        for (ULONG32 i = 0; i < m_sourcefulRangeCount; ++i)
        {
            delete m_sourcefulRanges[i];
        }
        delete[] m_sourcefulRanges;
    }
    if (NULL != m_sourcelessRanges)
    {
        for (ULONG32 i = 0; i < m_sourcelessRangeCount; ++i)
        {
            delete m_sourcelessRanges[i];
        }
        delete[] m_sourcelessRanges;
    }
    m_sourcelessRanges = m_sourcelessRanges = NULL;
    m_sourcelessRangeCount = m_sourcefulRangeCount = 0;
}

DebuggerFunction::~DebuggerFunction()
{

    if (m_arguments != NULL)
    {
        unsigned int i;

        for (i = 0; i < m_argCount; i++)
            delete [] (char *) m_arguments[i].GetName();

        delete [] m_arguments;
    }

    delete m_nativeCode;
    delete m_ilCode;

    if (g_pShell != NULL)
        g_pShell->m_invalidCache = false;

    if (m_ifunction != NULL)
        RELEASE(m_ifunction);

    this->CleanupRanges();
}

HRESULT
DebuggerFunction::SynchronizeSequencePoints()
{
    // There is no work to do if we are already up to date.
    if (g_pShell->m_cEditAndContinues == m_nEditAndContinueLastSynched)
    {
        return S_OK;
    }


    HRESULT hr = this->CacheSequencePoints();

    if (FAILED( hr ) )
    {
        return hr;
    }

    m_nEditAndContinueLastSynched = g_pShell->m_cEditAndContinues;

    return S_OK;
}

//
// DebuggerFunction::FindLineFromIP
//
// Description
//      Determines the line of source code that corresponds to a given IP
//
// Parameters
//      ip      The IP to search for
//      pDoc    [out] The source file that contains a line of source that maps to "ip".
//      line    [out] The line of source that maps to "ip"
//
//      Notes:
//          If "ip" does map to a sequence point in the current function, but the line number cannot be determined (e.g.
//          if "ip" maps to a 0xfeefee sequence point), pDoc is set to the document corresponding to that file and line
//          is set to the sentinel value DebuggerFunction::kNoSourceForIL. S_OK is returned in this case.
//
//          If "ip" does not map to any sequence point in the function. S_FALSE is returned. This means the IP does not
//          lie within this function. The values of pDoc and line are meaningless
//
//          It is safe to pass in NULL as the value of pDoc.
//          It is safe to pass in NULL as the value of line.
//
//
// Returns
//      HRESULT     S_OK
//                  S_FALSE     If there is no sequence point in the function that describes the given IP.
//                  Other       Other failures (see DebuggerFunction::CacheSequencePoints)
//
//
// Exceptions
//      None
//
HRESULT DebuggerFunction::FindLineFromIP(UINT_PTR ip,
                                         DebuggerSourceFile **pDoc,
                                         unsigned int *line)
{
    *pDoc = NULL;
    if (NULL != line)
    {
        *line = 0;
    }

    // reflash the line info.
    // We are doing this for dynamic module only. Because LoadModule
    // event is sent before UpdateModuleSym. But binding the breakpoint
    // by name will trigger up to cache an useless info before the pdb
    // is passed to us through stream.
    //
    // We don't care for failure in this case.
    if (m_symMethod == NULL && GetModule() != NULL && GetModule()->GetSymbolReader() != NULL)
    {
        HRESULT hr = S_OK;

        hr = GetModule()->GetSymbolReader()->GetMethod((ULONG)GetToken(), &m_symMethod);
        if (hr != S_OK)
        {
            m_symMethod.Clear();
        }
        else
        {
            this->CacheSequencePoints();
        }
    }

    this->SynchronizeSequencePoints();

    for (ULONG32 i = 0; i < m_sourcefulRangeCount; ++i)
    {
        if (m_sourcefulRanges[i]->Contains(ip))
        {
            if (NULL != line)
            {
                *line = m_sourcefulRanges[i]->GetSourceLine();
            }
            if (NULL != pDoc)
            {
                *pDoc = this->GetModule()->ResolveSourceFile(m_sourcefulRanges[i]->GetDocument());
            }
            return S_OK;
        }
    }

    // In this case, there is no source code that maps to the given IP.

    for (ULONG32 i = 0; i < m_sourcelessRangeCount; ++i)
    {
        if (m_sourcelessRanges[i]->Contains(ip))
        {
            if (NULL != line)
            {
                *line = kNoSourceForIL;
            }
            if (NULL != pDoc)
            {
                *pDoc = this->GetModule()->ResolveSourceFile(m_sourcelessRanges[i]->GetDocument());
            }
            return S_OK;
        }
    }

    return S_FALSE;
}

//
// DebuggerFunction::FindLineClosest
//
//
// Description
//      Determines the line of source code closest to a given IL offset.
//
// Parameters
//      ip              The IP to search for
//      sourceFile      [out] The source file that contains a line of source that maps to "ip".
//      line            [out] The line of source that maps to "ip"
//
//      Notes:
//
//          It is safe to pass in NULL as the value of sourceFile.
//          It is safe to pass in NULL as the value of line.
//
//
// Returns
//      None
//
//
// Exceptions
//      None
//

void
DebuggerFunction::FindLineClosestToIP(UINT_PTR ip,
                                      DebuggerSourceFile** sourceFile,
                                      unsigned int* line)
{
    this->SynchronizeSequencePoints();

    // NOTE
    //      This implementation assumes that the ranges listed m_sourcefulRanges are sorted from lowest IL offset to
    //      highest.

#ifdef _DEBUG
    ULONG32 lastEndOffset = 0;
#endif // _DEBUG

    for (ULONG32 i = 0; i < m_sourcefulRangeCount; ++i)
    {
        _ASSERTE(lastEndOffset <= m_sourcefulRanges[i]->GetRange().endOffset);
        if (ip < m_sourcefulRanges[i]->GetRange().endOffset)
        {
            if (NULL != line)
            {
                *line = m_sourcefulRanges[i]->GetSourceLine();
            }
            if (NULL != sourceFile)
            {
                *sourceFile = this->GetModule()->ResolveSourceFile(m_sourcefulRanges[i]->GetDocument());
            }
            return;
        }
#ifdef _DEBUG
        lastEndOffset = m_sourcefulRanges[i]->GetRange().endOffset;
#endif // _DEBUG
    }
}


// DebuggerFunction::GetRanges
//
// Description
//      Finds the step ranges associated with a given region of source
//      (a document and a line number)
//
//      Note: This implementation of GetRanges relies on pointer equality to find
//            out if the documents are "equal". If the parameter pDocument is congruent the Document associated with an
//            IL range, but is a different instance of ISymUnmanagedDocument, this function will not find any step
//            ranges and return E_FAIL.
//
// Parameters
//      pDocument
//          [in] the document
//      line
//          [in] the line of source
//      cRangeArraySize
//          [in] the size (in number of COR_DEBUG_STEP_RANGEs) of pRangeArray
//      pcRangeArraySizeOut
//          [out] The number of ranges (may be NULL)
//      pRangeArray
//          [out] an array of COR_DEBUG_STEP_RANGEs to storp start/end
//                offsset pairs (may be NULL)
//
// Returns
//      HRESULT
//
//      S_OK -- if the function completed normally
//      E_FAIL -- if there were no step ranges found for the given position in the document
//
// Exceptions
//      None
HRESULT
DebuggerFunction::GetRanges(ISymUnmanagedDocument* pDocument,
                            const unsigned int line,
                            const unsigned int cRangeArraySize,
                            unsigned int* pcRangeArraySizeOut,
                            COR_DEBUG_STEP_RANGE* pRangeArray) const

{
    unsigned int cRangesFound = 0;

    for (unsigned int i = 0; i < m_sourcefulRangeCount; ++i)
    {
        if ((line == m_sourcefulRanges[i]->GetSourceLine()) &&
            (pDocument== m_sourcefulRanges[i]->GetDocument()))
        {

            if ((NULL != pRangeArray) && (cRangesFound < cRangeArraySize))
            {
                pRangeArray[cRangesFound] = m_sourcefulRanges[i]->GetRange();
            }
            ++cRangesFound;
        }
    }

    if (NULL != pcRangeArraySizeOut)
    {
        *pcRangeArraySizeOut = cRangesFound;
    }

    // No source ranges were found,
    if (0 == cRangesFound)
    {
        return E_FAIL;
    }

    return S_OK;
}

//
// Get the line stepping range for the given ip.
//

void DebuggerFunction::GetStepRangesFromIP(UINT_PTR ip,
                                           COR_DEBUG_STEP_RANGE **range,
                                           SIZE_T *rangeCount)
{
    HRESULT hr;
    //
    // !!! Really, we should find _all_ source lines, not just one. (?)
    //

    _ASSERTE(NULL != rangeCount);
    _ASSERTE(NULL != range);

    DebuggerSourceFile *file = NULL;
    unsigned int line;

    _ASSERTE(range && rangeCount);
    *range = NULL;
    *rangeCount = 0;

    // Grab the current file and line.
    hr = this->FindLineFromIP(ip, &file, &line);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return;
    }

    if (file == NULL || !file->LoadText(g_pShell->m_currentSourcesPath, false))
        return;

    // This block of code here figures out how many sequence points are associated with the current line of source code.
    // If we're at a 0xfeefee sequence point, this means that there are no sequence points associated with the current
    // source line.
    //

    ULONG32 rangeArraySize;
    if (kNoSourceForIL != line)
    {
        // Here we're at a "normal" sequence point.

        hr = this->GetRanges(file->GetDocument(), line,
                             0, &rangeArraySize, NULL);
        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return;
        }

        _ASSERTE(rangeArraySize > 0);
    }
    else
    {
        // in this case, we ARE at a 0xfeefeee sequence point. There are no step ranges that correspond to source.
        rangeArraySize = 0;
    }

    // There must always be at least one range. It might correspond to a line of source, or it might correspond to a
    // magic sequence point. But there must be at least one range that we need to step over.
    //
    // Is this true if our debugging information is corrupt?
    _ASSERTE(rangeArraySize + m_sourcelessRangeCount > 0);


    // Allocate ranges to return
    *range = new COR_DEBUG_STEP_RANGE[rangeArraySize  + m_sourcelessRangeCount];

    if (rangeArraySize > 0)
    {
        // Finally, go get the ranges
        hr = this->GetRanges(file->GetDocument(), line,
                             rangeArraySize, &rangeArraySize, *range);

        if (FAILED(hr))
        {
            delete[] *range;
            *range = NULL;
            g_pShell->ReportError(hr);
            return;
        }
    }


    if (m_sourcelessRangeCount > 0)
    {
        COR_DEBUG_STEP_RANGE* r = *range + rangeArraySize;

        for (ULONG32 i = 0; i < m_sourcelessRangeCount; ++i)
        {
            *r = m_sourcelessRanges[i]->GetRange();
            ++r;
        }
    }

    *rangeCount = rangeArraySize + m_sourcelessRangeCount;
}


//
// Given a scope, determine how many local vars are "active" at the given
// line number. The count includes any locals that are active in scopes
// that are children of the given scope.
//
void DebuggerFunction::CountActiveLocalVars(ISymUnmanagedScope* head,
                                            unsigned int ip,
                                            unsigned int* varCount)
{
    if (head != NULL)
    {
        ULONG32 startOffset, endOffset;

        HRESULT hr = head->GetStartOffset(&startOffset);
        _ASSERTE(SUCCEEDED(hr));
        hr = head->GetEndOffset(&endOffset);
        _ASSERTE(SUCCEEDED(hr));

        if ((ip >= startOffset) && (ip <= endOffset))
        {
            ULONG32 childCount;

            // How many children?
            hr = head->GetChildren(0, &childCount, NULL);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return;
            }

            if (childCount > 0)
            {
                // Make room for the children
                CQuickBytes childrenBuf;
                ISymUnmanagedScope **children =
                    (ISymUnmanagedScope**)childrenBuf.Alloc(sizeof(ISymUnmanagedScope*) * childCount);
                if (children == NULL)
                {
                    g_pShell->Error(L"Could not get memory for children scopes\n");
                    return;
                }

                // Get the children
                hr = head->GetChildren(childCount, &childCount, children);

                if (FAILED(hr))
                {
                    g_pShell->ReportError(hr);
                    return;
                }

                for (ULONG32 i = 0; i < childCount; i++)
                {
                    CountActiveLocalVars(children[i], ip, varCount);
                    RELEASE(children[i]);
                }
            }

            ULONG32 vc;
            hr = head->GetLocalCount(&vc);
            _ASSERTE(SUCCEEDED(hr));

            *varCount += vc;
        }
    }
}


//
// Given a scope and an array of DebuggerVarInfo pointers, fill up the array
// with pointers to every "active" local variable within the scope, including
// any active locals in child scopes. The size of the varPtrs array is
// passed in as varCount and is used for bounds checking.
//
void DebuggerFunction::FillActiveLocalVars(ISymUnmanagedScope* head,
                                           unsigned int ip,
                                           unsigned int varCount,
                                           unsigned int* currentVar,
                                           DebuggerVariable* varPtrs)
{
    if (head != NULL)
    {
        ULONG32 startOffset, endOffset;

        HRESULT hr = head->GetStartOffset(&startOffset);
        _ASSERTE(SUCCEEDED(hr));
        hr = head->GetEndOffset(&endOffset);
        _ASSERTE(SUCCEEDED(hr));

        if ((ip >= startOffset) && (ip <= endOffset))
        {
            ULONG32 childCount;

            // How many children?
            hr = head->GetChildren(0, &childCount, NULL);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return;
            }

            if (childCount > 0)
            {
                // Make room for the children
                CQuickBytes childrenBuf;
                ISymUnmanagedScope **children =
                    (ISymUnmanagedScope**)childrenBuf.Alloc(sizeof(ISymUnmanagedScope*) * childCount);
                if (children == NULL)
                {
                    g_pShell->Error(L"Could not get memory for children scopes\n");
                    return;
                }

                // Get the children
                hr = head->GetChildren(childCount, &childCount, children);

                if (FAILED(hr))
                {
                    g_pShell->ReportError(hr);
                    return;
                }

                for (ULONG32 i = 0; i < childCount; i++)
                {
                    FillActiveLocalVars(children[i], ip, varCount,
                                        currentVar, varPtrs);
                    RELEASE(children[i]);
                }
            }

            // Fill in any locals on this scope.
            ULONG32 localCount;

            // How many locals?
            hr = head->GetLocalCount(&localCount);

            if (FAILED(hr))
            {
                g_pShell->ReportError(hr);
                return;
            }

            if (localCount > 0)
            {
                // Make room for the localc
                CQuickBytes localsBuf;
                ISymUnmanagedVariable **locals =
                    (ISymUnmanagedVariable**)localsBuf.Alloc(sizeof(ISymUnmanagedVariable*) *
                                            localCount);
                if (locals == NULL)
                {
                    g_pShell->Error(L"Could not get memory for children\n");
                    return;
                }

                hr = head->GetLocals(localCount, &localCount, locals);

                if (FAILED(hr))
                {
                    g_pShell->ReportError(hr);
                    return;
                }

                for (ULONG32 i = 0; i < localCount; i++)
                {
                    _ASSERTE(*currentVar < varCount);

                    // Get the size of the name.
                    ULONG32 nameSize;

                    hr = locals[i]->GetName(0, &nameSize, NULL);

                    if (FAILED(hr))
                    {
                        g_pShell->ReportError(hr);
                        return;
                    }

                    // Allocate space for the name.
                    if (!varPtrs[*currentVar].m_name.Allocate(nameSize))
                    {
                        g_pShell->ReportError(E_OUTOFMEMORY);
                        return;
                    }

                    // Get the name.
                    hr = locals[i]->GetName(nameSize + 1, &nameSize,
                                            varPtrs[*currentVar].m_name.GetData());

                    if (FAILED(hr))
                    {
                        g_pShell->ReportError(hr);
                        return;
                    }

#ifdef _DEBUG
                    ULONG32 addrKind;
                    hr = locals[i]->GetAddressKind(&addrKind);
                    _ASSERTE(addrKind == ADDR_IL_OFFSET);
#endif

                    hr = locals[i]->GetAddressField1(&(varPtrs[*currentVar].m_varNumber));

                    if (FAILED(hr))
                    {
                        g_pShell->ReportError(hr);
                        return;
                    }

                    (*currentVar)++;
                }
            }
        }
    }
}


//
// Build a list of the active local variables within a method at a given IP.
//
bool DebuggerFunction::GetActiveLocalVars(UINT_PTR ip,
                                          DebuggerVariable** vars,
                                          unsigned int* count)
{
    HRESULT hr = S_OK;

    *vars = NULL;
    *count = 0;

    // Grab the root scope for this method.
    ISymUnmanagedScope *rootScope = NULL;

    if (m_symMethod)
    {
        hr = m_symMethod->GetRootScope(&rootScope);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return false;
        }
    }

    // No work to do if there is no scope info for this method!
    if (!rootScope)
        return false;

    // Count the active locals.
    unsigned int localCount = 0;

    CountActiveLocalVars(rootScope, (unsigned int)ip, &localCount);

    // No work to do if there aren't any locals in scope.
    if (localCount == 0)
    {
        rootScope->Release();
        return true;
    }

    // Allocate and fillup a list of DebuggerVariable pointers to
    // each variable.
    DebuggerVariable *varPtrs = new DebuggerVariable[localCount];
    _ASSERTE(varPtrs != NULL);

    if (varPtrs != NULL)
    {
        unsigned int currentVar = 0;

        FillActiveLocalVars(rootScope, (unsigned int)ip, localCount,
                            &currentVar, varPtrs);
    }
    else
        localCount = 0;

    *vars = varPtrs;
    *count = localCount;

    rootScope->Release();

    return true;
}

DebuggerFunction *DebuggerFunction::FromCorDebug(ICorDebugFunction *function)
{
    mdMethodDef md;
    HRESULT hr;

    // Get the method def token for function
    hr = function->GetToken(&md);

    // Error check
    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return NULL;
    }

    // Now get the module interface, so we can get a pointer to the CorDbg
    // DebuggerModule object
    ICorDebugModule *imodule;
    hr = function->GetModule(&imodule);

    // Error check
    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return NULL;
    }

    // Get a pointer to the CorDbg module
    DebuggerModule *m = DebuggerModule::FromCorDebug(imodule);
    _ASSERTE(m);  // should never fail
    if (m == NULL)
    {
        return NULL;
    }

    // Release the interface
    imodule->Release();

    // Use the debugger module to resolve the function to a DebuggerFunction ptr
    return (m->ResolveFunction(md, function));
}

HRESULT DebuggerFunction::LoadCode(BOOL native)
{
    _ASSERTE(g_pShell != NULL);

    if (native)
    {
        ICorDebugCode *icode;
        HRESULT hr = m_ifunction->GetNativeCode(&icode);

        if (FAILED(hr))
            return (hr);


        ULONG32 size;
        icode->GetSize(&size);

        if (m_nativeCode)
            delete [] m_nativeCode;

        m_nativeCode = new BYTE [size];
        _ASSERTE(m_nativeCode != NULL);
        m_nativeCodeSize = size;

        if (m_nativeCode == NULL)
        {
            icode->Release();
            return (E_OUTOFMEMORY);
        }

        hr = icode->GetCode(0, size, size,
                            m_nativeCode, &m_nativeCodeSize);

        icode->Release();

        if (FAILED(hr))
        {
            delete m_nativeCode;
        // we have to null this out or we will try to
            // delete it later at destruction time
        m_nativeCode = NULL;
            return (hr);
        }

        return (S_OK);
    }
    else
    {
        ICorDebugCode *icode;
        HRESULT hr = m_ifunction->GetILCode(&icode);

        if (FAILED(hr))
            return (hr);

        ULONG32 size;
        icode->GetSize(&size);

        if (m_ilCode != NULL)
            delete m_ilCode;

        m_ilCode = new BYTE [size];
        m_ilCodeSize = size;

        if (m_ilCode == NULL)
        {
            icode->Release();
            return (E_OUTOFMEMORY);
        }

        hr = icode->GetCode(0, size, size,
                            m_ilCode, &m_ilCodeSize);

        icode->Release();

        if (FAILED(hr))
        {
            delete m_ilCode;
            return (hr);
        }

        return (S_OK);
    }
}

BOOL DebuggerFunction::ValidateInstruction(BOOL native, SIZE_T offset)
{
    if (FAILED(LoadCode(native)))
        return (FALSE);

    BYTE *codeStart;
    BYTE *codeEnd;

    if (native)
    {
        if (offset >= m_nativeCodeSize)
            return (false);

        codeStart = m_nativeCode;
        codeEnd = m_nativeCode + m_nativeCodeSize;
    }
    else
    {
        if (offset >= m_ilCodeSize)
            return (false);

        codeStart = m_ilCode;
        codeEnd = m_ilCode + m_ilCodeSize;
    }

    SIZE_T walk = 0;

    while (walk < offset)
        walk = WalkInstruction(native, walk, codeStart, codeEnd);

    return (walk == offset);
}

/* ------------------------------------------------------------------------- *
 * DebuggerCodeBreakpoint
 * ------------------------------------------------------------------------- */

DebuggerCodeBreakpoint::DebuggerCodeBreakpoint(int breakpointID,
                                               DebuggerModule *module,
                                               DebuggerFunction *function,
                                               SIZE_T functionVersion,
                                               SIZE_T offset, BOOL il,
                                               DWORD threadID)
    : m_id(breakpointID), m_module(module), m_function(function),
      m_functionVersion(functionVersion),
      m_offset(offset), m_il(il), m_threadID(threadID),
      m_ibreakpoint(NULL), m_parent(NULL)
{
    m_next = m_module->m_breakpoints;
    m_module->m_breakpoints = this;
    m_id = breakpointID;
}

DebuggerCodeBreakpoint::DebuggerCodeBreakpoint(
                            int breakpointID, DebuggerModule *module,
                            DebuggerSourceCodeBreakpoint *parent,
                            DebuggerFunction *function,
                            SIZE_T functionVersion,
                            SIZE_T offset, BOOL il,
                            DWORD threadID)
    : m_id(breakpointID), m_module(module), m_function(function),
      m_functionVersion(functionVersion),
      m_offset(offset), m_il(il),
      m_threadID(threadID),
      m_ibreakpoint(NULL), m_parent(parent)
{
    m_next = parent->m_breakpoints;
    parent->m_breakpoints = this;
    m_id = parent->m_id;
}

DebuggerCodeBreakpoint::~DebuggerCodeBreakpoint()
{
    DebuggerCodeBreakpoint  **next;

    //
    // Remove either global list or parent list
    //

    if (m_parent == NULL)
        next = &m_module->m_breakpoints;
    else
        next = &m_parent->m_breakpoints;

    while ((*next) != this)
        next = &((*next)->m_next);

    *next = (*next)->m_next;

    //
    // Destroy the cor breakpoint
    //

    if (m_ibreakpoint != NULL)
    {
        m_ibreakpoint->Release();
    }
}

bool DebuggerCodeBreakpoint::Activate()
{
    if (m_ibreakpoint == NULL && m_function != NULL)
    {
        ICorDebugCode *icode = NULL;

        HRESULT hr;

        if (m_il)
        {
            ICorDebugFunction *iFunction = GetFunctionVersion(m_function, m_functionVersion);

            if (iFunction != m_function->m_ifunction)
            {
                ICorDebugFunction2 *pFunction2;
                hr = iFunction->QueryInterface(IID_ICorDebugFunction2, (void **)&pFunction2);
                _ASSERTE(SUCCEEDED(hr));
                ULONG32 thisVersion;
                hr = pFunction2->GetVersionNumber(&thisVersion);
                g_pShell->Write(L"Activating breakpoint in function version %d\n", thisVersion);
            }
            hr = iFunction->GetILCode(&icode);
        }
        else
        {
            hr = m_function->m_ifunction->GetNativeCode(&icode);
        }

        if (SUCCEEDED(hr) || hr == CORDBG_E_CODE_NOT_AVAILABLE)
        {
            hr = icode->CreateBreakpoint((ULONG32)m_offset, &m_ibreakpoint);
            _ASSERTE(SUCCEEDED(hr));
        }

        if (icode)
            icode->Release();
    }

    if (m_ibreakpoint != NULL)
    {
        m_ibreakpoint->Activate(TRUE);
        return (true);
    }
    else
        return (false);
}

void DebuggerCodeBreakpoint::Deactivate()
{
    if (m_ibreakpoint != NULL)
    {
        m_ibreakpoint->Activate(FALSE);
        m_ibreakpoint->Release();
        m_ibreakpoint = NULL;
    }
}

bool DebuggerCodeBreakpoint::Match(ICorDebugBreakpoint *ibreakpoint)
{
    return (ibreakpoint == m_ibreakpoint);
}

void DebuggerCodeBreakpoint::Print()
{
    g_pShell->Write(L"%s+0x%x(%s) [%sactive]",
                    m_function->GetName(), m_offset,
                    m_il ? L"il" : L"native",
                    m_ibreakpoint == NULL ? L"in" : L"");
}

ICorDebugFunction *DebuggerCodeBreakpoint::GetFunctionVersion(DebuggerFunction *currentFunction,
                                                                  SIZE_T functionVersion)
{
    ICorDebugFunction *iFunction = currentFunction->m_ifunction;
    return iFunction;
}

DebuggerSourceCodeBreakpoint::DebuggerSourceCodeBreakpoint(
                                               int breakpointID,
                                               DebuggerSourceFile *file,
                                               SIZE_T functionVersion,
                                               SIZE_T lineNumber,
                                               DWORD threadID)
    : DebuggerCodeBreakpoint(breakpointID, file->m_module, NULL,
                             functionVersion, 0, FALSE, threadID),
      m_file(file), m_lineNumber(lineNumber), m_breakpoints(NULL),
      m_initSucceeded(false)
{
    DebuggerModule          *m = file->m_module;

    // GetMethodFromDocumentPosition to get an ISymUnmanagedMethod from this doc.
    ReleaseHolder<ISymUnmanagedMethod> pSymMethod;

    HRESULT hr = m->GetSymbolReader()->GetMethodFromDocumentPosition(
                                                        file->GetDocument(),
                                                        (ULONG32)lineNumber, 0,
                                                        &pSymMethod);

    if (FAILED(hr))
    {
        g_pShell->Write(L"Failed to find method to match source line. "
                        L"Unable to set breakpoint.\n");
        g_pShell->ReportError(hr);
        return;
    }

    // Make a regular breakpoint a the start of each line range.
    ULONG32 *rangeArray = NULL;
    ULONG32 rangeArraySize = 0;

    // How many ranges are there?
    hr = pSymMethod->GetRanges(file->GetDocument(),
                               (ULONG32)lineNumber, 0,
                               0, &rangeArraySize, NULL);

    if (FAILED(hr))
    {
        g_pShell->ReportError(hr);
        return;
    }

    if (rangeArraySize > 0)
    {
        // Make room and get the ranges.
        rangeArray = (ULONG32*)_alloca(sizeof(ULONG32) * rangeArraySize);
        _ASSERTE(rangeArray != NULL);

        ULONG32 arraySize;
        hr = pSymMethod->GetRanges(file->GetDocument(),
                                   (ULONG32)lineNumber, 0,
                                   rangeArraySize,
                                   &arraySize,
                                   rangeArray);
        _ASSERTE(arraySize == rangeArraySize);

        if (FAILED(hr))
        {
            g_pShell->ReportError(hr);
            return;
        }

        DebuggerFunction *f = m->ResolveFunction(pSymMethod, NULL);

         if (rangeArraySize == 2)
        {
            m_function = f;
            m_offset = rangeArray[0];
            m_il = TRUE;
        }
        else
        {
            for (ULONG32 i = 0; i < rangeArraySize; i += 2)
            {
                if (i > 0 && (rangeArray [i-1] == rangeArray [i]))
                    continue;

                new DebuggerCodeBreakpoint(breakpointID, file->m_module,
                                         this, f, functionVersion,
                                         rangeArray[i], TRUE, m_threadID);
            }
        }
    }

    m_initSucceeded = true;
}

DebuggerSourceCodeBreakpoint::~DebuggerSourceCodeBreakpoint()
{
    while (m_breakpoints != NULL)
        delete m_breakpoints;
}

bool DebuggerSourceCodeBreakpoint::Activate()
{
    bool result = true;

    DebuggerCodeBreakpoint::Activate();

    DebuggerCodeBreakpoint *b = m_breakpoints;

    while (b != NULL)
    {
        result = b->Activate() && result;
        b = b->m_next;
    }

    return (result);
}

void DebuggerSourceCodeBreakpoint::Deactivate()
{
    DebuggerCodeBreakpoint::Deactivate();

    DebuggerCodeBreakpoint *b = m_breakpoints;

    while (b != NULL)
    {
        b->Deactivate();
        b = b->m_next;
    }
}

bool DebuggerSourceCodeBreakpoint::Match(ICorDebugBreakpoint *ibreakpoint)
{
    if (ibreakpoint == m_ibreakpoint)
        return (true);

    DebuggerCodeBreakpoint *bp = m_breakpoints;
    while (bp != NULL)
    {
        if (bp->m_ibreakpoint == ibreakpoint)
            return (true);
        bp = bp->m_next;
    }

    return (false);
}

void DebuggerSourceCodeBreakpoint::Print()
{
    DebuggerCodeBreakpoint *bp = m_breakpoints;

    if (bp == NULL)
        DebuggerCodeBreakpoint::Print();
    else
        while (bp != NULL)
        {
            bp->Print();
            bp = bp->m_next;
        }
}

/* ------------------------------------------------------------------------- *
 * StepperHashTable class
 * ------------------------------------------------------------------------- */


HRESULT StepperHashTable::Initialize()
{
    HRESULT hr = NewInit(m_iBuckets,
                         sizeof(StepperHashEntry), 0xffff);
    if (FAILED(hr))
          return (hr);

    m_initialized = true;
    return S_OK;
}


HRESULT StepperHashTable::AddStepper(ICorDebugStepper *pStepper)
{
    if (!m_initialized)
    {
        HRESULT hr = NewInit(m_iBuckets,
                             sizeof(StepperHashEntry), 0xffff);
        if (FAILED(hr))
            return (hr);

        m_initialized = true;
    }

    StepperHashEntry *entry = (StepperHashEntry *) Add(HASH(pStepper));

    if (entry == NULL)
        return (E_FAIL);
    else
        entry->pStepper = pStepper;

    pStepper->AddRef(); //don't want this to disappear from under
        //the table's feet

    return (S_OK);
}

bool StepperHashTable::IsStepperPresent(ICorDebugStepper *pStepper)
{
    if (!m_initialized)
        return false;

    StepperHashEntry *entry
    = (StepperHashEntry *) Find(HASH(pStepper), KEY(pStepper));
    if (entry == NULL)
        return false;
    else
        return true;
}

BOOL StepperHashTable::RemoveStepper(ICorDebugStepper *pStepper)
{
    if (!m_initialized)
        return (FALSE);

    StepperHashEntry *entry
    = (StepperHashEntry *) Find(HASH(pStepper), KEY(pStepper));

    if (entry == NULL)
        return (FALSE);

    Delete(HASH(pStepper), (HASHENTRY *) entry);

    RELEASE(pStepper);

    return (TRUE);
}

void StepperHashTable::ReleaseAll()
{
    if (!m_initialized)
        return;

    HASHFIND find;
    ICorDebugStepper *pStepper = NULL;

    pStepper = FindFirst(&find);
    while( pStepper != NULL)
    {
        RELEASE( pStepper );
        RemoveStepper( pStepper );

        pStepper = FindNext(&find);
    }

    Clear();
}

ICorDebugStepper *StepperHashTable::FindFirst(HASHFIND *find)
{
    if (!m_initialized)
        return NULL;

    StepperHashEntry *entry = (StepperHashEntry *) FindFirstEntry(find);
    if (entry == NULL)
        return NULL;
    else
        return entry->pStepper;
}

ICorDebugStepper *StepperHashTable::FindNext(HASHFIND *find)
{
    if (!m_initialized)
        return NULL;

    StepperHashEntry *entry = (StepperHashEntry *) FindNextEntry(find);
    if (entry == NULL)
        return NULL;
    else
        return entry->pStepper;
}


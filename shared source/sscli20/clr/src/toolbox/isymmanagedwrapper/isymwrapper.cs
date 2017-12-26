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

using System;
using System.Text;
using System.Reflection;
using System.Diagnostics;
using System.Security.Permissions;
using System.Diagnostics.SymbolStore;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace System.Diagnostics.SymbolStore
{
    internal sealed class Win32Native {
 #if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
 #else // !PLATFORM_UNIX
  #if __APPLE__
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".dylib";
  #else
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".so";
  #endif
 #endif // !PLATFORM_UNIX
    }

    //
    // Class to CoCreate a class via PInvoke and DllGetClassObject
    //
    class CreateSymClass
    {
        public const String NATIVE_DLL = Win32Native.DLLPREFIX + "ildbsymbols" + Win32Native.DLLSUFFIX;

        private readonly static Guid IID_IClassFactory = new Guid(0x00000001, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

        [DllImport(NATIVE_DLL)]
        static extern int DllGetClassObject([In] ref Guid rclsid, [In] ref Guid riid, out IntPtr ClassObject);

        [DllImport(NATIVE_DLL)]
        static extern int ClassFactory_CreateInstance(IntPtr classfactory, [In] ref Guid rclsid, out IntPtr ClassInstance);

        [DllImport(NATIVE_DLL)]
        static extern void ClassFactory_Release(IntPtr classfactory);

        static public IntPtr CreateClass(Guid rclsid, Guid riid)
        {
            IntPtr classFactory;
            IntPtr objCreatedObject;

            COMException Exception;
            // Create a local copy so that we can pass it's reference
            Guid IClassFactory = IID_IClassFactory;
            int hr;
            hr = DllGetClassObject(ref rclsid, ref IClassFactory, out classFactory);
            try
            {
                if (hr < 0)
                {
                    Exception = new COMException("Call to DllGetClassObject failed.", hr);
                    throw Exception;
                }
                hr = ClassFactory_CreateInstance(classFactory, ref riid, out objCreatedObject);
                if (hr < 0)
                {
                    Exception = new COMException("Call to CreateInstance failed.", hr);
                    throw Exception;
                }
            }
            finally
            {
                ClassFactory_Release(classFactory);
            }

            return objCreatedObject;
        }
    }
    
    //
    // Document
    //
    public class SymDocument : ISymbolDocument
    {

        private IntPtr m_Document; // Unmanaged document pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_Release(IntPtr DocumentWriter);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetCheckSum(
            IntPtr Document,
            int cData,
            out int pcData,
            [In] byte[] data);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetCheckSumAlgorithmId(
            IntPtr Document,
            out Guid pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetDocumentType(
            IntPtr Document,
            out Guid pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetLanguage(
            IntPtr Document,
            out Guid pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetLanguageVendor(
            IntPtr Document,
            out Guid pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_HasEmbeddedSource(
            IntPtr Document,
            out bool pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetSourceLength(
            IntPtr Document,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetSourceRange(
            IntPtr Document,
            int startLine, int startColumn,
            int endLine, int endColumn,
            int cSourceBytes, out int pcSourceBytes,
            [In] byte[] source);


        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_FindClosestLine(
            IntPtr Document,
            int line,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocument_GetUrl(
            IntPtr Document,
            int cchUrl,
            out int pcchUrl,
            StringBuilder szUrl);
            
       
        //-----------------------------------------------------------------------
        // SymDocument
        //-----------------------------------------------------------------------

        public SymDocument(IntPtr pDocument)
        {
            m_Document = pDocument;
        }

        ~SymDocument()
        {
            SymDocument_Release(m_Document);   
        }

        public byte[] GetCheckSum()
        {
            int hr;
            byte[] Data;
            COMException Exception;
            int cData;
            hr = SymDocument_GetCheckSum(m_Document, 0, out cData, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetCheckSum failed.", hr);
                throw Exception;
            }
            Data = new byte[cData];
            hr = SymDocument_GetCheckSum(m_Document, cData, out cData, Data);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetCheckSum failed.", hr);
                throw Exception;
            }
            return Data;
        }

        public Guid CheckSumAlgorithmId
        {
            get
            {
                int hr;
                Guid AlgorigthId;
                COMException Exception;
                hr = SymDocument_GetCheckSumAlgorithmId(m_Document, out AlgorigthId);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetCheckSumAlgorithmId failed.", hr);
                    throw Exception;
                }
                return AlgorigthId;
            }
        }

        public Guid DocumentType 
        {
            get
            {
                int hr;
                Guid DocumentType;
                COMException Exception;
                hr = SymDocument_GetDocumentType(m_Document, out DocumentType);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetDocumentType failed.", hr);
                    throw Exception;
                }
                return DocumentType;
            }
        }

        public Guid Language 
        {
            get
            {
                int hr;
                Guid Language;
                COMException Exception;
                hr = SymDocument_GetLanguage(m_Document, out Language);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetLanguage failed.", hr);
                    throw Exception;
                }
                return Language;
            }
        }

        public Guid LanguageVendor 
        {
            get
            {
                int hr;
                Guid LanguageVendor;
                COMException Exception;
                hr = SymDocument_GetLanguageVendor(m_Document, out LanguageVendor);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetLanguageVendor failed.", hr);
                    throw Exception;
                }
                return LanguageVendor;
            }
        }

        public bool HasEmbeddedSource 
        {
            get
            {
                int hr;
                bool HasEmbeddedSource;
                COMException Exception;
                hr = SymDocument_HasEmbeddedSource(m_Document, out HasEmbeddedSource);
                if (hr < 0)
                {
                    Exception = new COMException("Call to HasEmbeddedSource failed.", hr);
                    throw Exception;
                }
                return HasEmbeddedSource;
            }
        }

        public int FindClosestLine(int line)
        {
            int hr;
            int ClosestLine = 0;
            COMException Exception;
            hr = SymDocument_FindClosestLine(m_Document, line, out ClosestLine);
            if (hr < 0)
            {
                Exception = new COMException("Call to FindClosestLine failed.", hr);
                throw Exception;
            }
            return ClosestLine;
        }

        public byte[] GetSourceRange(
            int startLine, int startColumn,
            int endLine, int endColumn)
        {
            int hr;
            byte[] Range;
            int cSourceBytes;
            COMException Exception;
            hr = SymDocument_GetSourceRange(m_Document, startLine, startColumn,
                                            endLine, endColumn,
                                            0, out cSourceBytes, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSourceRange failed.", hr);
                throw Exception;
            }
            Range = new byte[cSourceBytes];
            hr = SymDocument_GetSourceRange(m_Document, startLine, startColumn,
                                            endLine, endColumn,
                                            cSourceBytes, out cSourceBytes, Range);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSourceRange failed.", hr);
                throw Exception;
            }
            return Range;
        }

        public int SourceLength 
        {
            get
            {
                int hr;
                int SourceLength;
                COMException Exception;
                hr = SymDocument_GetSourceLength(m_Document, out SourceLength);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetSourceLength failed.", hr);
                    throw Exception;
                }
                return SourceLength;
            }
        }
        public string URL 
        {
            get
            {
                int hr;
                StringBuilder URL;
                int cchUrl;
                COMException Exception;
                hr = SymDocument_GetUrl(m_Document, 0, out cchUrl, null);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetUrl failed.", hr);
                    throw Exception;
                }
                // *2 since it's wchar's
                URL = new StringBuilder(cchUrl);
                hr = SymDocument_GetUrl(m_Document, cchUrl, out cchUrl, URL);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetUrl failed.", hr);
                    throw Exception;
                }
                return URL.ToString();
            }
        }

        // Public API
        public IntPtr InternalDocument
        {
            get
            {
                return m_Document;
            }
        }

    }

    public class SymDocumentWriter : ISymbolDocumentWriter
    {

        private IntPtr m_DocumentWriter;  // Unmanaged DocumentWriter pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocumentWriter_Release(IntPtr DocumentWriter);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocumentWriter_SetSource(
            IntPtr DocumentWriter,
            int sourceSize,
            byte[] source);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymDocumentWriter_SetCheckSum(
            IntPtr DocumentWriter,
            Guid algorithmId,
            int CheckSumSize,
            byte[] CheckSum);


        //-----------------------------------------------------------------------
        // SymDocumentWriter
        //-----------------------------------------------------------------------

        public SymDocumentWriter(IntPtr DocumentWriter)
        {
            m_DocumentWriter = DocumentWriter;
        }

        ~SymDocumentWriter()
        {
            SymDocumentWriter_Release(m_DocumentWriter);
        }

        public void SetSource(byte[] source)
        {
            int hr;
            COMException Exception;
            hr = SymDocumentWriter_SetSource(m_DocumentWriter, source.Length, source);
            if (hr < 0)
            {
                Exception = new COMException("Call to SetSource failed.", hr);
                throw Exception;
            }
        }

        public void SetCheckSum(Guid algorithmId, byte[] source)
        {
            int hr;
            COMException Exception;
            hr = SymDocumentWriter_SetCheckSum(m_DocumentWriter, algorithmId, source.Length, source);
            if (hr < 0)
            {
                Exception = new COMException("Call to SetCheckSum failed.", hr);
                throw Exception;
            }
        }

        public IntPtr InternalDocumentWriter
        {
            get
            {
                return m_DocumentWriter;
            }
        }
    }

    //-----------------------------------------------------------------------
    // SymMethod
    //-----------------------------------------------------------------------
    public class SymMethod : ISymbolMethod
    {
        private IntPtr m_Method; // Unmanaged method pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_Release(IntPtr Method);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetSequencePoints(
            IntPtr Method,
            int cPoint,
            out int pcPoints,
            [In] IntPtr[] document,
            [In] int[] offsets,
            [In] int[] lines, [In] int[] columns,
            [In] int[] endLines, [In] int[] endColumns);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetRootScope(
            IntPtr Method,
            out IntPtr pScope);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetScopeFromOffset(
            IntPtr Method,
            int offset,
            out IntPtr pScope);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetOffset(
            IntPtr Method,
            IntPtr Document,
            int line,
            int column,
            out int offset);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetToken(
            IntPtr Method,
            out int Token);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetSequencePointCount(
            IntPtr Method,
            out int Count);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetRanges(
            IntPtr Method,
            IntPtr Document,
            int line,
            int column,
            int cRanges,
            out int pcRanges,
            [In] int[] Ranges);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetParameters(
            IntPtr Method,
            int cParams,
            out int pcParams,
            [In] IntPtr[] parameters);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetNamespace(
            IntPtr Method,
            out IntPtr pNamespace);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymMethod_GetSourceStartEnd(
            IntPtr Method,
            [In] IntPtr[] docs,
            [In] int[] lines,
            [In] int[] columns,
            out bool pRetVal);

        //
        // Implementation
        //
        public SymMethod(IntPtr pMethod)
        {
            m_Method = pMethod;
        }

        ~SymMethod()
        {
            SymMethod_Release(m_Method);
        }

        public void GetSequencePoints(int[] offsets,
            ISymbolDocument[] documents,
            int[] lines,
            int[] columns,
            int[] endLines,
            int[] endColumns)
        {
            int hr;
            int spCount = 0;
            if (offsets != null)
                spCount = offsets.Length;
            else if (documents != null)
                spCount = documents.Length;
            else if (lines != null)
                spCount = lines.Length;
            else if (columns != null)
                spCount = columns.Length;
            else if (endLines != null)
                spCount = endLines.Length;
            else if (endColumns != null)
                spCount = endColumns.Length;

            // Don't do anything if they're not really asking for anything.
            if (spCount == 0)
                return;

            // Make sure all arrays are the same length.
            if ((offsets != null) && (spCount != offsets.Length))
                throw new ArgumentException();

            if ((lines != null) && (spCount != lines.Length))
                throw new ArgumentException();

            if ((columns != null) && (spCount != columns.Length))
                throw new ArgumentException();

            if ((endLines != null) && (spCount != endLines.Length))
                throw new ArgumentException();

            if ((endColumns != null) && (spCount != endColumns.Length))
                throw new ArgumentException();

            COMException Exception;
            IntPtr[] IDocuments = new IntPtr[documents.Length];
            int cPoints;
            uint i;
            hr = SymMethod_GetSequencePoints(m_Method, documents.Length, out cPoints,
                IDocuments, offsets,
                lines, columns,
                endLines, endColumns);

            if (hr < 0)
            {
                Exception = new COMException("Call to GetSequencePoints failed.", hr);
                throw Exception;
            }

            // Create the SymDocument form the IntPtr's
            for (i = 0; i < documents.Length; i++)
            {
                documents[i] = new SymDocument(IDocuments[i]);
            }

            return;
        }

        public ISymbolScope RootScope 
        {
            get
            {
                int hr;
                IntPtr pRootScope;
                COMException Exception;
                hr = SymMethod_GetRootScope(m_Method, out pRootScope);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetRootScope failed.", hr);
                    throw Exception;
                }
                return new SymScope(pRootScope);
            }
        }

        public ISymbolScope GetScope(int offset)
        {
            int hr;
            IntPtr pScope;
            COMException Exception;
            hr = SymMethod_GetScopeFromOffset(m_Method, offset, out pScope);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetScopeFromOffset failed.", hr);
                throw Exception;
            }
            return new SymScope(pScope);
        }

        public int GetOffset(ISymbolDocument document,
            int line,
            int column)
        {
            int hr;
            int offset =0;
            COMException Exception;
            hr = SymMethod_GetOffset(m_Method, 
                                    ((SymDocument)document).InternalDocument,
                                    line,
                                    column,
                                    out offset);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetOffset failed.", hr);
                throw Exception;
            }

            return offset;
        }
        public SymbolToken Token 
        {
            get
            {
                int hr;
                int tokenId;
                COMException Exception;
                hr = SymMethod_GetToken(m_Method, out tokenId);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetToken failed.", hr);
                    throw Exception;
                }

                SymbolToken t = new SymbolToken(tokenId);
                return t;
            }
        }

        public int SequencePointCount 
        {
            get
            {
                int hr;
                int Count;
                COMException Exception;
                hr = SymMethod_GetSequencePointCount(m_Method, out Count);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetSequencePointCount failed.", hr);
                    throw Exception;
                }

                return Count;
            }
        }
        public int[] GetRanges(
            ISymbolDocument document,
            int line,
            int column) 
        {
            int hr;
            int cRanges;
            int[] Ranges;
            COMException Exception;
            hr = SymMethod_GetRanges(m_Method, ((SymDocument)document).InternalDocument, line, column, 0, out cRanges, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetRanges failed.", hr);
                throw Exception;
            }
            Ranges = new int[cRanges];
            hr = SymMethod_GetRanges(m_Method, ((SymDocument)document).InternalDocument, line, column, cRanges, out cRanges, Ranges);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetRanges failed.", hr);
                throw Exception;
            }

            return Ranges;
        }

        public ISymbolVariable[] GetParameters()  
        {
            int hr;
            int cVariables;
            uint i;
            SymVariable[] Variables;
            IntPtr[] VariablePointers;
            COMException Exception;
            hr = SymMethod_GetParameters(m_Method, 0, out cVariables, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetParameters failed.", hr);
                throw Exception;
            }
            VariablePointers = new IntPtr[cVariables];
            hr = SymMethod_GetParameters(m_Method, cVariables, out cVariables, VariablePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetParameters failed.", hr);
                throw Exception;
            }

            Variables = new SymVariable[cVariables];

            for (i = 0; i < cVariables; i++)
            {
                Variables[i] = new SymVariable(VariablePointers[i]);
            }

            return Variables;
        }

        public ISymbolNamespace GetNamespace()
        {
            int hr;
            IntPtr pNamespace;
            COMException Exception;
            hr = SymMethod_GetNamespace(m_Method, out pNamespace);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespace failed.", hr);
                throw Exception;
            }
            return new SymNamespace(pNamespace);
        }

        public bool GetSourceStartEnd(
            ISymbolDocument[] docs  ,
            int[] lines ,
            int[] columns )
        {
            int hr;
            uint i;
            bool pRetVal;
            int spCount = 0;
            if (docs != null)
                spCount = docs.Length;
            else if (lines != null)
                spCount = lines.Length;
            else if (columns != null)
                spCount = columns.Length;

            // If we don't have at least 2 entries then return an error
            if (spCount < 2)
                throw new ArgumentException();

            // Make sure all arrays are the same length.
            if ((docs != null) && (spCount != docs.Length))
                throw new ArgumentException();

            if ((lines != null) && (spCount != lines.Length))
                throw new ArgumentException();

            if ((columns != null) && (spCount != columns.Length))
                throw new ArgumentException();

            COMException Exception;
            IntPtr[] Documents = new IntPtr[docs.Length];
            hr = SymMethod_GetSourceStartEnd(m_Method, Documents, lines, columns, out pRetVal);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSourceStartEnd failed.", hr);
                throw Exception;
            }
            if (pRetVal)
            {
                for (i = 0; i < docs.Length;i++)
                {
                    docs[i] = new SymDocument(Documents[i]);
                }
            }
            return pRetVal;

        }
    }

    //-----------------------------------------------------------------------
    // SymReader
    //-----------------------------------------------------------------------
    public class SymReader : ISymbolReader
    {

        private IntPtr m_Reader; // Unmanaged Reader pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_Release(IntPtr Reader);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetUserEntryPoint(
            IntPtr Reader,
            out int EntryPoint);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetDocuments(
            IntPtr Reader,
            int cDocs,
            out int pcDocs,
            [In] IntPtr[] pDocs);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetDocument(
            IntPtr Reader,
            [MarshalAs(UnmanagedType.LPWStr)]string Url,
            Guid Language,
            Guid LanguageVendor,
            Guid DocumentType,
            out IntPtr Document);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetMethod(
            IntPtr Reader,
            int MethodToken,
            out IntPtr Document);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetMethodByVersion(
            IntPtr Reader,
            int MethodToken,
            int Version,
            out IntPtr Document);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetVariables(
            IntPtr Reader,
            int ParentToken,
            int cVars,
            out int pcVars,
            [In] IntPtr[] Variables);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetGlobalVariables(
            IntPtr Reader,
            int cVars,
            out int pcVars,
            [In] IntPtr[] Variables);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetMethodFromDocumentPosition(
            IntPtr Reader,
            IntPtr Document,
            int line,
            int column,
            out IntPtr MethodPointer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetSymAttribute(
            IntPtr Reader,
            int parentToken,
            [MarshalAs(UnmanagedType.LPWStr)] string name,
            int cBuffer,
            out int pcBuffer,
            byte[] buffer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymReader_GetNamespaces(
            IntPtr Reader,
            int cNamespaces,
            out int pcNamespaces,
            [In] IntPtr[] NamespacePointers);

        //
        // Implementation
        //
        public SymReader(IntPtr Reader)
        {
            m_Reader = Reader;
        }

        ~SymReader()
        {
            SymReader_Release(m_Reader);
        }

        public SymbolToken UserEntryPoint 
        {
            get
            {
                COMException Exception;
                int hr;
                int EntryPoint;
                hr = SymReader_GetUserEntryPoint(m_Reader, out EntryPoint);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetUserEntryPoint failed.", hr);
                    throw Exception;
                }            

                SymbolToken t = new SymbolToken(EntryPoint);
                return t;
            }
        }

        public ISymbolDocument[] GetDocuments()
        {
            COMException Exception;
            int hr;
            uint i;
            int cDocs;
            IntPtr[] DocumentPointers;
            SymDocument[] Documents;
            hr = SymReader_GetDocuments(m_Reader, 0, out cDocs, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetDocuments failed.", hr);
                throw Exception;
            }        
            DocumentPointers = new IntPtr[cDocs];
            Documents = new SymDocument[cDocs];
            hr = SymReader_GetDocuments(m_Reader, cDocs, out cDocs, DocumentPointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetDocuments failed.", hr);
                throw Exception;
            }        

            for (i = 0; i < cDocs; i++)
            {
                Documents[i] = new SymDocument(DocumentPointers[i]);
            }

            return Documents;
        }

        public ISymbolDocument GetDocument(
            string url,
            Guid language,
            Guid languageVendor,
            Guid documentType)
        {
            COMException Exception;
            int hr;
            IntPtr DocumentPointer;
            hr = SymReader_GetDocument(m_Reader, url, language, languageVendor, documentType, out DocumentPointer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetDocument failed.", hr);
                throw Exception;
            }        
            return new SymDocument(DocumentPointer);
        }

        public ISymbolMethod GetMethod(SymbolToken method)
        {
            COMException Exception;
            int hr;
            IntPtr MethodPointer;
            hr = SymReader_GetMethod(m_Reader, method.GetToken(), out MethodPointer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetMethod failed.", hr);
                throw Exception;
            }        
            return new SymMethod(MethodPointer);
        }

        public ISymbolMethod GetMethod(SymbolToken method, int version)
        {
            COMException Exception;
            int hr;
            IntPtr MethodPointer;
            hr = SymReader_GetMethodByVersion(m_Reader, method.GetToken(), version, out MethodPointer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetMethodByVersion failed.", hr);
                throw Exception;
            }        
            return new SymMethod(MethodPointer);
        }

        public ISymbolVariable[] GetVariables(SymbolToken parent) 
        {
            COMException Exception;
            int hr;
            IntPtr[] VariablePointers;
            SymVariable[] Variables;
            int cVars;
            uint i;
            hr = SymReader_GetVariables(m_Reader, parent.GetToken(), 0, out cVars, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetVariables failed.", hr);
                throw Exception;
            }
            Variables = new SymVariable[cVars];
            VariablePointers = new IntPtr[cVars];

            hr = SymReader_GetVariables(m_Reader, parent.GetToken(), cVars, out cVars, VariablePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetVariables failed.", hr);
                throw Exception;
            }        
            for (i = 0; i < cVars; i++)
            {
                Variables[i] = new SymVariable(VariablePointers[i]);
            }
            return Variables;
        }

        public ISymbolVariable[] GetGlobalVariables() 
        {
            COMException Exception;
            int hr;
            IntPtr[] VariablePointers;
            SymVariable[] Variables;
            int cVars;
            uint i;
            hr = SymReader_GetGlobalVariables(m_Reader, 0, out cVars, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetGlobalVariables failed.", hr);
                throw Exception;
            }
            Variables = new SymVariable[cVars];
            VariablePointers = new IntPtr[cVars];

            hr = SymReader_GetGlobalVariables(m_Reader, cVars, out cVars, VariablePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetGlobalVariables failed.", hr);
                throw Exception;
            }        
            for (i = 0; i < cVars; i++)
            {
                Variables[i] = new SymVariable(VariablePointers[i]);
            }
            return Variables;
        }

        public ISymbolMethod GetMethodFromDocumentPosition(
            ISymbolDocument document,
            int line,
            int column)
        {
            COMException Exception;
            int hr;
            IntPtr MethodPointer;
            hr = SymReader_GetMethodFromDocumentPosition(m_Reader, ((SymDocument)document).InternalDocument, line, column, out MethodPointer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetMethodFromDocumentPosition failed.", hr);
                throw Exception;
            }        
            return new SymMethod(MethodPointer);
        }

        public byte[] GetSymAttribute(SymbolToken parent, string name) 
        {
            COMException Exception;
            int hr;
            int cBuffer;
            byte[] Buffer;
            hr = SymReader_GetSymAttribute(m_Reader, parent.GetToken(), name, 0, out cBuffer, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSymAttribute failed.", hr);
                throw Exception;
            }
            Buffer = new byte[cBuffer];
            hr = SymReader_GetSymAttribute(m_Reader, parent.GetToken(), name, cBuffer, out cBuffer, Buffer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSymAttribute failed.", hr);
                throw Exception;
            }
            return Buffer;
        }

        public ISymbolNamespace[] GetNamespaces() 
        {
            COMException Exception;
            int hr;
            uint i;
            int cNamespaces;
            IntPtr[] NamespacePointers;
            SymNamespace[] Namespaces;
            hr = SymReader_GetNamespaces(m_Reader, 0, out cNamespaces, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            NamespacePointers = new IntPtr[cNamespaces];
            Namespaces = new SymNamespace[cNamespaces];
            hr = SymReader_GetNamespaces(m_Reader, cNamespaces, out cNamespaces, NamespacePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            for (i = 0; i < cNamespaces; i++)
            {
                Namespaces[i] = new SymNamespace(NamespacePointers[i]);
            }
            return Namespaces;
        }
    }

    //-----------------------------------------------------------------------
    // SymScope
    //-----------------------------------------------------------------------
    public class SymScope : ISymbolScope
    {

        private IntPtr m_Scope; // Unmanaged scope pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_Release(IntPtr Variable);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetStartOffset(
            IntPtr Scope,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetEndOffset(
            IntPtr Scope,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetMethod(
            IntPtr Scope,
            out IntPtr Method);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetParent(
            IntPtr Scope,
            out IntPtr ParentScope);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetChildren(
            IntPtr Scope,
            int cChildren,
            out int pcChildren,
            [In] IntPtr[] ScopePointers);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetLocals(
            IntPtr Scope,
            int cVariables,
            out int pcVariables,
            [In] IntPtr[] VariablePointers);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymScope_GetNamespaces(
            IntPtr Scope,
            int cNamespaces,
            out int pcNamespaces,
            [In] IntPtr[] NamespacePointers);

        //
        // Implementation
        //
        public SymScope(IntPtr pScope)
        {
            m_Scope = pScope;
        }

        ~SymScope()
        {
            SymScope_Release(m_Scope);
        }

        public int StartOffset 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymScope_GetStartOffset(m_Scope, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetStartOffset failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }

        public int EndOffset 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymScope_GetEndOffset(m_Scope, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetEndOffset failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }

        public ISymbolMethod Method 
        {
            get
            {
                COMException Exception;
                int hr;
                IntPtr MethodPointer;
                hr = SymScope_GetMethod(m_Scope, out MethodPointer);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetMethod failed.", hr);
                    throw Exception;
                }        
                return new SymMethod(MethodPointer);
            }
        }
        public ISymbolScope Parent 
        {
            get
            {
                COMException Exception;
                int hr;
                IntPtr ScopePointer;
                hr = SymScope_GetParent(m_Scope, out ScopePointer);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetParent failed.", hr);
                    throw Exception;
                }        
                return new SymScope(ScopePointer);
            }
        }

        public ISymbolScope[] GetChildren()  
        {
            COMException Exception;
            int hr;
            uint i;
            int cChildren;
            IntPtr[] ChildrenPointers;
            SymScope[] Children;
            hr = SymScope_GetChildren(m_Scope, 0, out cChildren, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetChildren failed.", hr);
                throw Exception;
            }
            ChildrenPointers = new IntPtr[cChildren];
            Children = new SymScope[cChildren];
            hr = SymScope_GetChildren(m_Scope, cChildren, out cChildren, ChildrenPointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetChildren failed.", hr);
                throw Exception;
            }
            for (i = 0; i < cChildren; i++)
            {
                Children[i] = new SymScope(ChildrenPointers[i]);
            }
            return Children;
        }

        public ISymbolVariable[] GetLocals() 
        {
            COMException Exception;
            int hr;
            uint i;
            int cLocals;
            IntPtr[] LocalPointers;
            SymVariable[] Locals;
            hr = SymScope_GetLocals(m_Scope, 0, out cLocals, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetLocals failed.", hr);
                throw Exception;
            }
            LocalPointers = new IntPtr[cLocals];
            Locals = new SymVariable[cLocals];
            hr = SymScope_GetLocals(m_Scope, cLocals, out cLocals, LocalPointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetLocals failed.", hr);
                throw Exception;
            }
            for (i = 0; i < cLocals; i++)
            {
                Locals[i] = new SymVariable(LocalPointers[i]);
            }
            return Locals;
        }

        public ISymbolNamespace[] GetNamespaces()  
        {
            COMException Exception;
            int hr;
            uint i;
            int cNamespaces;
            IntPtr[] NamespacePointers;
            SymNamespace[] Namespaces;
            hr = SymScope_GetNamespaces(m_Scope, 0, out cNamespaces, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            NamespacePointers = new IntPtr[cNamespaces];
            Namespaces = new SymNamespace[cNamespaces];
            hr = SymScope_GetNamespaces(m_Scope, cNamespaces, out cNamespaces, NamespacePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            for (i = 0; i < cNamespaces; i++)
            {
                Namespaces[i] = new SymNamespace(NamespacePointers[i]);
            }
            return Namespaces;
        }
    }

    //-----------------------------------------------------------------------
    // SymVariable
    //-----------------------------------------------------------------------
    public class SymVariable : ISymbolVariable
    {
        private IntPtr m_Variable; // Unmanaged variable pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_Release(IntPtr Variable);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetAddressField1(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetAddressField2(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetAddressField3(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetAddressKind(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetAttributes(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetStartOffset(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetEndOffset(
            IntPtr Variable,
            out int pRetVal);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetName(
            IntPtr Variable,
            int cchName,
            out int pcchName,
            StringBuilder szName);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymVariable_GetSignature(
            IntPtr Variable,
            int cSig,
            out int pcSig,
            [In] byte[] sig);

        //
        // Implementation
        //
        public SymVariable(IntPtr pVariable)
        {
            m_Variable = pVariable;
        }

        ~SymVariable()
        {
            SymVariable_Release(m_Variable);
        }

        public int AddressField1
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymVariable_GetAddressField1(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetAddressField1 failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }
        public int AddressField2
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymVariable_GetAddressField2(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetAddressField2 failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }
        public int AddressField3 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymVariable_GetAddressField3(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetAddressField3 failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }
        public SymAddressKind AddressKind 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                SymAddressKind ret = SymAddressKind.ILOffset;
                hr = SymVariable_GetAddressKind(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetAddressKind failed.", hr);
                    throw Exception;
                }
                switch (RetVal)
                {
                    case 1:
                        ret = SymAddressKind.ILOffset;
                        break;
                    case 2:
                        ret = SymAddressKind.NativeRVA;
                        break;
                    case 3:
                        ret = SymAddressKind.NativeRegister;
                        break;
                    case 4:
                        ret = SymAddressKind.NativeRegisterRelative;
                        break;
                    case 5:
                        ret = SymAddressKind.NativeOffset;
                        break;
                    case 6:
                        ret = SymAddressKind.NativeRegisterRegister;
                        break;
                    case 7:
                        ret = SymAddressKind.NativeRegisterStack;
                        break;
                    case 8:
                        ret = SymAddressKind.NativeStackRegister;
                        break;
                    case 9:
                        ret = SymAddressKind.BitField;
                        break;
                }
                return ret;
            }
        }
        public object Attributes 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                object Attribute;
                hr = SymVariable_GetAttributes(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetAttributes failed.", hr);
                    throw Exception;
                }
                Attribute = (object)RetVal;
                return Attribute;
            }
        }

        public int StartOffset 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymVariable_GetStartOffset(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetStartOffset failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }

        public int EndOffset 
        {
            get
            {
                int hr;
                COMException Exception;
                int RetVal;
                hr = SymVariable_GetEndOffset(m_Variable, out RetVal);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetEndOffset failed.", hr);
                    throw Exception;
                }
                return RetVal;
            }
        }

        public string Name 
        {
            get
            {
                int hr;
                StringBuilder Name;
                int cchName;
                COMException Exception;
                hr = SymVariable_GetName(m_Variable, 0, out cchName, null);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetName failed.", hr);
                    throw Exception;
                }
                Name = new StringBuilder(cchName);
                hr = SymVariable_GetName(m_Variable, cchName, out cchName, Name);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetName failed.", hr);
                    throw Exception;
                }
                return Name.ToString();
            }
        }

        public byte[] GetSignature() 
        {
            int hr;
            byte[] Data;
            COMException Exception;
            int cData;
            hr = SymVariable_GetSignature(m_Variable, 0, out cData, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSignature failed.", hr);
                throw Exception;
            }
            Data = new byte[cData];
            hr = SymVariable_GetSignature(m_Variable, cData, out cData, Data);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetSignature failed.", hr);
                throw Exception;
            }
            return Data;
        }
    }

    //-----------------------------------------------------------------------
    // SymNamespace
    //-----------------------------------------------------------------------
    public class SymNamespace : ISymbolNamespace
    {
        private IntPtr m_Namespace; // unmanaged namespace pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymNamespace_Release(IntPtr Namespace);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymNamespace_GetNamespaces(
            IntPtr Namespace,
            int cNamespaces,
            out int pcNamespaces,
            [In] IntPtr[] NamespacePointers);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymNamespace_GetVariables(
            IntPtr Namespace,
            int cVariables,
            out int pcVariables,
            [In] IntPtr[] VariablePointers);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymNamespace_GetName(
            IntPtr Namespace,
            int cchName,
            out int pcchName,
            StringBuilder szName);

        //
        // Implementation
        //

        // Constructor
        public SymNamespace(IntPtr pNamespace)
        {
            m_Namespace = pNamespace;
        }

        // Destructor
        ~SymNamespace()
        {
            SymNamespace_Release(m_Namespace);
        }

        public ISymbolNamespace[] GetNamespaces()
        {
            COMException Exception;
            int hr;
            uint i;
            int cNamespaces;
            IntPtr[] NamespacePointers;
            SymNamespace[] Namespaces;
            hr = SymNamespace_GetNamespaces(m_Namespace, 0, out cNamespaces, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            NamespacePointers = new IntPtr[cNamespaces];
            Namespaces = new SymNamespace[cNamespaces];
            hr = SymNamespace_GetNamespaces(m_Namespace, cNamespaces, out cNamespaces, NamespacePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetNamespaces failed.", hr);
                throw Exception;
            }
            for (i = 0; i < cNamespaces; i++)
            {
                Namespaces[i] = new SymNamespace(NamespacePointers[i]);
            }
            return Namespaces;
        }

        public ISymbolVariable[] GetVariables()
        {
            COMException Exception;
            int hr;
            IntPtr[] VariablePointers;
            SymVariable[] Variables;
            int cVars;
            uint i;
            hr = SymNamespace_GetVariables(m_Namespace, 0, out cVars, null);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetVariables failed.", hr);
                throw Exception;
            }
            Variables = new SymVariable[cVars];
            VariablePointers = new IntPtr[cVars];

            hr = SymNamespace_GetVariables(m_Namespace, cVars, out cVars, VariablePointers);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetVariables failed.", hr);
                throw Exception;
            }        
            for (i = 0; i < cVars; i++)
            {
                Variables[i] = new SymVariable(VariablePointers[i]);
            }
            return Variables;
        }

        public string Name 
        {
            get
            {
                int hr;
                StringBuilder Name;
                int cchName;
                COMException Exception;
                hr = SymNamespace_GetName(m_Namespace, 0, out cchName, null);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetName failed.", hr);
                    throw Exception;
                }
                Name = new StringBuilder(cchName);
                hr = SymNamespace_GetName(m_Namespace, cchName, out cchName, Name);
                if (hr < 0)
                {
                    Exception = new COMException("Call to GetName failed.", hr);
                    throw Exception;
                }
                return Name.ToString();
            }
        }
    }

    //-----------------------------------------------------------------------
    // SymWriter
    //-----------------------------------------------------------------------    
    public class SymWriter : ISymbolWriter
    {

        private IntPtr m_Writer; // unmanaged writer pointer

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_Release(IntPtr Writer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_GetWriter(IntPtr ppWriter, out IntPtr Writer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_Initialize(
            IntPtr Writer,
            IntPtr emitter,
            [MarshalAs(UnmanagedType.LPWStr)]String szFilename,
            bool fullBuild);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineDocument(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]String szUrl,
            [In] ref Guid pLanguage,
            [In] ref Guid pLanguageVendor,
            [In] ref Guid pDocumentType,
            out IntPtr pDocumentWriter);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_SetUserEntryPoint(
            IntPtr Writer,
            int Token);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_OpenMethod(
            IntPtr Writer,
            int Token);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_CloseMethod(
            IntPtr Writer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineSequencePoints(
            IntPtr Writer,
            IntPtr document,
            int spCount,
            [In] int[] offsets,
            [In] int[] lines, [In] int[] columns,
            [In] int[] endLines, [In] int[] endColumns);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_OpenScope(
            IntPtr Writer,
            int startOffset,
            out int pScopeId);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_CloseScope(
            IntPtr Writer,
            int endOffset);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_SetScopeRange(
            IntPtr Writer,
            int scopeID,
            int startOffset,
            int endOffset);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineLocalVariable(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]string name,
            int attributes,
            int cSig,
            byte[] signature,
            int addrKind,
            int addr1, int addr2, int addr3,
            int startOffset, int endOffset);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineParameter(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]string name,
            int attributes,
            int sequence,
            int addrKind,
            int addr1, int addr2, int addr3);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineField(
            IntPtr Writer,
            int ParentToken,
            [MarshalAs(UnmanagedType.LPWStr)]string name,
            int attributes,
            byte[] signature ,
            int addrKind,
            int addr1, int addr2, int addr3);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_DefineGlobalVariable(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]string name,
            int attributes,
            byte[] signature,
            int cSig,
            int addrKind,
            int addr1, int addr2, int addr3);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_Close(IntPtr Writer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_SetSymAttribute(
            IntPtr Writer,
            int parent,
            [MarshalAs(UnmanagedType.LPWStr)]string name,
            byte[] data);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_OpenNamespace(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]string name);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_CloseNamespace(IntPtr Writer);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_UsingNamespace(
            IntPtr Writer,
            [MarshalAs(UnmanagedType.LPWStr)]string fullName);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymWriter_SetMethodSourceRange(
            IntPtr Writer,
            IntPtr startDoc,
            int startLine,
            int startColumn,
            IntPtr endDoc,
            int endLine,
            int endColumn);

        // Guids for Unmanaged Writer
        readonly static Guid CLSID_CorSymWriter = new Guid(0x0AE2DEB0, 0xF901, 0x478b, 0xBB, 0x9F, 0x88, 0x1E, 0xE8, 0x06, 0x67, 0x88);
        readonly static Guid IID_ISymUnmanagedWriter = new Guid(0xED14AA72, 0x78E2, 0x4884, 0x84, 0xE2, 0x33, 0x42, 0x93, 0xAE, 0x52, 0x14);

        public SymWriter()
        {
            // Create an unmanaged binder
            m_Writer = CreateSymClass.CreateClass(CLSID_CorSymWriter, IID_ISymUnmanagedWriter);
        }

        ~SymWriter()
        {
            SymWriter_Release(m_Writer);
        }

        public void SetUnderlyingWriter(IntPtr underlyingWriter)
        {
            // Demand the permission to access unmanaged code. We do this since we are casting an int to a COM interface, and
            // this can be used improperly.
            (new SecurityPermission(SecurityPermissionFlag.UnmanagedCode)).Demand();

            int hr;
            COMException Exception;
            SymWriter_Release(m_Writer);
            hr = SymWriter_GetWriter(underlyingWriter, out m_Writer);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetWriter failed.", hr);
                throw Exception;
            }
        }


        public void Initialize(IntPtr emitter, string filename, bool fullBuild)
        {
            COMException Exception;
            int hr;

            // Demand the permission to access unmanaged code. We do this since we are casting an int to a COM interface, and
            // this can be used improperly.
            (new SecurityPermission(SecurityPermissionFlag.UnmanagedCode)).Demand();

            hr = SymWriter_Initialize(m_Writer, emitter, filename, fullBuild);
            if (hr < 0)
            {
                Exception = new COMException("Call to Initialize failed.", hr);
                throw Exception;
            }            
        }

        public ISymbolDocumentWriter DefineDocument(string url,
            Guid language,
            Guid languageVendor,
            Guid documentType)
        {
            int hr;
            IntPtr pDocumentWriter;
            SymDocumentWriter DocumentWriter;
            COMException Exception;
            hr = SymWriter_DefineDocument(m_Writer, url, ref language, ref languageVendor, ref documentType, out pDocumentWriter);
            if (hr < 0)
            {
                Exception = new COMException("Call to DefineDocument failed.", hr);
                throw Exception;
            }
            DocumentWriter = new SymDocumentWriter(pDocumentWriter);
            return DocumentWriter;
        }

        public void SetUserEntryPoint(SymbolToken entryMethod)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_SetUserEntryPoint(m_Writer, entryMethod.GetToken());
            if (hr < 0)
            {
                Exception = new COMException("Call to SetUserEntryPoint failed.", hr);
                throw Exception;
            }
        }

        public void OpenMethod(SymbolToken method)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_OpenMethod(m_Writer, method.GetToken());
            if (hr < 0)
            {
                Exception = new COMException("Call to OpenMethod failed.", hr);
                throw Exception;
            }
        }

        public void CloseMethod()
        {
            int hr;
            COMException Exception;
            hr = SymWriter_CloseMethod(m_Writer);
            if (hr < 0)
            {
                Exception = new COMException("Call to CloseMethod failed.", hr);
                throw Exception;
            }
        }

        public void DefineSequencePoints(
            ISymbolDocumentWriter document,
            int[] offsets,
            int[] lines,
            int[] columns,
            int[] endLines,
            int[] endColumns)
        {
            int hr;
            COMException Exception;

            int spCount = 0;
            if (offsets != null)
                spCount = offsets.Length;
            else if (lines != null)
                spCount = lines.Length;
            else if (columns != null)
                spCount = columns.Length;
            else if (endLines != null)
                spCount = endLines.Length;
            else if (endColumns != null)
                spCount = endColumns.Length;

            // Don't do anything if they're not really asking for anything.
            if (spCount == 0)
                return;

            // Make sure all arrays are the same length.
            if ((offsets != null) && (spCount != offsets.Length))
                throw new ArgumentException();

            if ((lines != null) && (spCount != lines.Length))
                throw new ArgumentException();

            if ((columns != null) && (spCount != columns.Length))
                throw new ArgumentException();

            if ((endLines != null) && (spCount != endLines.Length))
                throw new ArgumentException();

            if ((endColumns != null) && (spCount != endColumns.Length))
                throw new ArgumentException();

            hr = SymWriter_DefineSequencePoints(
                m_Writer, 
                ((SymDocumentWriter)document).InternalDocumentWriter,
                offsets.Length,
                offsets,
                lines, columns,
                endLines, endColumns);

            if (hr < 0)
            {
                Exception = new COMException("Call to DefineSequencePoints failed.", hr);
                throw Exception;
            }
        }

        public int OpenScope(int startOffset)
        {
            int hr;
            int ScopeId = 0;
            COMException Exception;
            hr = SymWriter_OpenScope(m_Writer, startOffset, out ScopeId);
            if (hr < 0)
            {
                Exception = new COMException("Call to OpenScope failed.", hr);
                throw Exception;
            }
            return ScopeId;
        }

        public void CloseScope(int endOffset)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_CloseScope(m_Writer, endOffset);
            if (hr < 0)
            {
                Exception = new COMException("Call to CloseScope failed.", hr);
                throw Exception;
            }
        }

        public void SetScopeRange(int scopeID, int startOffset, int endOffset)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_SetScopeRange(m_Writer, scopeID, startOffset, endOffset);
            if (hr < 0)
            {
                Exception = new COMException("Call to SetScopeRange failed.", hr);
                throw Exception;
            }
        }

        public void DefineLocalVariable(
            string name,
            FieldAttributes attributes,
            byte[] signature ,
            SymAddressKind addrKind,
            int addr1,
            int addr2,
            int addr3,
            int startOffset,
            int endOffset)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_DefineLocalVariable(
                m_Writer, name,
                (int)attributes, signature.Length, signature, (int)addrKind,
                addr1, addr2, addr3, startOffset, endOffset);
            if (hr < 0)
            {
                Exception = new COMException("Call to DefineLocalVariable failed.", hr);
                throw Exception;
            }
        }

        public void DefineParameter(string name,
            ParameterAttributes attributes,
            int sequence,
            SymAddressKind addrKind,
            int addr1,
            int addr2,
            int addr3)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_DefineParameter(
                m_Writer, name,
                (int)attributes, sequence , (int)addrKind,
                addr1, addr2, addr3);
            if (hr < 0)
            {
                Exception = new COMException("Call to DefineParameter failed.", hr);
                throw Exception;
            }
        }

        public void DefineField(
            SymbolToken parent,
            string name,
            FieldAttributes attributes,
            byte[] signature ,
            SymAddressKind addrKind,
            int addr1,
            int addr2,
            int addr3)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_DefineField(
                m_Writer, parent.GetToken(), name,
                attributes.GetHashCode(), signature , (int)addrKind,
                addr1, addr2, addr3);
            if (hr < 0)
            {
                Exception = new COMException("Call to DefineField failed.", hr);
                throw Exception;
            }
        }

        public void DefineGlobalVariable(string name,
            FieldAttributes attributes,
            byte[] signature ,
            SymAddressKind addrKind,
            int addr1,
            int addr2,
            int addr3)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_DefineGlobalVariable(
                m_Writer,
                name,
                attributes.GetHashCode(), signature, signature.Length, addrKind.GetHashCode(),
                addr1, addr2, addr3);
            if (hr < 0)
            {
                Exception = new COMException("Call to DefineGlobalVariable failed.", hr);
                throw Exception;
            }
        }

        public void Close()
        {
            int hr;
            COMException Exception;
            hr = SymWriter_Close(m_Writer);
            if (hr < 0)
            {
                Exception = new COMException("Call to Close failed.", hr);
                throw Exception;
            }
        }

        public void SetSymAttribute(SymbolToken parent, string name, byte[] data)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_SetSymAttribute(m_Writer, parent.GetToken(), name, data);
            if (hr < 0)
            {
                Exception = new COMException("Call to SetSymAttribute failed.", hr);
                throw Exception;
            }
        }

        public void OpenNamespace(string name)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_OpenNamespace(m_Writer, name);
            if (hr < 0)
            {
                Exception = new COMException("Call to OpenNamespace failed.", hr);
                throw Exception;
            }
        }

        public void CloseNamespace()
        {
            int hr;
            COMException Exception;
            hr = SymWriter_CloseNamespace(m_Writer);
            if (hr < 0)
            {
                Exception = new COMException("Call to CloseNamespace failed.", hr);
                throw Exception;
            }
        }

        public void UsingNamespace(string fullName)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_UsingNamespace(m_Writer, fullName);
            if (hr < 0)
            {
                Exception = new COMException("Call to UsingNamespace failed.", hr);
                throw Exception;
            }
        }

        public void SetMethodSourceRange(
            ISymbolDocumentWriter startDoc,
            int startLine,
            int startColumn,
            ISymbolDocumentWriter endDoc,
            int endLine,
            int endColumn)
        {
            int hr;
            COMException Exception;
            hr = SymWriter_SetMethodSourceRange(m_Writer,
                                                ((SymDocumentWriter)startDoc).InternalDocumentWriter, startLine, startColumn,
                                                ((SymDocumentWriter)endDoc).InternalDocumentWriter, endLine, endColumn);
            if (hr < 0)
            {
                Exception = new COMException("Call to SetMethodSourceRange failed.", hr);
                throw Exception;
            }
        }
    }

    //-----------------------------------------------------------------------
    // SymBinder
    //-----------------------------------------------------------------------
    public class SymBinder : ISymbolBinder
    {
        private IntPtr m_Binder; // unmanaged binder pointer

        // Import calls
        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymBinder_Release(IntPtr Binder);

        [DllImport(CreateSymClass.NATIVE_DLL)]
        static extern int SymBinder_GetReaderForFile(
            IntPtr Binder,
            int importer,
            [MarshalAs(UnmanagedType.LPWStr)]String filename,
            [MarshalAs(UnmanagedType.LPWStr)]String SearchPath,
            out IntPtr reader);

        // Guid for Unmanaged Binder
        readonly static Guid CLSID_CorSymBinder = new Guid(0x0A29FF9E, 0x7F9C, 0x4437, 0x8B, 0x11, 0xF4, 0x24, 0x49, 0x1E, 0x39, 0x31);
        readonly static Guid IID_ISymUnmanagedBinder = new Guid(0xAA544D42, 0x28CB, 0x11d3, 0xBD, 0x22, 0x00, 0x00, 0xF8, 0x08, 0x49, 0xBD);

        // Constructor
        public SymBinder()
        {
            // Create an unmanaged binder
            m_Binder = CreateSymClass.CreateClass(CLSID_CorSymBinder, IID_ISymUnmanagedBinder);
        }

        // Destructor
        ~SymBinder()
        {
            SymBinder_Release(m_Binder);
        }

        // Get a reader for the the file
        public ISymbolReader GetReader(
            int importer, 
            string filename,
            string searchPath)
        {

            IntPtr Reader;

            // Demand the permission to access unmanaged code. We do this since we are casting an int to a COM interface, and
            // this can be used improperly.
            (new SecurityPermission(SecurityPermissionFlag.UnmanagedCode)).Demand();

            int hr;
            COMException Exception;

            hr = SymBinder_GetReaderForFile(m_Binder, importer, filename, searchPath, out Reader);
            if (hr < 0)
            {
                Exception = new COMException("Call to GetReaderForFile failed.", hr);
                throw Exception;
            }

            SymReader symReader = new SymReader(Reader);
            return symReader;
        }
    }
}

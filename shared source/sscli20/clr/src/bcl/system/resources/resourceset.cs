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
/*============================================================
**
** Class:  ResourceSet
**
**
** Purpose: Culture-specific collection of resources.
**
** 
===========================================================*/
namespace System.Resources {
    using System;
    using System.Collections;
    using System.IO;
    using System.Runtime.Remoting.Activation;
    using System.Globalization;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    using System.Reflection;
    using System.Runtime.Serialization;
    using System.Runtime.Versioning;

    // A ResourceSet stores all the resources defined in one particular CultureInfo.
    // 
    // The method used to load resources is straightforward - this class
    // enumerates over an IResourceReader, loading every name and value, and 
    // stores them in a hash table.  Custom IResourceReaders can be used.
    // 
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public class ResourceSet : IDisposable, IEnumerable
    {
        [NonSerialized] protected IResourceReader Reader;
        protected Hashtable Table;

        private Hashtable _caseInsensitiveTable;  // For case-insensitive lookups.


        protected ResourceSet()
        {
            // To not inconvenience people subclassing us, we should allocate a new
            // hashtable here just so that Table is set to something.
            Table = new Hashtable(0);
        }

        // For RuntimeResourceSet, ignore the Table parameter - it's a wasted 
        // allocation.
        internal ResourceSet(bool junk)
        {
        }

        // Creates a ResourceSet using the system default ResourceReader
        // implementation.  Use this constructor to open & read from a file 
        // on disk.
        // 
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public ResourceSet(String fileName)
        {
            Reader = new ResourceReader(fileName);
            Table = new Hashtable();
            ReadResources();
        }

    
        // Creates a ResourceSet using the system default ResourceReader
        // implementation.  Use this constructor to read from an open stream 
        // of data.
        // 
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public ResourceSet(Stream stream)
        {
            Reader = new ResourceReader(stream);
            Table = new Hashtable();
            ReadResources();
        }


        public ResourceSet(IResourceReader reader)
        {
            if (reader == null)
                throw new ArgumentNullException("reader");
            Reader = reader;
            Table = new Hashtable();
            ReadResources();
        }

    
        // Closes and releases any resources used by this ResourceSet, if any.
        // All calls to methods on the ResourceSet after a call to close may 
        // fail.  Close is guaranteed to be safely callable multiple times on a 
        // particular ResourceSet, and all subclasses must support these semantics.
        public virtual void Close()
        {
            Dispose(true);
        }
        
        protected virtual void Dispose(bool disposing)
        {
            if (disposing) {
                // Close the Reader in a thread-safe way.
                IResourceReader copyOfReader = Reader;
                Reader = null;
                if (copyOfReader != null)
                    copyOfReader.Close();
            }
            Reader = null;
            _caseInsensitiveTable = null;
            Table = null;
        }

        public void Dispose()
        {
            Dispose(true);
        }


        // Returns the preferred IResourceReader class for this kind of ResourceSet.
        // Subclasses of ResourceSet using their own Readers &; should override
        // GetDefaultReader and GetDefaultWriter.
        public virtual Type GetDefaultReader()
        {
            return typeof(ResourceReader);
        }
    
        // Returns the preferred IResourceWriter class for this kind of ResourceSet.
        // Subclasses of ResourceSet using their own Readers &; should override
        // GetDefaultReader and GetDefaultWriter.
        public virtual Type GetDefaultWriter()
        {
            return typeof(ResourceWriter);
        }

        [ComVisible(false)]
        public virtual IDictionaryEnumerator GetEnumerator()
        {
            return GetEnumeratorHelper();
        }

        /// <internalonly/>
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumeratorHelper();
        }

        private IDictionaryEnumerator GetEnumeratorHelper()
        {
            Hashtable copyOfTable = Table;  // Avoid a race with Dispose
            if (copyOfTable == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
            return copyOfTable.GetEnumerator();
        }

        // Look up a string value for a resource given its name.
        // 
        public virtual String GetString(String name)
        {
            Hashtable copyOfTable = Table;  // Avoid a race with Dispose
            if (copyOfTable == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
            if (name==null)
                throw new ArgumentNullException("name");
    
            try {
                return (String) copyOfTable[name];
            }
            catch (InvalidCastException) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceNotString_Name", name));
            }
        }

        public virtual String GetString(String name, bool ignoreCase)
        {
            Hashtable copyOfTable = Table;  // Avoid a race with Dispose
            if (copyOfTable == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
            if (name==null)
                throw new ArgumentNullException("name");

            String s;
            try {
                s = (String) copyOfTable[name];
            }
            catch (InvalidCastException) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceNotString_Name", name));
            }
            if (s != null || !ignoreCase)
                return s;

            // Try doing a case-insensitive lookup.
            Hashtable caseTable = _caseInsensitiveTable;  // Avoid race with Close
            if (caseTable == null) {
                caseTable = new Hashtable(StringComparer.OrdinalIgnoreCase);
#if _DEBUG
                //Console.WriteLine("ResourceSet::GetString loading up data.  ignoreCase: "+ignoreCase);
                BCLDebug.Perf(!ignoreCase, "Using case-insensitive lookups is bad perf-wise.  Consider capitalizing "+name+" correctly in your source");
#endif
                IDictionaryEnumerator en = copyOfTable.GetEnumerator();
                while (en.MoveNext()) {
                    caseTable.Add(en.Key, en.Value);
                }
                _caseInsensitiveTable = caseTable;
            }
            try {
                return (String) caseTable[name];
            }
            catch (InvalidCastException) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ResourceNotString_Name", name));
            }
        }
        
        // Look up an object value for a resource given its name.
        // 
        public virtual Object GetObject(String name)
        {
            Hashtable copyOfTable = Table;  // Avoid a race with Dispose
            if (copyOfTable == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
            if (name==null)
                throw new ArgumentNullException("name");
            
            return copyOfTable[name];
        }

        public virtual Object GetObject(String name, bool ignoreCase)
        {
            Hashtable copyOfTable = Table;  // Avoid a race with Dispose
            if (copyOfTable == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
            if (name==null)
                throw new ArgumentNullException("name");
            
            Object obj = copyOfTable[name];
            if (obj != null || !ignoreCase)
                return obj;

            // Try doing a case-insensitive lookup.
            Hashtable caseTable = _caseInsensitiveTable;  // Avoid race with Close
            if (caseTable == null) {
                caseTable = new Hashtable(StringComparer.OrdinalIgnoreCase);
#if _DEBUG
                //Console.WriteLine("ResourceSet::GetObject loading up case-insensitive data");
                BCLDebug.Perf(!ignoreCase, "Using case-insensitive lookups is bad perf-wise.  Consider capitalizing "+name+" correctly in your source");
#endif
                IDictionaryEnumerator en = copyOfTable.GetEnumerator();
                while (en.MoveNext()) {
                    caseTable.Add(en.Key, en.Value);
                }
                _caseInsensitiveTable = caseTable;
            }

            return caseTable[name];
        }
    
        protected virtual void ReadResources()
        {
            IDictionaryEnumerator en = Reader.GetEnumerator();
            while (en.MoveNext()) {
                Object value = en.Value;
                Table.Add(en.Key, value);
            }
            // While technically possible to close the Reader here, don't close it
            // to help with some WinRes lifetime issues.
        }
    }
}

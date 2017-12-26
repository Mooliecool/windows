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
** Class:  RuntimeResourceSet
**
**
** Purpose: CultureInfo-specific collection of resources.
**
** 
===========================================================*/
namespace System.Resources {    
    using System;
    using System.IO;
    using System.Collections;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.Versioning;

    //
    //
    //
    //
    // 
    // 
    // 
    // 
    //
    // 
    // 
    //
    //
    //
    //
    // 
    //
    // 
    // 
    // 
    // 
    internal sealed class RuntimeResourceSet : ResourceSet, IEnumerable
    {
        internal const int Version = 2;            // File format version number
        
        // Cache for resources.  Key is the resource name, which can be cached
        // for arbitrarily long times, since the object is usually a string
        // literal that will live for the lifetime of the appdomain.  The
        // value is a ResourceLocator instance, which might cache the object.
        private Dictionary<String, ResourceLocator> _resCache;


        // For our special load-on-demand reader, cache the cast.  The 
        // RuntimeResourceSet's implementation knows how to treat this reader specially.
        private ResourceReader _defaultReader;

        private Dictionary<String, ResourceLocator> _caseInsensitiveTable;

        // If we're not using our custom reader, then enumerate through all
        // the resources once, adding them into the table.
        private bool _haveReadFromReader;

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal RuntimeResourceSet(String fileName) : base(false)
        {
            BCLDebug.Log("RESMGRFILEFORMAT", "RuntimeResourceSet .ctor(String)");
            _resCache = new Dictionary<String, ResourceLocator>(FastResourceComparer.Default);
            Stream stream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
            _defaultReader = new ResourceReader(stream, _resCache);
            Reader = _defaultReader;
        }

        internal RuntimeResourceSet(Stream stream) : base(false)
        {
            BCLDebug.Log("RESMGRFILEFORMAT", "RuntimeResourceSet .ctor(Stream)");
            _resCache = new Dictionary<String, ResourceLocator>(FastResourceComparer.Default);
            _defaultReader = new ResourceReader(stream, _resCache);
            Reader = _defaultReader;
        }
    
        protected override void Dispose(bool disposing)
        {
            if (Reader == null)
                return;
            
            if (disposing) {
                lock(Reader) {
                    _resCache = null;
                    if (_defaultReader != null) {
                        _defaultReader.Close();
                        _defaultReader = null;
                    }
                    _caseInsensitiveTable = null;
                    // Set Reader to null to avoid a race in GetObject.
                    base.Dispose(disposing);
                }
            } 
            else {
                // Just to make sure we always clear these fields in the future...
                _resCache = null;
                _caseInsensitiveTable = null;
                _defaultReader = null;
                base.Dispose(disposing);
            }
        }

        public override IDictionaryEnumerator GetEnumerator()
        {
            return GetEnumeratorHelper();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumeratorHelper();
        }

        private IDictionaryEnumerator GetEnumeratorHelper()
        {
            IResourceReader copyOfReader = Reader;
            if (copyOfReader == null || _resCache == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));

            return copyOfReader.GetEnumerator();
        }


        public override String GetString(String key)
        {
            Object o = GetObject(key, false, true);
            return (String) o;
        }

        public override String GetString(String key, bool ignoreCase)
        {
            Object o = GetObject(key, ignoreCase, true);
            return (String) o;
        }

        public override Object GetObject(String key)
        {
            return GetObject(key, false, false);
        }

        public override Object GetObject(String key, bool ignoreCase)
        {
            return GetObject(key, ignoreCase, false);
        }

        private Object GetObject(String key, bool ignoreCase, bool isString)
        {
            if (key==null)
                throw new ArgumentNullException("key");
            Dictionary<String, ResourceLocator> copyOfCache = _resCache;
            if (Reader == null || copyOfCache == null)
                throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));

            Object value = null;
            ResourceLocator resLocation;
            if (copyOfCache.TryGetValue(key, out resLocation)) {
                value = ResolveResourceLocator(resLocation, key, copyOfCache, false);
                return value;
            }
            
            lock(Reader) {
                if (Reader == null)
                    throw new ObjectDisposedException(null, Environment.GetResourceString("ObjectDisposed_ResourceSet"));
                
                if (_defaultReader != null) {
                    BCLDebug.Log("RESMGRFILEFORMAT", "Going down fast path in RuntimeResourceSet::GetObject");
                    
                    // Find the offset within the data section
                    int dataPos = -1;
                    if (_resCache.TryGetValue(key, out resLocation)) {
                        value = resLocation.Value;
                        dataPos = resLocation.DataPosition;
                    }
                    
                    if (dataPos == -1 && value == null) {
                        dataPos = _defaultReader.FindPosForResource(key);
                    }

                    if (dataPos != -1 && value == null) {
                        BCLDebug.Assert(dataPos >= 0, "data section offset cannot be negative!");
                        // Normally calling LoadString or LoadObject requires
                        // taking a lock.  Note that in this case, we took a
                        // lock on the entire RuntimeResourceSet, which is 
                        // sufficient since we never pass this ResourceReader
                        // to anyone else.
                        ResourceTypeCode typeCode;
                        if (isString) {
                            value = _defaultReader.LoadString(dataPos);
                            typeCode = ResourceTypeCode.String;
                        }
                        else {
                            value = _defaultReader.LoadObject(dataPos, out typeCode);
                        }

                        resLocation = new ResourceLocator(dataPos, (ResourceLocator.CanCache(typeCode)) ? value : null);
                        lock(_resCache) {
                            _resCache[key] = resLocation;
                        }
                    }
                    
                    if (value != null || !ignoreCase) {
                        
                        return value;  // may be null
                    }
                }  // if (_defaultReader != null)

                // At this point, we either don't have our default resource reader
                // or we haven't found the particular resource we're looking for
                // and may have to search for it in a case-insensitive way.
                if (!_haveReadFromReader) {
                    // If necessary, init our case insensitive hash table.
                    if (ignoreCase && _caseInsensitiveTable == null) {
                        _caseInsensitiveTable = new Dictionary<String, ResourceLocator>(StringComparer.OrdinalIgnoreCase);
                    }
#if _DEBUG
                    BCLDebug.Perf(!ignoreCase, "Using case-insensitive lookups is bad perf-wise.  Consider capitalizing "+key+" correctly in your source");
#endif

                    if (_defaultReader == null) {
                        IDictionaryEnumerator en = Reader.GetEnumerator();
                        while (en.MoveNext()) {
                            DictionaryEntry entry = en.Entry;
                            String readKey = (String) entry.Key;
                            ResourceLocator resLoc = new ResourceLocator(-1, entry.Value);
                            _resCache.Add(readKey, resLoc);
                            if (ignoreCase)
                                _caseInsensitiveTable.Add(readKey, resLoc);
                        }
                        // Only close the reader if it is NOT our default one,
                        // since we need it around to resolve ResourceLocators.
                        if (!ignoreCase)
                            Reader.Close();
                    }
                    else {
                        BCLDebug.Assert(ignoreCase, "This should only happen for case-insensitive lookups");
                        ResourceReader.ResourceEnumerator en = _defaultReader.GetEnumeratorInternal();
                        while (en.MoveNext()) {
                            // Note: Always ask for the resource key before the data position.
                            String currentKey = (String) en.Key;
                            int dataPos = en.DataPosition;
                            ResourceLocator resLoc = new ResourceLocator(dataPos, null);
                            _caseInsensitiveTable.Add(currentKey, resLoc);
                        }
                    }
                    _haveReadFromReader = true;
                }
                Object obj = null;
                bool found = false;
                bool keyInWrongCase = false;
                if (_defaultReader != null) {
                    if (_resCache.TryGetValue(key, out resLocation))
                        found = true;
                }
                if (!found && ignoreCase) {
                    if (_caseInsensitiveTable.TryGetValue(key, out resLocation)) {
                        found = true;
                        keyInWrongCase = true;
                    }
                }
                if (found) {
                    obj = ResolveResourceLocator(resLocation, key, copyOfCache, keyInWrongCase);
                }
                return obj;
            } // lock(Reader)
        }
        
        // The last parameter indicates whether the lookup required a 
        // case-insensitive lookup to succeed, indicating we shouldn't add 
        // the ResourceLocation to our case-sensitive cache.
        private Object ResolveResourceLocator(ResourceLocator resLocation, String key, Dictionary<String, ResourceLocator> copyOfCache, bool keyInWrongCase)
        {
            // We need to explicitly resolve loosely linked manifest
            // resources, and we need to resolve ResourceLocators with null objects.
            Object value = resLocation.Value;
            if (value == null) {
                ResourceTypeCode typeCode;
                lock(Reader) {
                    value = _defaultReader.LoadObject(resLocation.DataPosition, out typeCode);
                }
                if (!keyInWrongCase && ResourceLocator.CanCache(typeCode)) {
                    resLocation.Value = value;
                    copyOfCache[key] = resLocation;
                }
            }
            return value;
        }
    }
}

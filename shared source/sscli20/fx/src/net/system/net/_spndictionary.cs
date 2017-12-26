//------------------------------------------------------------------------------
// <copyright file="_SpnDictionary.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

/*++   

Abstract:
    This internal class implements a static mutlithreaded dictionary for user-registered SPNs.
    An SPN is mapped based on a Uri prefix that contains scheme, host and port.


Author:

    Alexei Vopilov    15-Nov-2003

Revision History:

--*/

namespace System.Net {
    using System;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Security.Permissions;

    internal class SpnDictionary : StringDictionary {

        //
        //A Hashtable can support one writer and multiple readers concurrently
        //

        private Hashtable m_SyncTable = Hashtable.Synchronized(new Hashtable());

        //
        //
        internal SpnDictionary():base() {
        }
        //
        //
        //
        public override int Count {
            get {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                return m_SyncTable.Count;
            }
        }
        //
        // We are thread safe
        //
        public override bool IsSynchronized {
            get {
                return true;
            }
        }
        //
        // Internal lookup, bypasses security checks
        //
        internal string InternalGet(string canonicalKey)
        {
            int lastLength = 0;
            string key = null;

            foreach (object o in m_SyncTable.Keys){
                string s = (string) o;
                if(s != null && s.Length > lastLength){
                    if(String.Compare(s,0,canonicalKey,0,s.Length,StringComparison.OrdinalIgnoreCase) == 0){
                         lastLength = s.Length;
                         key = s;
                    }
                }
            }  
            return (key != null) ? (string)m_SyncTable[key]: null;
        }

        internal void InternalSet(string canonicalKey, string spn)
        {
            m_SyncTable[canonicalKey] = spn;
        }
        //
        // Public lookup method
        //
        public override string this[string key] {
            get {
                key = GetCanonicalKey(key);
                return InternalGet(key);
            }
            set {
                key = GetCanonicalKey(key);
                InternalSet(key, value);
            }
        }
        //
        public override ICollection Keys {
            get {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                return m_SyncTable.Keys;
            }
        }
        //
        public override object SyncRoot {
            [HostProtection(Synchronization=true)]
            get {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                return m_SyncTable;
            }
        }
        //
        public override ICollection Values {
            get {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                return m_SyncTable.Values;
            }
        }
        //
        public override void Add(string key, string value) {
            key = GetCanonicalKey(key);
            m_SyncTable.Add(key, value);
        }
        //
        public override void Clear() {
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            m_SyncTable.Clear();
        }
        //
        public override bool ContainsKey(string key) {
            key = GetCanonicalKey(key);
            return m_SyncTable.ContainsKey(key);
        }
        //
        public override bool ContainsValue(string value) {
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            return m_SyncTable.ContainsValue(value);
        }
        //
        public override void CopyTo(Array array, int index) {
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            m_SyncTable.CopyTo(array, index);
        }
        //
        public override IEnumerator GetEnumerator() {
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            return m_SyncTable.GetEnumerator();
        }
        //
        public override void Remove(string key) {
            key = GetCanonicalKey(key);
            m_SyncTable.Remove(key);
        }

        //
        // Private stuff: We want to serialize on updates on one thread
        //
        private static string GetCanonicalKey(string key)
        {
            if( key == null ) {
                throw new ArgumentNullException("key");
            }
            try {
                Uri uri = new Uri(key);
                key = uri.GetParts(UriComponents.Scheme | UriComponents.Host | UriComponents.Port | UriComponents.Path, UriFormat.SafeUnescaped);
                new WebPermission(NetworkAccess.Connect, new Uri(key)).Demand();
            }
            catch(UriFormatException e) {
                throw new ArgumentException(SR.GetString(SR.net_mustbeuri, "key"), "key", e);
            }
            return key;
        }
    }

}

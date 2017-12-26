//------------------------------------------------------------------------------
// <copyright file="AuthenticationManager.cs" company="Microsoft">
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

namespace System.Net {

    using System.Collections;
    using System.Collections.Generic;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.Globalization;
    using System.Net.Configuration;
    using System.Reflection;
    using System.Security.Permissions;
    using System;
    using System.Threading;


    //
    // A contract that applications can use to restrict auth scenarios in current appDomain
    //
    public interface ICredentialPolicy
    {
        bool ShouldSendCredential(Uri challengeUri, WebRequest request, NetworkCredential credential, IAuthenticationModule authenticationModule);
    }

    /// <devdoc>
    ///    <para>Manages the authentication modules called during the client authentication
    ///       process.</para>
    /// </devdoc>
    public class AuthenticationManager {

        //also used as a lock object
        private static PrefixLookup s_ModuleBinding = new PrefixLookup();

        private static ArrayList s_ModuleList;
        private static ICredentialPolicy s_ICredentialPolicy;
        private static SpnDictionary m_SpnDictionary = new SpnDictionary();

        // not creatable...
        //
        private AuthenticationManager() {
        }

        //
        //
        //
        public static ICredentialPolicy CredentialPolicy {
            get {
                return s_ICredentialPolicy;
            }
            set {
                ExceptionHelper.ControlPolicyPermission.Demand();
                s_ICredentialPolicy = value;
            }
        }
        //
        //
        public static StringDictionary CustomTargetNameDictionary {
            get {return m_SpnDictionary;}
        }
        //
        // This will give access to some internal methods
        //
        internal static SpnDictionary SpnDictionary {
            get {return m_SpnDictionary;}
        }

        //
        //
        internal static void EnsureConfigLoaded() {
            try {
                object o = ModuleList;
            }
            catch (Exception e) {
                if (e is ThreadAbortException || e is OutOfMemoryException || e is StackOverflowException)
                    throw;
                // A Config System has circular dependency on HttpWebRequest so they call this method to
                // trigger the config. For some reason they don't want any exceptions from here.
            }
            catch {
                // A Config System has circular dependency on HttpWebRequest so they call this method to
                // trigger the config. For some reason they don't want any exceptions from here.
            }
        }

        //
        // ModuleList - static initialized property -
        //  contains list of Modules used for Authentication
        //

        private static ArrayList ModuleList {

            get {

                //
                // GetConfig() might use us, so we have a circular dependency issue,
                // that causes us to nest here, we grab the lock, only
                // if we haven't initialized, or another thread is busy in initialization
                //

                if (s_ModuleList == null) {
                    lock (s_ModuleBinding) {
                        if (s_ModuleList == null) {
                            GlobalLog.Print("AuthenticationManager::Initialize(): calling ConfigurationManager.GetSection()");

                            // This will never come back as null. Additionally, it will
                            // have the items the user wants available.
                            List<Type> authenticationModuleTypes =  AuthenticationModulesSectionInternal.GetSection().AuthenticationModules;

                            //
                            // Should be registered in a growing list of encryption/algorithm strengths
                            //  basically, walk through a list of Types, and create new Auth objects
                            //  from them.
                            //
                            // order is meaningful here:
                            // load the registered list of auth types
                            // with growing level of encryption.
                            //

                            ArrayList moduleList = new ArrayList();
                            IAuthenticationModule moduleToRegister;
                            foreach (Type type in authenticationModuleTypes){
                                try {
                                    moduleToRegister = Activator.CreateInstance(type,
                                                        BindingFlags.CreateInstance
                                                        | BindingFlags.Instance
                                                        | BindingFlags.NonPublic
                                                        | BindingFlags.Public,
                                                        null,          // Binder
                                                        new object[0], // no arguments
                                                        CultureInfo.InvariantCulture
                                                        ) as IAuthenticationModule;
                                    if (moduleToRegister != null) {
                                        GlobalLog.Print("WebRequest::Initialize(): Register:" + moduleToRegister.AuthenticationType);
                                        RemoveAuthenticationType(moduleList, moduleToRegister.AuthenticationType);
                                        moduleList.Add(moduleToRegister);
                                    }
                                }
                                catch (Exception exception) {
                                    //
                                    // ignore failure (log exception for debugging)
                                    //
                                    GlobalLog.Print("AuthenticationManager::constructor failed to initialize: " + exception.ToString());
                                }
                                catch {
                                    //
                                    // ignore failure (log exception for debugging)
                                    //
                                    GlobalLog.Print("AuthenticationManager::constructor failed to initialize:  Non-CLS Compliant Exception");
                                }
                            }

                            s_ModuleList = moduleList;
                        }
                    }
                }

                return s_ModuleList;
            }
        }


        private static void RemoveAuthenticationType(ArrayList list, string typeToRemove) {
            for (int i=0; i< list.Count; ++i) {
                if (string.Compare(((IAuthenticationModule)list[i]).AuthenticationType, typeToRemove, StringComparison.OrdinalIgnoreCase) ==0) {
                    list.RemoveAt(i);
                    break;
                }

            }
        }

        /// <devdoc>
        ///    <para>Call each registered authentication module to determine the first module that
        ///       can respond to the authentication request.</para>
        /// </devdoc>
        public static Authorization Authenticate(string challenge, WebRequest request, ICredentials credentials) {
            //
            // parameter validation
            //
            if (request == null) {
                throw new ArgumentNullException("request");
            }
            if (credentials == null) {
                throw new ArgumentNullException("credentials");
            }
            if (challenge==null) {
                throw new ArgumentNullException("challenge");
            }

            GlobalLog.Print("AuthenticationManager::Authenticate() challenge:[" + challenge + "]");

            Authorization response = null;

            HttpWebRequest httpWebRequest = request as HttpWebRequest;
            if (httpWebRequest != null && httpWebRequest.CurrentAuthenticationState.Module != null)
            {
                response = httpWebRequest.CurrentAuthenticationState.Module.Authenticate(challenge, request, credentials);
            }
            else
            {
                // This is the case where we would try to find the module on the first server challenge
                lock (s_ModuleBinding) {
                    //
                    // fastest way of iterating on the ArryList
                    //
                    for (int i = 0; i < ModuleList.Count; i++) {
                        IAuthenticationModule authenticationModule = (IAuthenticationModule)ModuleList[i];
                        //
                        // the AuthenticationModule will
                        // 1) return a valid string on success
                        // 2) return null if it knows it cannot respond
                        // 3) throw if it could have responded but unexpectedly failed to do so
                        //
                        if (httpWebRequest != null) {
                            httpWebRequest.CurrentAuthenticationState.Module = authenticationModule;
                        }
                        response = authenticationModule.Authenticate(challenge, request, credentials);

                        if (response!=null) {
                            //
                            // found the Authentication Module, return it
                            //
                            GlobalLog.Print("AuthenticationManager::Authenticate() found IAuthenticationModule:[" + authenticationModule.AuthenticationType + "]");
                            break;
                        }
                    }
                }
            }

            return response;
        }

        /// <devdoc>
        ///    <para>Pre-authenticates a request.</para>
        /// </devdoc>
        public static Authorization PreAuthenticate(WebRequest request, ICredentials credentials) {
            GlobalLog.Print("AuthenticationManager::PreAuthenticate() request:" + ValidationHelper.HashString(request) + " credentials:" + ValidationHelper.HashString(credentials));
            if (request == null) {
                throw new ArgumentNullException("request");
            }
            if (credentials == null) {
                return null;
            }

            HttpWebRequest httpWebRequest = request as HttpWebRequest;
            IAuthenticationModule authenticationModule;
            if (httpWebRequest == null)
                return null;

            //
            // PrefixLookup is thread-safe
            //
            string moduleName = s_ModuleBinding.Lookup(httpWebRequest.ChallengedUri.AbsoluteUri) as string;
            GlobalLog.Print("AuthenticationManager::PreAuthenticate() s_ModuleBinding.Lookup returns:" + ValidationHelper.ToString(moduleName));
            if (moduleName == null)
                return null;
            authenticationModule = findModule(moduleName);
            if (authenticationModule == null)
            {
                // The module could have been unregistered
                // No preauthentication is possible
                return null;
            }

            // Otherwise invoke the PreAuthenticate method
            // we're guaranteed that CanPreAuthenticate is true because we check before calling BindModule()
            Authorization authorization = authenticationModule.PreAuthenticate(request, credentials);

            if (authorization != null && !authorization.Complete && httpWebRequest != null)
                httpWebRequest.CurrentAuthenticationState.Module = authenticationModule;

            GlobalLog.Print("AuthenticationManager::PreAuthenticate() IAuthenticationModule.PreAuthenticate() returned authorization:" + ValidationHelper.HashString(authorization));
            return authorization;
        }


        /// <devdoc>
        ///    <para>Registers an authentication module with the authentication manager.</para>
        /// </devdoc>
        public static void Register(IAuthenticationModule authenticationModule) {
            ExceptionHelper.UnmanagedPermission.Demand();
            if (authenticationModule == null) {
                throw new ArgumentNullException("authenticationModule");
            }
            GlobalLog.Print("AuthenticationManager::Register() registering :[" + authenticationModule.AuthenticationType + "]");
            lock (s_ModuleBinding) {
                IAuthenticationModule existentModule = findModule(authenticationModule.AuthenticationType);
                if (existentModule != null) {
                    ModuleList.Remove(existentModule);
                }
                ModuleList.Add(authenticationModule);
            }
        }

        /// <devdoc>
        ///    <para>Unregisters authentication modules for an authentication scheme.</para>
        /// </devdoc>
        public static void Unregister(IAuthenticationModule authenticationModule) {
            ExceptionHelper.UnmanagedPermission.Demand();
            if (authenticationModule == null) {
                throw new ArgumentNullException("authenticationModule");
            }
            GlobalLog.Print("AuthenticationManager::Unregister() unregistering :[" + authenticationModule.AuthenticationType + "]");
            lock (s_ModuleBinding) {
                if (!ModuleList.Contains(authenticationModule)) {
                    throw new InvalidOperationException(SR.GetString(SR.net_authmodulenotregistered));
                }
                ModuleList.Remove(authenticationModule);
            }
        }
        /// <devdoc>
        ///    <para>Unregisters authentication modules for an authentication scheme.</para>
        /// </devdoc>
        public static void Unregister(string authenticationScheme) {
            ExceptionHelper.UnmanagedPermission.Demand();
            if (authenticationScheme == null) {
                throw new ArgumentNullException("authenticationScheme");
            }
            GlobalLog.Print("AuthenticationManager::Unregister() unregistering :[" + authenticationScheme + "]");
            lock (s_ModuleBinding) {
                IAuthenticationModule existentModule = findModule(authenticationScheme);
                if (existentModule == null) {
                    throw new InvalidOperationException(SR.GetString(SR.net_authschemenotregistered));
                }
                ModuleList.Remove(existentModule);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Returns a list of registered authentication modules.
        ///    </para>
        /// </devdoc>
        public static IEnumerator RegisteredModules {
            get {
                return ModuleList.GetEnumerator();
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Binds an authentication response to a request for pre-authentication.
        ///    </para>
        /// </devdoc>
        // Create binding between an authorization response and the module
        // generating that response
        // This association is used for deciding which module to invoke
        // for preauthentication purposes
        internal static void BindModule(Uri uri, Authorization response, IAuthenticationModule module) {
            GlobalLog.Assert(module.CanPreAuthenticate, "AuthenticationManager::BindModule()|module.CanPreAuthenticate == false");
            if (response.ProtectionRealm!=null) {
                // The authentication module specified which Uri prefixes
                // will be preauthenticated
                string[] prefix = response.ProtectionRealm;

                for (int k=0; k<prefix.Length; k++) {
                    //
                    // PrefixLookup is thread-safe
                    //
                    s_ModuleBinding.Add(prefix[k], module.AuthenticationType);
                }
            }
            else {
                // Otherwise use the default policy for "fabricating"
                // some protection realm generalizing the particular Uri
                string prefix = generalize(uri);
                //
                // PrefixLookup is thread-safe
                //
                s_ModuleBinding.Add(prefix, module.AuthenticationType);
            }
        }

        //
        // Lookup module by AuthenticationType
        //
        private static IAuthenticationModule findModule(string authenticationType) {
            IAuthenticationModule returnAuthenticationModule = null;
            ArrayList moduleList = ModuleList;
            IAuthenticationModule authenticationModule;
            for (int k=0; k<moduleList.Count; k++) {
                authenticationModule = (IAuthenticationModule)moduleList[k];
                if (string.Compare(authenticationModule.AuthenticationType, authenticationType, StringComparison.OrdinalIgnoreCase) == 0) {
                    returnAuthenticationModule = authenticationModule;
                    break;
                }
            }
            return returnAuthenticationModule;
        }

        // This function returns a prefix of the given absolute Uri
        // which will be used for associating authentication information
        // The purpose is to associate the module-binding not with a single
        // Uri but some collection generalizing that Uri to the loosely-defined
        // notion of "protection realm"
        private static string generalize(Uri location) {
            string completeUri = location.AbsoluteUri;
            int lastFwdSlash = completeUri.LastIndexOf('/');
            if (lastFwdSlash < 0) {
                return completeUri;
            }
            return completeUri.Substring(0, lastFwdSlash+1);
        }

        //
        // The method will extract the blob that does correspond to the moduled with the name passed in signature parameter
        // The method avoids confusion arisen from the parameters passed in a quoted string, such as:
        // WWW-Authenticate: Digest username="NTLM", realm="wit", NTLM ...
        //
        internal static int FindSubstringNotInQuotes(string challenge, string signature) {
            int index = -1;
            if (challenge != null && signature != null && challenge.Length>=signature.Length) {
                int firstQuote = -1, secondQuote = -1;
                for (int i = 0; i < challenge.Length; i++)
                {
                    if (challenge[i]=='\"')
                    {
                        if (firstQuote <= secondQuote)
                            firstQuote = i;
                        else
                            secondQuote = i;
                    }

                    if (i==challenge.Length-1 || (challenge[i]=='\"' && firstQuote>secondQuote))
                    {
                        // see if the portion of challenge out of the quotes contains
                        // the signature of the IAuthenticationModule
                        if (i==challenge.Length-1)
                            firstQuote = challenge.Length;

                        if (firstQuote<secondQuote + 3)
                            continue;

                        index = IndexOf(challenge, signature, secondQuote + 1, firstQuote - secondQuote - 1);

                        if (index>=0)
                        {
                            if ((index==0 || challenge[index-1]==' ' || challenge[index-1]==',') &&
                                (index+signature.Length==challenge.Length || challenge[index+signature.Length]==' ' || challenge[index+signature.Length]==','))
                            {
                                break;
                            }
                            index = -1;
                        }
                    }
                }
            }
            GlobalLog.Print("AuthenticationManager::FindSubstringNotInQuotes(" + challenge + ", " + signature + ")=" + index.ToString());
            return index;
        }
        //
        private static int IndexOf(string challenge, string lwrCaseSignature, int start, int count)
        {
            count += start + 1 - lwrCaseSignature.Length;
            for (; start < count; ++start)
            {
                int i = 0;
                for (; i < lwrCaseSignature.Length; ++i)
                {
                    // force a challenge char to lowecase (safe assuming it works on trusted ASCII source)
                    if ((challenge[start+i] | 0x20) != lwrCaseSignature[i])
                        break;
                }
                if (i == lwrCaseSignature.Length)
                    return start;
            }
            return -1;
        }
        //
        // this method is called by the IAuthenticationModule implementations
        // (mainly Digest) to safely find their list of parameters in a challenge.
        // it returns the index of the first ',' that is not included in quotes,
        // -1 is returned on error or end of string. on return offset contains the
        // index of the first '=' that is not included in quotes, -1 if no '=' was found.
        //
        internal static int SplitNoQuotes(string challenge, ref int offset) {
            // GlobalLog.Print("SplitNoQuotes([" + challenge + "], " + offset.ToString() + ")");
            //
            // save offset
            //
            int realOffset = offset;
            //
            // default is not found
            //
            offset = -1;

            if (challenge != null && realOffset<challenge.Length) {
                int firstQuote = -1, secondQuote = -1;

                for (int i = realOffset; i < challenge.Length; i++) {
                    //
                    // firstQuote>secondQuote means we are in a quoted string
                    //
                    if (firstQuote>secondQuote && challenge[i]=='\\' && i+1 < challenge.Length && challenge[i+1]=='\"') {
                        //
                        // skip <\"> when in a quoted string
                        //
                        i++;
                    }
                    else if (challenge[i]=='\"') {
                        if (firstQuote <= secondQuote) {
                            firstQuote = i;
                        }
                        else {
                            secondQuote = i;
                        }
                    }
                    else if (challenge[i]=='=' && firstQuote<=secondQuote && offset<0) {
                        offset = i;
                    }
                    else if (challenge[i]==',' && firstQuote<=secondQuote) {
                        return i;
                    }
                }
            }

            return -1;
        }



    }; // class AuthenticationManager

    //
    // This internal class implements a data structure which can be
    // used for storing a set of objects keyed by string prefixes
    // Looking up an object given a string returns the value associated
    // with the longest matching prefix
    // (A prefix "matches" a string IFF the string starts with that prefix
    // The degree of the match is prefix length)
    //
    internal class PrefixLookup {
        //
        // our prefix store (a Hashtable) needs to support multiple readers and multiple writers.
        // the documentation on Hashtable says:
        // "A Hashtable can safely support one writer and multiple readers concurrently.
        // To support multiple writers, all operations must be done through the wrapper
        // returned by the Synchronized method."
        // it's safe enough, for our use, to just synchronize (with a call to lock()) all write operations
        // so we always fall in the supported "one writer and multiple readers" scenario.
        //
        private Hashtable m_Store = new Hashtable();

        internal void Add(string prefix, object value) {
            // Hashtable will overwrite existing key
            lock (m_Store) {
                // writers are locked
                m_Store[prefix] = value;
            }
        }

        /*
        internal void Remove(string prefix) {
            // Hashtable will be unchanged if key is not existing
            lock (m_Store) {
                // writers are locked
                m_Store.Remove(prefix);
            }
        }
        */

        internal object Lookup(string lookupKey) {
            if (lookupKey==null) {
                return null;
            }
            object mostSpecificMatch = null;
            int longestMatchPrefix = 0;
            int prefixLen;
            lock (m_Store) {
                //
                // readers don't need to be locked, but we lock() because:
                // "The enumerator does not have exclusive access to the collection.
                //
                // When an enumerator is instantiated, it takes a snapshot of the current state
                // of the collection. If changes are made to the collection, such as adding,
                // modifying or deleting elements, the snapshot gets out of sync and the
                // enumerator throws an InvalidOperationException. Two enumerators instantiated
                // from the same collection at the same time can have different snapshots of the
                // collection."
                //
                // enumerate through every credential in the cache
                //
                string prefix;
                foreach (DictionaryEntry entry in m_Store) {
                    prefix = (string)entry.Key;
                    if (lookupKey.StartsWith(prefix)) {
                        prefixLen = prefix.Length;
                        //
                        // check if the match is better than the current-most-specific match
                        //
                        if (prefixLen>longestMatchPrefix) {
                            //
                            // Yes-- update the information about currently preferred match
                            //
                            longestMatchPrefix = prefixLen;
                            mostSpecificMatch = entry.Value;
                        }
                    }
                }
            }
            return mostSpecificMatch;
        }

/*
    //
    // This is the core implementation of a general prefix string table
    // The keys are all converted to lowercase
    //
    internal struct PrefixTable: ICollection {

        internal enum Action {
            Find,
            Insert,
            Remove
        }

        private DictionaryEntry[]  m_Entries;

        internal object this[string key] {
            get {
                DictionaryEntry[] local = m_Entries;
                int idx = BinaryFindInsertRemove(ref local, key, Action.Find);
                if (idx < 0) {
                    return null;
                }
                return local[idx].Value;
            }
        }

        internal void Set(string key, object value) {
            DictionaryEntry[] local = m_Entries;
            int idx = BinaryFindInsertRemove(ref local, key, Action.Insert);
            if (idx < 0) {
                //need to add a new entry
                local[~idx].Key   = key;
                local[~idx].Value = value;
                m_Entries = local;
            }
            else {
                local[idx].Value  = value;
            }
        }

        internal void Remove(string key) {
            DictionaryEntry[] local = m_Entries;
            int idx = BinaryFindInsertRemove(ref local, key, Action.Remove);
            if (idx >= 0) {
                //found and removed, update entries.
                m_Entries = local;
            }
        }


        //
        // ICollection interfaces
        //

        int ICollection.Count {
            get {
                DictionaryEntry[] local = m_Entries;
                return local == null? 0: local.Length;
            }
        }

        public bool IsSynchronized {
            get {return true;}
        }

        public IEnumerator GetEnumerator() {
            return new DictionaryEnumerator(m_Entries);
        }

        object ICollection.SyncRoot {
            get {return this;}
        }

        void ICollection.CopyTo (Array array, int index) {
            DictionaryEntry[] local = m_Entries;
            if (local == null) {
                return;
            }
            local.CopyTo(array, index);
        }

        //
        // Binary search and performing an action on a sorted array of prefixes
        // Derived from Array code, adopted for multithreading and substring search.
        //
        // For Find returns either the matching prefix index or -1
        // For Insert returns either the matching index or ~pos for the first larger prefix in which case the slot is allocated.
        // For Remove returns -1 if not found or pos of successfully removed slot
        //
        private unsafe static int BinaryFindInsertRemove(ref DictionaryEntry[] entries, string key, Action action) {

            if (entries == null || entries.Length == 0) {
                if (action != Action.Insert) {
                    return -1;
                }
                entries = new DictionaryEntry[1];
                return ~0;
            }

            int lo = 0;
            int hi = entries.Length-1;

            int lastPrefix = -1;
            fixed (char* pKey = key) {
                while (lo <= hi) {
                    int i = (lo + hi) >> 1;
                    int c = ComparePrefix((string)entries[i].Key, pKey, key.Length);
                    if (c < 0) {
                        if (c < -1) {
                            lastPrefix = i;
                        }
                        lo = i + 1;
                    }
                    else if (c > 0) {
                        hi = i - 1;
                    }
                    else {
                        //Exact match
                        if (action == Action.Remove) {
                            DictionaryEntry[] newEntries = new DictionaryEntry[entries.Length-1];
                            if (i != 0) {
                                Array.Copy(entries, 0, newEntries, 0, i);
                            }
                            if (i != entries.Length-1) {
                                Array.Copy(entries, i+1, newEntries, i, entries.Length-i-1);
                            }
                            entries = newEntries;
                        }
                        return i;
                    }
                }
            }

            // Here 'lo' points to the closest "larger" key, i.e. the place to insert an new element to.
            // The lastPrefix if not -1, will point to the closest prefix match if found.

            switch (action) {
            case Action.Remove:
                            return -1;
            case Action.Insert:
                            DictionaryEntry[] newEntries = new DictionaryEntry[entries.Length+1];
                            if (lo != 0) {
                                Array.Copy(entries, 0, newEntries, 0, lo);
                            }
                            if (lo != entries.Length) {
                                Array.Copy(entries, lo, newEntries, lo+1, entries.Length-lo);
                            }
                            entries = newEntries;
                            break;
            default:
                    // Assuming Find.
                    if (lastPrefix >= 0) {
                        return lastPrefix;
                    }
                    return -1;
            }
            return ~lo;
        }


        //
        // Performs bitwise string comparison assuming that both parameteres are in lower case
        // +1  prefix is greater
        // >+1 prefix is greater but only because the prefix length is greater
        // 0   equal
        // -1  prefix is less
        // <-1 prefix is less but is THE prefix for the key
        private unsafe static int ComparePrefix(string prefix, char* key, int keyLength) {
            int len = prefix.Length < keyLength? prefix.Length: keyLength;

            fixed (char* pPrefix = prefix) {
                for (int i =0; i < len; ++i) {
                    if (pPrefix[i] == key[i]) {
                        continue;
                    }
                    else if (pPrefix[i] > key[i]) {
                        return 1;
                    }
                    else {
                        return -1;
                    }
                }
            }
            return (len == keyLength)
                        ? prefix.Length == keyLength ? 0: 8
                        : -8;
        }
    }

    internal class DictionaryEnumerator: IDictionaryEnumerator {
        DictionaryEntry[]   m_Entries;
        int                 m_CurIdx;

        internal DictionaryEnumerator(DictionaryEntry[] entries) {
            if (entries == null) {
                entries = new DictionaryEntry[0];
            }
            m_Entries = entries;
            m_CurIdx = -1;
        }

        public Object Current {
            get {
                if (m_CurIdx < 0 || m_CurIdx >= m_Entries.Length) {
                    throw new InvalidOperationException("index");
                }
                return m_Entries[m_CurIdx];
            }
        }

        public void Reset() {
            m_CurIdx = -1;
        }

        public bool MoveNext() {
            if (m_CurIdx >= m_Entries.Length) {
                return false;
            }
            ++m_CurIdx;
            return m_CurIdx < m_Entries.Length;
        }

        public DictionaryEntry Entry {
            get {
                if (m_CurIdx < 0 || m_CurIdx >= m_Entries.Length) {
                    throw new InvalidOperationException("index");
                }
                return m_Entries[m_CurIdx];
            }
        }

        public object Key {
            get {
                if (m_CurIdx < 0 || m_CurIdx >= m_Entries.Length) {
                    throw new InvalidOperationException("index");
                }
                return m_Entries[m_CurIdx].Key;
            }
        }

        public object Value {
            get {
                if (m_CurIdx < 0 || m_CurIdx >= m_Entries.Length) {
                    throw new InvalidOperationException("index");
                }
                return m_Entries[m_CurIdx].Value;
            }
        }
    }

*/

    } // class PrefixLookup


} // namespace System.Net

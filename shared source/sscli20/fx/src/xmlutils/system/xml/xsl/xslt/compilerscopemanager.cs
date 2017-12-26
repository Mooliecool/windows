//------------------------------------------------------------------------------
// <copyright file="CompilerScopeManager.cs" company="Microsoft">
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

using System.Collections;
using System.Diagnostics;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Xslt {

    // Compiler scope manager keeps track of
    //   Variable declarations
    //   Namespace declarations
    //   Extension and excluded namespaces
    internal sealed class CompilerScopeManager<V> : IEnumerable
        where V : class
    {
        public struct ScopeRecord {
            public int    scopeCount;
            public string ncName;     // local-name for variable, prefix for namespace, null for extension or excluded namespace
            public string nsUri;      // namespace uri
            public V      value;      // value for variable, null for namespace

            // Exactly one of these three properties is true for every given record
            public bool IsVariable      { get { return value != default(V); } }
            public bool IsNamespace     { get { return value == default(V) && ncName != null; } }
            public bool IsExNamespace   { get { return value == default(V) && ncName == null; } }
        }

        // Number of predefined records minus one
        private const int       LastPredefRecord = 0;

        private ScopeRecord[]   records = new ScopeRecord[32];
        private int             lastRecord = 0;

        // This is cash of records[lastRecord].scopeCount field;
        // most often we will have PushScope()/PopScope pare over the same record.
        // It has sence to avoid adresing this field through array access.
        private int             lastScopes = 0;

        public CompilerScopeManager() {
            Reset();
        }

        private void Reset() {
            // The prefix 'xml' is by definition bound to the namespace name http://www.w3.org/XML/1998/namespace
            records[0].ncName = "xml";
            records[0].nsUri  = XmlReservedNs.NsXml;
            lastRecord = LastPredefRecord;
        }

        public void PushScope() {
            lastScopes++;
        }

        public void PopScope() {
            if (0 < lastScopes) {
                lastScopes--;
            } else {
                while (records[--lastRecord].scopeCount == 0) {
                }
                lastScopes = records[lastRecord].scopeCount;
                lastScopes--;
            }
        }

        private void AddRecord(string ncName, string uri, V value) {
            Debug.Assert(uri != null);

            records[lastRecord].scopeCount = lastScopes;
            if (++lastRecord == records.Length) {
                ScopeRecord[] newRecords = new ScopeRecord[lastRecord * 2];
                Array.Copy(records, 0, newRecords, 0, lastRecord);
                records = newRecords;
            }
            lastScopes = 0;

            records[lastRecord].ncName  = ncName;
            records[lastRecord].nsUri   = uri;
            records[lastRecord].value   = value;
        }

        // Add namespace declaration (prefix != null) or namespace extension or exclusion (prefix == null) to the current scope.
        public void AddNamespace(string prefix, string uri) {
            AddRecord(prefix, uri, default(V));
        }

        // Add variable to the current scope.  Returns false in case of duplicates.
        public void AddVariable(QilName varName, V value) {
            Debug.Assert(varName.LocalName != null && varName.NamespaceUri != null && value != default(V));
            AddRecord(varName.LocalName, varName.NamespaceUri, value);
        }

        // Since the prefix might be redefined in an inner scope, we search in descending order in [to, from]
        // If interval is empty (from < to), the function returns null.
        private string LookupNamespace(string prefix, int from, int to) {
            Debug.Assert(prefix != null);
            for (int record = from; to <= record; --record) {
                if (
                    records[record].IsNamespace &&
                    records[record].ncName == prefix
                ) {
                    return records[record].nsUri;
                }
            }
            return null;
        }

        public string LookupNamespace(string prefix) {
            return LookupNamespace(prefix, lastRecord, 0);
        }

        public bool IsExNamespace(string nsUri) {
            Debug.Assert(nsUri != null);
            for (int record = lastRecord; 0 <= record; record--) {
                if (
                    records[record].IsExNamespace &&
                    records[record].nsUri == nsUri
                ) {
                    return true;
                }
            }
            return false;
        }

        private int SearchVariable(string localName, string uri) {
            Debug.Assert(localName != null);
            for (int record = lastRecord; 0 <= record; --record) {
                if (
                    records[record].IsVariable &&
                    records[record].ncName == localName &&
                    records[record].nsUri  == uri
                ) {
                    return record;
                }
            }
            return -1;
        }

        public V LookupVariable(string localName, string uri) {
            int record = SearchVariable(localName, uri);
            return (record < 0) ? default(V) : records[record].value;
        }

        public bool IsLocalVariable(string localName, string uri) {
            int record = SearchVariable(localName, uri);
            while (0 <= --record) {
                if (records[record].scopeCount != 0) {
                    return true;
                }
            }
            return false;
        }

        IEnumerator IEnumerable.GetEnumerator() {
            return new NamespaceEnumerator(this);
        }

        // Class for enumerating namespaces that must be output with literal result element.
        private sealed class NamespaceEnumerator : IEnumerator {
            CompilerScopeManager<V> scope;
            int                     lastRecord;
            int                     currentRecord;

            public NamespaceEnumerator(CompilerScopeManager<V> scope) {
                this.scope      = scope;
                this.lastRecord = scope.lastRecord;
                Reset();
            }

            public void Reset() {
                currentRecord = lastRecord + 1;
            }

            public bool MoveNext() {
                while (LastPredefRecord < --currentRecord) {
                    if (scope.records[currentRecord].IsNamespace) {
                        // This is a namespace declaration
                        if (scope.LookupNamespace(scope.records[currentRecord].ncName, lastRecord, currentRecord + 1) == null) {
                            // Its prefix has not been redefined later in [currentRecord + 1, lastRecord]
                            return true;
                        }
                    }
                }
                return false;
            }

            public object Current {
                get {
                    Debug.Assert(LastPredefRecord <= currentRecord && currentRecord <= scope.lastRecord, "MoveNext() either was not called or returned false");
                    Debug.Assert(scope.records[currentRecord].IsNamespace);
                    return scope.records[currentRecord];
                }
            }
        }
    }
}

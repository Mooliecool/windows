//------------------------------------------------------------------------------
// <copyright file="OutputScopeManager.cs" company="Microsoft">
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
using System;
using System.Diagnostics;
using System.Xml;
using System.Collections;

namespace System.Xml.Xsl.Xslt {
    internal class OutputScopeManager {
        public struct ScopeReord {
            public int        scopeCount;
            public string     prefix;
            public string     nsUri;
        }
        ScopeReord[] records = new ScopeReord[32];
        int lastRecord = 0;
        int lastScopes = 0;  // This is cash of records[lastRecord].scopeCount field;
                             // most often we will have PushScope()/PopScope pare over the same record.
                             // It has sence to avoid adresing this field through array access.

        public OutputScopeManager() {
            Reset();
        }

        public void Reset() {
//          AddNamespace(null, null);                  --  lookup barier
            records[0].prefix = null;
            records[0].nsUri  = null;
            PushScope();
        }

        public void PushScope() {
            lastScopes ++;
        }

        public void PopScope() {
            if (0 < lastScopes) {
                lastScopes --;
            }
            else {
                while(records[-- lastRecord].scopeCount == 0) ;
                lastScopes = records[lastRecord].scopeCount;
                lastScopes --;
            }
        }

        // This can be ns declaration or ns exclussion. Las one when prefix == null;
        public void AddNamespace(string prefix, string uri) {
            Debug.Assert(prefix != null);
            Debug.Assert(uri    != null);
//            uri = nameTable.Add(uri);
            AddRecord(prefix, uri);
        }

        private void AddRecord(string prefix, string uri) {
            records[lastRecord].scopeCount = lastScopes;
            lastRecord ++;
            if (lastRecord == records.Length) {
                ScopeReord[] newRecords = new ScopeReord[lastRecord * 2];
                Array.Copy(records, 0, newRecords, 0, lastRecord);
                records = newRecords;
            }
            lastScopes = 0;
            records[lastRecord].prefix = prefix;
            records[lastRecord].nsUri  = uri;
        }
        
        //

        
        public void InvalidateAllPrefixes() {
            if (records[lastRecord].prefix == null) {
                return;                            // Averything was invalidated already. Nothing to do.
            }
            AddRecord(null, null);            
        }
        
        public void InvalidateNonDefaultPrefixes() {
            string defaultNs = LookupNamespace(string.Empty);
            if (defaultNs == null) {             // We don't know default NS anyway.
                InvalidateAllPrefixes();
            }
            else {
                if (
                    records[lastRecord    ].prefix.Length == 0 &&
                    records[lastRecord - 1].prefix == null
                ) {
                    return;                       // Averything was already done
                }
                AddRecord(null, null);
                AddRecord(string.Empty, defaultNs);
            }
        }

        public string LookupNamespace(string prefix) {
            Debug.Assert(prefix != null);
            for (
                int record = lastRecord;              // from last record 
                 records[record].prefix != null;      // till lookup barrier
                -- record                             // in reverce direction
            ) {
                Debug.Assert(0 < record, "first record is lookup bariaer, so we don't need to check this condition runtime");
                if (records[record].prefix == prefix) {
                    return records[record].nsUri;
                }
            }
            return null;
        }
    }
}

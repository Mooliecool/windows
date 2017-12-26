//------------------------------------------------------------------------------
// <copyright file="_ProxyRegBlob.cs" company="Microsoft">
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
    using System;
    using System.Security.Permissions;
    using System.Globalization;
    using System.Text;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Net.Sockets;
    using System.Threading;
    using System.Runtime.InteropServices;
    using Microsoft.Win32;
    using System.Runtime.Versioning;

    internal class ProxyRegBlob {

        private static string ReadConfigString(string ConfigName) {
            const int parameterValueLength = 255;
            StringBuilder parameterValue = new StringBuilder(parameterValueLength);
            bool rc = UnsafeNclNativeMethods.FetchConfigurationString(true, ConfigName, parameterValue, parameterValueLength);
            if (rc) {
                return parameterValue.ToString();
            }
            return "";
        }

        //
        // Parses out a string from IE and turns it into a URI
        //
        private static Uri ParseProxyUri(string proxyString, bool validate) {
            if (validate) {
                if (proxyString.Length == 0) {
                    return null;
                }
                if (proxyString.IndexOf('=') != -1) {
                    return null;
                }
            }
            if (proxyString.IndexOf("://") == -1) {
                proxyString = "http://" + proxyString;
            }
            return new Uri(proxyString);
        }

        //
        // Builds a hashtable containing the protocol and proxy URI to use for it.
        //
        private static Hashtable ParseProtocolProxies(string proxyListString) {
            if (proxyListString.Length == 0) {
                return null;
            }
            // parse something like "http=http://http-proxy;https=http://https-proxy;ftp=http://ftp-proxy"
            char[] splitChars = new char[] { ';', '=' };
            string[] proxyListStrings = proxyListString.Split(splitChars);
            bool protocolPass = true;
            string protocolString = null;
            Hashtable proxyListHashTable = new Hashtable(CaseInsensitiveAscii.StaticInstance);
            foreach (string elementString in proxyListStrings) {
                string elementString2 = elementString.Trim().ToLower(CultureInfo.InvariantCulture);
                if (protocolPass) {
                    protocolString = elementString2;
                }
                else {
                    proxyListHashTable[protocolString] = ParseProxyUri(elementString2, false);
                }
                protocolPass = !protocolPass;
            }
            if (proxyListHashTable.Count == 0) {
                return null;
            }
            return proxyListHashTable;
        }

        //
        // Converts a simple IE regular expresion string into one
        //  that is compatible with Regex escape sequences.
        //
        private static string BypassStringEscape(string bypassString) {
            StringBuilder escapedBypass = new StringBuilder();
            // (\, *, +, ?, |, {, [, (,), ^, $, ., #, and whitespace) are reserved
            foreach (char c in bypassString){
                if (c == '\\' || c == '.' || c == '?' || c == '(' || c == ')' || c == '|' || c == '^' || c == '+' ||
                    c == '{' || c == '[' || c == '$' || c == '#')
                {
                    escapedBypass.Append('\\');
                }
                else if (c == '*') {
                    escapedBypass.Append('.');
                }
                escapedBypass.Append(c);
            }
            escapedBypass.Append('$');
            return escapedBypass.ToString();
        }


        //
        // Parses out a string of bypass list entries and coverts it to Regex's that can be used
        //   to match against.
        //
        private static ArrayList ParseBypassList(string bypassListString, out bool bypassOnLocal) {
            char[] splitChars = new char[] {';'};
            string[] bypassListStrings = bypassListString.Split(splitChars);
            bypassOnLocal = false;
            if (bypassListStrings.Length == 0) {
                return null;
            }
            ArrayList bypassList = null;
            foreach (string bypassString in bypassListStrings) {
                if (bypassString!=null) {
                    string bypassString2 = bypassString.Trim();
                    if (bypassString2.Length>0) {
                        if (string.Compare(bypassString2, "<local>", StringComparison.OrdinalIgnoreCase)==0) {
                            bypassOnLocal = true;
                        }
                        else {
                            bypassString2 = BypassStringEscape(bypassString2);
                            if (bypassList==null) {
                                bypassList = new ArrayList();
                            }
                            GlobalLog.Print("ProxyRegBlob::ParseBypassList() bypassList.Count:" + bypassList.Count + " adding:" + ValidationHelper.ToString(bypassString2));
                            if (!bypassList.Contains(bypassString2)) {
                                bypassList.Add(bypassString2);
                                GlobalLog.Print("ProxyRegBlob::ParseBypassList() bypassList.Count:" + bypassList.Count + " added:" + ValidationHelper.ToString(bypassString2));
                            }
                        }
                    }
                }
            }
            return bypassList;
        }

        //
        // Updates an instance of WbeProxy with the proxy settings from IE for:
        // the current user and a given connectoid.
        //
        [ResourceExposure(ResourceScope.Machine)]  // Check scoping on this SafeRegistryHandle
        [ResourceConsumption(ResourceScope.Machine)]
        internal static WebProxyData GetWebProxyData(string connectoid)
        {
            GlobalLog.Print("ProxyRegBlob::GetWebProxyData() connectoid:" + ValidationHelper.ToString(connectoid));
            WebProxyData webProxyData = new WebProxyData();
            Hashtable proxyHashTable = null;
            Uri address = null;

            string proxyAddressString = ReadConfigString("ProxyUri");
            string proxyBypassString = ReadConfigString("ProxyBypass");
            try {
                address = ParseProxyUri(proxyAddressString, true);
                if ( address == null ) {
                    proxyHashTable = ParseProtocolProxies(proxyAddressString);
                }
                if ((address != null || proxyHashTable != null) && proxyBypassString != null ) {
                    webProxyData.bypassList = ParseBypassList(proxyBypassString, out webProxyData.bypassOnLocal);
                }
                // success if we reach here
            }
            catch {
            }

            if (proxyHashTable!=null) {
                address = proxyHashTable["http"] as Uri;
            }
            webProxyData.proxyAddress = address;

            return webProxyData;
        }
    
    }
}

//------------------------------------------------------------------------------
// <copyright file="_emptywebproxy.cs" company="Microsoft">
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

namespace System.Net
{
    using System.Collections.Generic;

    [Serializable]
    internal sealed class EmptyWebProxy : IAutoWebProxy {

        [NonSerialized]
        private ICredentials m_credentials;

        public EmptyWebProxy() {
        }

        //
        // IWebProxy interface
        //
        public Uri GetProxy(Uri uri) {
            // this method won't get called by NetClasses because of the IsBypassed test below
            return uri; 
        }
        public bool IsBypassed(Uri uri) {
            return true; // no proxy, always bypasses
        }
        public ICredentials Credentials {
            get {
                return m_credentials;
            }
            set {
                m_credentials = value; // doesn't do anything, but doesn't break contract either
            }
        }

        //
        // IAutoWebProxy interface
        //
        ProxyChain IAutoWebProxy.GetProxies(Uri destination)
        {
            return new DirectProxy(destination);
        }
    }
}

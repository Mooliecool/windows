//------------------------------------------------------------------------------
// <copyright file="GlobalProxySelection.cs" company="Microsoft">
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
    [Obsolete("This class has been deprecated. Please use WebRequest.DefaultWebProxy instead to access and set the global default proxy. Use 'null' instead of GetEmptyWebProxy. http://go.microsoft.com/fwlink/?linkid=14202")]
    public class GlobalProxySelection
    {
        public static IWebProxy Select
        {
            get
            {
                IWebProxy proxy = WebRequest.DefaultWebProxy;
                if (proxy == null)
                {
                    return GetEmptyWebProxy();
                }
                WebRequest.WebProxyWrapper wrap = proxy as WebRequest.WebProxyWrapper;
                if (wrap != null)
                {
                    return wrap.WebProxy;
                }
                return proxy;
            }

            set
            {
                WebRequest.DefaultWebProxy = value;
            }
        }

        public static IWebProxy GetEmptyWebProxy() {
            return new EmptyWebProxy();
        }
    }
}

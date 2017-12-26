//------------------------------------------------------------------------------
// <copyright file="EmptyImpersonationContext.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Configuration.Internal;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Security;
    using System.Text;
    using System.Xml;

    // An impersonation context that does nothing
    // Used in cases where the Host does not require impersonation.
    class EmptyImpersonationContext : IDisposable {
        static IDisposable s_emptyImpersonationContext;

        static internal IDisposable GetStaticInstance() {
            if (s_emptyImpersonationContext == null) {
                s_emptyImpersonationContext = new EmptyImpersonationContext();
            }

            return s_emptyImpersonationContext;
        }

        public void Dispose() {}
    }
}

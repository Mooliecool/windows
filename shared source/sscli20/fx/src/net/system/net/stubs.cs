//------------------------------------------------------------------------------
// <copyright file="stubs.cs" company="Microsoft">
//      
//       Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//      
//       The use and distribution terms for this software are contained in the file
//       named license.txt, which can be found in the root of this distribution.
//       By using this software in any fashion, you are agreeing to be bound by the
//       terms of this license.
//      
//       You must not remove this notice, or any other, from this software.
//      
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net.NetworkInformation
{

    internal abstract class IPGlobalProperties
    {
        internal static IPGlobalProperties InternalGetIPGlobalProperties()
        {
            return new SystemIPGlobalProperties();
        }

        public abstract string DomainName { get;}
    }

    internal class SystemIPGlobalProperties : IPGlobalProperties
    {
        public override string DomainName
        {
            get
            {
                return NclUtilities._LocalDomainName;
            }
        }
    }

}

namespace System.Security.Cryptography.X509Certificates {

    internal class X509Certificate {
    }

}

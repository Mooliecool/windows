//------------------------------------------------------------------------------
// <copyright file="ProtectedConfigurationProviderCollection.cs" company="Microsoft">
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

namespace System.Configuration
{
    using System.Collections.Specialized;
    using System.Runtime.Serialization;
    using System.Configuration.Provider;
    using System.Xml;

    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    public class ProtectedConfigurationProviderCollection : ProviderCollection
    {
        public override void Add(ProviderBase provider)
        {
            if( provider == null )
            {
                throw new ArgumentNullException( "provider" );
            }

            if( !( provider is ProtectedConfigurationProvider ) )
           {
                throw new ArgumentException(SR.GetString(SR.Config_provider_must_implement_type, typeof(ProtectedConfigurationProvider).ToString()), "provider");
           }

            base.Add( provider );
        }

        new public ProtectedConfigurationProvider this[string name]
        {
            get
            {
                return (ProtectedConfigurationProvider)base[name];
            }
        }
    }
}

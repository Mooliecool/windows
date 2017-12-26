//------------------------------------------------------------------------------
// <copyright file="ProtectedConfiguration.cs" company="Microsoft">
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
    using System.Collections;
    using System.Collections.Specialized;
    using System.Runtime.Serialization;
    using System.Configuration.Provider;
    using System.Xml;
    using System.Security.Permissions;

    [PermissionSet(SecurityAction.LinkDemand, Name = "FullTrust")]
    public static class ProtectedConfiguration
    {
        public static ProtectedConfigurationProviderCollection Providers
        {
            get
            {
                ProtectedConfigurationSection config = PrivilegedConfigurationManager.GetSection(BaseConfigurationRecord.RESERVED_SECTION_PROTECTED_CONFIGURATION) as ProtectedConfigurationSection;
                if (config == null)
                    return new ProtectedConfigurationProviderCollection();

                return config.GetAllProviders();
            }
        }

        public const string RsaProviderName = "RsaProtectedConfigurationProvider";
        public const string DataProtectionProviderName = "DataProtectionConfigurationProvider";
        public const string ProtectedDataSectionName = BaseConfigurationRecord.RESERVED_SECTION_PROTECTED_CONFIGURATION;

        public static string DefaultProvider {
            get {
                ProtectedConfigurationSection config = PrivilegedConfigurationManager.GetSection(BaseConfigurationRecord.RESERVED_SECTION_PROTECTED_CONFIGURATION) as ProtectedConfigurationSection;
                if (config != null)
                    return config.DefaultProvider;

                return "";
            }
        }


        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
#if CACHE_PROVIDERS_IN_STATIC
        private static void InstantiateProviders()
        {
            if (_Providers != null)
                return;

            lock (_Lock)
            {
                if (_Providers != null)
                    return;

                ProtectedConfigurationProviderCollection providers = new ProtectedConfigurationProviderCollection();
                ProtectedConfigurationSection config = PrivilegedConfigurationManager.GetSection(BaseConfigurationRecord.RESERVED_SECTION_PROTECTED_CONFIGURATION) as ProtectedConfigurationSection;

                if (config != null)
                {
                    foreach (DictionaryEntry de in config.ProviderNodes)
                    {
                        ProviderNode pn = de.Value as ProviderNode;

                        if (pn == null)
                            continue;

                        providers.Add(pn.Provider);
                    }
                }

                _Providers = providers;
            }
        }
        private static object _Lock = new object();
        private static ProtectedConfigurationProviderCollection _Providers = null;
#endif

    }
}

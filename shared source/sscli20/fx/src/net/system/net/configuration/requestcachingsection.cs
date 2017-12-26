//------------------------------------------------------------------------------
// <copyright file="RequestCachingSection.cs" company="Microsoft Corporation">
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

namespace System.Net.Configuration
{
    using Microsoft.Win32;
    using System.Configuration;
    using System.Globalization;
    using System.Net.Cache;
    using System.Threading;

    public sealed class RequestCachingSection : ConfigurationSection
    {
        public RequestCachingSection()
        {
            this.properties.Add(this.disableAllCaching);
            this.properties.Add(this.defaultPolicyLevel);
            this.properties.Add(this.isPrivateCache);
            this.properties.Add(this.defaultHttpCachePolicy);
            this.properties.Add(this.defaultFtpCachePolicy);
            this.properties.Add(this.unspecifiedMaximumAge);
        }

        [ConfigurationProperty(ConfigurationStrings.DefaultHttpCachePolicy)]
        public HttpCachePolicyElement DefaultHttpCachePolicy
        {
            get { return (HttpCachePolicyElement)this[this.defaultHttpCachePolicy]; }
        }

        [ConfigurationProperty(ConfigurationStrings.DefaultFtpCachePolicy)]
        public FtpCachePolicyElement DefaultFtpCachePolicy
        {
            get { return (FtpCachePolicyElement)this[this.defaultFtpCachePolicy]; }
        }

        [ConfigurationProperty(ConfigurationStrings.DefaultPolicyLevel, DefaultValue=(RequestCacheLevel) RequestCacheLevel.BypassCache)]
        public RequestCacheLevel DefaultPolicyLevel
        {
            get { return (RequestCacheLevel)this[this.defaultPolicyLevel]; }
            set { this[this.defaultPolicyLevel] = value; }
        }

        [ConfigurationProperty(ConfigurationStrings.DisableAllCaching, DefaultValue=true)]
        public bool DisableAllCaching
        {
            get { return (bool)this[this.disableAllCaching]; }
            set { this[this.disableAllCaching] = value; }
        }

        [ConfigurationProperty(ConfigurationStrings.IsPrivateCache, DefaultValue=true)]
        public bool IsPrivateCache
        {
            get { return (bool)this[this.isPrivateCache]; }
            set { this[this.isPrivateCache] = value; }
        }

        [ConfigurationProperty(ConfigurationStrings.UnspecifiedMaximumAge, DefaultValue = "1.00:00:00")]
        public TimeSpan UnspecifiedMaximumAge
        {
            get { return (TimeSpan)this[this.unspecifiedMaximumAge]; }
            set { this[this.unspecifiedMaximumAge] = value; }
        }

        //
        // If DisableAllCaching is set once to true it will not change.
        //
        protected override void DeserializeElement(System.Xml.XmlReader reader, bool serializeCollectionKey)
        {

            bool tempDisableAllCaching = this.DisableAllCaching;

            base.DeserializeElement(reader, serializeCollectionKey);
            if (tempDisableAllCaching)
            {
                this.DisableAllCaching = true;
            }
        }

        protected override void PostDeserialize()
        {
            // Perf optimization. If the configuration is coming from machine.config
            // It is safe and we don't need to check for permissions.
            if (EvaluationContext.IsMachineLevel)
                return;

            try {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
            } catch (Exception exception) {
                throw new ConfigurationErrorsException(
                              SR.GetString(SR.net_config_section_permission, 
                                           ConfigurationStrings.RequestCachingSectionName),
                              exception);
            }
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get { return this.properties; }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty defaultHttpCachePolicy =
            new ConfigurationProperty(ConfigurationStrings.DefaultHttpCachePolicy, 
                                      typeof(HttpCachePolicyElement), 
                                      null,
                                      ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty defaultFtpCachePolicy =
            new ConfigurationProperty(ConfigurationStrings.DefaultFtpCachePolicy, 
                                      typeof(FtpCachePolicyElement), 
                                      null,
                                      ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty defaultPolicyLevel =
            new ConfigurationProperty(ConfigurationStrings.DefaultPolicyLevel, 
                                      typeof(RequestCacheLevel), 
                                      RequestCacheLevel.BypassCache,
                                      ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty disableAllCaching =
            new ConfigurationProperty(ConfigurationStrings.DisableAllCaching, 
                                      typeof(bool), 
                                      true,
                                      ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty isPrivateCache =
            new ConfigurationProperty(ConfigurationStrings.IsPrivateCache, 
                                      typeof(bool), 
                                      true,
                                      ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty unspecifiedMaximumAge =
            new ConfigurationProperty(ConfigurationStrings.UnspecifiedMaximumAge, 
                                      typeof(TimeSpan), 
                                      TimeSpan.FromDays(1),
                                      ConfigurationPropertyOptions.None);
    }

    internal sealed class RequestCachingSectionInternal
    {

        private RequestCachingSectionInternal() { }

        internal RequestCachingSectionInternal(RequestCachingSection section)
        {
            this.httpRequestCacheValidator = new HttpRequestCacheValidator(false, this.UnspecifiedMaximumAge);
            this.disableAllCaching = true;

            FtpCachePolicyElement ftpPolicy = section.DefaultFtpCachePolicy;

            if (ftpPolicy.WasReadFromConfig)
            {
                this.defaultFtpCachePolicy = new RequestCachePolicy(ftpPolicy.PolicyLevel);
            }

        }

        internal static object ClassSyncObject
        {
            get
            {
                if (classSyncObject == null)
                {
                    object o = new object();
                    Interlocked.CompareExchange(ref classSyncObject, o, null);
                }
                return classSyncObject;
            }
        }

        internal bool DisableAllCaching
        {
            get { return this.disableAllCaching; }
        }

        internal RequestCache DefaultCache
        {
            get { return this.defaultCache; }
        }

        internal RequestCachePolicy DefaultCachePolicy
        {
            get { return this.defaultCachePolicy; }
        }

        internal bool IsPrivateCache
        {
            get { return this.isPrivateCache; }
        }

        internal TimeSpan UnspecifiedMaximumAge
        {
            get { return this.unspecifiedMaximumAge; }
        }

        internal HttpRequestCachePolicy DefaultHttpCachePolicy
        {
            get { return this.defaultHttpCachePolicy; }
        }

        internal RequestCachePolicy DefaultFtpCachePolicy
        {
            get { return this.defaultFtpCachePolicy; }
        }

        internal HttpRequestCacheValidator DefaultHttpValidator
        {
            get { return this.httpRequestCacheValidator; }
        }

        internal FtpRequestCacheValidator DefaultFtpValidator
        {
            get { return this.ftpRequestCacheValidator; }
        }

        static internal RequestCachingSectionInternal GetSection()
        {
            lock (RequestCachingSectionInternal.ClassSyncObject)
            {
                RequestCachingSection section = PrivilegedConfigurationManager.GetSection(ConfigurationStrings.RequestCachingSectionPath) as RequestCachingSection;
                if (section == null)
                    return null;

                try
                {
                    return new RequestCachingSectionInternal(section);
                }
                catch (Exception exception)
                {
                    if (NclUtilities.IsFatal(exception)) throw;

                    throw new ConfigurationErrorsException(SR.GetString(SR.net_config_requestcaching), exception);
                }
                catch
                {
                    throw new ConfigurationErrorsException(SR.GetString(SR.net_config_requestcaching), new Exception(SR.GetString(SR.net_nonClsCompliantException)));
                }
            }
        }

        static object classSyncObject;
        RequestCache defaultCache;
        HttpRequestCachePolicy defaultHttpCachePolicy;
        RequestCachePolicy defaultFtpCachePolicy;
        RequestCachePolicy defaultCachePolicy;
        bool disableAllCaching;
        HttpRequestCacheValidator httpRequestCacheValidator;
        FtpRequestCacheValidator  ftpRequestCacheValidator;
        bool isPrivateCache;
        TimeSpan unspecifiedMaximumAge;
    }
}

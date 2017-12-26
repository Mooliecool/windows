//------------------------------------------------------------------------------
// <copyright file="ConnectionManagementSection.cs" company="Microsoft Corporation">
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
    using System.Configuration;
    using System.Collections;
    using System.Threading;    
    public sealed class ConnectionManagementSection : ConfigurationSection
    {
        public ConnectionManagementSection() 
        {
            this.properties.Add(this.connectionManagement);
        }

        [ConfigurationProperty("", IsDefaultCollection = true)]
        public ConnectionManagementElementCollection ConnectionManagement
        {
            get { return (ConnectionManagementElementCollection)this[this.connectionManagement]; }
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get { return this.properties; }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty connectionManagement =
            new ConfigurationProperty(null, typeof(ConnectionManagementElementCollection), null,
                    ConfigurationPropertyOptions.IsDefaultCollection);
    }

    internal sealed class ConnectionManagementSectionInternal
    {
        internal ConnectionManagementSectionInternal(ConnectionManagementSection section)
        {
            if (section.ConnectionManagement.Count > 0)
            {
                this.connectionManagement = new Hashtable(section.ConnectionManagement.Count);
                foreach(ConnectionManagementElement connectionManagementElement in section.ConnectionManagement)
                {
                    this.connectionManagement[connectionManagementElement.Address] = connectionManagementElement.MaxConnection;
                }
            }
        }

        internal Hashtable ConnectionManagement
        {
            get 
            {
                Hashtable retval = this.connectionManagement;
                if (retval == null)
                {
                    retval = new Hashtable();
                }
                return retval; 
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

        static internal ConnectionManagementSectionInternal GetSection()
        {
            lock (ConnectionManagementSectionInternal.ClassSyncObject)
            {
                ConnectionManagementSection section = PrivilegedConfigurationManager.GetSection(ConfigurationStrings.ConnectionManagementSectionPath) as ConnectionManagementSection; 
                if (section == null)
                    return null;
                
                return new ConnectionManagementSectionInternal(section);
            }
        }

        Hashtable connectionManagement = null;
        static object classSyncObject = null;
    }
}

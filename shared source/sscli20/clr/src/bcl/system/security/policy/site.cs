// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

//
//  Site.cs
//
//  Site is an IIdentity representing internet sites.
//

namespace System.Security.Policy {
    using System.Security.Util;
    using System.Globalization;
    using SiteIdentityPermission = System.Security.Permissions.SiteIdentityPermission;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class Site : IIdentityPermissionFactory, IBuiltInEvidence
    {
        private SiteString m_name;

        internal Site()
        {
            m_name = null;
        }

        public Site(String name)
        {
            if (name == null)
                throw new ArgumentNullException("name");

            m_name = new SiteString( name );
        }

        internal Site( byte[] id, String name )
        {
            m_name = ParseSiteFromUrl( name );
        }

        public static Site CreateFromUrl( String url )
        {
            Site site = new Site();

            site.m_name = ParseSiteFromUrl( url );

            return site;
        }

        private static SiteString ParseSiteFromUrl( String name )
        {
            URLString urlString = new URLString( name );

            if (String.Compare( urlString.Scheme, "file", StringComparison.OrdinalIgnoreCase) == 0)
                throw new ArgumentException( Environment.GetResourceString( "Argument_InvalidSite" ) );

            return new SiteString( new URLString( name ).Host );
        }

        public String Name
        {
            get
            {
                if (m_name != null)
                    return m_name.ToString();
                else
                    return null;
            }
        }

        internal SiteString GetSiteString()
        {
            return m_name;
        }

        public IPermission CreateIdentityPermission( Evidence evidence )
        {
            return new SiteIdentityPermission( Name );
        }

        public override bool Equals(Object o)
        {
            if (o is Site)
            {
                Site s = (Site) o;
                    if (Name == null)
                        return (s.Name == null);

                return String.Compare( Name, s.Name, StringComparison.OrdinalIgnoreCase) == 0;
            }

            return false;
        }

        public override int GetHashCode()
        {
            String name = this.Name;

            if (name == null)
                return 0;
            else
                return name.GetHashCode();
        }

        public Object Copy()
        {
            return new Site(this.Name);
        }

        internal SecurityElement ToXml()
        {
            SecurityElement elem = new SecurityElement( "System.Security.Policy.Site" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.Site" ), "Class name changed!" );

            elem.AddAttribute( "version", "1" );
            
            if(m_name != null)
                elem.AddChild( new SecurityElement( "Name", m_name.ToString() ) );
                
            return elem;
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            buffer[position++] = BuiltInEvidenceHelper.idSite;
            String name = this.Name;
            int length = name.Length;

            if (verbose)
            {
                BuiltInEvidenceHelper.CopyIntToCharArray(length, buffer, position);
                position += 2;
            }
            name.CopyTo( 0, buffer, position, length );
            return length + position;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose)
        {
            if (verbose)
                return this.Name.Length + 3;
            else
                return this.Name.Length + 1;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position)
        {
            int length = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;

            m_name = new SiteString( new String(buffer, position, length ));

            return position + length;
        }

        public override String ToString()
        {
            return ToXml().ToString();
        }

        // INormalizeForIsolatedStorage is not implemented for startup perf
        // equivalent to INormalizeForIsolatedStorage.Normalize()
        internal Object Normalize()
        {
            return m_name.ToString().ToUpper(CultureInfo.InvariantCulture);
        }
    }
}

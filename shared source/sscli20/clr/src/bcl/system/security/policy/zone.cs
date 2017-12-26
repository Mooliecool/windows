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
//  Zone.cs
//
//  Zone is an IIdentity representing Internet/Intranet/MyComputer etc.
//

namespace System.Security.Policy {
    using System.Security.Util;
    using ZoneIdentityPermission = System.Security.Permissions.ZoneIdentityPermission;
    using System.Runtime.CompilerServices;
    using System.Runtime.Serialization;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class Zone : IIdentityPermissionFactory, IBuiltInEvidence
    {
        [OptionalField(VersionAdded = 2)]
        private String m_url;
        private SecurityZone m_zone;

        private static readonly String[] s_names =
            {"MyComputer", "Intranet", "Trusted", "Internet", "Untrusted", "NoZone"};

        internal Zone() 
        {
            m_url = null;
            m_zone = SecurityZone.NoZone; 
        }

        public Zone(SecurityZone zone)
        {
            if (zone < SecurityZone.NoZone || zone > SecurityZone.Untrusted)
                throw new ArgumentException( Environment.GetResourceString( "Argument_IllegalZone" ) );

            m_url = null;
            m_zone = zone;
        }

        private Zone(String url)
        {
            m_url = url;
            m_zone = SecurityZone.NoZone;
        }

        public static Zone CreateFromUrl( String url )
        {
            if (url == null)
                throw new ArgumentNullException( "url" );

            return new Zone( url );
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static SecurityZone _CreateFromUrl( String url );

        public IPermission CreateIdentityPermission( Evidence evidence )
        {
            return new ZoneIdentityPermission( SecurityZone );
        }

        public SecurityZone SecurityZone
        {
            get
            {
                if (m_url != null)
                    m_zone = _CreateFromUrl( m_url );
                return m_zone;
            }
        }

        public override bool Equals(Object o)
        {
            if (o is Zone)
            {
                Zone z = (Zone) o;
                return SecurityZone == z.SecurityZone;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return (int)SecurityZone;
        }

        public Object Copy()
        {
            Zone z = new Zone();

            z.m_zone = m_zone;
            z.m_url = m_url;

            return z;
        }

        internal SecurityElement ToXml()
        {
            SecurityElement elem = new SecurityElement( "System.Security.Policy.Zone" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.Zone" ), "Class name changed!" );

            elem.AddAttribute( "version", "1" );
            if (SecurityZone != SecurityZone.NoZone)
                elem.AddChild( new SecurityElement( "Zone", s_names[(int)SecurityZone] ) );
            else
                elem.AddChild( new SecurityElement( "Zone", s_names[s_names.Length-1] ) );
            return elem;
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            buffer[position] = BuiltInEvidenceHelper.idZone;
            BuiltInEvidenceHelper.CopyIntToCharArray( (int)SecurityZone, buffer, position + 1 );
            return position + 3;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose)
        {
            return 3;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position )
        {
            m_url = null;
            m_zone = (SecurityZone)BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            return position + 2;
        }

        public override String ToString()
        {
            return ToXml().ToString();
        }

        // INormalizeForIsolatedStorage is not implemented for startup perf
        // equivalent to INormalizeForIsolatedStorage.Normalize()
        internal Object Normalize()
        {
            return s_names[(int)SecurityZone];
        }
    }
}

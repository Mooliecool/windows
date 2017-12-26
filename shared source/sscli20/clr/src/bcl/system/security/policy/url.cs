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
//  Url.cs
//
//  Url is an IIdentity representing url internet sites.
//

namespace System.Security.Policy {
    using System.IO;
    using System.Security.Util;
    using UrlIdentityPermission = System.Security.Permissions.UrlIdentityPermission;
    using System.Runtime.Serialization;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    sealed public class Url : IIdentityPermissionFactory, IBuiltInEvidence
    {
        private URLString m_url;

        internal Url()
        {
            m_url = null;
        }

        internal Url( SerializationInfo info, StreamingContext context )
        {
            m_url = new URLString( (String)info.GetValue( "Url", typeof( String ) ) );
        }

        internal Url( String name, bool parsed )
        {
            if (name == null)
                throw new ArgumentNullException( "name" );

            m_url = new URLString( name, parsed );
        }

        public Url( String name )
        {
            if (name == null)
                throw new ArgumentNullException( "name" );

            m_url = new URLString( name );
        }

        public String Value
        {
            get
            {
                if (m_url == null) return null;
                return m_url.ToString();
            }
        }

        internal URLString GetURLString()
        {
            return m_url;
        }

        public IPermission CreateIdentityPermission( Evidence evidence )
        {
            return new UrlIdentityPermission( m_url );
        }

        public override bool Equals(Object o)
        {
            if (o == null)
                return false;

            if (o is Url)
            {
                Url url = (Url) o;

                if (this.m_url == null)
                {
                    return url.m_url == null;
                }
                else if (url.m_url == null)
                {
                    return false;
                }
                else
                {
                    return this.m_url.Equals( url.m_url );
                }
            }
            return false;
        }

        public override int GetHashCode()
        {
            if (this.m_url == null)
                return 0;
            else
                return this.m_url.GetHashCode();
        }

        public Object Copy()
        {
            Url url = new Url();

            url.m_url = this.m_url;

            return url;
        }

        internal SecurityElement ToXml()
        {
            SecurityElement root = new SecurityElement( "System.Security.Policy.Url" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.Url" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );

            if (m_url != null)
                root.AddChild( new SecurityElement( "Url", m_url.ToString() ) );

            return root;
        }

        public override String ToString()
        {
            return ToXml().ToString();
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            buffer[position++] = BuiltInEvidenceHelper.idUrl;
            String value = this.Value;
            int length = value.Length;
            if (verbose)
            {
                BuiltInEvidenceHelper.CopyIntToCharArray(length, buffer, position);
                position += 2;
            }

            value.CopyTo( 0, buffer, position, length );
            return length + position;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose )
        {
            if (verbose)
                return this.Value.Length + 3;
            else
                return this.Value.Length + 1;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position)
        {
            int length = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;

            m_url = new URLString( new String(buffer, position, length ));

            return position + length;
        }


        // INormalizeForIsolatedStorage is not implemented for startup perf
        // equivalent to INormalizeForIsolatedStorage.Normalize()
        internal Object Normalize()
        {
            return m_url.NormalizeUrl();
        }
    }
}

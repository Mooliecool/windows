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
// StrongName.cs
//
// StrongName is an IIdentity representing strong names.
//

namespace System.Security.Policy {
    using System.IO;
    using System.Security.Util;
    using System.Security.Permissions;
    using CultureInfo = System.Globalization.CultureInfo;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    sealed public class StrongName : IIdentityPermissionFactory, IBuiltInEvidence
    {
        private StrongNamePublicKeyBlob m_publicKeyBlob;
        private String m_name;
        private Version m_version;

        internal StrongName() {}

        public StrongName( StrongNamePublicKeyBlob blob, String name, Version version )
        {
            if (name == null)
                throw new ArgumentNullException( "name" );
            if (name.Equals( "" ))
                throw new ArgumentException( Environment.GetResourceString( "Argument_EmptyStrongName" ) );      

            if (blob == null)
                throw new ArgumentNullException( "blob" );

            if (version == null)
                throw new ArgumentNullException( "version" );

            m_publicKeyBlob = blob;
            m_name = name;
            m_version = version;
        }

        internal StrongName( StrongNamePublicKeyBlob blob, String name, Version version, bool trash )
        {
            m_publicKeyBlob = blob;
            m_name = name;
            m_version = version;
        }

        public StrongNamePublicKeyBlob PublicKey
        {
            get
            {
                return m_publicKeyBlob;
            }
        }

        public String Name
        {
            get
            {
                return m_name;
            }
        }

        public Version Version
        {
            get
            {
                return m_version;
            }
        }

        internal static bool CompareNames( String asmName, String mcName )
        {
            if (mcName.Length > 0 && mcName[mcName.Length-1] == '*' && mcName.Length - 1 <= asmName.Length)
                return String.Compare( mcName, 0, asmName, 0, mcName.Length - 1, StringComparison.OrdinalIgnoreCase) == 0;
            else
                return String.Compare( mcName, asmName, StringComparison.OrdinalIgnoreCase) == 0;
        }

        public IPermission CreateIdentityPermission( Evidence evidence )
        {
            return new StrongNameIdentityPermission( m_publicKeyBlob, m_name, m_version );
        }

        public Object Copy()
        {
            return new StrongName( m_publicKeyBlob, m_name, m_version );
        }

        internal SecurityElement ToXml()
        {
            SecurityElement root = new SecurityElement( "StrongName" );
            root.AddAttribute( "version", "1" );

            if (m_publicKeyBlob != null)
                root.AddAttribute( "Key", System.Security.Util.Hex.EncodeHexString( m_publicKeyBlob.PublicKey ) );

            if (m_name != null)
                root.AddAttribute( "Name", m_name );

            if (m_version != null)
                root.AddAttribute( "Version", m_version.ToString() );

            return root;
        }

        internal void FromXml (SecurityElement element)
        {
            if (element == null)
                throw new ArgumentNullException("element");
            if (String.Compare(element.Tag, "StrongName", StringComparison.Ordinal) != 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidXML"));

            m_publicKeyBlob = null;
            m_version = null;

            string key = element.Attribute("Key");
            if (key != null)
                m_publicKeyBlob = new StrongNamePublicKeyBlob(System.Security.Util.Hex.DecodeHexString(key));

            m_name = element.Attribute("Name");

            string version = element.Attribute("Version");
            if (version != null)
                m_version = new Version(version);
        }

        public override String ToString()
        {
            return ToXml().ToString();
        }

        public override bool Equals( Object o )
        {
            StrongName that = (o as StrongName);
            return (that != null) &&
                   Equals( this.m_publicKeyBlob, that.m_publicKeyBlob ) &&
                   Equals( this.m_name, that.m_name ) &&
                   Equals( this.m_version, that.m_version );
        }

        public override int GetHashCode()
        {
            if (m_publicKeyBlob != null)
            {
                return m_publicKeyBlob.GetHashCode();
            }
            else if (m_name != null || m_version != null)
            {
                return (m_name == null ? 0 : m_name.GetHashCode()) + (m_version == null ? 0 : m_version.GetHashCode());
            }
            else
            {
                return typeof( StrongName ).GetHashCode();
            }
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            // StrongNames have a byte[], a string, and a Version (4 ints).
            // Copy in the id, the byte[], the four ints, and then the string.

            buffer[position++] = BuiltInEvidenceHelper.idStrongName;
            int lengthPK = m_publicKeyBlob.PublicKey.Length;
            if (verbose)
            {
                BuiltInEvidenceHelper.CopyIntToCharArray(lengthPK, buffer, position);
                position += 2;
            }
            Buffer.InternalBlockCopy(m_publicKeyBlob.PublicKey, 0, buffer, position * 2, lengthPK);
            position += ((lengthPK - 1) / 2) + 1;

            BuiltInEvidenceHelper.CopyIntToCharArray( m_version.Major,    buffer, position     );
            BuiltInEvidenceHelper.CopyIntToCharArray( m_version.Minor,    buffer, position + 2 );
            BuiltInEvidenceHelper.CopyIntToCharArray( m_version.Build,    buffer, position + 4 );
            BuiltInEvidenceHelper.CopyIntToCharArray( m_version.Revision, buffer, position + 6 );

            position += 8;
            int lengthName = m_name.Length;
            if (verbose)
            {
                BuiltInEvidenceHelper.CopyIntToCharArray(lengthName, buffer, position);
                position += 2;
            }
            m_name.CopyTo( 0, buffer, position, lengthName );

            return lengthName + position;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose )
        {
            int length = ((m_publicKeyBlob.PublicKey.Length - 1) / 2) + 1; // blob
            if (verbose)
                length += 2;        // length of blob

            length += 8;            // version 

            length += m_name.Length;// Name
            if (verbose)
                length += 2;        // length of name

            length += 1;            // identifier
            return length;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position )
        {
            int length = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;
            m_publicKeyBlob = new StrongNamePublicKeyBlob();
            m_publicKeyBlob.PublicKey = new byte[length];

            int lengthInChars = ((length - 1) / 2) + 1;
            Buffer.InternalBlockCopy(buffer, position * 2, m_publicKeyBlob.PublicKey, 0, length);
            position += lengthInChars;

            int major = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            int minor = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position + 2);
            int build = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position + 4);
            int revision = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position + 6);
            m_version = new Version(major, minor, build, revision);
            position += 8;

            length = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;

            m_name = new String(buffer, position, length );

            return position + length;
        }

        // INormalizeForIsolatedStorage is not implemented for startup perf
        // equivalent to INormalizeForIsolatedStorage.Normalize()
        internal Object Normalize()
        {
            MemoryStream ms = new MemoryStream();
            BinaryWriter bw = new BinaryWriter(ms);

            bw.Write(m_publicKeyBlob.PublicKey);
            bw.Write(m_version.Major);
            bw.Write(m_name);

            ms.Position = 0;
            return ms;
        }
    }
}

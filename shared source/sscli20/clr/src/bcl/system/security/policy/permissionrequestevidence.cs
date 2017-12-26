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
//  PermissionRequestEvidence.cs
//
//  Encapsulation of permission request as an evidence type.
//

namespace System.Security.Policy {
	using System.Runtime.Remoting;
	using System;
	using System.IO;
	using System.Security.Util;
	using System.Collections;
    using System.Runtime.Serialization;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class PermissionRequestEvidence : IBuiltInEvidence
    {
        private PermissionSet m_request;
        private PermissionSet m_optional;
        private PermissionSet m_denied;
        private String m_strRequest;
        private String m_strOptional;
        private String m_strDenied;
        private const char idRequest = (char)0;
        private const char idOptional = (char)1;
        private const char idDenied = (char)2;
    
        public PermissionRequestEvidence(PermissionSet request, PermissionSet optional, PermissionSet denied)
        {
            if (request == null)
                m_request = null;
            else
                m_request = request.Copy();
                
            if (optional == null)
                m_optional = null;
            else
                m_optional = optional.Copy();
                
            if (denied == null)
                m_denied = null;
            else
                m_denied = denied.Copy();
        }
    
        internal PermissionRequestEvidence()
        {
        }

        public PermissionSet RequestedPermissions
        {
            get
            {
                return m_request;
            }
        }

        public PermissionSet OptionalPermissions
        {
            get
            {
                return m_optional;
            }
        }

        public PermissionSet DeniedPermissions
        {
            get
            {
                return m_denied;
            }
        }

        public PermissionRequestEvidence Copy()
        {
            return new PermissionRequestEvidence(m_request, m_optional, m_denied);
        }

        internal SecurityElement ToXml() {
            SecurityElement root = new SecurityElement( "System.Security.Policy.PermissionRequestEvidence" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.PermissionRequestEvidence" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            
            SecurityElement elem;
            
            if (m_request != null)
            {
                elem = new SecurityElement( "Request" );
                elem.AddChild( m_request.ToXml() );
                root.AddChild( elem );
            }
                
            if (m_optional != null)
            {
                elem = new SecurityElement( "Optional" );
                elem.AddChild( m_optional.ToXml() );
                root.AddChild( elem );
            }
                
            if (m_denied != null)
            {
                elem = new SecurityElement( "Denied" );
                elem.AddChild( m_denied.ToXml() );
                root.AddChild( elem );
            }
            
            return root;
        }
    

        internal void CreateStrings()
        {
            if (m_strRequest == null && m_request != null)
                m_strRequest = m_request.ToXml().ToString();

            if (m_strOptional == null && m_optional != null)
                m_strOptional = m_optional.ToXml().ToString();

            if (m_strDenied == null && m_denied != null)
                m_strDenied = m_denied.ToXml().ToString();
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            CreateStrings();

            int currentPosition = position;
            int numPermSetsPos = 0, numPermSets = 0;
            int tempLength;

            buffer[currentPosition++] = BuiltInEvidenceHelper.idPermissionRequestEvidence;

            if (verbose)
            {
                // Reserve some space to store the number of permission sets added
                numPermSetsPos = currentPosition;
                currentPosition += 2;
            }

            if (m_strRequest != null)
            {
                tempLength = m_strRequest.Length;
                if (verbose)
                {
                    buffer[currentPosition++] = idRequest;
                    BuiltInEvidenceHelper.CopyIntToCharArray(tempLength, buffer, currentPosition);
                    currentPosition += 2;
                    numPermSets++;
                }
                m_strRequest.CopyTo( 0, buffer, currentPosition, tempLength );
                currentPosition += tempLength;
            }

            if (m_strOptional != null)
            {
                tempLength = m_strOptional.Length;
                if (verbose)
                {
                    buffer[currentPosition++] = idOptional;
                    BuiltInEvidenceHelper.CopyIntToCharArray(tempLength, buffer, currentPosition);
                    currentPosition += 2;
                    numPermSets++;
                }
                m_strOptional.CopyTo( 0, buffer, currentPosition, tempLength );
                currentPosition += tempLength;
            }

            if (m_strDenied != null)
            {
                tempLength = m_strDenied.Length;
                if (verbose)
                {
                    buffer[currentPosition++] = idDenied;
                    BuiltInEvidenceHelper.CopyIntToCharArray(tempLength, buffer, currentPosition);
                    currentPosition += 2;
                    numPermSets++;
                }
                m_strDenied.CopyTo( 0, buffer, currentPosition, tempLength );
                currentPosition += tempLength;
            }

            if (verbose)
                    BuiltInEvidenceHelper.CopyIntToCharArray(numPermSets, buffer, numPermSetsPos);

            return currentPosition;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose )
        {
            CreateStrings();

            int currentPosition = 1;

            if (m_strRequest != null)
            {
                if (verbose)
                    currentPosition += 3;   // identifier + length
                currentPosition += m_strRequest.Length;
            }

            if (m_strOptional != null)
            {
                if (verbose)
                    currentPosition += 3;
                currentPosition += m_strOptional.Length;
            }

            if (m_strDenied != null)
            {
                if (verbose)
                    currentPosition += 3;
                currentPosition += m_strDenied.Length;
            }

            if (verbose)
                currentPosition += 2;   // Number of permission sets in the evidence

            return currentPosition;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position )
        {
            int numPermSets = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;

            int tempLength;
            for (int i = 0; i < numPermSets; i++)
            {
                char psKind = buffer[position++];

                tempLength = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
                position += 2;

                String tempStr = new String(buffer, position, tempLength);
                position += tempLength;
                Parser p = new Parser( tempStr );

                PermissionSet psTemp = new PermissionSet();
                psTemp.FromXml(p.GetTopElement());

                switch(psKind)
                {
                    case idRequest:
                        m_strRequest = tempStr;
                        m_request = psTemp;
                        break;

                    case idOptional:
                        m_strOptional = tempStr;
                        m_optional = psTemp;
                        break;

                    case idDenied:
                        m_strDenied = tempStr;
                        m_denied = psTemp;
                        break;

                    default:
                        throw new SerializationException(Environment.GetResourceString("Serialization_UnableToFixup"));
                }
            }

            return position;
        }

		public override String ToString()
		{
			return ToXml().ToString();
		}
    }
}

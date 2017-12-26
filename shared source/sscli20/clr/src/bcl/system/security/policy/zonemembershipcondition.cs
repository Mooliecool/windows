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
//  ZoneMembershipCondition.cs
//
//  Implementation of membership condition for zones
//

namespace System.Security.Policy {
    
	using System;
	using SecurityManager = System.Security.SecurityManager;
	using PermissionSet = System.Security.PermissionSet;
	using SecurityElement = System.Security.SecurityElement;
	using System.Collections;
	using System.Globalization;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class ZoneMembershipCondition : IMembershipCondition, IConstantMembershipCondition
    {
        //------------------------------------------------------
        //
        // PRIVATE CONSTANTS
        //
        //------------------------------------------------------
        
        private static readonly String[] s_names =
            {"MyComputer", "Intranet", "Trusted", "Internet", "Untrusted"};
        
        //------------------------------------------------------
        //
        // PRIVATE STATE DATA
        //
        //------------------------------------------------------
        
        private SecurityZone m_zone;
        private SecurityElement m_element;
        
        //------------------------------------------------------
        //
        // PUBLIC CONSTRUCTORS
        //
        //------------------------------------------------------
    
        internal ZoneMembershipCondition()
        {
            m_zone = SecurityZone.NoZone;
        }
        
        public ZoneMembershipCondition( SecurityZone zone )
        {
            VerifyZone( zone );
        
            this.SecurityZone = zone;
        }
        
        
        //------------------------------------------------------
        //
        // PUBLIC ACCESSOR METHODS
        //
        //------------------------------------------------------
    
        public SecurityZone SecurityZone
        {
            set
            {
                VerifyZone( value );
            
                m_zone = value;
            }
            
            get
            {
                if (m_zone == SecurityZone.NoZone && m_element != null)
                    ParseZone();
            
                return m_zone;
            }
        }
        
        //------------------------------------------------------
        //
        // PRIVATE AND PROTECTED HELPERS FOR ACCESSORS AND CONSTRUCTORS
        //
        //------------------------------------------------------
    
        private static void VerifyZone( SecurityZone zone )
        {
            if (zone < SecurityZone.MyComputer || zone > SecurityZone.Untrusted)
            {
                throw new ArgumentException( Environment.GetResourceString( "Argument_IllegalZone" ) );
            }
        }
        
        //------------------------------------------------------
        //
        // IMEMBERSHIPCONDITION IMPLEMENTATION
        //
        //------------------------------------------------------
    
        public bool Check( Evidence evidence )
        {
            if (evidence == null)
                return false;
        
            IEnumerator enumerator = evidence.GetHostEnumerator();
            while (enumerator.MoveNext())
            {
                Object obj = enumerator.Current;
            
                if (obj is Zone)
                {
                    if (m_zone == SecurityZone.NoZone && m_element != null)
                        ParseZone();
                        
                    if (((Zone)obj).SecurityZone == m_zone)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        
        public IMembershipCondition Copy()
        {
            if (m_zone == SecurityZone.NoZone && m_element != null)
                ParseZone();
        
            return new ZoneMembershipCondition( m_zone );
        }    
       
        public SecurityElement ToXml()
        {
            return ToXml( null );
        }
    
        public void FromXml( SecurityElement e )
        {
            FromXml( e, null );
        }
        
        public SecurityElement ToXml( PolicyLevel level )
        {
            if (m_zone == SecurityZone.NoZone && m_element != null)
                ParseZone();
                
            SecurityElement root = new SecurityElement( "IMembershipCondition" );
            System.Security.Util.XMLUtil.AddClassAttribute( root, this.GetType(), "System.Security.Policy.ZoneMembershipCondition" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.ZoneMembershipCondition" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            
            if (m_zone != SecurityZone.NoZone)
                root.AddAttribute( "Zone", Enum.GetName( typeof( SecurityZone ), m_zone ) );
            
            return root;
        }
          
        public void FromXml( SecurityElement e, PolicyLevel level )
        {
            if (e == null)
                throw new ArgumentNullException("e");
                
            if (!e.Tag.Equals( "IMembershipCondition" ))
            {
                throw new ArgumentException( Environment.GetResourceString( "Argument_MembershipConditionElement" ) );
            }
            
            lock (this)
            {
                m_zone = SecurityZone.NoZone;
                m_element = e;
            }
        }
        
        private void ParseZone()
        {
            lock (this)
            {
                if (m_element == null)
                    return;

                String eZone = m_element.Attribute( "Zone" );
            
                m_zone = SecurityZone.NoZone;
                if (eZone != null)
                {
                    m_zone = (SecurityZone)Enum.Parse( typeof( SecurityZone ), eZone );
                }
                else
                {
                    throw new ArgumentException( Environment.GetResourceString( "Argument_ZoneCannotBeNull" ) );
                }
                VerifyZone(m_zone);

                m_element = null;
            }
        }
        
        public override bool Equals( Object o )
        {
            ZoneMembershipCondition that = (o as ZoneMembershipCondition);
            
            if (that != null)
            {
                if (this.m_zone == SecurityZone.NoZone && this.m_element != null)
                    this.ParseZone();
                if (that.m_zone == SecurityZone.NoZone && that.m_element != null)
                    that.ParseZone();
                
                if(this.m_zone == that.m_zone)
                {
                    return true;
                }
            }
            return false;
        }
        
        public override int GetHashCode()
        {
            if (this.m_zone == SecurityZone.NoZone && this.m_element != null)
                this.ParseZone();
                
            return (int)m_zone;
        }
        
        public override String ToString()
        {
            if (m_zone == SecurityZone.NoZone && m_element != null)
                ParseZone();
        
            return String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Zone_ToString" ), s_names[(int)m_zone] );
        }
    }
}

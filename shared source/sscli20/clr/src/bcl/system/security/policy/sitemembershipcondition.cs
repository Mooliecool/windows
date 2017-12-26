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
//  SiteMembershipCondition.cs
//
//  Implementation of membership condition for zones
//

namespace System.Security.Policy {
    
	using System;
	using SecurityManager = System.Security.SecurityManager;
	using SiteString = System.Security.Util.SiteString;
	using PermissionSet = System.Security.PermissionSet;
	using SecurityElement = System.Security.SecurityElement;
	using System.Collections;
	using System.Globalization;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class SiteMembershipCondition : IMembershipCondition, IConstantMembershipCondition
    {
        
        //------------------------------------------------------
        //
        // PRIVATE STATE DATA
        //
        //------------------------------------------------------
        
        private SiteString m_site;
        private SecurityElement m_element;
        
        //------------------------------------------------------
        //
        // PUBLIC CONSTRUCTORS
        //
        //------------------------------------------------------
    
        internal SiteMembershipCondition()
        {
            m_site = null;
        }
        
        public SiteMembershipCondition( String site )
        {
            if (site == null)
                throw new ArgumentNullException( "site" );
        
            m_site = new SiteString( site );
        }
      
        //------------------------------------------------------
        //
        // PUBLIC ACCESSOR METHODS
        //
        //------------------------------------------------------
    

        public String Site
        {
            set
            {
                if (value == null)
                    throw new ArgumentNullException( "value" );
            
                m_site = new SiteString( value );
            }
        
            get
            {
                if (m_site == null && m_element != null)
                    ParseSite();

                if (m_site != null)
                    return m_site.ToString();
                else
                    return "";
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
                if (obj is Site)
                {
                    if (m_site == null && m_element != null)
                        ParseSite();
                        
                    if (((Site)obj).GetSiteString().IsSubsetOf( this.m_site ))
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        
        public IMembershipCondition Copy()
        {
            if (m_site == null && m_element != null)
                ParseSite();
                        
            return new SiteMembershipCondition( m_site.ToString() );
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
            if (m_site == null && m_element != null)
                ParseSite();
                        
            SecurityElement root = new SecurityElement( "IMembershipCondition" );
            System.Security.Util.XMLUtil.AddClassAttribute( root, this.GetType(), "System.Security.Policy.SiteMembershipCondition" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.SiteMembershipCondition" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            
            if (m_site != null)
                root.AddAttribute( "Site", m_site.ToString() );
            
            return root;
        }
    
        public void FromXml( SecurityElement e, PolicyLevel level  )
        {
            if (e == null)
                throw new ArgumentNullException("e");
        
            if (!e.Tag.Equals( "IMembershipCondition" ))
            {
                throw new ArgumentException( Environment.GetResourceString( "Argument_MembershipConditionElement" ) );
            }
            
            lock (this)
            {
                m_site = null;
                m_element = e;
            }
        }
            
        private void ParseSite()
        {   
            lock (this)
            {
                if (m_element == null)
                    return;

                String elSite = m_element.Attribute( "Site" );
                if (elSite == null)
                    throw new ArgumentException( Environment.GetResourceString( "Argument_SiteCannotBeNull" ) );
                else
                    m_site = new SiteString( elSite );
                m_element = null;
            }
        }
        
        public override bool Equals( Object o )
        {
            SiteMembershipCondition that = (o as SiteMembershipCondition);
            
            if (that != null)
            {
                if (this.m_site == null && this.m_element != null)
                    this.ParseSite();
                if (that.m_site == null && that.m_element != null)
                    that.ParseSite();
                
                if( Equals (this.m_site, that.m_site ))
                {
                    return true;
                }
            }
            return false;
        }
        
        public override int GetHashCode()
        {
            if (m_site == null && m_element != null)
                ParseSite();
            
            if (m_site != null)
            {
                return m_site.GetHashCode();
            }
            else
            {
                return typeof( SiteMembershipCondition ).GetHashCode();
            }
        }
        
        public override String ToString()
        {
            if (m_site == null && m_element != null)
                ParseSite();
        
            if (m_site != null)
                return String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Site_ToStringArg" ), m_site );
            else
                return Environment.GetResourceString( "Site_ToString" );
        }
    }
}

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
//  UrlMembershipCondition.cs
//
//  Implementation of membership condition for urls
//

namespace System.Security.Policy {
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Security;
    using System.Security.Util;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    sealed public class UrlMembershipCondition : IMembershipCondition, IConstantMembershipCondition
    {
        //------------------------------------------------------
        //
        // PRIVATE STATE DATA
        //
        //------------------------------------------------------

        private URLString m_url;
        private SecurityElement m_element;

        //------------------------------------------------------
        //
        // PUBLIC CONSTRUCTORS
        //
        //------------------------------------------------------

        internal UrlMembershipCondition()
        {
            m_url = null;
        }

        public UrlMembershipCondition( String url )
        {
            if (url == null)
                throw new ArgumentNullException( "url" );

            // Parse the Url to check that it's valid.
            m_url = new URLString(url, false /* not parsed */, true /* parse eagerly */);
        }

        //------------------------------------------------------
        //
        // PUBLIC ACCESSOR METHODS
        //
        //------------------------------------------------------

        public String Url
        {
            set
            {
                if (value == null)
                    throw new ArgumentNullException("value");
                    
                m_url = new URLString( value );
            }

            get
            {
                if (m_url == null && m_element != null)
                    ParseURL();

                return m_url.ToString();
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
                if (enumerator.Current is Url)
                {
                    if (m_url == null && m_element != null)
                        ParseURL();

                    if (((Url)enumerator.Current).GetURLString().IsSubsetOf( m_url ))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        public IMembershipCondition Copy()
        {
            if (m_url == null && m_element != null)
                ParseURL();

            UrlMembershipCondition mc = new UrlMembershipCondition();
            mc.m_url = new URLString( m_url.ToString() );
            return mc;
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
            if (m_url == null && m_element != null)
                ParseURL();

            SecurityElement root = new SecurityElement( "IMembershipCondition" );
            System.Security.Util.XMLUtil.AddClassAttribute( root, this.GetType(), "System.Security.Policy.UrlMembershipCondition" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.UrlMembershipCondition" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            if (m_url != null)
                root.AddAttribute( "Url", m_url.ToString() );

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
                m_element = e;
                m_url = null;
            }
        }

        private void ParseURL()
        {
            lock (this)
            {
                if (m_element == null)
                    return;

                String elurl = m_element.Attribute( "Url" );
                if (elurl == null)
                    throw new ArgumentException( Environment.GetResourceString( "Argument_UrlCannotBeNull" ) );
                else
                    m_url = new URLString( elurl );

                m_element = null;
            }
        }

        public override bool Equals( Object o )
        {
            UrlMembershipCondition that = (o as UrlMembershipCondition);

            if (that != null)
            {
                if (this.m_url == null && this.m_element != null)
                    this.ParseURL();
                if (that.m_url == null && that.m_element != null)
                    that.ParseURL();

                if (Equals( this.m_url, that.m_url ))
                {
                    return true;
                }
            }
            return false;
        }

        public override int GetHashCode()
        {
            if (m_url == null && m_element != null)
                ParseURL();

            if (m_url != null)
            {
                return m_url.GetHashCode();
            }
            else
            {
                return typeof( UrlMembershipCondition ).GetHashCode();
            }
        }

        public override String ToString()
        {
            if (m_url == null && m_element != null)
                ParseURL();

            if (m_url != null)
                return String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Url_ToStringArg" ), m_url.ToString() );
            else
                return Environment.GetResourceString( "Url_ToString" );
        }
    }
}

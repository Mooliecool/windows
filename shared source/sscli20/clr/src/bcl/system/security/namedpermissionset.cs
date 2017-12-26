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
//  NamedPermissionSet.cs
//
//  Extends PermissionSet to allow an associated name and description
//

namespace System.Security {
    
	using System;
	using System.Security.Util;
	using PermissionState = System.Security.Permissions.PermissionState;
	using System.Runtime.Serialization;
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class NamedPermissionSet : PermissionSet
    {
        // The name of this PermissionSet
        private String m_name;
        
        // The description of this PermissionSet
        private String m_description;
        [OptionalField(VersionAdded = 2)]                
        internal String m_descrResource;

        internal NamedPermissionSet()
            : base()
        {
        }
        
        public NamedPermissionSet( String name )
            : base()
        {
            CheckName( name );
            m_name = name;
        }
        
        public NamedPermissionSet( String name, PermissionState state)
            : base( state )
        {
            CheckName( name );
            m_name = name;
        }
        
        
        public NamedPermissionSet( String name, PermissionSet permSet )
            : base( permSet )
        {
            CheckName( name );
            m_name = name;
        }

        public NamedPermissionSet( NamedPermissionSet permSet )
            : base( permSet )
        {
            m_name = permSet.m_name;
            m_description = permSet.Description;
        }

        public String Name {
            get { return m_name; }
            set { CheckName( value ); m_name = value; }
        }
    
        private static void CheckName( String name )
        {
            if (name == null || name.Equals( "" ))
                throw new ArgumentException( Environment.GetResourceString( "Argument_NPMSInvalidName" ));
        }
        
        public String Description {
            get
            {
                if(m_descrResource != null)
                {
                    m_description = Environment.GetResourceString(m_descrResource);
                    m_descrResource = null;
                }
                return m_description;
            }

            set
            {
                m_description = value;
                m_descrResource = null;
            }
        }
        
        public override PermissionSet Copy()
        {
            return new NamedPermissionSet( this );
        }
        
        public NamedPermissionSet Copy( String name )
        {
            NamedPermissionSet set = new NamedPermissionSet( this );
            set.Name = name;
            return set;
        }
        
        public override SecurityElement ToXml()
        {
            SecurityElement elem = base.ToXml("System.Security.NamedPermissionSet");
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.NamedPermissionSet" ), "Class name changed!" );

            if (m_name != null && !m_name.Equals( "" ))
            {
                elem.AddAttribute( "Name", SecurityElement.Escape( m_name ) );
            }
            
            if (Description != null && !Description.Equals( "" ))
            {
                elem.AddAttribute( "Description", SecurityElement.Escape( Description ) );
            }
            
            return elem;
        }
        
        public override void FromXml( SecurityElement et )
        {
            FromXml( et, false, false );
        }

        internal override void FromXml( SecurityElement et, bool allowInternalOnly, bool ignoreTypeLoadFailures )
        {
            if (et == null)
                throw new ArgumentNullException( "et" );

            String elem;

            elem = et.Attribute( "Name" );
            m_name = elem == null ? null : elem;

            elem = et.Attribute( "Description" );
            m_description = (elem == null ? "" : elem);
            m_descrResource = null;

            base.FromXml( et, allowInternalOnly, ignoreTypeLoadFailures );
        }

        internal void FromXmlNameOnly( SecurityElement et )
        {
            // This function gets only the name for the permission set, ignoring all other info.

            String elem;

            elem = et.Attribute( "Name" );
            m_name = (elem == null ? null : elem);
        }

        // NamedPermissionSet Equals should have the exact semantic as PermissionSet.
        // We explicitly override them here to make sure that no one accidently
        // changes this.

        [System.Runtime.InteropServices.ComVisible(false)]
        public override bool Equals( Object obj )
        {
            return base.Equals( obj );
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }


}



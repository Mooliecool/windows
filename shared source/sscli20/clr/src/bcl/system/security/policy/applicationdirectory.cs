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
//  ApplicationDirectory.cs
//
//  ApplicationDirectory is an evidence type representing the directory the assembly
//  was loaded from.
//

namespace System.Security.Policy {
    
    using System;
    using System.IO;
    using System.Security.Util;
    using System.Collections;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class ApplicationDirectory : IBuiltInEvidence
    {
        private URLString m_appDirectory;
    
        internal ApplicationDirectory()
        {
            m_appDirectory = null;
        }
    
        public ApplicationDirectory( String name )
        {
            if (name == null)
                throw new ArgumentNullException( "name" );
        
            m_appDirectory = new URLString( name );
        }
    
        public String Directory
        {
            get
            {
                return m_appDirectory.ToString();
            }
        }
        
        public override bool Equals(Object o)
        {
            if (o == null)
                return false;
        
            if (o is ApplicationDirectory)
            {
                ApplicationDirectory appDir = (ApplicationDirectory) o;
                
                if (this.m_appDirectory == null)
                {
                    return appDir.m_appDirectory == null;
                }
                else if (appDir.m_appDirectory == null)
                {
                    return false;
                }
                else
                {
                    return this.m_appDirectory.IsSubsetOf( appDir.m_appDirectory ) && appDir.m_appDirectory.IsSubsetOf( this.m_appDirectory );
                }
            }
            return false;
        }
    
        public override int GetHashCode()
        {
            return this.Directory.GetHashCode();
        } 
    
        public Object Copy()
        {
            ApplicationDirectory appDir = new ApplicationDirectory();
    
            appDir.m_appDirectory = this.m_appDirectory;
    
            return appDir;
        }
    
        internal SecurityElement ToXml()
        {
            SecurityElement root = new SecurityElement( "System.Security.Policy.ApplicationDirectory" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.ApplicationDirectory" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            
            if (m_appDirectory != null)
                root.AddChild( new SecurityElement( "Directory", m_appDirectory.ToString() ) );
            
            return root;
        }

        /// <internalonly/>
        int IBuiltInEvidence.OutputToBuffer( char[] buffer, int position, bool verbose )
        {
            buffer[position++] = BuiltInEvidenceHelper.idApplicationDirectory;
            String directory = this.Directory;
            int length = directory.Length;

            if (verbose)
            {
                BuiltInEvidenceHelper.CopyIntToCharArray(length, buffer, position);
                position += 2;
            }
            directory.CopyTo( 0, buffer, position, length );
            return length + position;
        }

        /// <internalonly/>
        int IBuiltInEvidence.InitFromBuffer( char[] buffer, int position)
        {
            int length = BuiltInEvidenceHelper.GetIntFromCharArray(buffer, position);
            position += 2;

            m_appDirectory = new URLString( new String(buffer, position, length ));

            return position + length;
        }

        /// <internalonly/>
        int IBuiltInEvidence.GetRequiredSize(bool verbose)
        {
            if (verbose)
                return this.Directory.Length + 3; // Directory + identifier + length
            else
                return this.Directory.Length + 1; // Directory + identifier
        }
        
        public override String ToString()
		{
			return ToXml().ToString();
		}
    }
}

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
// GenericIdentity.cs
//
// A generic identity
//

namespace System.Security.Principal
{
    using System.Runtime.Remoting;
    using System;
    using System.Security.Util;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class GenericIdentity : IIdentity {
        private string m_name;
        private string m_type;

        public GenericIdentity (string name) {
            if (name == null)
                throw new ArgumentNullException("name");

            m_name = name;
            m_type = "";
        }

        public GenericIdentity (string name, string type) {
            if (name == null)
                throw new ArgumentNullException("name");
            if (type == null)
                throw new ArgumentNullException("type");

            m_name = name;
            m_type = type;
        }

        public virtual string Name {
            get {
                return m_name;
            }
        }

        public virtual string AuthenticationType {
            get {
                return m_type;
            }
        }

        public virtual bool IsAuthenticated {
            get {
                return !m_name.Equals("");
            } 
        }
    }
}

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

namespace System.Security 
{
    //PermissionSetEnumerator.cs
    
    using System;
    using System.Collections;
    using TokenBasedSetEnumerator = System.Security.Util.TokenBasedSetEnumerator;
    using TokenBasedSet = System.Security.Util.TokenBasedSet;
    
    internal class PermissionSetEnumerator : IEnumerator 
    {
        PermissionSetEnumeratorInternal enm;
        
        public Object Current
        {
            get
            {
                return enm.Current;
            }
        }
        
        public bool MoveNext()
        {
            return enm.MoveNext();
        }
        
        public void Reset()
        {
            enm.Reset();
        }
        
        internal PermissionSetEnumerator(PermissionSet permSet)
        {
            enm = new PermissionSetEnumeratorInternal(permSet);
        }
    }
    
    internal struct PermissionSetEnumeratorInternal 
    {
        private PermissionSet m_permSet;
        private TokenBasedSetEnumerator enm;
        
        public Object Current
        {
            get
            {
                return enm.Current;
            }
        }

        internal PermissionSetEnumeratorInternal(PermissionSet permSet)
        {
            m_permSet = permSet;
            enm = new TokenBasedSetEnumerator(permSet.m_permSet);
        }

        public int GetCurrentIndex()
        {
            return enm.Index;
        }
        
        public void Reset()
        {
            enm.Reset();
        }
        
        public bool MoveNext()
        {
            while (enm.MoveNext())
            {
                Object obj = enm.Current;
                IPermission perm = obj as IPermission;
                if (perm != null)
                {
                    enm.Current = perm;
                    return true;
                }

                SecurityElement elem = obj as SecurityElement;

                if (elem != null)
                {
                    perm = m_permSet.CreatePermission(elem, enm.Index);
                    if (perm != null)
                    {
                        enm.Current = perm;
                        return true;
                    }
                }
            }
            return false;
        }
    }
}


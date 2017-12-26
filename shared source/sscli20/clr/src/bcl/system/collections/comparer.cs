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
/*============================================================
**
** Class:  Comparer
**
**
** Purpose: Default IComparer implementation.
**
** 
===========================================================*/
namespace System.Collections {
    
	using System;
    using System.Globalization;
    using System.Runtime.Serialization;
    using System.Security.Permissions;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class Comparer : IComparer , ISerializable
    {
        private CompareInfo m_compareInfo;   
        public static readonly Comparer Default = new Comparer(CultureInfo.CurrentCulture);
        public static readonly Comparer DefaultInvariant = new Comparer(CultureInfo.InvariantCulture);
        
        private const String CompareInfoName = "CompareInfo";

        private Comparer() {
            m_compareInfo = null;
        }

        public Comparer(CultureInfo culture) {
            if (culture==null) {
                throw new ArgumentNullException("culture");
            }
            m_compareInfo = culture.CompareInfo;
        }
        
        private Comparer(SerializationInfo info, StreamingContext context) {            
            m_compareInfo = null;
            SerializationInfoEnumerator enumerator = info.GetEnumerator();                        
            while( enumerator.MoveNext()) {
                switch( enumerator.Name) {
                    case CompareInfoName:
                        m_compareInfo = (CompareInfo) info.GetValue(CompareInfoName, typeof(CompareInfo));
                        break;
                }
            }
        }
    
        // Compares two Objects by calling CompareTo.  If a == 
        // b,0 is returned.  If a implements 
        // IComparable, a.CompareTo(b) is returned.  If a 
        // doesn't implement IComparable and b does, 
        // -(b.CompareTo(a)) is returned, otherwise an 
        // exception is thrown.
        // 
        public int Compare(Object a, Object b) {
            if (a == b) return 0;
            if (a == null) return -1;
            if (b == null) return 1;
            if (m_compareInfo != null) {
                String sa = a as String;
                String sb = b as String;
                if (sa != null && sb != null)
                    return m_compareInfo.Compare(sa, sb);
            }

            IComparable ia = a as IComparable;
            if (ia != null)
                return ia.CompareTo(b);

            throw new ArgumentException(Environment.GetResourceString("Argument_ImplementIComparable"));
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public void GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }

            if( m_compareInfo != null) {
                info.AddValue(CompareInfoName, m_compareInfo);
            }
        }        
    }
}

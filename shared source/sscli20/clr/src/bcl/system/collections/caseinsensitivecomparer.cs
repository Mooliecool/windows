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
** Class: CaseInsensitiveComparer
**
**
**
============================================================*/
namespace System.Collections {
//This class does not contain members and does not need to be serializable
    using System;
    using System.Collections;
    using System.Globalization;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class CaseInsensitiveComparer : IComparer {
        private CompareInfo m_compareInfo;
        private static CaseInsensitiveComparer m_InvariantCaseInsensitiveComparer;
        
        public CaseInsensitiveComparer() {
            m_compareInfo = CultureInfo.CurrentCulture.CompareInfo;
        }

        public CaseInsensitiveComparer(CultureInfo culture) {
            if (culture==null) {
                throw new ArgumentNullException("culture");
            }
            m_compareInfo = culture.CompareInfo;
        }

		public static CaseInsensitiveComparer Default
		{ 
			get
			{
				return new CaseInsensitiveComparer(CultureInfo.CurrentCulture);
			}
		}
		
      	public static CaseInsensitiveComparer DefaultInvariant
		{ 
			get
			{
                if (m_InvariantCaseInsensitiveComparer == null) {
				    m_InvariantCaseInsensitiveComparer = new CaseInsensitiveComparer(CultureInfo.InvariantCulture);
                }
                return m_InvariantCaseInsensitiveComparer;
			}
		}
	
		// Behaves exactly like Comparer.Default.Compare except that the comparison is case insensitive
    	// Compares two Objects by calling CompareTo.  If a == 
    	// b,0 is returned.  If a implements 
    	// IComparable, a.CompareTo(b) is returned.  If a 
    	// doesn't implement IComparable and b does, 
    	// -(b.CompareTo(a)) is returned, otherwise an 
    	// exception is thrown.
    	// 
		public int Compare(Object a, Object b) {
            String sa = a as String;
            String sb = b as String;
			if (sa != null && sb != null)
				return m_compareInfo.Compare(sa, sb, CompareOptions.IgnoreCase);
			else
				return Comparer.Default.Compare(a,b);
    	}
    }
}

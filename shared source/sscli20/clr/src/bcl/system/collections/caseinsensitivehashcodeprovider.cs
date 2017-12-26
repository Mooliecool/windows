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
** Class: CaseInsensitiveHashCodeProvider
**
**
** Purpose: Designed to support hashtables which require 
** case-insensitive behavior while still maintaining case,
** this provides an efficient mechanism for getting the 
** hashcode of the string ignoring case.
**
**
============================================================*/
namespace System.Collections {
//This class does not contain members and does not need to be serializable
    using System;
    using System.Collections;
    using System.Globalization;

    [Serializable]
    [Obsolete("Please use StringComparer instead.")]    
[System.Runtime.InteropServices.ComVisible(true)]
    public class CaseInsensitiveHashCodeProvider : IHashCodeProvider {
        private TextInfo m_text;
        private static CaseInsensitiveHashCodeProvider m_InvariantCaseInsensitiveHashCodeProvider = null;

        public CaseInsensitiveHashCodeProvider() {
            m_text = CultureInfo.CurrentCulture.TextInfo;
        }

        public CaseInsensitiveHashCodeProvider(CultureInfo culture) {
            if (culture==null) {
                throw new ArgumentNullException("culture");
            }
            m_text = culture.TextInfo;
        }

		public static CaseInsensitiveHashCodeProvider Default
		{
			get
			{
				return new CaseInsensitiveHashCodeProvider(CultureInfo.CurrentCulture);
			}
		}
        
      	public static CaseInsensitiveHashCodeProvider DefaultInvariant
		{ 
			get
			{
                if (m_InvariantCaseInsensitiveHashCodeProvider == null) {
				    m_InvariantCaseInsensitiveHashCodeProvider = new CaseInsensitiveHashCodeProvider(CultureInfo.InvariantCulture);
                }
                return m_InvariantCaseInsensitiveHashCodeProvider;
			}
		}

        public int GetHashCode(Object obj) {
            if (obj==null) {
                throw new ArgumentNullException("obj");
            }

            String s = obj as String;
            if (s==null) {
                return obj.GetHashCode();
            }

            return m_text.GetCaseInsensitiveHashCode(s);
        }
    }
}

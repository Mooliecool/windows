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
////////////////////////////////////////////////////////////////////////////
//
//  Class:    SortKey
//
//  Purpose:  This class implements a set of methods for retrieving
//            sort key information.
//
//  Date:     August 12, 1998
//
////////////////////////////////////////////////////////////////////////////

namespace System.Globalization {
    
    using System;
    using System.Runtime.CompilerServices;

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class SortKey
    {
        //--------------------------------------------------------------------//
        //                        Internal Information                        //
        //--------------------------------------------------------------------//
    
        //
        //  Variables.
        //
        internal int win32LCID;           // 32-bit Win32 locale ID.  It may contain a 4-bit sort ID.
        internal CompareOptions options;               // options
        internal String m_String;         // original string
        internal byte[] m_KeyData;        // sortkey data
        
        // Mask used to check if the c'tor has the right flags.
        private const CompareOptions ValidSortkeyCtorMaskOffFlags = ~(CompareOptions.IgnoreCase | CompareOptions.IgnoreSymbols | CompareOptions.IgnoreNonSpace | CompareOptions.IgnoreWidth | CompareOptions.IgnoreKanaType | CompareOptions.StringSort);

        ////////////////////////////////////////////////////////////////////////
        //
        //  SortKey Constructor
        //
        //  Implements CultureInfo.CompareInfo.GetSortKey().
        //  Package access only.
        //
        ////////////////////////////////////////////////////////////////////////
    
        unsafe internal SortKey(void* pSortingFile, int win32LCID, String str, CompareOptions options)
        {
            if (str==null) {
                throw new ArgumentNullException("str");
            }

            //
            //
            if ((options & ValidSortkeyCtorMaskOffFlags) != 0) 
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidFlag"), "options"); 
            }

            //This takes the sort id from CompareInfo, so we should be able to skip initializing it.
            this.win32LCID = win32LCID;
    
            this.options = options;
            m_String = str;
    
            //We need an initialized SortTable here, but since we're getting this as an instance 
            //method off of a CompareInfo, we're guaranteed that that already happened.
            m_KeyData = CompareInfo.nativeCreateSortKey(pSortingFile, str, (int)options, win32LCID);
        }

    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  GetOriginalString
        //
        //  Returns the original string used to create the current instance
        //  of SortKey.
        //
        ////////////////////////////////////////////////////////////////////////
        public virtual String OriginalString
        {
            get {
                return (m_String);
            }
        }
    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  GetKeyData
        //
        //  Returns a byte array representing the current instance of the
        //  sort key.
        //
        ////////////////////////////////////////////////////////////////////////
        public virtual byte[] KeyData
        {
            get {
                return (byte[])(m_KeyData.Clone());
            }
        }
    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  Compare
        //
        //  Compares the two sort keys.  Returns 0 if the two sort keys are
        //  equal, a number less than 0 if sortkey1 is less than sortkey2,
        //  and a number greater than 0 if sortkey1 is greater than sortkey2.
        //
        ////////////////////////////////////////////////////////////////////////
        public static int Compare(SortKey sortkey1, SortKey sortkey2) {
    
            if (sortkey1==null || sortkey2==null) {
                throw new ArgumentNullException((sortkey1==null ? "sortkey1": "sortkey2"));
            }
    
            byte[] key1Data = sortkey1.m_KeyData;
            byte[] key2Data = sortkey2.m_KeyData;
    
            BCLDebug.Assert(key1Data!=null, "key1Data!=null");
            BCLDebug.Assert(key2Data!=null, "key2Data!=null");

            if (key1Data.Length == 0) {
                if (key2Data.Length == 0) {
                    return (0);
                }
                return (-1);
            }
            if (key2Data.Length == 0) {
                return (1);
            }
    
            int compLen = (key1Data.Length<key2Data.Length)?key1Data.Length:key2Data.Length;

            for (int i=0; i<compLen; i++) {
                if (key1Data[i]>key2Data[i]) {
                    return (1);
                }
                if (key1Data[i]<key2Data[i]) {
                    return (-1);
                }
            }
    
            return 0;
    
        }
    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  Equals
        //
        //  Implements Object.Equals().  Returns a boolean indicating whether
        //  or not object refers to the same SortKey as the current instance.
        //
        ////////////////////////////////////////////////////////////////////////
        public override bool Equals(Object value)
        {
            SortKey that = value as SortKey;
            
            if (that != null)
            {
                return Compare(this, that) == 0;
            }

            return (false);
        }
    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  GetHashCode
        //
        //  Implements Object.GetHashCode().  Returns the hash code for the
        //  SortKey.  The hash code is guaranteed to be the same for
        //  SortKey A and B where A.Equals(B) is true.
        //
        ////////////////////////////////////////////////////////////////////////
        public override int GetHashCode()
        {
            return (CompareInfo.GetCompareInfo(win32LCID).GetHashCodeOfString(m_String, options));
        }
    
    
        ////////////////////////////////////////////////////////////////////////
        //
        //  ToString
        //
        //  Implements Object.ToString().  Returns a string describing the
        //  SortKey.
        //
        ////////////////////////////////////////////////////////////////////////
        public override String ToString()
        {
            return ("SortKey - " + win32LCID + ", " + options + ", " + m_String);
        }
    }
}

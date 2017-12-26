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
namespace System.Text
{
    using System;
    using System.Text;


    [Serializable]
    public sealed class EncodingInfo
    {
        int     iCodePage;          // Code Page #
        String  strEncodingName;    // Short name (web name)
        String  strDisplayName;     // Full localized name

        internal EncodingInfo(int codePage, string name, string displayName)
        {
            this.iCodePage = codePage;
            this.strEncodingName = name;
            this.strDisplayName = displayName;
        }


        public int CodePage
        {
            get
            {
                return iCodePage;
            }
        }


        public String Name
        {
            get
            {
                return strEncodingName;
            }
        }


        public String DisplayName
        {
            get
            {
                return strDisplayName;
            }
        }


        public Encoding GetEncoding()
        {
            return Encoding.GetEncoding(this.iCodePage);
        }

        public override bool Equals(Object value)
        {
            EncodingInfo that = value as EncodingInfo;
            if (that != null)
            {
                return (this.CodePage == that.CodePage);
            }
            return (false);
        }
        
        public override int GetHashCode()
        {
            return this.CodePage;
        }
        
    }
}

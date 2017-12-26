//------------------------------------------------------------------------------
// <copyright file="Ref.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Xml {
    using System;

    internal abstract class Ref {
        public static bool Equal(string strA, string strB) {
#if DEBUG
            // We can't use Debug.Assert in XmlReader.
            if(((object) strA != (object) strB) && String.Equals(strA, strB)) {
                throw new Exception("ASSERT: String atomization failure str='" + strA + "'");
            }
#endif
            return (object) strA == (object) strB;
        }
    }
}

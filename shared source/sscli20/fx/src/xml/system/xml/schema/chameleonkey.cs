//------------------------------------------------------------------------------
// <copyright file="XmlSchemaExternal.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner>                                                                 
//------------------------------------------------------------------------------

namespace System.Xml.Schema {

    using System.Collections;
    using System.ComponentModel;
    using System.Xml.Serialization;

    // Case insensitive file name key for use in a hashtable.

    internal class ChameleonKey {
        internal string targetNS;
        internal Uri chameleonLocation;
        int hashCode;

        public ChameleonKey(string ns, Uri location) {
            targetNS = ns;
            chameleonLocation = location;
        }
        
        public override int GetHashCode() {
            if (hashCode == 0) {
                hashCode = targetNS.GetHashCode() + chameleonLocation.GetHashCode();
            }
            return hashCode;
        }

        public override bool Equals(object obj) {
            if (Ref.ReferenceEquals(this,obj)) {
                return true;
            }
            ChameleonKey cKey = obj as ChameleonKey;
            if (cKey != null) {
                return this.targetNS.Equals(cKey.targetNS) && this.chameleonLocation.Equals(cKey.chameleonLocation);
            }
            return false;
        }
    }
}

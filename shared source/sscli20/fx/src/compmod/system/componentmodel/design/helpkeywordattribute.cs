//------------------------------------------------------------------------------
// <copyright file="HelpKeywordAttribute.cs" company="Microsoft">
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

/*
 */

namespace System.ComponentModel.Design {
    using System;
    using System.Security.Permissions;

    [AttributeUsage(AttributeTargets.All, AllowMultiple = false, Inherited = false)]
    [Serializable]
    public sealed class HelpKeywordAttribute : Attribute {

        /// <devdoc>
        /// Default value for HelpKeywordAttribute, which is null.  
        /// </devdoc>
        public static readonly HelpKeywordAttribute Default = new HelpKeywordAttribute();   

        private string contextKeyword;

        /// <devdoc>
        /// Default constructor, which creates an attribute with a null HelpKeyword.
        /// </devdoc>
        public HelpKeywordAttribute() {
        }

        /// <devdoc>
        /// Creates a HelpKeywordAttribute with the value being the given keyword string.
        /// </devdoc>
        public HelpKeywordAttribute(string keyword) {
            if (keyword == null) {
                throw new ArgumentNullException("keyword");
            }
            this.contextKeyword = keyword;
        }

        /// <devdoc>
        /// Creates a HelpKeywordAttribute with the value being the full name of the given type.
        /// </devdoc>
        public HelpKeywordAttribute(Type t) {
            if (t == null) {
                throw new ArgumentNullException("t");
            }
            this.contextKeyword = t.FullName;
        }

        /// <devdoc>
        /// Retrieves the HelpKeyword this attribute supplies.
        /// </devdoc>
        public string HelpKeyword {
            get {
                return contextKeyword;
            }
        }


        /// <devdoc>
        /// Two instances of a HelpKeywordAttribute are equal if they're HelpKeywords are equal.
        /// </devdoc>
        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }
            if ((obj != null) && (obj is HelpKeywordAttribute)) {
                return ((HelpKeywordAttribute)obj).HelpKeyword == HelpKeyword;
            }

            return false;
        }

        /// <devdoc>
        /// </devdoc>
        public override int GetHashCode() {
            return base.GetHashCode();
        }

        /// <devdoc>
        /// Returns true if this Attribute's HelpKeyword is null.
        /// </devdoc>
        public override bool IsDefaultAttribute() {
            return this.Equals(Default);
        }
    }
}


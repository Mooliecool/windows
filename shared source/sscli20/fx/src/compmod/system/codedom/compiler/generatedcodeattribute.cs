//------------------------------------------------------------------------------
// <copyright file="GeneratedCodeAttribute.cs" company="Microsoft">
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

namespace System.CodeDom.Compiler {
    [AttributeUsage(AttributeTargets.All, Inherited = false, AllowMultiple = false)]
    public sealed class GeneratedCodeAttribute : Attribute {
        private readonly string tool;
        private readonly string version;

        public GeneratedCodeAttribute(string tool, string version) {
            this.tool = tool;
            this.version = version;
        }

        public string Tool {
            get { return this.tool; }
        }

        public string Version {
            get { return this.version; }
        }
    }
}

//------------------------------------------------------------------------------
// <copyright file="XmlSchemaDerivationMethod.cs" company="Microsoft">
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

    public sealed class XmlSchemaCompilationSettings {

        bool enableUpaCheck;

        public XmlSchemaCompilationSettings() {
            enableUpaCheck = true;
        }

        public bool EnableUpaCheck {
            get {
                return enableUpaCheck;
            }
            set {
                enableUpaCheck = value;
            }
        }
    }
    
}

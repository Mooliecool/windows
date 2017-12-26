//------------------------------------------------------------------------------
// <copyright file="SchemaNotation.cs" company="Microsoft">
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

    using System;
    using System.Diagnostics;

    internal sealed class SchemaNotation {
        internal const int   SYSTEM = 0;
        internal const int   PUBLIC = 1;

        private XmlQualifiedName  name;
        private String systemLiteral;   // System literal
        private String pubid;    // pubid literal

        internal SchemaNotation(XmlQualifiedName name) {
            this.name = name;
        }

        internal XmlQualifiedName Name {
            get { return name;}
        }

        internal String SystemLiteral {
            get { return systemLiteral;}
            set { systemLiteral = value;}
        }

        internal String Pubid {
            get { return pubid;}
            set { pubid = value;}
        }

    };

}

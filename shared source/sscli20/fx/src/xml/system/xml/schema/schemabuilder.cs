//------------------------------------------------------------------------------
// <copyright file="SchemaBuilder.cs" company="Microsoft">
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

    internal abstract class SchemaBuilder {
        internal abstract bool ProcessElement(string prefix, string name, string ns);
        internal abstract void ProcessAttribute(string prefix, string name, string ns, string value);
        internal abstract bool IsContentParsed();
        internal abstract void ProcessMarkup(XmlNode[] markup);
        internal abstract void ProcessCData(string value);
        internal abstract void StartChildren();
        internal abstract void EndChildren();
    };

}

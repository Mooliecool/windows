//------------------------------------------------------------------------------
// <copyright file="XmlDomTextWriter.cs" company="Microsoft">
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
    using System.IO;
    using System.Text;

    // Represents a writer that will make it possible to work with prefixes even
    // if the namespace is not specified.
    // This is not possible with XmlTextWriter. But this class inherits XmlTextWriter.
    internal class XmlDOMTextWriter : XmlTextWriter {

        public XmlDOMTextWriter( Stream w, Encoding encoding ) : base( w,encoding ) {
        }

        public XmlDOMTextWriter( String filename, Encoding encoding ) : base( filename,encoding ){
        }

        public XmlDOMTextWriter( TextWriter w ) : base( w ){
        }

        // Overrides the baseclass implementation so that emptystring prefixes do
        // do not fail if namespace is not specified.
        public override void WriteStartElement( string prefix, string localName, string ns ){
            if( ( ns.Length == 0 ) && ( prefix.Length != 0 ) )
                prefix = "" ;

            base.WriteStartElement( prefix, localName, ns );
        }

        // Overrides the baseclass implementation so that emptystring prefixes do
        // do not fail if namespace is not specified.
        public override  void WriteStartAttribute( string prefix, string localName, string ns ){
            if( ( ns.Length == 0 ) && ( prefix.Length != 0 )  )
                prefix = "" ;

            base.WriteStartAttribute( prefix, localName, ns );
        }
    }
}
    

  

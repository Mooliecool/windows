//------------------------------------------------------------------------------
// <copyright file="XmlDeclaration.cs" company="Microsoft">
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
    using System.Text;
    using System.Diagnostics;

    // Represents the xml declaration nodes: <?xml version='1.0' ...?>
    public class XmlDeclaration : XmlLinkedNode {

        const string YES = "yes";
        const string NO = "no";
        const string VERNUM = "1.0";

        private string  encoding;
        private string  standalone;

        protected internal XmlDeclaration( string version, string encoding, string standalone, XmlDocument doc ) : base( doc ) {
            if ( version != VERNUM )
                throw new ArgumentException( Res.GetString( Res.Xdom_Version ) );
            if( ( standalone != null ) && ( standalone.Length > 0 )  )
                if ( ( standalone != YES ) && ( standalone != NO ) )
                    throw new ArgumentException( Res.GetString(Res.Xdom_standalone, standalone) );
            this.Encoding = encoding;
            this.Standalone = standalone;
        }


        // The version attribute (1.0) for <?xml version= '1.0' ... ?>
        public string Version {
            get { return VERNUM; }
        }

        // Specifies the value of the encoding attribute, as for
        // <?xml version= '1.0' encoding= 'UTF-8' ?>
        public string Encoding {
            get { return this.encoding; }
            set { this.encoding = ( (value == null) ? String.Empty : value ); }
        }

        // Specifies the value of the standalone attribute.
        public string Standalone {
            get { return this.standalone; }
            set {
                if ( value == null )
                    this.standalone = String.Empty;
                else if ( value.Length == 0 || value == YES || value == NO )
                    this.standalone = value;
                else
                    throw new ArgumentException( Res.GetString(Res.Xdom_standalone, value) );
            }
        }

        public override String Value {
            get { return InnerText; }
            set { InnerText = value; }
        }


        // Gets or sets the concatenated values of the node and
        // all its children.
        public override string InnerText {
            get {
                StringBuilder strb = new StringBuilder("version=\"" + Version + "\"");
                if ( Encoding.Length > 0 ) {
                    strb.Append(" encoding=\"");
                    strb.Append(Encoding);
                    strb.Append("\"");
                }
                if ( Standalone.Length > 0 ) {
                    strb.Append(" standalone=\"");
                    strb.Append(Standalone);
                    strb.Append("\"");
                }
                return strb.ToString();
            }

            set {
                string tempVersion = null;
                string tempEncoding = null;
                string tempStandalone = null;
                string orgEncoding   = this.Encoding;
                string orgStandalone = this.Standalone;

                XmlLoader.ParseXmlDeclarationValue( value, out tempVersion, out tempEncoding, out tempStandalone );

                try {
                    if ( tempVersion != null && tempVersion != VERNUM )
                        throw new ArgumentException(Res.GetString(Res.Xdom_Version));
                    if ( tempEncoding != null )
                        Encoding = tempEncoding;
                    if ( tempStandalone != null )
                        Standalone = tempStandalone;
                }
                catch {
                    Encoding = orgEncoding;
                    Standalone = orgStandalone;
                    throw;
                }
            }
        }

        //override methods and properties from XmlNode

        // Gets the name of the node.
        public override String Name {
            get {
                return "xml";
            }
        }

        // Gets the name of the current node without the namespace prefix.
        public override string LocalName {
            get { return Name;}
        }

        // Gets the type of the current node.
        public override XmlNodeType NodeType {
            get { return XmlNodeType.XmlDeclaration;}
        }

        // Creates a duplicate of this node.
        public override XmlNode CloneNode(bool deep) {
            Debug.Assert( OwnerDocument != null );
            return OwnerDocument.CreateXmlDeclaration( Version, Encoding, Standalone );
        }

        // Saves the node to the specified XmlWriter.
        public override void WriteTo(XmlWriter w) {
            w.WriteProcessingInstruction(Name, InnerText);
        }


        // Saves all the children of the node to the specified XmlWriter.
        public override void WriteContentTo(XmlWriter w) {
            // Intentionally do nothing since the node doesn't have children.
        }
    }
}

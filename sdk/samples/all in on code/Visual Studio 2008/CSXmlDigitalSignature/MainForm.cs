/******************************** Module Header ********************************\
* Module Name:  MainForm.cs
* Project:      CSXmlDigitalSignature
* Copyright (c) Microsoft Corporation.
* 
* You can use the classes in the System.Security.Cryptography.Xml namespace to
* sign an XML document or part of an XML document with a digital signature. 
* XML digital signatures (XMLDSIG) allow you to verify that data was not 
* altered after it was signed. For more information about the XMLDSIG standard
* see the World Wide Web Consortium (W3C) specification at 
* http://www.w3.org/TR/xmldsig-core/. 
*
* The code example in this procedure demonstrates how to digitally sign an 
* entire XML document and attach the signature to the document in a 
* <Signature> element. The example creates an RSA signing key, adds the key
* to a secure key container, and then uses the key to digitally sign an XML 
* document. The key can then be retrieved to verify the XML digital signature,
* or be used to sign another XML document.
* 
* Code Logic:
* 1 Create Xml document 
* 2 Click Sign button to sign one Xml document
* 2.1 Create one RSACryptoServiceProvider object for signing
* 2.2 Creaet one SignedXml object to facilitate creating XML signatures
* 2.3 Append the element to the XML document
* 2.4 Save Xml document to file
* 3 Click Verify button to verify Xml document
* 3.1 Create one RSACryptoServiceProvider object to verify digital signature
* 3.2 Creaet one SignedXml object to verify digital signature
* 4 Modify Xml Document. Then verify the XmlDocument to know its correctness
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/14/2009 12:00 AM Riquel Dong Created
\*******************************************************************************/

#region Using Directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml.Linq;
using System.Xml;
using System.Security.Cryptography;
using System.Security.Cryptography.Xml;
#endregion


namespace CSXmlDigitalSignature
{
    public partial class MainForm : Form
    {
        private XmlDocument xmlDoc;

        public MainForm()
        {
            InitializeComponent();
        }

        private void XMLDigitalSignatureForm_Load(object sender, EventArgs e)
        {
            GenerateXmlDocument();
            tbxPlaintext.Text = xmlDoc.OuterXml;
        }

        private void GenerateXmlDocument()
        {
            XDocument xdoc = new XDocument(
                new XDeclaration("1.0", "utf-8", "yes"),
                new XComment("This is a comment"),
                new XElement("invoice",
                    new XElement("items",
                        new XElement("item",
                            new XElement("desc", ""),
                            new XElement("unitprice", "14.95"),
                            new XElement("quantity", "1")),
                            new XElement("creditcard",
                                new XElement("number", "19834209"),
                                new XElement("expiry", "02/02/2002")
                                ))));
            xdoc.Save("test.xml");
            xmlDoc = new XmlDocument();
            xmlDoc.Load("test.xml");
        }

        // Save public key to verify digital signature
        private String publicOnlyKey;
        private void btnSign_Click(object sender, EventArgs e)
        {
            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a new RSA signing key and export public key for 
                // verification.

                RSACryptoServiceProvider rsaKey = new RSACryptoServiceProvider();
                publicOnlyKey = rsaKey.ToXmlString(false);
                tbxRSAParameters.Text = publicOnlyKey;


                /////////////////////////////////////////////////////////////////
                // Sign the XML document.
                // 

                SignXml(xmlDoc, rsaKey);
                MessageBox.Show("XML file signed.");


                /////////////////////////////////////////////////////////////////
                // Save and display the signed document.
                // 

                xmlDoc.Save("test1.xml");
                tbxDigitalSignature.Text = xmlDoc.OuterXml;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public static void SignXml(XmlDocument Doc, RSA Key)
        {
            // Check arguments.
            if (Doc == null)
                throw new ArgumentException("Doc");
            if (Key == null)
                throw new ArgumentException("Key");

            try
            {
                // Create a SignedXml object to generate signature.
                SignedXml signedXml = new SignedXml(Doc);

                // Add the key to the SignedXml document
                signedXml.SigningKey = Key;

                // Create a reference to be signed
                Reference reference = new Reference();
                reference.Uri = "";

                // Add an enveloped transformation to the reference
                XmlDsigEnvelopedSignatureTransform env =
                    new XmlDsigEnvelopedSignatureTransform();
                reference.AddTransform(env);

                // Add the reference to the SignedXml object
                signedXml.AddReference(reference);

                // Compute the signature
                signedXml.ComputeSignature();

                // Get the XML representation of the signature and save
                // it to an XmlElement object.
                XmlElement xmlDigitalSignature = signedXml.GetXml();

                // Append the element to the XML document.
                Doc.DocumentElement.AppendChild(Doc.ImportNode(xmlDigitalSignature, true));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void btnVerify_Click(object sender, EventArgs e)
        {
            /////////////////////////////////////////////////////////////////////
            // Create a new RSA signing key and import public key for 
            // verification.
            //

            RSACryptoServiceProvider rsaKey = new RSACryptoServiceProvider();
            rsaKey.FromXmlString(publicOnlyKey);


            /////////////////////////////////////////////////////////////////////
            // Load the signed XML, and call VerifyXml to verify the signature of 
            // the signed XML.
            // 

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load("test1.xml");

            bool result = VerifyXml(xmlDoc, rsaKey);

            if (result)
            {
                MessageBox.Show("The XML signature is valid.");
            }
            else
            {
                MessageBox.Show("The XML signature is not valid.");
            }
        }

        public static Boolean VerifyXml(XmlDocument Doc, RSA Key)
        {
            // Check arguments.
            if (Doc == null)
                throw new ArgumentException("Doc");
            if (Key == null)
                throw new ArgumentException("Key");


            /////////////////////////////////////////////////////////////////////
            // Create a SignedXml object to verify the signature
            //

            SignedXml signedXml = new SignedXml(Doc);

            // Find Signature node and create a new XmlNodeList object
            XmlNodeList nodeList = Doc.GetElementsByTagName("Signature");

            // Throw an exception if no signature was found.
            if (nodeList.Count <= 0)
            {
                throw new CryptographicException(
                    "Verification failed:" +
                    " No Signature was found in the document.");
            }

            // This example only supports one signature for entire XML document
            if (nodeList.Count >= 2)
            {
                throw new CryptographicException(
                    "Verification failed: More that one signature was found.");
            }

            // Load the first <signature> node.  
            signedXml.LoadXml((XmlElement)nodeList[0]);

            // Check the signature and return the result.
            return signedXml.CheckSignature(Key);
        }

        private void btnChangeXML_Click(object sender, EventArgs e)
        {
            // Modify the value of the Xml document for test. 

            XDocument xDoc = XDocument.Load("test1.xml");

            if (xDoc != null)
            {
                xDoc.Element("invoice").Element("items").
                    Element("creditcard").Element("number").SetValue("19834210");

                xDoc.Save("test1.xml");

                tbxModifiedMessage.Text = xDoc.ToString();
            }
        }
    }
}
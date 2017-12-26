/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSXmlEncryption
* Copyright (c) Microsoft Corporation.
* 
* CSXmlEncryption demonstrates how to use .NET built-in classes to encrypt 
* and decrypt Xml document:
* 
* Use .NET built-in classes to encrypt and decrypt Xml document.
* 
* A. Creates a cryptographic provider object which supplies public/private
*    key pair.
* B. Creates a separate session key using symmetric algorithm.
* C. Uses the session key to encrypt the XML document and then uses public 
*    key of stepA f to encrypt the session key.
* D. Saves the encrypted AES session key and the encrypted XML data to the 
*    XML document within a new <EncryptedData> element.
* E. To decrypt the XML element, we retrieve the private key of stepA, use 
*    it to decrypt the session key, and then use the session key to decrypt 
*    the document.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/14/2009 12:00 AM Riquel Dong Created
\***************************************************************************/

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


namespace CSXmlEncryption
{
    public partial class MainForm : Form
    {
        // XmlDocument object for encryption and decryption
        private XmlDocument xmlDoc;

        public MainForm()
        {
            InitializeComponent();
        }

        private void btnEncrypt_Click(object sender, EventArgs e)
        {
            /////////////////////////////////////////////////////////////////////
            // Create a new RSA key and save it in the container. 
            // This key will encrypt a symmetric key, which 
            // Will then be encryped in the XML document
            //

            CspParameters cspParams = new CspParameters();
            cspParams.KeyContainerName = "XML_ENC_RSA_KEY";
            RSACryptoServiceProvider rsaKey =
                new RSACryptoServiceProvider(cspParams);
            this.tbxRSAParameters.Text = rsaKey.ToXmlString(true);
            try
            {
                /////////////////////////////////////////////////////////////////
                // Call Encrypt subroutine to encrypt the "creditcard" element
                // 

                Encrypt(xmlDoc, "creditcard", "EncryptedElement1", rsaKey, "rsaKey");

                // Save the XML document.
                xmlDoc.Save("test.xml");

                // Display message of "Encrypted XML" to user
                MessageBox.Show("Encrypted XML");

                tbxCipherTextasText.Text = xmlDoc.OuterXml;

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                // Clear the RSA key.
                rsaKey.Clear();
            }
        }

        private void btnDecrypt_Click(object sender, EventArgs e)
        {
            // Create an XmlDocument object
            XmlDocument xmlDoc = new XmlDocument();
            // Load an XML file into the XmlDocument object.
            try
            {
                xmlDoc.Load("test.xml");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            CspParameters cspParams = new CspParameters();
            cspParams.KeyContainerName = "XML_ENC_RSA_KEY";


            //////////////////////////////////////////////////////////////////////
            // Get the RSA key from the key container.  This key will decrypt
            // a symmetric key that was imbedded in the XML document
            //

            RSACryptoServiceProvider rsaKey =
                new RSACryptoServiceProvider(cspParams);

            try
            {
                //////////////////////////////////////////////////////////////////
                // Call Decrypt subroutine to decrypt data
                // 

                // Decrypt the elements.
                Decrypt(xmlDoc, rsaKey, "rsaKey");
                // Save the XML document.
                xmlDoc.Save("test.xml");
                // Display the encrypted XML to the console.
                MessageBox.Show("Decrypted XML:");
                tbxRecoveredPlaintext.Text = xmlDoc.OuterXml;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                // Clear the RSA key.
                rsaKey.Clear();
            }
        }

        private void XMLEncryptDecryptForm_Load(object sender, EventArgs e)
        {
            GenerateXmlDocument();
            tbxPlaintext.Text = xmlDoc.OuterXml;
        }
        /// <summary>
        /// Generate XmlDocument 
        /// </summary>
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

        public static void Encrypt(XmlDocument Doc, string ElementToEncrypt,
            string EncryptionElementID, RSA Alg, string KeyName)
        {
            //////////////////////////////////////////////////////////////////////
            // Check the arguments
            // 

            if (Doc == null)
                throw new ArgumentNullException("Doc");
            if (ElementToEncrypt == null)
                throw new ArgumentNullException("ElementToEncrypt");
            if (EncryptionElementID == null)
                throw new ArgumentNullException("EncryptionElementID");
            if (Alg == null)
                throw new ArgumentNullException("Alg");
            if (KeyName == null)
                throw new ArgumentNullException("KeyName");

            // Find the specified element in the XmlDocument object and create a
            // new XmlElemnt object.
            XmlElement elementToEncrypt =
                Doc.GetElementsByTagName(ElementToEncrypt)[0] as XmlElement;

            // Throw an XmlException if the element was not found.
            if (elementToEncrypt == null)
            {
                throw new XmlException("The specified element was not found");
            }

            RijndaelManaged sessionKey = null;
            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a new instance of the EncryptedXml class and use it
                // to encrypt the XmlElement with the a new random symmetric key
                // 

                // Create a 256 bit Rijndael key.
                sessionKey = new RijndaelManaged();
                sessionKey.KeySize = 256;

                EncryptedXml eXml = new EncryptedXml();

                byte[] encryptedElement = eXml.EncryptData(elementToEncrypt,
                    sessionKey, false);


                /////////////////////////////////////////////////////////////////
                // Construct an EncryptedData object and populate it with the 
                // desired encryption information.
                // 

                EncryptedData edElement = new EncryptedData();
                edElement.Type = EncryptedXml.XmlEncElementUrl;
                edElement.Id = EncryptionElementID;


                /////////////////////////////////////////////////////////////////
                // Create an EncryptionMethod element so that the receiver knows 
                // which algorithm to use for decryption.
                //

                edElement.EncryptionMethod =
                    new EncryptionMethod(EncryptedXml.XmlEncAES256Url);
                // Encrypt the session key and add it to an EncryptedKey element.
                EncryptedKey ek = new EncryptedKey();
                byte[] encryptedKey =
                    EncryptedXml.EncryptKey(sessionKey.Key, Alg, false);
                ek.CipherData = new CipherData(encryptedKey);
                ek.EncryptionMethod = new EncryptionMethod(EncryptedXml.XmlEncRSA15Url);


                /////////////////////////////////////////////////////////////////
                // Create a new DataReference element for the KeyInfo element.This 
                // optional element specifies which EncryptedData uses this key. 
                // An XML document can have multiple EncryptedData elements that use
                // different keys.
                //

                DataReference dRef = new DataReference();
                // Specify the EncryptedData URI.
                dRef.Uri = "#" + EncryptionElementID;
                // Add the DataReference to the EncryptedKey.
                ek.AddReference(dRef);
                // Add the encrypted key to the EncryptedData object.
                edElement.KeyInfo.AddClause(new KeyInfoEncryptedKey(ek));
                // Set the KeyInfo element to specify the name of the RSA key.
                // Create a new KeyInfo element.
                edElement.KeyInfo = new KeyInfo();
                // Create a new KeyInfoName element.
                KeyInfoName kin = new KeyInfoName();
                // Specify a name for the key.
                kin.Value = KeyName;
                // Add the KeyInfoName element to the EncryptedKey object.
                ek.KeyInfo.AddClause(kin);
                // Add the encrypted key to the EncryptedData object.
                edElement.KeyInfo.AddClause(new KeyInfoEncryptedKey(ek));


                /////////////////////////////////////////////////////////////////
                // Add the encrypted element data to the EncryptedData object.
                //

                edElement.CipherData.CipherValue = encryptedElement;
                // Replace the element from the original XmlDocument object with 
                // the EncryptedData element           
                EncryptedXml.ReplaceElement(elementToEncrypt, edElement, false);
            }
            catch (Exception e)
            {
                // re-throw the exception.
                throw e;
            }
            finally
            {
                if (sessionKey != null)
                {
                    sessionKey.Clear();
                }
            }
        }

        public static void Decrypt(XmlDocument Doc, RSA Alg, string KeyName)
        {
            //////////////////////////////////////////////////////////////////////
            // Check the arguments
            //

            if (Doc == null)
                throw new ArgumentNullException("Doc");
            if (Alg == null)
                throw new ArgumentNullException("Alg");
            if (KeyName == null)
                throw new ArgumentNullException("KeyName");


            //////////////////////////////////////////////////////////////////////
            // Create a new EncryptedXml object to decrypt data
            //

            EncryptedXml exml = new EncryptedXml(Doc);
            // Add a key-name mapping.
            // This method can only decrypt documents
            // that present the specified key name.
            exml.AddKeyNameMapping(KeyName, Alg);
            // Decrypt the element.
            exml.DecryptDocument();
        }
    }
}
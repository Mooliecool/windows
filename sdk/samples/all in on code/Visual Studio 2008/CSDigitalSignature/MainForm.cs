/******************************** Module Header ********************************\
* Module Name:  DigitalSignatureForm.cs
* Project:      CSDigitalSignature
* Copyright (c) Microsoft Corporation.
* 
* Digital Signatures:
* We take the original message and create a message digest by applying hash
* algorithm on the message. The message digest is then encrypted using the 
* private key known only to the private key owner (i.e.sender).The signed
* message is formed by concatenating the original message with the unique 
* digital signature and the public key that is associated with private key 
* that produced that signature. This entire signed message is then sent to 
* desired recipient.

* The received signed message is broken into its three components: original
* message, the public key, and the digital signature. For comparison against 
* the hash of the original message, it is necessary to compute the hash of the 
* received message. If the message digest has not changed, you can be very
* confident that the message itself has not changed. On the other hand, if
* message digest has changed, then you can be quite certain that the received 
* message has been corrupted or tampered with.
* 
* This Form demonstrates how to use RSA algorithm to implement Digital 
* signature. I refer to some code from the book .NET Security and 
* Cryptography.
* 
* Code Logic:
* 1 Input data into the textbox control to create message object
* 2 Serialize message object to get message degest
* 3 Encrypt the message digest to get the digital signature via private key
* 4 Verify the digital signature to via public key
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/14/2009 10:00 AM Riquel Dong Created
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
using System.Security.Cryptography;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
#endregion


namespace CSDigitalSignature
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void DigitalSignatureForm_Load(object sender, EventArgs e)
        {
            toolTip1.AutoPopDelay = 5000;
            toolTip1.InitialDelay = 1000;
            toolTip1.ReshowDelay = 500;
            // Force the ToolTip text to be displayed
            toolTip1.ShowAlways = true;

            // Set up the ToolTip text for the Button to notify user to change 
            // original text        
            toolTip1.SetToolTip(this.btnVerify,
                "try changing original message first!");
            btnVerify.Enabled = false;
        }

        // Byte array variable as the RSA signature for specified hash value
        byte[] signatureBytes;
        // Standard parameters for the RSA algorithm.
        RSAParameters rsaParas;


        /// <summary>
        /// Use asymmetric algorithm to create digital signature.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSign_Click(object sender, EventArgs e)
        {
            int number = 0;
            if (string.IsNullOrEmpty(tbxInputMessage.Text.Trim()) ||
                !int.TryParse(tbxInputInterger.Text, out number))
            {
                MessageBox.Show("Invalid parameters!");
            }

            try
            {
                /////////////////////////////////////////////////////////////////
                // Declare message object for digital signature.
                // 

                Message msg = new Message();
                msg.Num = number;
                msg.MessageBody = tbxInputMessage.Text.Trim();
                byte[] messagebytes;

                // Serialize message object               
                BinaryFormatter bf = new BinaryFormatter();
                using (MemoryStream stream = new MemoryStream())
                {
                    bf.Serialize(stream, msg);
                    messagebytes = stream.ToArray();
                }


                ///////////////////////////////////////////////////////////////// 
                // Compute the SHA256 hash to get message digest.
                // 

                // Computes the hash value for the byte array
                SHA256 sha256 = new SHA256CryptoServiceProvider();
                byte[] hashbytes = sha256.ComputeHash(messagebytes);

                // Display message digest in textbox control
                StringBuilder sb = new StringBuilder();
                for (Int32 i = 0; i < hashbytes.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", hashbytes[i]));
                }
                tbxSHA256HashBytes.Text = sb.ToString();


                ///////////////////////////////////////////////////////////////// 
                // Create a RSA object to compute digital signature.
                // 

                RSACryptoServiceProvider rsa =
                    new RSACryptoServiceProvider();
                // Computes the signature for hash value by encrypting
                signatureBytes = rsa.SignHash(hashbytes,
                    "2.16.840.1.101.3.4.2.1");


                ///////////////////////////////////////////////////////////////// 
                // Update the UI.
                // 

                // Display digital signature in textbox control
                sb = new StringBuilder();
                for (Int32 i = 0; i < signatureBytes.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", signatureBytes[i]));
                }
                tbxDigitalSignature.Text = sb.ToString();

                // Exports the RSAParameters to contain the public key
                rsaParas = rsa.ExportParameters(false);

                // Displayes the Exponent parameter for the RSA algorithm
                sb = new StringBuilder();
                for (Int32 i = 0; i < rsaParas.Exponent.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", rsaParas.Exponent[i]));
                }
                Exponenttbx.Text = sb.ToString();

                // Displayes the Modulus parameter for the RSA algorithm
                sb = new StringBuilder();
                for (Int32 i = 0; i < rsaParas.Modulus.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", rsaParas.Modulus[i]));
                }
                tbxModulus.Text = sb.ToString();

                // Update UI
                btnVerify.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        /// <summary>
        /// Use asymmetric algorithm to verify digital signature.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnVerify_Click(object sender, EventArgs e)
        {
            int number = 0;
            if (string.IsNullOrEmpty(tbxInputMessage.Text.Trim()) ||
                !int.TryParse(tbxInputInterger.Text, out number))
            {
                MessageBox.Show("Invalid parameters!");
            }

            try
            {
                /////////////////////////////////////////////////////////////////
                // Declare message object for digital signature.
                // 

                Message msg = new Message();
                msg.Num = number;
                msg.MessageBody = tbxInputMessage.Text.Trim();


                /////////////////////////////////////////////////////////////////
                // Serialize the message object.
                // 

                byte[] messagebytes;
                BinaryFormatter bf = new BinaryFormatter();
                using (MemoryStream stream = new MemoryStream())
                {
                    bf.Serialize(stream, msg);
                    messagebytes = stream.ToArray();
                }

                /////////////////////////////////////////////////////////////////
                // Compute the SHA256 hash to get message digest.
                // 

                SHA256 sha256 = new SHA256CryptoServiceProvider();
                byte[] hashbytes = sha256.ComputeHash(messagebytes);


                /////////////////////////////////////////////////////////////////
                // Create a RSA object to verify digital signature 
                // 

                RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();

                // Imports publick key to verify the digital signature
                rsa.ImportParameters(rsaParas);
                // Verifies signature data
                bool match = rsa.VerifyHash(hashbytes, "2.16.840.1.101.3.4.2.1",
                    signatureBytes);

                if (match)
                {
                    MessageBox.Show("Verification succeeds");
                }
                else
                {
                    MessageBox.Show("Verification fails");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }

    [Serializable]
    public class Message
    {
        private string _messageBody;
        private int _num;
        public string MessageBody
        {
            get { return _messageBody; }
            set { _messageBody = value; }
        }
        public int Num
        {
            get { return _num; }
            set { _num = value; }
        }
    }
}
/******************************** Module Header ********************************\
* Module Name:	AsymmetricsAlgorithmForm.cs
* Project:		CSEncryption
* Copyright (c) Microsoft Corporation.
* 
* Asymmetric algorithms:
* It uses a public and private key pair to encrypt and decrypt data. The public
* key is made available to anyone and is used to encrypt data to be sent to 
* the owner of the private key. The private key, as the name implies, is kept 
* private. The private key is used to decrypt the data and will only work if 
* the correct public key was used when encrypting the data. The private key is
* the only key that will allow data encrypted with the public key to be 
* decrypted. The following public-key algorithms are available for use in 
* the .NET Framework:
*  RSA
*  DSA
* 
* This Form demonstrates how to use RSA algorithm to encrypt &
* decrypt data. I refer to some code from the book .NET Security and 
* Cryptography.
* 
* Code Logic:
* 1 Input the data into textbox so as to create message object 
* 2 Click encrypt button
* 2.1 Serialize the message object to get plaintext data as byte array
* 2.2 Encrypt data by using RSACryptoServiceProvider object
* 2.3 Update UI display
* 3 Click decrypt button to decrypt) data
* 3.1 Create RSACryptoServiceProvider object. Initialize it with key, IV,etc
* 3.2 Decrypt ciphered data to get plain text
* 3.3 Update UI display
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/14/2009 09:00 AM Riquel Dong Created
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
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
#endregion


namespace CSEncryption
{
    public partial class AsymmetricAlgorithmForm : Form
    {
        // Byte array to contain ciphered message   
        List<byte[]> cipherArray;
        List<byte[]> recoveredArray;

        public AsymmetricAlgorithmForm()
        {
            InitializeComponent();
            cipherArray = new List<byte[]>();
            recoveredArray = new List<byte[]>();
        }

        /// <summary>
        /// Generate the public and private key for data encryption and decryption
        /// </summary>
        private void GenerateNewRSAParammeters()
        {
            try
            {
                // Create a RSA obj to perform asymmetric RSA algorithm 
                RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();

                // Display publicprivate key in textbox control
                tbxRSAParameters.Text = rsa.ToXmlString(true);

                // Save the public and private keys to a XML file
                using (StreamWriter sw = new StreamWriter("PublicPrivateKey.xml"))
                {
                    sw.Write(tbxRSAParameters.Text);
                }

                // Save the public key to a XML file for encryption
                using (StreamWriter sw = new StreamWriter("PublicOnlyKey.xml"))
                {
                    sw.Write(rsa.ToXmlString(false));
                }

                // Update UI 
                ClearOutputFields();
                cipherArray.Clear();
                recoveredArray.Clear();
                btnEncrypt.Enabled = true;
                btnDecrypt.Enabled = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// Clear output value in textbox
        /// </summary>
        private void ClearOutputFields()
        {
            tbxIntegerPart.Text = string.Empty;
            tbxMessagePart.Text = string.Empty;
            tbxCiphertextasByteArray.Text = string.Empty;
            tbxPlaintextasBytesArray.Text = string.Empty;
        }

        /// <summary>
        /// Encrypt plaintext via public key. 
        /// This method is used just for demo. In real scenario we use symmetric
        /// algorithm to encrypt data and use asymmetric algorithm to encrypt the
        /// secret key used by symmetric algorithm
        /// </summary>    
        private void btnEncrypt_Click(object sender, EventArgs e)
        {
            int number = 0;
            if (string.IsNullOrEmpty(tbxInputMessage.Text) ||
                !int.TryParse(tbxInputInterger.Text, out number))
            {
                MessageBox.Show("Invalid parameters!");
                return;
            }

            // Clear old output in the textbox control
            ClearOutputFields();
            cipherArray.Clear();
            Message msg = new Message();
            msg.Num = number;
            msg.MessageBody = tbxInputMessage.Text.Trim();

            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a RSACryptoServiceProvider object and initialize it 
                // with a public key.
                // 

                RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();
                String publicOnlyKeyXml = string.Empty;
                using (StreamReader sr = new StreamReader("PublicOnlyKey.xml"))
                {
                    publicOnlyKeyXml = sr.ReadToEnd();
                }
                rsa.FromXmlString(publicOnlyKeyXml);


                /////////////////////////////////////////////////////////////////
                // Serialize message object to get plaintext as byte array.
                // 

                BinaryFormatter bf = new BinaryFormatter();
                byte[] plainBytes = null;
                using (MemoryStream stream = new MemoryStream())
                {
                    bf.Serialize(stream, msg);
                    plainBytes = stream.ToArray();
                }


                /////////////////////////////////////////////////////////////////
                // Encrypt plaintext data via public key. Because there is the
                // length limit in asymmetric algorithm, I split byte array to
                // encrypt every small part byte array. In real scenario we 
                // should use symmetric algorithm to encrypt large quantities
                // of data and use asymmetric algorithm to encrypt secret key 
                // used by symmetric algorithm.
                // 

                int index = 0;
                int length = 0;
                if (plainBytes.Length > 80)
                {
                    byte[] partPlainBytes;
                    byte[] cipherbytes;
                    while (plainBytes.Length - index > 0)
                    {
                        if (plainBytes.Length - index > 80)
                        {
                            partPlainBytes = new byte[80];
                        }
                        else
                        {
                            partPlainBytes = new byte[plainBytes.Length - index];
                        }
                        for (int i = 0; i < 80 && (index + i) < plainBytes.Length; i++)
                            partPlainBytes[i] = plainBytes[index + i];
                        cipherbytes = rsa.Encrypt(partPlainBytes, false);
                        length += cipherbytes.Length;
                        cipherArray.Add(cipherbytes);
                        index += 80;
                    }
                }


                /////////////////////////////////////////////////////////////////
                // Display plaintext and ciphered data in UI.
                // 

                byte[] cipheredPlaintext = new byte[length];
                index = 0;
                for (int i = 0; i < cipherArray.Count; i++)
                {
                    for (int j = 0; j < cipherArray[i].Length; j++)
                    {
                        cipheredPlaintext[index + j] = cipherArray[i][j];
                    }
                    index += cipherArray[i].Length;
                }

                // Display ciphered message in textbox control as byte array
                StringBuilder sb = new StringBuilder();
                for (Int32 i = 0; i < cipheredPlaintext.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", cipheredPlaintext[i]));
                }
                tbxCiphertextasByteArray.Text = sb.ToString();

                // Display plaintext in textbox control as byte array
                sb = new StringBuilder();
                for (Int32 i = 0; i < plainBytes.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", plainBytes[i]));
                }
                tbxPlaintextasBytesArray.Text = sb.ToString();
                btnDecrypt.Enabled = true;
                btnEncrypt.Enabled = false;
            }
            catch (Exception ex)
            {
                // Display the error information to user
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// Decrypt the ciphered data via private key
        /// </summary>
        private void btnDecrypt_Click(object sender, EventArgs e)
        {
            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a RSACryptoServiceProvider object and initialize it 
                // with a public-private key pair.
                // 

                RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();
                String publicPrivateKeyXml = string.Empty;
                using (StreamReader sr = new StreamReader("PublicPrivateKey.xml"))
                {
                    publicPrivateKeyXml = sr.ReadToEnd();
                }
                rsa.FromXmlString(publicPrivateKeyXml);
                recoveredArray.Clear();


                /////////////////////////////////////////////////////////////////
                // Get cipherd data as byte array and use the 
                // RSACryptoServiceProvider object to decrypt it.
                //

                int length = 0;
                for (int i = 0; i < cipherArray.Count; i++)
                {
                    byte[] partRecoveredPlainBytes = rsa.Decrypt(cipherArray[i], false);
                    recoveredArray.Add(partRecoveredPlainBytes);
                    length += partRecoveredPlainBytes.Length;
                }
                byte[] recoveredPlaintext = new byte[length];
                int index = 0;
                for (int i = 0; i < recoveredArray.Count; i++)
                {
                    for (int j = 0; j < recoveredArray[i].Length; j++)
                    {
                        recoveredPlaintext[index + j] = recoveredArray[i][j];
                    }
                    index += recoveredArray[i].Length;
                }


                /////////////////////////////////////////////////////////////////
                // Deserialize plaintext data to create message object and 
                // Display the data in UI.
                // 

                BinaryFormatter bf = new BinaryFormatter();
                using (MemoryStream stream = new MemoryStream())
                {
                    stream.Write(recoveredPlaintext, 0, recoveredPlaintext.Length);
                    stream.Position = 0;
                    Message msgobj = (Message)bf.Deserialize(stream);
                    tbxIntegerPart.Text = msgobj.Num.ToString();
                    tbxMessagePart.Text = msgobj.MessageBody;
                }

                btnDecrypt.Enabled = false;
                btnEncrypt.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void btnNewRSAParameters_Click(object sender, EventArgs e)
        {
            GenerateNewRSAParammeters();
        }

        private void tbxInputInterger_TextChanged(object sender, EventArgs e)
        {
            ClearOutputFields();
            btnDecrypt.Enabled = false;
            btnEncrypt.Enabled = true;
        }

        private void tbxInputMessage_TextChanged(object sender, EventArgs e)
        {
            ClearOutputFields();
            btnDecrypt.Enabled = false;
            btnEncrypt.Enabled = true;
        }

        private void AsymmetricAlgorithmForm_Load(object sender, EventArgs e)
        {
            GenerateNewRSAParammeters();
            btnEncrypt.Enabled = true;
            btnDecrypt.Enabled = false;
        }
    }
}
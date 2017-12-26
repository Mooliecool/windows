/******************************** Module Header ********************************\
* Module Name:	SymmetricsAlgorithmsForm.cs
* Project:		CSEncryption
* Copyright (c) Microsoft Corporation.
* 
* Symmetric algorithms:
* A symmetric algorithm is a cipher in which encryption and decryption use the 
* same key or keys that are mathematically related to one another in such a 
* way that it is easy to compute one key from knowledge of the other, which is 
* effectively a single key.Some symmetric algorithms commonly used are DES,
* TripleDES, RC2,RijnDael.
* 
* This Form demonstrates how to use DES,RC2,TripleDES and Rijndael to encrypt &
* decrypt string. I refer to some code from the book .NET Security and 
* Cryptography.
* 
* Code Logic:
* 1 Select one symmetric algorithm
* 2 Input the data into textbox so as to create message object 
* 3 Click encrypt button
* 3.1 Serialize the message object to get plaintext data as byte array
* 3.2 Encrypt data by using SymmetricAlgorithm Provider
* 3.3 Update UI display
* 4 Click decrypt button to decrypt) data
* 4.1 Create SymmetricAlgorithm Provider and initialize it with key, IV,etc
* 4.2 Decrypt ciphered data to get plain text
* 4.3 Update UI display
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/10/2009 06:50 PM Riquel Dong Created
\*******************************************************************************/

#region Using directives
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
    public partial class SymmetricAlgorithmForm : Form
    {
        // One byte array variable as a key to use for the algorithm
        byte[] key;

        // The initialization vector (IV) variable for the symmetric algorithm
        byte[] initVector;

        // Specifies the block cipher mode to use for encryption
        CipherMode cipherMode;

        // Specifies the type of padding
        PaddingMode paddingMode;

        // Byte array to contain ciphered message
        byte[] cipherBytes;

        /// <summary>
        /// Bases on combobox current value to set cipher mode
        /// </summary>
        private void SetCipherMode()
        {
            switch (comboboxMode.Text.Trim())
            {
                case "ECB":
                    cipherMode = CipherMode.ECB;
                    break;
                case "CBC":
                    cipherMode = CipherMode.CBC;
                    break;
                case "CFB":
                    cipherMode = CipherMode.CFB;
                    break;
                case "OFB":
                    cipherMode = CipherMode.OFB;
                    break;
                case "CTS":
                    cipherMode = CipherMode.CTS;
                    break;
            }
        }

        /// <summary>
        /// Based on combobox current value to set padding mode
        /// </summary>
        private void SetPaddingMode()
        {
            switch (comboboxPadding.Text.Trim())
            {
                case "PKCS7":
                    paddingMode = PaddingMode.PKCS7;
                    break;
                case "Zeros":
                    paddingMode = PaddingMode.Zeros;
                    break;
                default:
                    paddingMode = PaddingMode.None;
                    break;
            }
        }

        /// <summary>
        /// Based on user selection to create a cryptographic object to 
        /// perform the according Symmetric algorithm.
        /// </summary>
        SymmetricAlgorithm GetSymmetricAlgorithmProvider()
        {
            //create new instance of symmetric algorithm
            if (this.radiobtnRC2.Checked == true)
                return RC2.Create();
            if (this.radiobtnRijndael.Checked == true)
                return Rijndael.Create();
            if (this.radiobtnDES.Checked == true)
                return DES.Create();
            if (this.radiobtnTrippleDES.Checked == true)
                return TripleDES.Create();
            return null;
        }

        /// <summary>
        /// Generates a random key (Key) to use for the algorithm
        /// </summary>
        private void GenRandomKey()
        {
            // Generate new random IV
            SymmetricAlgorithm sa =
                GetSymmetricAlgorithmProvider();
            sa.GenerateKey();
            key = sa.Key;

            // Do UI stuff
            UpdateKeyTextBox();
            ClearOutputFields();
        }

        /// <summary>
        /// Display key in textbox control
        /// </summary>
        private void UpdateKeyTextBox()
        {
            StringBuilder sb = new StringBuilder();
            for (Int32 i = 0; i < key.Length; i++)
            {
                sb.Append(string.Format("{0:X2} ", key[i]));
            }
            this.tbxRandomKey.Text = sb.ToString();
        }

        /// <summary>
        /// Display IV in textbox control
        /// </summary>
        private void UpdateIVTextBox()
        {
            StringBuilder sb = new StringBuilder();
            for (Int32 i = 0; i < initVector.Length; i++)
            {
                sb.Append(string.Format("{0:X2} ", initVector[i]));
            }
            this.tbxRandomInitVector.Text = sb.ToString();
        }

        /// <summary>
        /// Clear output value in textbox
        /// </summary>
        private void ClearOutputFields()
        {
            tbxIntegerPart.Text = string.Empty;
            tbxMessagePart.Text = string.Empty;
            tbxcipherTextasByteArray.Text = string.Empty;
            tbxPlaintextasBytesArray.Text = string.Empty;
        }

        /// <summary>
        /// Generates a random IV to use for the algorithm
        /// </summary>
        private void GenRandomIV()
        {
            // Generate new random IV
            SymmetricAlgorithm sa =
                GetSymmetricAlgorithmProvider();
            sa.GenerateIV();
            initVector = sa.IV;

            // Do UI stuff
            UpdateIVTextBox();
            ClearOutputFields();
        }

        public SymmetricAlgorithmForm()
        {
            InitializeComponent();
        }

        private void SymmetricAlgorithmForm_Load(object sender, EventArgs e)
        {
            // Populate the combobox for cipher mode
            comboboxMode.Items.AddRange(new
                object[] { "ECB", "CBC", "CFB", "OFB", "CTS" });
            comboboxMode.SelectedIndex = 1;

            // Populate the combobox to specify padding mode
            comboboxPadding.Items.AddRange(new object[] { "PKCS7", "Zeros", "None" });
            comboboxPadding.SelectedIndex = 1;

            // Set TextBox to readonly to prevent editting
            tbxcipherTextasByteArray.ReadOnly = true;
            tbxPlaintextasBytesArray.ReadOnly = true;
            radiobtnDES.Checked = true;
            GenRandomIV();
            GenRandomKey();

            // Specifies cipher mode
            SetCipherMode();

            // Specifies padding mode 
            SetPaddingMode();
            btnDecrypt.Enabled = false;
        }

        private void btnNewRandomKey_Click(object sender, EventArgs e)
        {
            // Generates a random key (Key) to use for the algorithm
            GenRandomKey();
            ClearOutputFields();
            btnDecrypt.Enabled = false;
            btnEncrypt.Enabled = true;
        }

        private void btnNewRandomInitVector_Click(object sender, EventArgs e)
        {
            // Generates a random initialization vector (IV)
            GenRandomIV();
            ClearOutputFields();
            btnDecrypt.Enabled = false;
            btnEncrypt.Enabled = true;
        }

        private void btnEncrypt_Click(object sender, EventArgs e)
        {
            int number = 0;
            if (string.IsNullOrEmpty(tbxInputMessage.Text) ||
                !int.TryParse(tbxInputInterger.Text, out number))
            {
                MessageBox.Show("Invalid parameters!");
                return;
            }

            // Clear textbox control so as to display new output
            ClearOutputFields();
            Message msg = new Message();
            msg.Num = number;
            msg.MessageBody = tbxInputMessage.Text.Trim();

            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a cryptographic provider object
                // 

                SymmetricAlgorithm sa = GetSymmetricAlgorithmProvider();
                // Sets the secret key for the symmetric algorithm
                sa.Key = key;
                // Sets the IV for the symmetric algorithm
                sa.IV = initVector;
                // Sets the mode for operation of the symmetric algorithm
                sa.Mode = cipherMode;
                // Sets the padding mode used in the symmetric algorithm
                sa.Padding = paddingMode;


                /////////////////////////////////////////////////////////////////
                // Serialize message object to get plain text               
                // 
                BinaryFormatter bf = new BinaryFormatter();
                byte[] plainBytes;
                using (MemoryStream stream = new MemoryStream())
                {
                    bf.Serialize(stream, msg);
                    plainBytes = stream.ToArray();
                }


                /////////////////////////////////////////////////////////////////
                // Encryt plaintext to get ciphered data                
                //

                // Defines a stream object to get ciphered data
                MemoryStream ms = new MemoryStream();

                // Defines a stream for cryptographic transformations
                CryptoStream cs = new CryptoStream(ms,
                    sa.CreateEncryptor(), CryptoStreamMode.Write);

                // Writes a sequence of bytes for encrption
                cs.Write(plainBytes, 0, plainBytes.Length);

                // Closes the current stream and releases any resources 
                cs.Close();
                // Save the ciphered message into one byte array
                cipherBytes = ms.ToArray();
                // Closes the memorystream object
                ms.Close();


                /////////////////////////////////////////////////////////////////
                // Display ciphered message in textbox control as byte array
                //

                StringBuilder sb = new StringBuilder();
                for (Int32 i = 0; i < cipherBytes.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", cipherBytes[i]));
                }
                tbxcipherTextasByteArray.Text = sb.ToString();

                // Display plaintext as a byte array in textbox control
                sb = new StringBuilder();
                for (Int32 i = 0; i < plainBytes.Length; i++)
                {
                    sb.Append(string.Format("{0:X2} ", plainBytes[i]));
                }
                tbxPlaintextasBytesArray.Text = sb.ToString();

                //update UI               
                radiobtnDES.Enabled = false;
                radiobtnTrippleDES.Enabled = false;
                radiobtnRC2.Enabled = false;
                radiobtnRijndael.Enabled = false;

                comboboxMode.Enabled = false;
                comboboxPadding.Enabled = false;
                btnNewRandomInitVector.Enabled = false;
                btnNewRandomKey.Enabled = false;
                btnEncrypt.Enabled = false;
                btnDecrypt.Enabled = true;
            }
            catch (Exception ex)
            {
                // Display the error information to user
                MessageBox.Show(ex.Message);
            }
        }

        private void btnDecrypt_Click(object sender, EventArgs e)
        {
            try
            {
                /////////////////////////////////////////////////////////////////
                // Create a cryptographic provider object used to decrypt data
                //

                SymmetricAlgorithm sa = GetSymmetricAlgorithmProvider();
                // Sets the secret key
                sa.Key = key;
                // Sets the IV 
                sa.IV = initVector;
                // Sets the mode 
                sa.Mode = cipherMode;
                // Sets the padding mode
                sa.Padding = paddingMode;


                /////////////////////////////////////////////////////////////////
                // Decrypt ciphered data to get plain text
                //

                byte[] plainbytes;
                // Defines a stream object to contain the decrypted data
                using (MemoryStream ms = new MemoryStream(cipherBytes))
                {
                    // Defines a stream for cryptographic transformations
                    using (CryptoStream cs = new CryptoStream(ms, sa.CreateDecryptor(),
                        CryptoStreamMode.Read))
                    {
                        // Defines one byte array for recovered plaintext
                        plainbytes = new byte[cipherBytes.Length];
                        // Reads a sequence of bytes for decryption
                        cs.Read(plainbytes, 0, cipherBytes.Length);
                    }
                }


                /////////////////////////////////////////////////////////////////
                // Deserialize data to get message object and display it in UI
                //

                Message recoveredMessage;
                using (MemoryStream stream = new MemoryStream(plainbytes, false))
                {
                    BinaryFormatter bf = new BinaryFormatter();
                    recoveredMessage = (Message)bf.Deserialize(stream);
                }

                // Display the recovered message in textbox control
                this.tbxIntegerPart.Text = recoveredMessage.Num.ToString();
                this.tbxMessagePart.Text = recoveredMessage.MessageBody;

                // Update UI            
                comboboxMode.Enabled = true;
                comboboxPadding.Enabled = true;
                btnNewRandomInitVector.Enabled = true;
                btnNewRandomKey.Enabled = true;
                radiobtnDES.Enabled = true;
                radiobtnTrippleDES.Enabled = true;
                radiobtnRC2.Enabled = true;
                radiobtnRijndael.Enabled = true;
                btnEncrypt.Enabled = true;
                btnDecrypt.Enabled = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        /// <summary>
        /// Based on user seleciton to create a cryptographic object to perform
        /// the according Symmetric algorithm 
        /// </summary>  
        private void radiobtnSymmetricAlgorithm_CheckedChanged(object sender, EventArgs e)
        {
            if (((RadioButton)sender).Checked)
            {
                SymmetricAlgorithm sa = GetSymmetricAlgorithmProvider();
                key = sa.Key;
                initVector = sa.IV;
                UpdateKeyTextBox();
                UpdateIVTextBox();
                ClearOutputFields();
            }
        }

        private void comboboxMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetCipherMode();
        }

        private void comboboxPadding_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetPaddingMode();
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
/****************************** Module Header ******************************\
* Module Name:  CommonEncryption.aspx.cs
* Project:      CSASPNETEncryptAndDecryptConfiguration
* Copyright (c) Microsoft Corporation.
* 
*  This sample shows how to use RSA encryption algorithm API to encrypt and decrypt 
*configuration section in order to protect the sensitive information from interception
*or hijack in ASP.NET web application.
*
*  The project contains two snippets. The First One demonstrates how to use RSA provider 
*and RSA container to encrypt and decrypt some words or values in web application.
*the purpose of first snippet is to let us know the overview of RSA mechanism.
*Second one shows how to use RSA configuration provider to encrypt and decrypt
*configuration section in web.config.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
\***************************************************************************/

using System;
using System.Web.UI;
using System.Security.Cryptography;

namespace CSASPNETEncryptAndDecryptConfiguration
{
    public partial class CommonEncryption : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            AddKeyUpEventOnTextControl();
            this.btnDecrypt.PreRender += new EventHandler(btnDecrypt_PreRender);
            this.btnEncrypt.PreRender += new EventHandler(btnEncrypt_PreRender);
        }

        private void RSAEncryption()
        {
            CspParameters param = new CspParameters();
            param.KeyContainerName = "MyKeyContainer";
            using (RSACryptoServiceProvider rsa = new RSACryptoServiceProvider(param))
            {
                string plaintext = this.tbData.Text;
                byte[] plaindata = System.Text.Encoding.Default.GetBytes(plaintext);
                byte[] encryptdata = rsa.Encrypt(plaindata, false);
                string encryptstring = Convert.ToBase64String(encryptdata);
                this.tbEncryptData.Text = encryptstring;
            }
        }

        private void RSADecryption()
        {
            CspParameters param = new CspParameters();
            param.KeyContainerName = "MyKeyContainer";
            using (RSACryptoServiceProvider rsa = new RSACryptoServiceProvider(param))
            {
                byte[] encryptdata = Convert.FromBase64String(this.tbEncryptData.Text);
                byte[] decryptdata = rsa.Decrypt(encryptdata, false);
                string plaindata = System.Text.Encoding.Default.GetString(decryptdata);
                this.tbDecryptData.Text = plaindata;
            }
        }

        protected void Encrypt_Click(object sender, EventArgs e)
        {
            RSAEncryption();
        }

        protected void Decrypt_Click(object sender, EventArgs e)
        {
            RSADecryption();
        }

        void btnDecrypt_PreRender(object sender, EventArgs e)
        {
            EnableDecryptButton();
        }

        void btnEncrypt_PreRender(object sender, EventArgs e)
        {
            EnableEncryptButton();
        }

        private void AddKeyUpEventOnTextControl()
        {
            string script = string.Format(@"function PressFn(sender) {{
                                            document.getElementById('{0}').disabled = sender.value == '' ? true : false;
                                            }}", btnEncrypt.ClientID);
            tbData.Attributes["onkeyup"] = "PressFn(this)";
            Page.ClientScript.RegisterStartupScript(this.GetType(), "DataKeyUp", script, true);
        }

        private void EnableDecryptButton()
        {
            btnDecrypt.Enabled = this.tbEncryptData.Text != string.Empty ? true : false;
        }

        private void EnableEncryptButton()
        {
            btnEncrypt.Enabled = this.tbData.Text != string.Empty ? true : false;
        }

    }
}
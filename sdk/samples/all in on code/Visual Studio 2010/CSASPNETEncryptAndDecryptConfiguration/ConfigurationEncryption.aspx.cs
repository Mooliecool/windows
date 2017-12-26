/****************************** Module Header ******************************\
* Module Name:  ConfigurationEncryption.aspx.cs
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
using System.Configuration;
using System.Web.Configuration;

namespace CSASPNETEncryptAndDecryptConfiguration
{
    public partial class ConfigurationEncryption : System.Web.UI.Page
    {
        private const string provider = "RSAProtectedConfigurationProvider";  //Use RSA Provider to encrypt configuration sections

        protected void Page_Load(object sender, EventArgs e)
        {

        }

        protected void btnEncrypt_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(this.ddlSection.SelectedValue))
            {
                Response.Write("please select a configuration section");
                return;
            }

            string sectionString = this.ddlSection.SelectedValue;

            Configuration config = WebConfigurationManager.OpenWebConfiguration(Request.ApplicationPath);
            ConfigurationSection section = config.GetSection(sectionString);
            if (section != null)
            {
                section.SectionInformation.ProtectSection(provider);
                config.Save();
                Response.Write("encrypt successed, please check the configuration file.");
            }
        }

        protected void btnDecrypt_Click(object sender, EventArgs e)
        {
            string sectionString = this.ddlSection.SelectedValue;

            Configuration config = WebConfigurationManager.OpenWebConfiguration(Request.ApplicationPath);
            ConfigurationSection section = config.GetSection(sectionString);
            if (section != null && section.SectionInformation.IsProtected)
            {
                section.SectionInformation.UnprotectSection();
                config.Save();
                Response.Write("decrypt success, please check the configuration file.");
            }

        }


    }
}
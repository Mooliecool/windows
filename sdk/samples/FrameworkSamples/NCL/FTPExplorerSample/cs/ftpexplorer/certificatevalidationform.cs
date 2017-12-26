//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
using System;
using System.Net.Security;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.Windows.Forms;

namespace Microsoft.Samples.FTPExplorer
{
    partial class CertificateValidationForm : Form
    {
        X509Certificate _certificate; 
        X509Chain _chain;
        SslPolicyErrors _sslPolicyErrors;
        public CertificateValidationForm(string hostName, X509Certificate certificate,
            X509Chain chain,
            SslPolicyErrors sslPolicyErrors)
        {
            InitializeComponent();
            _certificate = certificate;
            _chain = chain;
            _sslPolicyErrors= sslPolicyErrors;
            this.Text += hostName;
            label1.Text = sslPolicyErrors.ToString()+"\n"+"Do you want to continue";
        }

        private void cmdViewCertificate_Click(object sender, EventArgs e)
        {
            X509Certificate2UI.DisplayCertificate(new X509Certificate2(_certificate.Handle));
        }
    }
}
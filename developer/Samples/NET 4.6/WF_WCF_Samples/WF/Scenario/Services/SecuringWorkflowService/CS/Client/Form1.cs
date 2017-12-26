//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel.Security;
using System.Threading;
using System.Windows.Forms;

namespace Microsoft.Samples.WF.SecuringWorkFlow.Client
{

    public partial class Form1 : Form
    {
        SecuredServiceClient proxy;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Application.ThreadException += new ThreadExceptionEventHandler(Application_ThreadException);
            proxy = new SecuredServiceClient("SecuredEndpoint");
            txtRet.Text = "What is your favorite number?";
        }

        void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            string err = e.Exception.Message + Environment.NewLine + e.Exception.StackTrace;
            if (e.Exception.InnerException != null)
                err += Environment.NewLine + e.Exception.InnerException.Message + Environment.NewLine + e.Exception.InnerException.StackTrace;
            txtRet.Text = err;
        }

        private void cmdCallWorkflowService_Click(object sender, EventArgs e)
        {
            //if the proxy isn't created or is for some reason not opened, create a new instance of the proxy
            if (proxy == null || proxy.State == System.ServiceModel.CommunicationState.Opened)
                proxy = new SecuredServiceClient("SecuredEndpoint");

            try
            {
                int response = proxy.AskQuestion(txtRet.Text).Value;
                txtAnswer.Text = response.ToString();
                MessageBox.Show("You have the magic character!");
            }
            catch (SecurityAccessDeniedException ex)
            {
                MessageBox.Show(
                "Request Denied!  You must have the magic character (see Constants.cs) in your name in order to get an answer!"
                    + "\n\t" + ex.Message.ToString() );
                txtAnswer.Text = "Denied";
            }
            
        }

    }
}

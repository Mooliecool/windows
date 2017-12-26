/****************************** Module Header ******************************\
* Module Name:  LoginForm.cs
* Project:      CSWinFormSingleInstanceApp
* Copyright (c) Microsoft Corporation.
* 
* The  sample demonstrates how to achieve the goal that only 
* one instance of the application is allowed in Windows Forms application..
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/31/2009 3:00 PM Bruce Zhou Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CSWinFormSingleInstanceApp
{
    public partial class LoginForm : Form
    {
        private Boolean isWrongPassword = false;
        #region Contructor
        public LoginForm()
        {
            InitializeComponent();
            // add event handlers
            this.Load += new EventHandler(LoginForm_Load);
            this.FormClosed += new FormClosedEventHandler(LoginForm_FormClosed);
            this.FormClosing += new FormClosingEventHandler(LoginForm_FormClosing);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.TopMost = true;
        }

        #endregion

        #region EventHandlers
        void LoginForm_Load(object sender, EventArgs e)
        {
            this.AcceptButton = this.buttonLogin;
            this.CancelButton = this.buttonCancel;
        }

        void LoginForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (isWrongPassword)
                e.Cancel = true;
        }

        void LoginForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if(!GlobleData.IsUserLoggedIn)
            Application.Exit();
        }

        private void buttonLogin_Click(object sender, EventArgs e)
        {
            string name = textBoxName.Text;
            string password = textBoxPassword.Text;
            if (name == "abc" && password == "abc")
            {
                GlobleData.IsUserLoggedIn = true;
                GlobleData.UserName = "anonymous";
                this.DialogResult = DialogResult.OK;
                isWrongPassword = false;
            }
            else
            {
                isWrongPassword = true;
                MessageBox.Show("Wrong Password!");
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
        #endregion
    }
}

/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSCustomAuthorization
* Copyright (c) Microsoft Corporation.
* 
* The .NET Framework provides an extensible framework for authorizing and
* authenticating users.
* 
* This sample demonstrates how to implement custom authentication and 
* authorization by using classes that derive from IIdentity and IPrincipal. 
* It also demonstrates how to override the application thread's default 
* identity, the Windows identity, by setting CurrentPrincipal to an instance 
* of the class that derives from IPrincipal. Based on credentials supplied 
* by the user, we can provide access to resources based on that role. 
* 
* 1. To create a class that implements IIdentity. An identity object represent
* the user on whose behalf the code is running.
* 
* 2. To create a class that implements IPrincipal. A principal object 
* represents the security context of the user on whose behalf the code is 
* running, including that user's identity (IIdentity) and any roles to which 
* they belong.
* 
* 3. Collect user informat to authenticate user. Sets the thread's current 
* principal for role-based security). 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/27/2009 18:00 AM Riquel Dong Created
\***************************************************************************/

#region Using Directive
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security;
using System.Security.Principal;
#endregion


// Based on credentials supplied by the user, we can provide access to 
// resources based on that role. If use enters TestUser in the User Name text
// box and Password in the Password text box, and then click OK. The user can
// edit data in datagridview and save data chanage. If user is not 
// authenticated, then user can view the data.

public partial class MainForm : Form
{
    private DataTable dt;

    public MainForm()
    {
        InitializeComponent();
    }

    private void MainForm_Load(object sender, EventArgs e)
    {
        // Display the login form
        LoginForm loginForm = new LoginForm();
        loginForm.ShowDialog();

        // Load data
        dt = new DataTable();
        dt.ReadXmlSchema("ProductSchema.xml");
        dt.ReadXml("Product.xml");
        
        if (System.Threading.Thread.CurrentPrincipal.Identity.IsAuthenticated)
        {
            btnSave.Enabled = true;
            dataGridView1.DataSource = dt;
        }
        else
        {
            btnSave.Enabled = false;
            dataGridView1.ReadOnly = true;
            dataGridView1.DataSource = dt;
        }
    }

    private void btnSave_Click(object sender, EventArgs e)
    {
        dt.WriteXmlSchema("ProductSchema.xml");
        dt.WriteXml("Product.xml");
    }
}

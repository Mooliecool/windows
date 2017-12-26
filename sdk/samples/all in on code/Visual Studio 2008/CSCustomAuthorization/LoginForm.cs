#region Using Directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion


// We use the login form to gather a user name and password. It can use this
// information to initialize an instance of the SampleIPrincipal class and 
// use the Thread.CurrentPrincipal object to set the current thread's identity
// to that instance
public partial class LoginForm : Form
{
    public LoginForm()
    {
        InitializeComponent();
    }

    private void OK_Click(object sender, EventArgs e)
    {
        SampleIPrincipal samplePrincipal = new SampleIPrincipal(this.tbUserName.Text, this.tbPassword.Text);
        this.tbPassword.Text = "";
        if ((!samplePrincipal.Identity.IsAuthenticated))
        {
            // The user is still not validated. 
            MessageBox.Show("The username and password pair is incorrect");
            System.Threading.Thread.CurrentPrincipal = samplePrincipal;
            this.Close();
        }
        else
        {
            // Update the current principal. 
            System.Threading.Thread.CurrentPrincipal = samplePrincipal;
            this.Close();
        }
    }

    private void Cancel_Click(object sender, EventArgs e)
    {
        this.Close();
        System.Windows.Forms.Application.Exit();
    }
}
   
//-----------------------------------------------------------------------------
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//
//-----------------------------------------------------------------------------

using System;
using System.Web.Security;

public partial class Login : System.Web.UI.Page
{
    protected void Page_Load( object sender, EventArgs e )
    {
        // Note: Add code to validate user name, password. This code is for illustrative purpose only.
        // Do not use it in production environment.
        if ( !string.IsNullOrEmpty( txtUserName.Text ) )
        {
            if ( Request.QueryString["ReturnUrl"] != null )
            {
                FormsAuthentication.RedirectFromLoginPage( txtUserName.Text, false );
            }
            else
            {
                FormsAuthentication.SetAuthCookie( txtUserName.Text, false );
                Response.Redirect( "default.aspx" );
            }
        }
        else if ( !IsPostBack )
        {
            txtUserName.Text = "Adam Carter";
        }
    }    
}

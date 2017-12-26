/****************************** Module Header ******************************\
Module Name:  Default.aspx.cs
Project:      CSAzureWebRoleIdentity
Copyright (c) Microsoft Corporation.

Default page for web role.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Threading;
using Microsoft.IdentityModel.Claims;

namespace WebRoleIndentity
{
	public partial class _Default : System.Web.UI.Page
	{
		protected void Page_Load(object sender, EventArgs e)
		{
			IClaimsPrincipal principal = Thread.CurrentPrincipal as IClaimsPrincipal;
			IClaimsIdentity identity = principal.Identity as IClaimsIdentity;

			Response.Write("Welcome to Windows Azure Web Role and WIF Sample!<br/><br/>Claims:<br/>");
			foreach (Claim c in identity.Claims)
			{
				Response.Write(c.ClaimType + " - " + c.Value + "<br/>");
			}

		}
	}
}

/****************************** Module Header ******************************\
Module Name:  Global.aspx.cs
Project:      CSAzureWebRoleIdentity
Copyright (c) Microsoft Corporation.

CSAzureWebRoleIdentity is a web role hosted in Windows Azure. It federates the 
authentication to a local STS. This breaks the authentication code from the business 
logic so that web developer can off load the authentication and authorization to 
the STS with the help of WIF. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Collections.Generic;
using System.Text;
using System.Web;
using Microsoft.IdentityModel.Tokens;
using Microsoft.IdentityModel.Web;
using Microsoft.IdentityModel.Web.Configuration;

namespace WebRoleIndentity
{
	public class Global : System.Web.HttpApplication
	{
		/// <summary>
		/// Retrieves the address that was used in the browser for accessing 
		/// the web application, and injects it as WREPLY parameter in the
		/// request to the STS 
		/// </summary>
		void WSFederationAuthenticationModule_RedirectingToIdentityProvider(object sender, RedirectingToIdentityProviderEventArgs e)
		{
			//
			// In the Windows Azure environment, build a wreply parameter for  the SignIn request
			// that reflects the real address of the application.
			//
			HttpRequest request = HttpContext.Current.Request;
			Uri requestUrl = request.Url;
			StringBuilder wreply = new StringBuilder();
			wreply.Append(requestUrl.Scheme);     // e.g. "http" or "https"
			wreply.Append("://");
			wreply.Append(request.Headers["Host"] ?? requestUrl.Authority);
			wreply.Append(request.ApplicationPath);
			if (!request.ApplicationPath.EndsWith("/"))
				wreply.Append("/");
			e.SignInRequestMessage.Reply = wreply.ToString();
		}

		void OnServiceConfigurationCreated(object sender, ServiceConfigurationCreatedEventArgs e)
		{
			//
			// Use the <serviceCertificate> to protect the cookies that are
			// sent to the client.
			//
			List<CookieTransform> sessionTransforms =
			  new List<CookieTransform>(new CookieTransform[] {
												new DeflateCookieTransform(), 
												new RsaEncryptionCookieTransform(e.ServiceConfiguration.ServiceCertificate),
												new RsaSignatureCookieTransform(e.ServiceConfiguration.ServiceCertificate)  
											}
										);

			SessionSecurityTokenHandler sessionHandler = new SessionSecurityTokenHandler(sessionTransforms.AsReadOnly());
			e.ServiceConfiguration.SecurityTokenHandlers.AddOrReplace(sessionHandler);
		}


		void Application_Start(object sender, EventArgs e)
		{
			// Code that runs on application startup
			FederatedAuthentication.ServiceConfigurationCreated += new EventHandler<ServiceConfigurationCreatedEventArgs>(OnServiceConfigurationCreated);

		}

		void Application_End(object sender, EventArgs e)
		{
			//  Code that runs on application shutdown

		}

		void Application_Error(object sender, EventArgs e)
		{
			// Code that runs when an unhandled error occurs

		}

		void Session_Start(object sender, EventArgs e)
		{
			// Code that runs when a new session is started

		}

		void Session_End(object sender, EventArgs e)
		{
			// Code that runs when a session ends. 
			// Note: The Session_End event is raised only when the sessionstate mode
			// is set to InProc in the Web.config file. If session mode is set to StateServer 
			// or SQLServer, the event is not raised.

		}

	}
}

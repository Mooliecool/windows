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

using System.Security.Cryptography.X509Certificates;
using System.Web;
using System.Web.Configuration;

using Microsoft.IdentityModel.Configuration;
using Microsoft.IdentityModel.SecurityTokenService;

/// <summary>
/// A custom SecurityTokenServiceConfiguration implementation.
/// </summary>
public class CustomSecurityTokenServiceConfiguration : SecurityTokenServiceConfiguration
{
    static readonly object syncRoot = new object();
    const string CustomSecurityTokenServiceConfigurationKey = "CustomSecurityTokenServiceConfigurationKey";

    /// <summary>
    /// Provides a model for creating a single Configuration object for the application. The first call creates a new CustomSecruityTokenServiceConfiguration and 
    /// places it into the current HttpApplicationState using the key "CustomSecurityTokenServiceConfigurationKey". Subsequent calls will return the same
    /// Configuration object.  This maintains any state that is set between calls and improves performance.
    /// </summary>
    public static CustomSecurityTokenServiceConfiguration Current
    {
        get
        {
            HttpApplicationState httpAppState = HttpContext.Current.Application;

            CustomSecurityTokenServiceConfiguration customConfiguration = httpAppState.Get( CustomSecurityTokenServiceConfigurationKey ) as CustomSecurityTokenServiceConfiguration;

            if ( customConfiguration == null )
            {
                lock ( syncRoot )
                {
                    customConfiguration = httpAppState.Get( CustomSecurityTokenServiceConfigurationKey ) as CustomSecurityTokenServiceConfiguration;

                    if ( customConfiguration == null )
                    {
                        customConfiguration = new CustomSecurityTokenServiceConfiguration();
                        httpAppState.Add( CustomSecurityTokenServiceConfigurationKey, customConfiguration );
                    }
                }
            }
            
            return customConfiguration;
        }
    }

    /// <summary>
    /// CustomSecurityTokenServiceConfiguration constructor.
    /// </summary>
    public CustomSecurityTokenServiceConfiguration()
        : base( WebConfigurationManager.AppSettings[Common.IssuerName],
                new X509SigningCredentials( CertificateUtil.GetCertificate(
                    StoreName.My, StoreLocation.LocalMachine,
                    WebConfigurationManager.AppSettings[Common.SigningCertificateName] ) ) )
    {
        this.SecurityTokenService = typeof( CustomSecurityTokenService );
    }
}

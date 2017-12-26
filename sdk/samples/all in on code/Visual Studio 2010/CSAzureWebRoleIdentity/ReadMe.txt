=============================================================================
          APPLICATION : CSAzureWebRoleIdentity Project Overview
=============================================================================

///////////////////////////////////////////////////////////////////////////////
Summary: 

CSAzureWebRoleIdentity is a web role hosted in Windows Azure. It federates the 
authentication to a local STS. This seperates the authentication code from the 
business logic so that web developer can offload the authentication and 
authorization to the STS with the help of WIF. 


////////////////////////////////////////////////////////////////////////////////
Prerequisite:

1. Microsoft Windows Vista SP2 (32-bits or 64-bits) , Microsoft Windows Server 2008 SP2 
   (32-bit or 64-bit), Microsoft Windows Server 2008 R2,  Microsoft Windows 7 RTM (32-bits or 64-bits)
2. Microsoft Internet Information Services (IIS) 7.0 with IIS Metabase and IIS 
   6 configuration compatibility.
3. Microsoft .NET Framework 4
4. Microsoft Visual Studio 2010
5. Microsoft Windows Identity Foundation Runtime
6. Microsoft Windows Identity Foundation SDK 4.0
7. Windows Azure Tools for Microsoft Visual Studio 1.4


//////////////////////////////////////////////////////////////////////////////////
Setup:

1.	Create a New Hosted Service on Windows Azure. In this sample, it is csazurewebroleidentity.
2.	Create certificate for the service. The subject name should be the same as the service name.
3.	Import the certificate into Personal and Trusted Root Certification Authorities
4.	Upload the certificate to the hosted service.
5.	Deploy the solution to Windows Azure
6.	Copy csazurewebroleidentity_sts in STS folder to C:\inetpub\wwwroot\ folder and create a 
    Virtual Directory in Default Web Site for it.


Please note:

1. You need to make sure the certificate used for SSL has the same name as your hosted service. 
   You can use CreateCert script in sample Assert folder to create such folder

   >CreateCert yourservicename

2. If you run into exception, "A potentially dangerous Request.Form value was detected from the client
   "after click "Submit" button on STS, you need to add the following in web.config of the 
   replying party. It will disable the validation in ASP.NET.

	...
	<httpRuntime requestValidationMode="2.0"/>
	<pages validateRequest="false" />
	</system.web>

3. Make sure "Copy Local" attribute of Microsoft.IdentityModel assembly in Web role replying party set as True.

Test:
When browse to the web role ( please note, you need to use HTTPS instead of HTTP protocol ), which 
is now hosted in Windows Azure. You will be redirected to the local STS first. You can observe 
this from the browser address bar. After log in, the web role will display your claim in the default page.


////////////////////////////////////////////////////////////////////////////////
Code Logic:

Local STS will first validate the incoming request. If it is coming from the local host or Windows Azure 
domain, then it will reject the request by throwing an InvalidRequestException. After validation, STS 
will issue claims of identity. These claims include: name and role. This is done in ValidateReplyTo() 
and GetOutputClaimsIdentity() method in CustomSecurityTokenService.cs

The ASP.NET web role uses default template except it displays login user’s claims on default page.

All its authentication and authorization work now is federated to local STS. 


////////////////////////////////////////////////////////////////////////////////
References:

Identity Management: Windows Identity Foundation
http://msdn.microsoft.com/en-us/security/aa570351


////////////////////////////////////////////////////////////////////////////////
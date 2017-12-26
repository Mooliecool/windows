===============================================================================
  ASP.NET APPLICATION : CSASPNETEncryptAndDecryptConfiguration Project Overview
===============================================================================
///////////////////////////////////////////////////////////////////////////////
Use:

This sample shows how to use RSA encryption algorithm API to encrypt and decrypt 
configuration section in order to protect the sensitive information from interception
or hijack in ASP.NET web application.

This project contains two snippets. The First One demonstrates how to use RSA provider 
and RSA container to encrypt and decrypt some words or values in web application.
the purpose of first snippet is to let us know the overview of RSA mechanism.
Second one shows how to use RSA configuration provider to encrypt and decrypt
configuration section in web.config.

///////////////////////////////////////////////////////////////////////////////
Prerequisites:

If your application hasn't web.config, please create one. And also specify some 
section such as appSetting, connectSetting in this web.config.

How to create Web.config in application:
http://support.microsoft.com/kb/815179/

Working with Web.config Files:
http://msdn.microsoft.com/en-us/library/ms460914.aspx

/////////////////////////////////////////////////////////////////////////////

Start this project:

1.launch CSASPNETEncryptAndDecryptConfiguration.sln as administrator.


2.Right click the CommonEncryption.aspx or ConfigurationEncryption.aspx. Choose
 "view in browser" option in menu.

  CommonEncrytion:
	1) Enter some values in the top textbox. click the "encrypt it" button 
	   below this textbox. You will observe the RSA encrypt result string in 
           the multiLine textbox.
	2) Then you can click the another button in this page named "decrypt it".
           And the decrypt result will show on next multiline textbox. You will 
           find that this string is equal to the value which you first entered in
           the top textbox.

  ConfigurationEncryption:
	1) Choose a configuration section in dropdownlist.
	2) Click "encrypt it" button in below. if the encryption successed, then open 
           web.config file, you will observe the specific section is 	           
           encrypted and is replaced by some RSA data section.
       
        3) If you want to recover this section to plain text. Click "decrypt it" 
           button and check web.config again.

  Note: If you are running this application from the file system, when you close the
	application, Visual Studio will display a dialog with the message of "The file 
	has been modified outside the editor. Do you want to reload it?" Click yes and 
	then view the web.config.


/////////////////////////////////////////////////////////////////////////////

Code Logic:

CommonEncrytion:

1. Create a new instance of a CspParameters class and pass the name that you want to call 
the key container to the CspParameters.KeyContainerName field.
2. Create a new RSACryptoServiceProvider instance and pass CsParameter to its' constructor.
3. Create byte arrays to hold original, encrypted, and decrypted data.
4. Pass the byte arrays data to ENCRYPT mehotd and get the result of encrypt byte arrays.
5. Convert this byte array data to its equivalent string by using Convert.ToBase64String 
and display it in multiline textbox.

ConfigurationEncryption:

1. Get the dropdownlist selected value to assign which configuration section to encrypt 
or decrypt.
2. Open the web.config in this web application. 
3. Find the specific section and use RSAProtectedConfigurationProvider to encrypt or 
decrypt it.
4. If success, this section will be encrypted by RSA and replaced by soem RSA section 
in web.config.


Note: If you store sensitive data in any of the following configuration sections, you cannot
 encrypt it by using a protected configuration provider and the Aspnet_regiis.exe tool:

<processModel>
<runtime>
<mscorlib>
<startup>
<system.runtime.remoting>
<configProtectedData>
<satelliteassemblies>
<cryptographySettings>
<cryptoNameMapping>
<cryptoClasses>

The RSAProtectedConfigurationProvider supports machine-level and user-level key containers
for key storage. In this project, we all use machine-level.

Understanding Machine-Level and User-Level RSA Key Containers:
http://msdn.microsoft.com/en-us/library/f5cs0acs.aspx

Without use RSA provider and API, we can also use Aspnet_regiis.exe tool to encrypt and decrypt section.
http://msdn.microsoft.com/en-us/library/ms998283.aspx

/////////////////////////////////////////////////////////////////////////////

References:

RSACryptoServiceProvider
http://msdn.microsoft.com/en-us/library/system.security.cryptography.rsacryptoserviceprovider(VS.80).aspx

CspParameters
http://msdn.microsoft.com/en-us/library/system.security.cryptography.cspparameters(VS.80).aspx


ConfigurationSection
http://msdn.microsoft.com/en-us/library/system.configuration.configurationsection.aspx

SectionInformation.ProtectSection 
http://msdn.microsoft.com/en-us/library/system.configuration.sectioninformation.protectsection.aspx


/////////////////////////////////////////////////////////////////////////////





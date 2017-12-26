========================================================================
    WINDOWS APPLICATION : CSXmlEncryption Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CSXmlEncryption demonstrates how to use .NET built-in classes to encrypt and 
decrypt Xml document:


/////////////////////////////////////////////////////////////////////////////
Code Logic:

Use .NET built-in classes to encrypt and decrypt Xml document.

A. Creates a cryptographic provider object which supplies public/private
   key pair.
B. Creates a separate session key using symmetric algorithm.
C. Uses the session key to encrypt the XML document and then uses public key
   of stepA f to encrypt the session key.
D. Saves the encrypted AES session key and the encrypted XML data to the XML
   document within a new <EncryptedData> element.
E. To decrypt the XML element, we retrieve the private key of stepA, use it 
   to decrypt the session key, and then use the session key to decrypt the 
   document.


/////////////////////////////////////////////////////////////////////////////
References:

.NET Security and Cryptography 
http://www.amazon.com/Security-Cryptography-Integrated-Object-Innovations/dp/013100851X

RSACryptoServiceProvider Class
http://msdn.microsoft.com/en-us/library/system.security.cryptography.rsacryptoserviceprovider.aspx

Cryptography in .NET
http://www.developer.com/net/net/article.php/1548761

DSACryptoServiceProvider Class
http://msdn.microsoft.com/en-us/library/system.security.cryptography.dsacryptoserviceprovider.aspx

Cryptography in Microsoft.NET Part I: Encryption
http://www.c-sharpcorner.com/UploadFile/gsparamasivam/CryptEncryption11282005061028AM/CryptEncryption.aspx

Cryptography in Microsoft.NET Part II: Digital Envelop and Digital Signatures
http://www.c-sharpcorner.com/UploadFile/Gowri%20S%20Paramasivam/Cryptography211242005003308AM/Cryptography2.aspx


/////////////////////////////////////////////////////////////////////////////

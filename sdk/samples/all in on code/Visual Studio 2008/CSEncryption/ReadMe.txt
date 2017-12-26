========================================================================
    WINDOWS APPLICATION : CSEncryption Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Cryptography is used to protect data and has many valuable uses. It can 
protect data from being viewed, modified, or to ensure the integrity 
from the originator. Cryptography can be used as a mechanism to provide secure
communication over an unsecured network, such as the Internet, by encrypting 
data, sending it across the network in the encrypted state, and then the 
decrypting the data on the receiving end. Encryption can also be used as an
additional security mechanism to further protect data such as passwords 
stored in a database to prevent them from being human readable or 
understandable. There are two types of cryptography: asymmetric algorithm 
and symmetric algorithm.


Symmetric algorithms:
A symmetric algorithm is a cipher in which encryption and decryption use the 
same key or keys that are mathematically related to one another in such a way
that it is easy to compute one key from knowledge of the other, which is 
effectively a single key.Some symmetric algorithms commonly used are DES,
TripleDES, RC2,RijnDael.The following private-key algorithms are available in
the .NET Framework. 

Data Encryption Standard (DES)  
RC2 
TripleDES 
Rijndael  


Asymmetric algorithms:
It uses a public and private key pair to encrypt and decrypt data. The public
key is made available to anyone and is used to encrypt data to be sent to the 
owner of the private key. The private key, as the name implies, is kept private 
. The private key is used to decrypt the data and will only work if the correct
public key was used when encrypting the data. The private key is the only key 
that will allow data encrypted with the public key to be decrypted. The 
following public-key algorithms are available for use in the .NET Framework:

Digital Signature Algorithm (DSA) 
RSA 

Hashing Algorithms
A hash is a function that maps an arbitrary-length binary data input to a 
small, fixed-length binary data output, often referred to as a message digest
or finger print.A good hash function should have the property that it is a very
low probability that two distinct inputs collide, producing the same hash 
value result.The .NET Framework provides support for the following hash 
algorithms:

HMACSHA1 
MACTripleDES 
MD5CryptoServiceProvider 
SHA1Managed 
SHA256Managed 
SHA384Managed 
SHA512Managed 


CSEncryption demonstrates how to use .NET built-in classes to encrypt and 
decrypt data.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Use symmetric algorithm to encrypt and decrypt data in .NET.

A. Create the secret key, Initial vector, padding mode, cipher mode for a 
   cryptographic provider object which is used later to encrypt or decrypt 
   data
B. Get plaintext as byte array
C. Create one symmetric algorithm provider which is initialized by parameters
   from stepA. Using this object encrypts data via same secret key which is 
   also used in decryption and gets ciphered data.
D. Create one symmetric algorithm provider which is initialized by parameters
   from stepA. Using this object encrypts data via same secret key and gets 
   ciphered data.

2. Use asymmetric algorithm to encrypt and decrypt data in .NET.

A. Create a cryptographic provider object to get public key and private key.
   The private key is used to decrypt data. The public key is used to encrypt
   data.
B. Get plaintext as byte array
C. Create one asymmetric algorithm provider which is initialized by public key
   from stepA. Using this object encrypts data via public key and gets 
   ciphered data.
D. Create one symmetric algorithm provider which is initialized by public key
   and private key from stepA. Using this object encrypts data and gets 
   ciphered data.


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

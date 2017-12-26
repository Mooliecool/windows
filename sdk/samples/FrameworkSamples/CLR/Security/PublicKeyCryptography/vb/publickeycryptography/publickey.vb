'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'=====================================================================
'  File:      PublicKey.vb
'
'  Summary:   Demonstrates public key cryptography using the .NET
'			 Framework implementation of RSA. 
'
'=====================================================================

Imports System
Imports System.Security.Cryptography
Imports System.IO
Imports System.Text


Module PublicKey
   Sub Main()
    ' Instantiate 3 People for example. See the Person class below
    Dim alice As New Person("Alice")
    Dim bob As New Person("Bob")
    Dim steve As New Person("Steve")
    
    ' Messages that will exchanged. See CipherMessage class below
    Dim aliceMessage As CipherMessage
    Dim bobMessage As CipherMessage
    Dim steveMessage As CipherMessage
    
    ' Example of encrypting/decrypting your own message
    Console.WriteLine("Encrypting/Decrypting Your Own Message")
    Console.WriteLine("-----------------------------------------")
    
    ' Alice encrypts a message using her own public key
    aliceMessage = alice.EncryptMessage("Alice wrote this message")
    ' then using her private key can decrypt the message
    alice.DecryptMessage(aliceMessage)
    ' Example of Exchanging Keys and Messages
    Console.WriteLine()
    Console.WriteLine("Exchanging Keys and Messages")
    Console.WriteLine("-----------------------------------------")
    
    ' Alice Sends a copy of her public key to Bob and Steve
    bob.GetPublicKey(alice)
    steve.GetPublicKey(alice)
    
    ' Bob and Steve both encrypt messages to send to Alice
    bobMessage = bob.EncryptMessage("Hi Alice! - Bob.")
    steveMessage = steve.EncryptMessage("How are you? - Steve")
    
    ' Alice can decrypt and read both messages
    alice.DecryptMessage(bobMessage)
    alice.DecryptMessage(steveMessage)
    
    Console.WriteLine()
    Console.WriteLine("Private Key required to read the messages")
    Console.WriteLine("-----------------------------------------")
    
    ' Steve cannot read the message that Bob encrypted
    steve.DecryptMessage(bobMessage)
    ' Not even Bob can use the Message he encrypted for Alice.
    ' The RSA private key is required to decrypt the RS2 key used
    ' in the decryption.
    bob.DecryptMessage(bobMessage)
  End Sub 'Main 

  Class CipherMessage
    Public cipherBytes() As Byte ' RC2 encrypted message text
    Public rc2Key() As Byte ' RSA encrypted rc2 key
    Public rc2IV() As Byte ' RC2 initialization vector
  End Class 'CipherMessage
  
  Class Person
    Private rsa As RSACryptoServiceProvider
    Private rc2 As RC2CryptoServiceProvider
    Private name As String
    
    ' Maximum key size for the RC2 algorithm
    Private keySize As Integer = 128
    
    
    ' Person constructor
    Public Sub New(p_Name As String)
      rsa = New RSACryptoServiceProvider()
      rc2 = New RC2CryptoServiceProvider()
      rc2.KeySize = keySize
      name = p_Name
    End Sub 'New
    
    
    ' Used to send the rsa public key parameters
    Public Function SendPublicKey() As RSAParameters
      Dim result As New RSAParameters()
      Try
        result = rsa.ExportParameters(False)
      Catch e As CryptographicException
        Console.WriteLine(e.Message)
      End Try
      Return result
    End Function 'SendPublicKey
    
    ' Used to import the rsa public key parameters
    Public Sub GetPublicKey(receiver As Person)
      Try
        rsa.ImportParameters(receiver.SendPublicKey())
      Catch e As CryptographicException
        Console.WriteLine(e.Message)
      End Try
    End Sub 'GetPublicKey
    
    
    Public Function EncryptMessage([text] As String) As CipherMessage
      ' Convert string to a byte array
      Dim message As New CipherMessage()
      Dim plainBytes As Byte() = Encoding.Unicode.GetBytes([text].ToCharArray())
      
      ' A new key and iv are generated for every message
      rc2.GenerateKey()
      rc2.GenerateIV()
      
      ' The rc2 initialization doesnt need to be encrypted, but will
      ' be used in conjunction with the key to decrypt the message.
      message.rc2IV = rc2.IV
      Try
        ' Encrypt the RC2 key using RSA encryption
        message.rc2Key = rsa.Encrypt(rc2.Key, False)
      Catch e As CryptographicException
        ' The High Encryption Pack is required to run this  sample
        ' because we are using a 128-bit key. See the readme for
        ' additional information.
        Console.WriteLine(("Encryption Failed. Ensure that the" + " High Encryption Pack is installed."))
        Console.WriteLine(("Error Message: " + e.Message))
        Environment.Exit(0)
      End Try
      ' Encrypt the Text Message using RC2 (Symmetric algorithm)
      Dim sse As ICryptoTransform = rc2.CreateEncryptor()
      Dim ms As New MemoryStream()
      Dim cs As New CryptoStream(ms, sse, CryptoStreamMode.Write)
      Try
        cs.Write(plainBytes, 0, plainBytes.Length)
        cs.FlushFinalBlock()
        message.cipherBytes = ms.ToArray()
      Catch e As Exception
        Console.WriteLine(e.Message)
      Finally
        ms.Close()
        cs.Close()
      End Try
      Return message
    End Function 'EncryptMessage
    
    Public Sub DecryptMessage(message As CipherMessage)
      ' Get the RC2 Key and Initialization Vector
      rc2.IV = message.rc2IV
      Try
        ' Try decrypting the rc2 key
        rc2.Key = rsa.Decrypt(message.rc2Key, False)
      Catch e As CryptographicException
        Console.WriteLine(("Decryption Failed: " + e.Message))
        Return
      End Try
      
      Dim ssd As ICryptoTransform = rc2.CreateDecryptor()
      ' Put the encrypted message in a memorystream
      Dim ms As New MemoryStream(message.cipherBytes)
      ' the CryptoStream will read cipher text from the MemoryStream
      Dim cs As New CryptoStream(ms, ssd, CryptoStreamMode.Read)
      Dim initialText() As Byte = New [Byte](message.cipherBytes.Length) {}
      
      Try
        ' Decrypt the message and store in byte array
        cs.Read(initialText, 0, initialText.Length)
      Catch e As Exception
        Console.WriteLine(e.Message)
      Finally
        ms.Close()
        cs.Close()
      End Try 
      
      ' Display the message received
      Console.WriteLine((name + " received the following message:"))
      Console.WriteLine(("  " &  Encoding.Unicode.GetString(initialText)))
    End Sub 'DecryptMessage 
  End Class 'Person 
End Module 'PublicKey 

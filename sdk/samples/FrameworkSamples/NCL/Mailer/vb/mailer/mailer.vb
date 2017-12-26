'---------------------------------------------------------------------
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
'---------------------------------------------------------------------

Imports System.net.mail

Module Mailer

    Enum MailMessagePart
        ToAddress
        FromAddress
        Subject
        Message
    End Enum

    Sub Main(ByVal args() As String)
        If args.Length < 4 Then
            Console.WriteLine( _
                "Expected: mailer.exe [from] [to] [subject] [message]")
            Exit Sub
        End If

        ' Set mailServerName to be the name of the mail server
        ' you wish to use to deliver this message
        Dim mailServerName As String = "smtphost"
        Dim fromAddress As String = args(MailMessagePart.ToAddress)
        Dim toAddress As String = args(MailMessagePart.FromAddress)
        Dim subject As String = args(MailMessagePart.Subject)
        Dim body As String = args(MailMessagePart.Message)

        Try
            ' MailMessage is used to represent the e-mail being sent
            Using message As _
                New MailMessage(fromAddress, toAddress, subject, body)

                ' SmtpClient is used to send the e-mail
                Dim mailClient As New SmtpClient(mailServerName)
                ' UseDefaultCredentials tells the mail client to use the 
                ' Windows credentials of the account (i.e. user account) 
                ' being used to run the application
                mailClient.UseDefaultCredentials = True

                ' Send delivers the message to the mail server
                mailClient.Send(message)
            End Using
            Console.WriteLine("Message sent.")
        Catch ex As FormatException
            Console.WriteLine(ex.Message)
        Catch ex As SmtpException
            Console.WriteLine(ex.Message)
        End Try
    End Sub
End Module

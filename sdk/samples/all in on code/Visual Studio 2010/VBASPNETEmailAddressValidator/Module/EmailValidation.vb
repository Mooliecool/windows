'***************************** Module Header ******************************\
'* Module Name:    EmailValidation.vb
'* Project:        VBASPNETEmailAddressValidator
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to send a confirmation Email to check whether an
'* Email address is available.
'* 
'* In this file, we create a class which used as the main logic to validate  
'* Email address by confirmation Email.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************



Imports System.Linq
Imports System.Web
Imports System.Net.Mail
Imports System.Net


' Create an enum class which we used to return the 
' process status after confirmation Email sent.
Public Enum ValidateEmailResult
    EmailValidated
    EmailValidating
    EmailNotValidate
    EmailStartToValidate
End Enum

Public Class EmailValidation

    ' Some readonly fields to store the SMTP host infomation.
    ' As we all know, if we want to send an Email, we need
    ' a mail server which could send message to another mail
    ' server, it could be called, SMTP server. Vice versa, 
    ' the mail server which receive the message, we call it
    ' POP server. Here 'MAILHOST' is the smtp server name or 
    ' IP address, 'SENDMAILADDRESS' is the address which used
    ' to send the confirmation Email, 'SENDMAILPASSWORD' is the
    ' password of the address and 'NEEDSSL' is used that some 
    ' SMTP need SSL to process the Email message exchanging, 
    ' for example Hotmail.
    Private ReadOnly mailHost As String
    Private ReadOnly sendMailAddress As String
    Private ReadOnly sendMailPassword As String
    Private ReadOnly needSSL As Boolean

    Public Sub New(ByVal sendAddress As String, _
                   ByVal password As String, _
                   ByVal hostserver As String, _
                   ByVal enableSSL As Boolean)

        mailHost = hostserver
        sendMailAddress = sendAddress
        sendMailPassword = password
        needSSL = enableSSL
    End Sub

    ' This method used to sending confirmation Email.
    ' We use a simple database table to store the information about the
    ' Email addresses which are ready for validation.
    Public Function StartToValidateEmail(ByVal emailaddress As String) _
                                                As ValidateEmailResult

        ' use Linq to SQL to process the database queries.
        Using context As New EmailAddressValidationDataContext

            ' Check whether the email address has already been recorded
            ' into the database.
            Dim eval As tblEmailValidation =
                context.tblEmailValidations.Where(Function(t) _
                  t.EmailAddress = emailaddress).FirstOrDefault()

            If eval IsNot Nothing Then

                ' If yes, return the process of the validation.
                If eval.IsValidated Then
                    Return ValidateEmailResult.EmailValidated
                Else
                    Return ValidateEmailResult.EmailValidating
                End If
            End If

            ' Generate a unique key to validate the address.
            Dim querykey As String =
                Guid.NewGuid().ToString().Replace("-", "")

            ' Send the confirmation Email.
            SendValidationEmail(emailaddress, querykey)

            ' If not, create a new record to the database.
            context.tblEmailValidations.InsertOnSubmit(
                        New tblEmailValidation() With { _
                           .EmailAddress = emailaddress, _
                           .IsValidated = False, _
                           .IsSendCheckEmail = False, _
                           .ValidatingStartTime = DateTime.Now, _
                           .ValidateKey = querykey _
                          })
            context.SubmitChanges()


            Return ValidateEmailResult.EmailStartToValidate
        End Using
    End Function

    ' This method is used to get the current domain which will be
    ' made as a link to send to the address.
    ' User click the address from the confirmation Email and
    ' the server will fire a Handler ,EmailAvailableValidationHandler, 
    ' to update the record in the database to finish the validation.
    Private Function GetDomainURI() As String
        If HttpContext.Current Is Nothing Then
            Throw New NullReferenceException("Need web context")
        End If
        Dim request As HttpRequest = HttpContext.Current.Request
        Dim rsl As String = ""
        rsl += If(request.ServerVariables("HTTPS") = "on", "https://", "http://")
        rsl += request.ServerVariables("SERVER_NAME")
        rsl += If((request.ServerVariables("SERVER_PORT") <> "80"),
                  (":" & request.ServerVariables("SERVER_PORT")), "")
        Return rsl
    End Function

    ' Here we use SmtpClient and MaillMessage classed to send the confirmation
    ' Emaill.
    Private Sub SendValidationEmail(ByVal address As String, ByVal querykey As String)

        Using smtp As New SmtpClient(mailHost)
            Dim from As New MailAddress(sendMailAddress, "Confirmation Email")
            Dim [to] As New MailAddress(address)

            Using message As New MailMessage(from, [to])
                message.IsBodyHtml = True
                message.Subject = "Confirmation Email"

                ' Here we only send a link for the last step of the validation.
                ' We can also create our own html styles to make it look better.
                message.Body = String.Format("<a href='{0}/mail.axd?k={1}'>" &
                     "Please click here to finish email address validation.</a>",
                     GetDomainURI(),
                     querykey)

                smtp.DeliveryMethod = SmtpDeliveryMethod.Network
                smtp.UseDefaultCredentials = False
                smtp.Credentials = New NetworkCredential(sendMailAddress,
                                                         sendMailPassword)
                smtp.Port = 25
                smtp.EnableSsl = needSSL
                smtp.Send(message)
            End Using
        End Using

    End Sub

    ' This method is used to re-send the Confirmation Email.
    Public Sub ReSendValidationEmail(ByVal address As String)
        Using context As New EmailAddressValidationDataContext()

            Dim eval As tblEmailValidation =
                context.tblEmailValidations.Where( _
                    Function(t) t.EmailAddress = address).FirstOrDefault()
            If eval IsNot Nothing Then
                SendValidationEmail(address, eval.ValidateKey)
            End If
        End Using

    End Sub
End Class

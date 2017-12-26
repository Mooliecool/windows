'***************************** Module Header ******************************\
'* Module Name:    EmailAvailableValidationHandler.vb
'* Project:        VBASPNETEmailAddressValidator
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to send a confirmation Email to check whether an
'* Email address is available.
'* 
'* In this file, we create a HttpHandler which used to update the record in  
'* the database and finish the validation. We need to config this Handler in
'* the Web.config.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports System.Web
Imports System.Web.Configuration
Imports System.Configuration
Imports System.Linq


Public Class EmailAvailableValidationHandler
    Implements IHttpHandler
    Public ReadOnly Property IsReusable() As Boolean _
                            Implements IHttpHandler.IsReusable
        Get
            Return True
        End Get
    End Property


    Public Sub ProcessRequest(ByVal context As HttpContext) _
                            Implements IHttpHandler.ProcessRequest

        ' Get the unique key which is used to compare with
        ' the key stored in the database.
        Dim key As String = context.Request.QueryString("k")

        If Not String.IsNullOrEmpty(key) Then
            Using service As New EmailAddressValidationDataContext
                Dim EValidation As tblEmailValidation =
                    service.tblEmailValidations.Where(Function(t) _
                    t.ValidateKey.Trim() = key).FirstOrDefault()
                If EValidation IsNot Nothing Then

                    ' Update the record and make it as validated.
                    EValidation.IsValidated = True
                    service.SubmitChanges()

                    ' We can custom the output of the return message.
                    ' Here is just a simple message.
                    context.Response.Write("Congratulation! Your Email Addess: " _
                    + EValidation.EmailAddress & " has been validated!")
                Else
                    context.Response.Write("Please submit your address first.")
                End If
            End Using
        End If
    End Sub



End Class

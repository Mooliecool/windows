'***************************** Module Header ******************************\
'* Module Name:    Default.aspx.vb
'* Project:        VBASPNETEmailAddressValidator
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to send a confirmation Email to check whether an
'* Email address is available.
'* 
'* In this file, we create some controls to call the codes which will send the 
'* confirmation Email.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports System.Linq
Imports System.Web
Imports System.Web.UI
Imports System.Web.UI.WebControls


Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
    End Sub

    ' If we click the Validate button, it will start to
    ' send the confirmation Email.
    Protected Sub btnValidate_Click(ByVal sender As Object, ByVal e As EventArgs)
        btnSendAgain.Visible = False
        Try

            ' Make an instance of the EmailValidation to be 
            ' ready for sending confirmation Email.
            Dim validator As New EmailValidation(
                tbSendMail.Text, _
                Session("password").ToString(), _
                tbHost.Text, _
                chkUseSSL.Checked)

            ' Use StartToValidateEmail method to start validate
            ' the Email address and send the confirmation Email.
            Dim rsl As ValidateEmailResult =
                validator.StartToValidateEmail(tbValidateEmail.Text)
            Select Case rsl
                Case ValidateEmailResult.EmailStartToValidate
                    lbMessage.Text = "The validation mail has send successfully." _
                        & "Please check the email box."
                    btnSendAgain.Visible = True
                    Exit Select
                Case ValidateEmailResult.EmailValidated
                    lbMessage.Text = "This address has been validated."
                    Exit Select
                Case ValidateEmailResult.EmailValidating
                    lbMessage.Text = "This address is waiting for user " _
                        & "to click the validation link in his mail box"
                    btnSendAgain.Visible = True
                    Exit Select
            End Select
        Catch err As Exception
            lbMessage.Text = "Error:" & err.Message
        End Try
    End Sub

    ' If we have send the confirmation Email, but the user still 
    ' not receive the Email, we could let him send again.
    Protected Sub btnSendEmailAgain_Click(ByVal sender As Object, ByVal e As EventArgs)
        Try

            ' Make an instance of the EmailValidation to be 
            ' ready for sending confirmation Email again.
            Dim validator As New EmailValidation(
                tbSendMail.Text, _
                Session("password").ToString(), _
                tbHost.Text, _
                chkUseSSL.Checked)

            ' Use ReSendValidationEmail method to re-send the 
            ' confirmation Email.
            validator.ReSendValidationEmail(tbValidateEmail.Text)
            lbMessage.Text = "Email has been re-sent. Please check it again."
        Catch err As Exception
            lbMessage.Text = "Error:" & err.Message
        End Try
    End Sub

    Protected Sub ValidationWizard_OnNextButtonClick(ByVal sender As Object, _
                                      ByVal e As WizardNavigationEventArgs)
        If e.CurrentStepIndex = 0 Then
            Session("password") = tbSendMailPassword.Text
        End If
    End Sub
End Class

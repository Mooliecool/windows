'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
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


Imports System.Net.Mail
Imports System.Workflow.ComponentModel.Compiler

Public Class SendEmailValidator
    Inherits System.Workflow.ComponentModel.Compiler.ActivityValidator

    ' Define private constants for the Validation Errors 
    Private Const InvalidToAddress As Integer = 1
    Private Const InvalidFromAddress As Integer = 2
    Private Const InvalidSMTPPort As Integer = 3

    ' customizing the default activity validation
    Public Overrides Function ValidateProperties(ByVal manager As ValidationManager, ByVal obj As Object) As ValidationErrorCollection

        ' Create a new collection for storing the validation errors
        Dim validationErrors As New ValidationErrorCollection()

        Dim activity As SendEmailActivity = TryCast(obj, SendEmailActivity)

        If activity IsNot Nothing Then

            ' Validate the Email and SMTP Properties
            Me.ValidateEmailProperties(validationErrors, activity)
            Me.ValidateSMTPProperties(validationErrors, activity)

            ' Raise an exception if we have ValidationErrors
            If validationErrors.HasErrors Then

                Dim validationErrorsMessage As String = String.Empty

                For Each validationError As ValidationError In validationErrors
                    validationErrorsMessage += _
                        String.Format("Validation Error:  Number {0} - '{1}' \n", _
                        validationError.ErrorNumber, validationError.ErrorText)
                Next

                ' Throw a new exception with the validation errors.
                Throw New InvalidOperationException(validationErrorsMessage)
            End If
        End If
        Return validationErrors
    End Function

    Private Sub ValidateEmailProperties(ByVal validationErrors As ValidationErrorCollection, ByVal activity As SendEmailActivity)
        'Validate the To property
        If String.IsNullOrEmpty(activity.EmailTo) Then
            Dim validationError As ValidationError = System.Workflow.ComponentModel.Compiler.ValidationError.GetNotSetValidationError(SendEmailActivity.ToProperty.Name)
            validationErrors.Add(validationError)
        ElseIf Not activity.EmailTo.Contains("@") Then
            Dim validationError As New ValidationError("Invalid To e-mail address", _
              InvalidToAddress, False, SendEmailActivity.ToProperty.Name)
            validationErrors.Add(validationError)
        End If

        ' Validate the From property
        If String.IsNullOrEmpty(activity.FromEmail) Then
            validationErrors.Add(ValidationError.GetNotSetValidationError(SendEmailActivity.FromEmailProperty.Name))
        ElseIf Not activity.FromEmail.Contains("@") Then
            Dim validationError As New ValidationError("Invalid From e-mail address", _
                InvalidFromAddress, False, SendEmailActivity.FromEmailProperty.Name)
            validationErrors.Add(validationError)
        End If
    End Sub


    Private Sub ValidateSMTPProperties(ByVal validationErrors As ValidationErrorCollection, ByVal activity As SendEmailActivity)
        ' Validate the SMTPHost property
        If String.IsNullOrEmpty(activity.SmtpHost) Then
            Dim validationError As ValidationError = System.Workflow.ComponentModel.Compiler.ValidationError.GetNotSetValidationError(SendEmailActivity.SmtpHostProperty.Name)
            validationErrors.Add(validationError)
        End If

        ' Validate the Port property
        If activity.Port = 0 Then
            validationErrors.Add(ValidationError.GetNotSetValidationError(SendEmailActivity.PortProperty.Name))
        ElseIf activity.Port < 1 Then
            Dim validationError As New ValidationError("Invalid Port Number", _
                InvalidSMTPPort, False, SendEmailActivity.PortProperty.Name)
            validationErrors.Add(validationError)
        End If
    End Sub
End Class

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
'
'=====================================================================
'  File:      Tester.vb
'
'  Summary:   A simple DLL that exposes a function or two to be tested
'             by the invoke sample
'
'=====================================================================

Option Strict On
Option Explicit On


Imports System
Imports System.Windows.Forms
Imports System.ComponentModel


Namespace Microsoft.Samples

    Public Class SomeType

        Public Shared Function AddTwoDoubles(ByVal num1 As Double, ByVal num2 As Double) As Double
            Return num1 + num2
        End Function 'AddTwoDoubles


        Public Function ShowMessage(ByVal message As String) As DialogResult
            Dim DR_Ret As DialogResult = DialogResult.Cancel ' Assume action was cancelled, by default

            Try
                DR_Ret = MessageBox.Show(message)
            Catch ex As InvalidOperationException
                MessageBox.Show("ERROR: " + ex.Message, _
                                System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, _
                                System.Windows.Forms.MessageBoxButtons.OK, _
                                System.Windows.Forms.MessageBoxIcon.Error)
            End Try

            Return DR_Ret

        End Function 'ShowMessage


        Public Function ShowMessage(ByVal message As String, _
                                    ByVal caption As String) As DialogResult
            Dim DR_Ret As DialogResult = DialogResult.Cancel ' Assume action was cancelled, by default

            Try
                DR_Ret = MessageBox.Show(message, caption)
            Catch ex As InvalidOperationException
                MessageBox.Show("ERROR: " + ex.Message, _
                                System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, _
                                System.Windows.Forms.MessageBoxButtons.OK, _
                                System.Windows.Forms.MessageBoxIcon.Error)
            End Try

            Return DR_Ret

        End Function 'ShowMessage


        Public Function ShowMessage(ByVal message As String, _
                                    ByVal caption As String, _
                                    ByVal style As Integer) As DialogResult
            Dim DR_Ret As DialogResult = DialogResult.Cancel ' Assume action was cancelled, by default

            Try
                DR_Ret = MessageBox.Show(message, caption, CType(style, MessageBoxButtons))
            Catch ex As InvalidEnumArgumentException
                MessageBox.Show("ERROR: " + ex.Message, _
                                System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, _
                                System.Windows.Forms.MessageBoxButtons.OK, _
                                System.Windows.Forms.MessageBoxIcon.Error)
            Catch ex As InvalidOperationException
                MessageBox.Show("ERROR: " + ex.Message, _
                                System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, _
                                System.Windows.Forms.MessageBoxButtons.OK, _
                                System.Windows.Forms.MessageBoxIcon.Error)
            End Try

            Return DR_Ret

        End Function 'ShowMessage

    End Class 'SomeType 

End Namespace

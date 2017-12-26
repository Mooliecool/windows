'****************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:      VBCOMClient
' Copyright (c) Microsoft Corporation.
' 
' Windows Form hosts ActiveX control.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Public Class MainForm

    Private Sub BnSetFloatProperty_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles BnSetFloatProperty.Click
        Dim fProp As Single
        If Single.TryParse(Me.TbFloatProperty.Text, fProp) Then
            Me.AxMFCActiveX1.FloatProperty = fProp
        Else
            MessageBox.Show("Invalid FloatProperty value.")
        End If
    End Sub

    Private Sub BnGetFloatProperty_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) _
    Handles BnGetFloatProperty.Click
        Dim message As String = String.Format("FloatProperty equals {0}", _
            Me.AxMFCActiveX1.FloatProperty)
        MessageBox.Show(message, "MFCActiveX!FloatProperty")
    End Sub

    Private Sub AxMFCActiveX1_FloatPropertyChanging(ByVal sender As System.Object, ByVal e As AxMFCActiveXLib._DMFCActiveXEvents_FloatPropertyChangingEvent) _
    Handles AxMFCActiveX1.FloatPropertyChanging

        Dim message As String = String.Format( _
            "FloatProperty is being changed to {0}", e.newValue)

        ' OK or cancel the change of FloatProperty
        e.cancel = (DialogResult.Cancel = _
            MessageBox.Show(message, "MFCActiveX!FloatPropertyChanging", _
            MessageBoxButtons.OKCancel))
    End Sub

End Class
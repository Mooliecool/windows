'******************************** Module Header ************************************'
' Module Name:  Settings.cs
' Project:      VBImageFullScreenSlideShow
' Copyright (c) Microsoft Corporation.
'
' The code sets the internal for Timer control.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************'

Partial Public Class Setting
    Inherits Form
    Private _timr As Timer

    ''' <summary>
    ''' Customize Constructer for importing control reference address.
    ''' </summary>
    Public Sub New(ByRef timr As Timer)
        InitializeComponent()
        _timr = timr
        Me.dtpInternal.Value = timr.Interval
    End Sub

    ''' <summary>
    ''' Cancel manipulate.
    ''' </summary>
    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCancel.Click
        Me.Close()
    End Sub

    ''' <summary>
    ''' Save the internal for Timer control and close the child Windows.
    ''' </summary>
    Private Sub btnConfirm_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnConfirm.Click
        _timr.Interval = Integer.Parse(Me.dtpInternal.Value.ToString())
        Me.Close()
    End Sub

End Class


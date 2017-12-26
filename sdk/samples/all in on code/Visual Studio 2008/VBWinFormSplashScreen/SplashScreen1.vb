'****************************** Module Header ******************************\
' Module Name:  SplashScreen1.cs
' Project:      VBWinFormSplashScreen
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to achieve splash screen effect in
' Windows Forms Application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class SplashScreen1

    Public Sub New()
        MyBase.New()
        InitializeComponent()

        Splash1Setting()
    End Sub

    Private Sub Splash1Setting()
        Me.FormBorderStyle = Windows.Forms.FormBorderStyle.None
        Me.StartPosition = FormStartPosition.CenterScreen
        Me.BackgroundImage = VBWinFormSplashScreen.My.Resources.SplashImage
    End Sub
End Class
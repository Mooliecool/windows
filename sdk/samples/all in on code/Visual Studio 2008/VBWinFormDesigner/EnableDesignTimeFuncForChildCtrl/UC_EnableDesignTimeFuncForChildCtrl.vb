'************************************* Module Header **************************************\
' Module Name:	UC_EnableDesignTimeFuncForChildCtrl.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' The EnableDesignTimeFuncOnChildCtrl sample demonstrates how to enable design time 
' functionality on for a child control.  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.

' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Imports System.ComponentModel
Imports System.Windows.Forms.Design

<Designer(GetType(UC_EnableDesignTimeFuncForChildCtrlDesigner))> _
Public Class UC_EnableDesignTimeFuncForChildCtrl
    Public Sub New()
        Me.InitializeComponent()
    End Sub

    <DesignerSerializationVisibility(DesignerSerializationVisibility.Content)> _
    Public ReadOnly Property Panel1() As Panel
        Get
            Return Me._panel1
        End Get
    End Property

End Class

Friend Class UC_EnableDesignTimeFuncForChildCtrlDesigner
    Inherits ControlDesigner

    Public Overrides Sub Initialize(ByVal component As IComponent)
        MyBase.Initialize(component)
        Dim uc As UC_EnableDesignTimeFuncForChildCtrl = TryCast(component, UC_EnableDesignTimeFuncForChildCtrl)
        MyBase.EnableDesignMode(uc.Panel1, "Panel1")
    End Sub

End Class

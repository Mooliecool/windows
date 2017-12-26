'****************************** Module Header ******************************\
' Module Name:  Page.cs
' Project:      CSSL3DeepZoomProject
' Copyright (c) Microsoft Corporation.
' 
' A simple UserControl that displays a conversation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 9/4/2009 17:12 Yilun Luo Created
'***************************************************************************/

Partial Public Class ConversationControl
	Inherits UserControl
	' Methods
	Public Sub New()
		Me.InitializeComponent()
	End Sub

	' Properties
	Public Property ConversationText() As String
		Get
			Return Me.converstaionText.Text
		End Get
		Set(ByVal value As String)
			Me.converstaionText.Text = value
		End Set
	End Property
End Class



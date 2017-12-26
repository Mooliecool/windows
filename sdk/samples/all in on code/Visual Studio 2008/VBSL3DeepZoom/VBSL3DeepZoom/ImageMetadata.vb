'****************************** Module Header ******************************\
' Module Name:  Page.cs
' Project:      CSSL3DeepZoomProject
' Copyright (c) Microsoft Corporation.
' 
' This class wraps the data downloaded from Metadata.xml.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 9/4/2009 17:12 Yilun Luo Created
'***************************************************************************/

Public Class ImageMetadata
	' Properties
	Public Property FileName() As String
		Get
			Return Me.FileNamek__BackingField
		End Get
		Set(ByVal value As String)
			Me.FileNamek__BackingField = value
		End Set
	End Property

	Public Property Height() As Double
		Get
			Return Me.Heightk__BackingField
		End Get
		Set(ByVal value As Double)
			Me.Heightk__BackingField = value
		End Set
	End Property

	Public Property Tag() As String
		Get
			Return Me.Tagk__BackingField
		End Get
		Set(ByVal value As String)
			Me.Tagk__BackingField = value
		End Set
	End Property

	Public Property Width() As Double
		Get
			Return Me.Widthk__BackingField
		End Get
		Set(ByVal value As Double)
			Me.Widthk__BackingField = value
		End Set
	End Property

	Public Property X() As Double
		Get
			Return Me.Xk__BackingField
		End Get
		Set(ByVal value As Double)
			Me.Xk__BackingField = value
		End Set
	End Property

	Public Property Y() As Double
		Get
			Return Me.Yk__BackingField
		End Get
		Set(ByVal value As Double)
			Me.Yk__BackingField = value
		End Set
	End Property

	Public Property ZOrder() As Integer
		Get
			Return Me.ZOrderk__BackingField
		End Get
		Set(ByVal value As Integer)
			Me.ZOrderk__BackingField = value
		End Set
	End Property


	' Fields
	Private FileNamek__BackingField As String
	Private Heightk__BackingField As Double
	Private Tagk__BackingField As String
	Private Widthk__BackingField As Double
	Private Xk__BackingField As Double
	Private Yk__BackingField As Double
	Private ZOrderk__BackingField As Integer
End Class




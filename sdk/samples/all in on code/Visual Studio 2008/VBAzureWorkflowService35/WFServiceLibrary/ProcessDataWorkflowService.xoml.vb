'****************************** Module Header ******************************\
' Module Name:	ProcessDataWorkflowService.xoml.vb
' Project:		WFServiceLibrary
' Copyright (c) Microsoft Corporation.
' 
' This is the code behind of the workflow.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class ProcessDataWorkflowService
	Inherits SequentialWorkflowActivity

	' DependencyProperties.
	Public Shared ReturnValueProperty As DependencyProperty = DependencyProperty.Register("ReturnValue", GetType(String), GetType(ProcessDataWorkflowService))

	Public Property ReturnValue() As String
		Get
			Return CStr(MyBase.GetValue(ProcessDataWorkflowService.ReturnValueProperty))
		End Get
		Set(ByVal value As String)
			MyBase.SetValue(ProcessDataWorkflowService.ReturnValueProperty, value)
		End Set
	End Property

	Public Shared ValueProperty As DependencyProperty = DependencyProperty.Register("Value", GetType(Integer), GetType(ProcessDataWorkflowService))

	Public Property Value() As Integer
		Get
			Return CInt(MyBase.GetValue(ProcessDataWorkflowService.ValueProperty))
		End Get
		Set(ByVal value As Integer)
			MyBase.SetValue(ProcessDataWorkflowService.ValueProperty, value)
		End Set
	End Property

	' Assign the return values.
	Private Sub AssignLargeValue_ExecuteCode(ByVal sender As Object, ByVal e As EventArgs)
		Me.ReturnValue = "You've entered a large value."
	End Sub

	Private Sub AssignSmallValue_ExecuteCode(ByVal sender As Object, ByVal e As EventArgs)
		Me.ReturnValue = "You've entered a small value."
	End Sub

End Class

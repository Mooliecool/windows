'****************************** Module Header ******************************\
' Module Name:	IProcessDataWorkflowService.vb
' Project:		WFServiceLibrary
' Copyright (c) Microsoft Corporation.
' 
' This is the contract of the workflow service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

' This simple workflow service doesn't need sesion.
<ServiceContract(SessionMode:=SessionMode.NotAllowed)> _
Public Interface IProcessDataWorkflow
	<OperationContract()> _
	Function ProcessData(ByVal value As Integer) As String
End Interface

'****************************** Module Header ******************************\
' Project Name:   CSAzureWebRoleBackendProcessing
' Module Name:    Common
' File Name:      WordDataContext.vb
' Copyright (c) Microsoft Corporation
'
' This class represents a System.Data.Services.Client.DataServiceContext object 
' for use with the Windows Azure Table service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.StorageClient

Public Class WordDataContext
    Inherits TableServiceContext
    Public Sub New(baseAddress As String, credentials As StorageCredentials)
        MyBase.New(baseAddress, credentials)
    End Sub

    Public ReadOnly Property WordEntry() As IQueryable(Of WordEntry)
        Get
            Return Me.CreateQuery(Of WordEntry)("WordEntry")
        End Get
    End Property
End Class
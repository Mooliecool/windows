'***************************** Module Header ******************************\
' Project Name:   CSAzureWebRoleBackendProcessing
' Module Name:    Common
' File Name:      WordEntry.vb
' Copyright (c) Microsoft Corporation
'
' This class represents an entity in Table storage.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Microsoft.WindowsAzure.StorageClient

Public Class WordEntry
    Inherits TableServiceEntity
    Public Sub New()
        PartitionKey = ""
        RowKey = String.Format("{0:10}_{1}", DateTime.MaxValue.Ticks - DateTime.Now.Ticks, Guid.NewGuid())
    End Sub

    Public Property Content As String
    Public Property IsProcessed As Boolean
End Class
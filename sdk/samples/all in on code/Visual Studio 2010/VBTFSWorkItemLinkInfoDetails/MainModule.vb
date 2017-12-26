'*************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBTFSWorkItemLinkInfoDetails
' Copyright (c) Microsoft Corporation.
' 
' The main entry of the application. To run this application, use following command
' arguments:
' 
'    VBTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Net
Imports Microsoft.TeamFoundation.Client
Imports Microsoft.TeamFoundation.WorkItemTracking.Client

Module MainModule
    Sub Main(ByVal args() As String)
        Try
            ' There are 2 arguments.
            If args.Length = 2 Then

                ' Get CollectionUrl and WorkItemID from the arguments.
                Dim collectionUri As New Uri(args(0))
                Dim workitemID As Integer = Integer.Parse(args(1))

                Using query As New WorkItemLinkQuery(collectionUri)

                    ' Get the WorkItemLinkInfoDetails list of a work item.
                    Dim detailsList = query.GetWorkItemLinkInfos(workitemID)

                    For Each details As WorkItemLinkInfoDetails In detailsList
                        Console.WriteLine(details.ToString())
                    Next details
                End Using
            Else
                Console.WriteLine("Use following command arguments to use this application:")
                Console.WriteLine("VBTFSWorkItemLinkInfoDetails.exe <CollectionUrl> <WorkItemID>")
            End If
        Catch ex As Exception
            Console.WriteLine(ex.Message)
        End Try
    End Sub
End Module

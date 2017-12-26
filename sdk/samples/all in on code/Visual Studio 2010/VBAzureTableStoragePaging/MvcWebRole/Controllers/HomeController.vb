'***************************** Module Header ******************************\
'* Module Name:	HomeController.vb
'* Project:		AzureTableStoragePaging
'* Copyright (c) Microsoft Corporation.
'* 
'* This module controls the paging behavior.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.StorageClient
Imports MvcWebRole.Models
Imports MvcWebRole.Utilities

<HandleError(ExceptionType:=GetType(Exception), View:="Error")> _
Public Class HomeController
    Inherits Controller
    Private Shared r As Random = New Random()
    Private cloudStorageAccount As CloudStorageAccount = cloudStorageAccount.FromConfigurationSetting("SampleDataConnectionString")
    Public Function Index() As ActionResult
        Try
            Dim cloudTableClient = cloudStorageAccount.CreateCloudTableClient()
            cloudTableClient.CreateTableIfNotExist("Customers")
            Dim context = New CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials)
            Dim list = context.Customers.ToList()
            ' If there are entities in the table show UI that renders the table
            If list.Count() > 0 Then
                Dim provider As MVCSessionCachedDataProvider(Of Customer) = New MVCSessionCachedDataProvider(Of Customer)(Me, "provider1")
                Dim pagingUtility As TableStoragePagingUtility(Of Customer) = New TableStoragePagingUtility(Of Customer)(provider, cloudStorageAccount, context, 10, "Customers")
                'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
                'ORIGINAL LINE: Return View("Index", New CustomersSet() { Customers = pagingUtility.GetCurrentOrFirstPage().ToList(), ReadyToShowUI = True });
                Return View("Index", New CustomersSet() With {.Customers = pagingUtility.GetCurrentOrFirstPage().ToList(), .ReadyToShowUI = True})
            Else
                'If there is no entity in the table show link to guide user to add data to table.
                'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
                'ORIGINAL LINE: ViewResult vr = View("Index", New CustomersSet() { ReadyToShowUI = False });
                Dim vr As ViewResult = View("Index", New CustomersSet() With {.ReadyToShowUI = False})
                Return vr
            End If
        Catch ex As Exception
            Return View("Error", New HandleErrorInfo(ex, "HomeController", "Index"))
        End Try

    End Function

    Public Function AddDataToTest() As ActionResult

        Dim cloudTableClient = cloudStorageAccount.CreateCloudTableClient()
        Dim context = New CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials)
        Try
            ' Add 65 entities to the table
            For i As Integer = 0 To 64
                'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
                'ORIGINAL LINE: context.AddObject("Customers", New Customer() { Age = r.Next(16, 70), Name = "Customer" + i.ToString() });
                context.AddObject("Customers", New Customer() With {.Age = r.Next(16, 70), .Name = "Customer" & i.ToString()})
            Next i
            context.SaveChanges()
            Dim provider As MVCSessionCachedDataProvider(Of Customer) = New MVCSessionCachedDataProvider(Of Customer)(Me, "provider1")
            Dim pagingUtility As TableStoragePagingUtility(Of Customer) = New TableStoragePagingUtility(Of Customer)(provider, cloudStorageAccount, context, 10, "Customers")
            'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
            'ORIGINAL LINE: Return View("Index", New CustomersSet() { Customers = pagingUtility.GetNextPage().ToList(), ReadyToShowUI = True });
            Return View("Index", New CustomersSet() With {.Customers = pagingUtility.GetNextPage().ToList(), .ReadyToShowUI = True})
        Catch ex As Exception
            Return View("Error", New HandleErrorInfo(ex, "HomeController", "AddDataToTest"))
        End Try


    End Function
    Public Function Previous() As ActionResult
        Dim cloudTableClient = cloudStorageAccount.CreateCloudTableClient()
        Dim context = New CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials)
        Dim provider As MVCSessionCachedDataProvider(Of Customer) = New MVCSessionCachedDataProvider(Of Customer)(Me, "provider1")
        Dim pagingUtility As TableStoragePagingUtility(Of Customer) = New TableStoragePagingUtility(Of Customer)(provider, cloudStorageAccount, context, 10, "Customers")
        'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
        'ORIGINAL LINE: Return View("Index", New CustomersSet() { Customers = pagingUtility.GetPreviousPage().ToList(), ReadyToShowUI=True });
        Return View("Index", New CustomersSet() With {.Customers = pagingUtility.GetPreviousPage().ToList(), .ReadyToShowUI = True})
    End Function
    Public Function [Next]() As ActionResult
        Dim cloudTableClient = cloudStorageAccount.CreateCloudTableClient()
        Dim context = New CustomerDataContext(cloudStorageAccount.TableEndpoint.AbsoluteUri, cloudStorageAccount.Credentials)
        Dim provider As MVCSessionCachedDataProvider(Of Customer) = New MVCSessionCachedDataProvider(Of Customer)(Me, "provider1")
        Dim pagingUtility As TableStoragePagingUtility(Of Customer) = New TableStoragePagingUtility(Of Customer)(provider, cloudStorageAccount, context, 10, "Customers")
        'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
        'ORIGINAL LINE: Return View("Index", New CustomersSet() { Customers = pagingUtility.GetNextPage().ToList(), ReadyToShowUI=True });
        Return View("Index", New CustomersSet() With {.Customers = pagingUtility.GetNextPage().ToList(), .ReadyToShowUI = True})
    End Function
End Class


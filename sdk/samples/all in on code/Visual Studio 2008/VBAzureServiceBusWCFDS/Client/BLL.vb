'***************************** Module Header ******************************\
'* Module Name:	BLL.vb
'* Project:		Client
'* Copyright (c) Microsoft Corporation.
'* 
'* This module is business logic layer of ASP.NET application that retrieves data
'* from CSAzureServiceBusWCFDS that is exposed via Service Bus 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports Client.ServiceReference1
Imports System.Data.Services.Client
Imports System.Net
Imports System.IO

Public Class BLL
    Private Shared serviceUri As Uri = New Uri("https://[Your Service Namespace].servicebus.windows.net/DataService")
    Private Shared _scope As String = "http://[Your Service Namespace].servicebus.windows.net/DataService"
    Private Shared _servicenamespace As String = "[Your Service Namespace]"
    Private Shared _issuerkey As String = "[Your Secret]"

    Public Shared Function [Select]() As IEnumerable(Of Customers)
        Dim _entities As NorthwindEntities = New NorthwindEntities(serviceUri)
        AddHandler _entities.SendingRequest, AddressOf _entities_SendingRequest
        Dim _collection As List(Of Customers) = New List(Of Customers)()
        ' Query the first 10 records to test. You can do paging by Skip and Take but for the
        ' simplicity purpose it's not shown here
        Dim query As DataServiceQuery(Of Customers) = CType((From c In _entities.Customers Select c).Skip(0).Take(10), DataServiceQuery(Of Customers))
        Dim result = query.Execute()
        Return result
    End Function

    Shared Sub _entities_SendingRequest(ByVal sender As Object, ByVal e As SendingRequestEventArgs)
        ' from Access Control Service first before accessing our service.
        Dim token = GetTokenFromACS()
        Dim headerValue As String = String.Format("WRAP access_token=""{0}""", HttpUtility.UrlDecode(token))
        ' We then attach the token to Authorization HTTP header. To learn more details please refer
        ' to WRAP specifications.
        e.Request.Headers.Add("Authorization", headerValue)
    End Sub

    Public Shared Sub Insert(ByVal CustomerID As String, ByVal CompanyName As String)
        Dim _entities As NorthwindEntities = New NorthwindEntities(serviceUri)
        AddHandler _entities.SendingRequest, AddressOf _entities_SendingRequest
        'TODO: INSTANT VB TODO TASK: Assignments within expressions are not supported in VB.NET
        'ORIGINAL LINE: _entities.AddToCustomers(New Customers() { CustomerID = CustomerID, CompanyName = CompanyName });
        Dim _customertoinsert = New Customers()
        _customertoinsert.CustomerID = CustomerID
        _customertoinsert.CompanyName = CompanyName
        _entities.AddToCustomers(_customertoinsert)
        _entities.SaveChanges()
    End Sub

    Private Shared Function GetTokenFromACS() As String
        Dim s As String = String.Empty
        Try
            ' Request a token from ACS
            Dim client As WebClient = New WebClient()
            client.BaseAddress = String.Format("https://{0}-sb.accesscontrol.windows.net", _servicenamespace)

            Dim values As NameValueCollection = New NameValueCollection()
            values.Add("wrap_name", "owner")
            values.Add("wrap_password", _issuerkey)
            values.Add("wrap_scope", _scope)

            Dim responseBytes As Byte() = client.UploadValues("WRAPv0.9", "POST", values)
            Dim response As String = Encoding.UTF8.GetString(responseBytes)
            Console.WriteLine(Constants.vbLf & "received token from ACS: {0}" & Constants.vbLf, response)

            Return response.Split("&"c).Single(Function(value As String) value.StartsWith("wrap_access_token=", StringComparison.OrdinalIgnoreCase)).Split("="c)(1)
        Catch ex As WebException
            ' You can set a breakpoint here to check detailed exception from detailedexception
            Dim sr As StreamReader = New StreamReader(ex.Response.GetResponseStream())
            Dim detailedexception = sr.ReadToEnd()
        End Try
        Return s


    End Function
End Class

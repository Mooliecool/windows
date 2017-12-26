=============================================================================
          APPLICATION : VBAzureManagementAPI Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

This project shows how to create a new hosted service on Azure via Management API.


/////////////////////////////////////////////////////////////////////////////
Demo:

1.	Fill in your subscription ID to below:
       Dim subscriptionId As String = ""

2.	Fill in the management certificate thumbprint to below:
        
        ' This is the thumbprint of the management certificate
        ' make sure the certificate exists in your personal/Certificates. Please verify it by certmgr.msc
        ' this certificate must be uploaded to Azure Management Portal, in Management Certificates 
		    ' http://msdn.microsoft.com/en-us/library/gg551726.aspx
 
        Dim certThumbprint As String = ""
  
3.	Fill in the ServiceName and the labelName:
        
        ' This will be use as the DNS prefix, so if it exists(someone else might took it) then you will 
		    ' receive 409 conflict error
        Dim hostServiceName As String = ""

        ' The Name of the hosted service
        Dim labelName As String = ""

        ' The Name of the hosted service
        string labelName = "";

4.	Run the program.

5.  You will see a service with name you defined in    
        ' The Name of the hosted service
        Dim labelName As String = "";
    created on your Azure account.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Sending the request xml string asynchronously.

        Dim formData As Byte() = UTF8Encoding.UTF8.GetBytes(sbRequestXml.ToString())
        request.ContentLength = formData.Length

        Using post As Stream = request.GetRequestStream()
            post.Write(formData, 0, formData.Length)
        End Using

        Console.WriteLine("Creating Hosted Service: " & hostServiceName)

        Try
            Dim state As New RequestState()
            state.Request = request
            Dim result As IAsyncResult = request.BeginGetResponse(New AsyncCallback(AddressOf RespCallback), state)
        Catch ex As Exception
            Console.WriteLine("Error: " & ex.Message)
        End Try

2.  Response callback.

    Private Sub RespCallback(ByVal result As IAsyncResult)

        Dim state As RequestState = DirectCast(result.AsyncState, RequestState)         ' Grab the custom state object
        Dim request As WebRequest = DirectCast(state.Request, WebRequest)

        Dim response As HttpWebResponse = DirectCast(request.EndGetResponse(result), HttpWebResponse) ' Get the Response

        Dim statusCode As String = response.StatusCode.ToString()

        ' A value that uniquely identifies a request made against the Management service.
        ' For an asynchronous operation, 
        Dim reqId As String = response.GetResponseHeader("x-ms-request-id")

        ' You can call get operation status with the value of the header to determine whether the operation is complete, 
        ' has failed, or is still in progress. 

        Console.WriteLine("Creation Return Value: " & statusCode)

        Console.WriteLine("RequestId: " & reqId)
    End Sub


/////////////////////////////////////////////////////////////////////////////
References:

Windows Azure Service Management REST API Reference
http://msdn.microsoft.com/en-us/library/ee460799.aspx

How to Add a Management Certificate to a Windows Azure Subscription
http://msdn.microsoft.com/en-us/library/gg551726.aspx


/////////////////////////////////////////////////////////////////////////////

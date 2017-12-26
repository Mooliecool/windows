'***************************** Module Header ******************************\
'* Module Name:    UploadProcessModule.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* In this sample, we can see the features listed below:
'* 1. How to get the client's request entity body by HttpWorkerRequest.
'* 2. How to control the server side to read the request data.
'* 3. How to retrieve and store the upload state.
'* 
'* Based on this module, we can extend it to realize the features listed below:
'* 1. Control the status of multiple files uploading.
'* 2. Control big file upload to the server and 
'*    store the files without the server cache.
'* (Notice: in this sample I have not realized the features above.
'*          I will add them in the near-future.)
'* 
'* The IIS restrict the request content length by default(About 28MB),
'* if we want to make this module work for large files,
'* please follow the readme file in the root directory.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports System.Web
Imports System.Text
Imports System.Web.Caching
Imports System.Collections
Imports System.Collections.Specialized
Imports System.Collections.Generic
Imports System.Diagnostics
Imports System.IO
Imports System.Text.RegularExpressions


Public Class UploadProcessModule
    Implements IHttpModule
    ''' <summary>
    ''' This is an ASP.NET HttpModule for 
    '''   Non-Component File Upload Status Display 
    '''   based on .Net Framework 4.0.
    ''' For details, please view the Readme file in the root directory.
    ''' </summary>
    Dim _cacheContainer As String = "fuFile"
    Dim _uploadedFilesFolder As String = "UploadedFiles"
    Dim _folderPath As String = ""

    Public Sub Dispose() Implements IHttpModule.Dispose

    End Sub

    Public Sub Init(ByVal context As HttpApplication) Implements IHttpModule.Init
        AddHandler context.BeginRequest, New EventHandler(AddressOf context_BeginRequest)

        ' Check about the folder for the uploaded files.
        _folderPath = HttpContext.Current.Server.MapPath(_uploadedFilesFolder)
        If Not Directory.Exists(_folderPath) Then
            Directory.CreateDirectory(_folderPath)
        End If
    End Sub

    Private Sub context_BeginRequest(ByVal sender As Object, ByVal e As EventArgs)

        Dim app As HttpApplication = TryCast(sender, HttpApplication)
        Dim context As HttpContext = app.Context

        ' We need the HttpWorkerRequest of the current context to process 
        ' the request data. For more details about HttpWorkerRequest, please 
        ' follow the Readme file in the root directory.
        Dim provider As IServiceProvider = DirectCast(context, IServiceProvider)
        Dim request As System.Web.HttpWorkerRequest = _
            DirectCast(provider.GetService(GetType(HttpWorkerRequest)), HttpWorkerRequest)


        ' Get the content type of the current request.
        Dim contentType As String = _
            request.GetKnownRequestHeader(HttpWorkerRequest.HeaderContentType)

        ' If we could not get the content type, then skip out the module
        If contentType Is Nothing Then
            Return
        End If

        ' If the content type is not multipart/form-data,
        '   means that there is not file upload request
        '   then skip out the moudle
        If contentType.IndexOf("multipart/form-data") = -1 Then
            Return
        End If

        Dim boundary As String = contentType.Substring(contentType.IndexOf("boundary=") + 9)

        ' Get the content length of the current request
        Dim contentLength As Long
        contentLength = Convert.ToInt64( _
            request.GetKnownRequestHeader(HttpWorkerRequest.HeaderContentLength))



        ' Get the data of the portion of the HTTP request body
        ' that has currently been read.
        ' This is the first step for us to store the upload file.
        Dim data As Byte() = request.GetPreloadedEntityBody()

        ' Create an instance of the manager class which 
        ' help to filter the request data.
        Dim storeManager As New FileUploadDataManager(boundary)
        ' Append the preloaded data.
        storeManager.AppendData(data)


        Dim status As UploadStatus = Nothing
        If context.Cache(_cacheContainer) Is Nothing Then
            'Initialize the UploadStatus which used to 
            'store the state for the client.
            ' Send the current context to the state
            '   which will be used for the events.
            ' Initialize the file length.
            status = New UploadStatus(context, contentLength)
            ' Bind a event when update the state.

            AddHandler status.OnDataChanged, AddressOf status_OnDataChanged
        Else
            status = TryCast(context.Cache(_cacheContainer), UploadStatus)
            If status.IsFinished Then
                Return
            End If
        End If


        ' Get the length of the left request data. 
        Dim leftdata As Long = status.ContentLength - status.LoadedLength

        ' Define a custom buffer length
        Dim customBufferLength As Integer = 2048

        While Not request.IsEntireEntityBodyIsPreloaded() AndAlso leftdata > 0
            ' Check if user abort the upload, then close the connection
            If status.Aborted Then
                ' Delete the cached files.
                For Each file As UploadFile In storeManager.FilterResult
                    file.ClearCache()
                Next
                request.CloseConnection()
                Return
            End If

            ' If the length the remained request data
            ' is less than the buffer length,
            ' then set the buffer length as the remained data length.
            If leftdata < customBufferLength Then
                customBufferLength = CInt(leftdata)
            End If

            ' Read a custom buffer length of the request data
            data = New Byte(customBufferLength - 1) {}
            Dim redlen As Integer = request.ReadEntityBody(data, customBufferLength)
            If customBufferLength > redlen Then
                data = BinaryHelper.SubData(data, 0, redlen)
            End If
            ' Append the left data.
            storeManager.AppendData(data)

            ' Add the buffer length to the status to update the upload status
            status.UpdateLoadedLength(redlen)

            leftdata -= redlen
        End While

        ' After all the data has been read,
        ' save the uploaded files.
        For Each file As UploadFile In storeManager.FilterResult
            file.Save(Nothing)
        Next


    End Sub

    Private Sub status_OnDataChanged(ByVal sender As Object, ByVal e As UploadStatusEventArgs)
        ' Store the state class to the Cache of the current context.
        Dim state As UploadStatus = TryCast(sender, UploadStatus)
        If e.context.Cache(_cacheContainer) Is Nothing Then
            e.context.Cache.Add(_cacheContainer, _
                                state, _
                                Nothing, _
                                DateTime.Now.AddDays(1), _
                                Cache.NoSlidingExpiration, _
                                CacheItemPriority.High, _
                                Nothing)
        Else
            e.context.Cache(_cacheContainer) = state
        End If

    End Sub



End Class


'***************************** Module Header ******************************\
'* Module Name:    Default.aspx.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* This is the page which we test the Upload status for the client
'* We use ICallbackEventHandler to realize the communication between
'* the server side and client side without refresh the page.
'* But we need to use an iframe to hold the upload controls and buttons,
'* because the upload need postback to the server, we can't call the server code
'* by javascript in one postback page.
'* So we let the iframe do the upload postback operation.
'* 
'* For more details about ICallbackEventHandler,
'* please read the readme file in the root directory.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'\****************************************************************************



Imports System.Collections.Generic
Imports System.Linq
Imports System.Web
Imports System.Web.UI
Imports System.Web.UI.WebControls
Imports System.Threading
Imports System.Web.Script.Serialization
Imports System.Text
Imports VBASPNETFileUploadStatus


Partial Public Class _Default
    Inherits System.Web.UI.Page
    Implements ICallbackEventHandler

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        'Register a client script for ICallbackEventHandler
        Dim cm As ClientScriptManager = Page.ClientScript
        Dim cbReference As String = cm.GetCallbackEventReference(Me, "arg", "ReceiveServerData", "")
        Dim callbackScript As String = "function CallServer(arg, context) {" & cbReference & "; }"
        cm.RegisterClientScriptBlock(Me.[GetType](), "CallServer", callbackScript, True)

    End Sub

    Private uploadModuleProgress As String = ""
    Public Function GetCallbackResult() As String Implements ICallbackEventHandler.GetCallbackResult
        Return uploadModuleProgress
    End Function

    Public Sub RaiseCallbackEvent(ByVal eventArgument As String) Implements ICallbackEventHandler.RaiseCallbackEvent
        If eventArgument = "Clear" Then
            'operation for clear the cache
            ClearCache("fuFile")
            uploadModuleProgress = "Cleared"
        End If
        If eventArgument = "Abort" Then
            'operation for abort uploading
            AbortUpload("fuFile")
            uploadModuleProgress = "Aborted"
        End If

        Try
            Dim state As UploadStatus = TryCast(HttpContext.Current.Cache("fuFile"), UploadStatus)
            If state Is Nothing Then
                Return
            End If
            ' We use JSON to send the data to the client,
            ' because it is simple and easy to handle.
            ' For more details about JavaScriptSerializer, please
            ' read the readme file in the root directory.
            Dim jss As New JavaScriptSerializer()

            ' The StringBuilder object will hold the serialized result.
            Dim sbUploadProgressResult As New StringBuilder()
            jss.Serialize(state, sbUploadProgressResult)

            uploadModuleProgress = sbUploadProgressResult.ToString()
        Catch err As Exception
            If err.InnerException IsNot Nothing Then
                uploadModuleProgress = "Error:" + err.InnerException.Message
            Else
                uploadModuleProgress = "Error:" + err.Message
            End If
        End Try
    End Sub

    Private Sub AbortUpload(ByVal cacheID As String)
        Dim state As UploadStatus = TryCast(HttpContext.Current.Cache(cacheID), UploadStatus)
        If state Is Nothing Then
            Return
        Else
            state.Abort()
        End If
    End Sub

    Private Sub ClearCache(ByVal cacheID As String)
        HttpContext.Current.Cache.Remove(cacheID)
    End Sub


End Class

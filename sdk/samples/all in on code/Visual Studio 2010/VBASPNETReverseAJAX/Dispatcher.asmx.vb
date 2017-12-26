'****************************** Module Header ******************************\
' Module Name:    Dispatcher.asmx.vb
' Project:        VBASPNETReverseAJAX
' Copyright (c) Microsoft Corporation
'
' This web service is designed to be called by the Ajax client.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Web.Services

''' <summary>
''' This web service contains methods that help dispatching events to the client.
''' </summary>
<System.Web.Script.Services.ScriptService()> _
<System.Web.Services.WebService(Namespace:="http://tempuri.org/")> _
<System.Web.Services.WebServiceBinding(ConformsTo:=WsiProfiles.BasicProfile1_1)> _
<System.ComponentModel.ToolboxItem(False)> _
Public Class Dispatcher
    Inherits System.Web.Services.WebService

    ''' <summary>
    ''' Dispatch the new message event.
    ''' </summary>
    ''' <param name="userName">The loged in user name</param>
    ''' <returns>the message content</returns>
    <WebMethod()> _
    Public Function WaitMessage(ByVal userName As String) As String
        Return ClientAdapter.Instance.GetMessage(userName)
    End Function

End Class
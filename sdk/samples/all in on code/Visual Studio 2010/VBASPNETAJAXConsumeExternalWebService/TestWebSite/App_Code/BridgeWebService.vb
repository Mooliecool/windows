'***************************** Module Header ******************************\
'* Module Name:    BridgeWebService.vb
'* Project:        VBASPNETAJAXConsumeExternalWebService
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to consume an external Web Service from a
'* different domain.
'* 
'* In this file, we create a local web service to consume the remote web service
'* like a bridge.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports System.Web
Imports System.Web.Services
Imports System.Web.Services.Protocols

<System.Web.Script.Services.ScriptService()> _
<WebService(Namespace:="http://tempuri.org/")> _
<WebServiceBinding(ConformsTo:=WsiProfiles.BasicProfile1_1)> _
<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
 Public Class BridgeWebService
    Inherits System.Web.Services.WebService

    <WebMethod()> _
    Public Function GetServerTime() As DateTime
        ' Get an instance of the external web service
        Dim ews As ExternalWebService.ExternalWebService = New ExternalWebService.ExternalWebService()
        ' Return the result from the web service method.
        Return ews.GetServerTime()
    End Function

End Class
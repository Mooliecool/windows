'***************************** Module Header ******************************\
'* Module Name:    ExternalWebService.vb
'* Project:        VBASPNETAJAXConsumeExternalWebService
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to consume an external Web Service from a
'* different domain.
'* 
'* In this file, we impersonate a remote web service in a different
'* domain. Make sure this web service is online when we test this sample.
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
 Public Class ExternalWebService
    Inherits System.Web.Services.WebService

    <WebMethod()> _
    Public Function GetServerTime() As DateTime
        Return DateTime.Now
    End Function

End Class
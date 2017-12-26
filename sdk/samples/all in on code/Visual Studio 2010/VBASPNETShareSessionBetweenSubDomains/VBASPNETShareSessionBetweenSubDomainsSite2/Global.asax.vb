'/****************************** Module Header ******************************\
'* Module Name:    Global.asax.vb
'* Project:        VBASPNETShareSessionBetweenSubDomainsSite2
'* Copyright (c) Microsoft Corporation
'*
'* This project demonstrates how to configure a SQL Server as SessionState and 
'* make a module to share Session between two Web Sites with the same root domain.
'* 
'* The code in Global.asax is just to ensure you have set up Session State Sql Server
'* and can run this sample without any configuration or command line
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\*****************************************************************************/

Public Class Global_asax
    Inherits System.Web.HttpApplication
    Sub Application_Error(ByVal sender As Object, ByVal e As EventArgs)
        Response.Clear()
        Response.Write("Before runing this sample, please run this command:<br />")
        Response.Write("""<b>C:\Windows\Microsoft.NET\Framework\v4.0.30319\aspnet_regsql.exe -S localhost\sqlexpress -E -ssadd</b>""<br />")
        Response.Write("to configure localhost Sql Server Experssion to support Session State.<br /><br />")
        Response.Write("To know how to rollback this configuration, please check the ReadMe.txt file.")
        Response.End()
    End Sub
End Class
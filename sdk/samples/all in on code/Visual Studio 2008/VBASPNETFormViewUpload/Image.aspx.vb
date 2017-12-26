'****************************** Module Header *******************************
' Module Name:  Image.aspx.vb
' Project:      VBASPNETFormViewUpload
' Copyright (c) Microsoft Corporation.
' 
' This page is used to retrieve the image from a SQL Server database and 
' display it in the Web page.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************

#Region "Using directives"
Imports System.Data.SqlClient
#End Region

Partial Public Class Image
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        If Request.QueryString("PersonID") IsNot Nothing Then
            ' Get the connection string from Web.config. 
            ' When we use Using statement, 
            ' we don't need to explicitly dispose the object in the code, 
            ' the using statement takes care of it.
            Using conn As New SqlConnection(ConfigurationManager.ConnectionStrings("SQLServer2005DBConnectionString").ToString())
                ' Create a command object.
                Dim cmd As New SqlCommand()

                ' Assign the connection to the command.
                cmd.Connection = conn

                ' Set the command text
                ' SQL statement or the name of the stored procedure. 
                cmd.CommandText = "SELECT Picture FROM Person WHERE PersonID = @PersonID AND Picture IS NOT NULL"

                ' Set the command type
                ' CommandType.Text for ordinary SQL statements; 
                ' CommandType.StoredProcedure for stored procedures.
                cmd.CommandType = CommandType.Text

                ' Append the parameter to the SqlCommand and set value.
                cmd.Parameters.Add("@PersonID", SqlDbType.Int).Value = Request.QueryString("PersonID")

                ' Open the connection.
                conn.Open()

                ' Convert the returned result to a bytes array.
                Dim bytes As Byte() = DirectCast(cmd.ExecuteScalar(), Byte())

                If bytes IsNot Nothing Then
                    ' Set the HTTP MIME type of the output stream.
                    Response.ContentType = "image/jpeg"
                    ' Write a string of Binary characters to the HTTP 
                    ' output stream.
                    Response.BinaryWrite(bytes)
                    ' Send all currently buffered output to the client. 
                    Response.End()
                End If
            End Using
        End If

    End Sub

End Class
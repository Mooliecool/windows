'****************************** Module Header ******************************\
' Module Name:  ImageHandler.ashx
' Project:      VBASPNETImageEditUpload
' Copyright (c) Microsoft Corporation
'
' This is a common http-handler to read out byte collection
' from certain database according to the specific Id record.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'***************************************************************************/

Imports System.Web
Imports System.Data.SqlClient
Imports System.IO
Imports System.Drawing
Imports System.Drawing.Imaging


Public Class ImageHandler
    Implements System.Web.IHttpHandler

    Sub ProcessRequest(ByVal context As HttpContext) Implements IHttpHandler.ProcessRequest

        Using cmd As New SqlCommand()
            cmd.Connection = New SqlConnection(
                ConfigurationManager.ConnectionStrings("db_PersonsConnectionString") _
                .ConnectionString)
            cmd.Connection.Open()
            cmd.CommandText = "select PersonImage,PersonImageType from tb_personInfo" _
                & " where id=" + context.Request.QueryString("id")

            Dim reader As SqlDataReader = cmd.ExecuteReader(
                CommandBehavior.CloseConnection Or CommandBehavior.SingleRow)
            If (reader.Read) Then
                Dim imgbytes() As Byte = Nothing
                Dim imgtype As String = Nothing

                If (reader.GetValue(0) IsNot DBNull.Value) Then
                    imgbytes = CType(reader.GetValue(0), Byte())
                    imgtype = reader.GetString(1)

                    ' If bmp, convert to jpg and show because of the different formation type.
                    If (imgtype.Equals("image/bmp", StringComparison.OrdinalIgnoreCase)) Then
                        Using ms As New MemoryStream(imgbytes)
                            Using bt As New Bitmap(ms)
                                bt.Save(context.Response.OutputStream, ImageFormat.Jpeg)
                            End Using
                        End Using
                    Else
                        context.Response.ContentType = imgtype
                        context.Response.BinaryWrite(imgbytes)
                    End If
                Else
                    imgbytes = File.ReadAllBytes(
                        context.Server.MapPath("~/DefaultImage/DefaultImage.JPG"))
                    imgtype = "image/pjpeg"
                End If
                context.Response.ContentType = imgtype
                context.Response.BinaryWrite(imgbytes)
            End If

            reader.Close()
            context.Response.End()
        End Using
    End Sub


    ReadOnly Property IsReusable() As Boolean Implements IHttpHandler.IsReusable
        Get
            Return False
        End Get
    End Property

End Class
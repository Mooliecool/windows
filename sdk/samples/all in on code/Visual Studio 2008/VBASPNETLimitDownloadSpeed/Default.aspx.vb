'*************************** Module Header ********************************\
'* Module Name:    Default.aspx.vb
'* Project:        VBASPNETLimitDownloadSpeed
'* Copyright (c) Microsoft Corporation
'*
'* This project illustrates how to limit the download speed via coding. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\**************************************************************************

Imports System.IO
Imports System.Threading

Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' You can enlarge the size of the file to have a longer download period later.
        ' 1024 * 1024 * 1 = 1 Mb
        Dim length As Integer = 1024 * 1024 * 1
        Dim buffer As Byte() = New Byte(length - 1) {}

        Dim filepath As String = Server.MapPath("~/bigFileSample.dat")
        Using fs As New FileStream(filepath, FileMode.Create, FileAccess.Write)
            fs.Write(buffer, 0, length)
        End Using
    End Sub

    Protected Sub btnDownload_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnDownload.Click
        Dim outputFileName As String = "bigFileSample.dat"
        Dim filePath As String = Server.MapPath("~/bigFileSample.dat")

        Dim value As String = ddlDownloadSpeed.SelectedValue

        ' 1024 * 20 = 20 Kb/s.
        Dim downloadSpeed As Integer = 1024 * Integer.Parse(value)
        Response.Clear()

        ' Call DownloadFileWithLimitedSpeed method to download the file.
        Try
            DownloadFileWithLimitedSpeed(outputFileName, filePath, downloadSpeed)
        Catch ex As Exception
            Response.Write("<p><font color=""red"">")
            Response.Write(ex.Message)
            Response.Write("</font></p>")
        End Try
        Response.End()
    End Sub

    Public Sub DownloadFileWithLimitedSpeed(ByVal fileName As String, ByVal filePath As String, ByVal downloadSpeed As Long)
        If Not File.Exists(filePath) Then
            Throw New Exception("Err: There is no such a file to download.")
        End If

        ' Get the BinaryReader instance to the file to download.
        Using fs As New FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite)
            Using br As New BinaryReader(fs)
                Response.Buffer = False

                ' The file length.
                Dim fileLength As Long = fs.Length

                ' The minimum size of a package 1024 = 1 Kb.
                Dim pack As Integer = 1024

                ' The original formula is: sleep = 1000 / (downloadspeed / pack)
                ' which equals to 1000.0 * pack / downloadSpeed.
                ' And here 1000.0 stands for 1000 millisecond = 1 second
                Dim sleep As Integer = CInt(Math.Truncate(Math.Ceiling(1000.0 * pack / downloadSpeed)))

                ' Set the Header of the current Response.
                Response.AddHeader("Content-Length", fileLength.ToString())
                Response.ContentType = "application/octet-stream"

                Dim utf8EncodingFileName As String = HttpUtility.UrlEncode(fileName, System.Text.Encoding.UTF8)
                Response.AddHeader("Content-Disposition", "attachment;filename=" & utf8EncodingFileName)

                ' The maxCount stands for a total count that the thread sends the file pack.
                Dim maxCount As Integer = CInt(Math.Truncate(Math.Ceiling(Convert.ToDouble(fileLength) / pack)))

                For i As Integer = 0 To maxCount - 1
                    If Response.IsClientConnected Then
                        Response.BinaryWrite(br.ReadBytes(pack))

                        ' Sleep the response thread after it sends a file pack.
                        Thread.Sleep(sleep)
                    Else
                        Exit For
                    End If
                Next
            End Using
        End Using
    End Sub

End Class
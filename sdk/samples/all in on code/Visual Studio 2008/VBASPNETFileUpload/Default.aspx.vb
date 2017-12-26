'****************************** Module Header ******************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETFileUpload
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to upload files in an ASP.NET project by use 
' of FileUpload control and VB.NET language. When submit Button is clicked,
' the HasFile property of the FileUpload control is checked to verify that
' a file has been selected to upload. Before the file is saved, File.Exists 
' method is called to check whether a file that has the same name already 
' exists in the path. If so, the name of uploaded file is postfixed with a 
' increasing number so that the exsited file will not be overwritten. After
' the file is uploaded successfully, some basic information is displayed, 
' including the original path, name, the size and the content type of the
' uploaded file. All the information is get from the HttpPostedFile object
' pointed by FileUpload.PostedFile property. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 7/13/2009 11:30 AM Bravo Yang Created
'***************************************************************************/

#Region "Imports Directory"

Imports System.IO
Imports System.Text

#End Region


Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub btnSubmit_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSubmit.Click

        ' FileUpload.HasFile property returns the indicating of whether a file 
        ' is contained in FileUpload control.
        If FileUploader.HasFile = False Then
            ' If there is no file contained in FileUpload control, notify the 
            ' user and exit the function.
            Response.Write("NO FILE SELECTED")
            Exit Sub
        Else

            ' Specify the path to save the uploaded file.
            Dim savePath As String = Server.MapPath("~/UploadFiles")

            ' Set the uploaded file to an HttpPostedFile instance
            Dim uploadedFile As HttpPostedFile = FileUploader.PostedFile

            ' Call SaveUploadFile method to save the file to the server.
            If SaveUploadFile(uploadedFile, savePath) Then

                Response.Write("FILE UPLOADING SUCCEEDED")
                Response.Write("<br />")

                ' Call GetUploadFileInfo method to get the basic information 
                ' of the uploaded file and display the information to users.
                Response.Write(GetUploadFileInfo(uploadedFile).Replace(vbCrLf, "<br />"))
            Else
                Response.Write("ERROR: FILE UPLOADING FAILED")
            End If

        End If
    End Sub

    Protected Function SaveUploadFile(ByVal uploadedFile As HttpPostedFile, ByVal SavePath As String) As Boolean

        ' Get the name and the extension of the uploaded file
        Dim fileName As String = Path.GetFileName(uploadedFile.FileName)
        Dim fileNameWithOutExt As String = Path.GetFileNameWithoutExtension(fileName)
        Dim fileExtension As String = Path.GetExtension(fileName)

        Dim tempCount As Integer = 0

        ' Call File.Exists to check whether a file with the same name already
        ' exists
        While File.Exists(Path.Combine(SavePath, fileName))
            ' If the file with the same name exists, rename the uploaded file 
            ' with a increasing number postfixed
            tempCount += 1
            fileName = fileNameWithOutExt & "[" & tempCount & "]" & fileExtension
        End While

        Try
            ' Call SaveAs() method to save the uploaded file.
            uploadedFile.SaveAs(Path.Combine(SavePath, fileName))
            Return True

        Catch ex As Exception
            Response.Write("ERROR: " & ex.Message & "<br />")
            Return False
        End Try

    End Function

    Protected Function GetUploadFileInfo(ByVal uploadedFile As HttpPostedFile) As String
        Dim info As New StringBuilder

        ' Get the original path and name of the uploaded file.
        info.AppendLine("File Name: " & uploadedFile.FileName)

        ' Get the size of the uploaded file.
        info.AppendLine("File Size: " & uploadedFile.ContentLength & "Bytes")

        ' Get the content type of the uploaded file.
        info.AppendLine("File Content Type: " & uploadedFile.ContentType)

        Return info.ToString
    End Function

End Class


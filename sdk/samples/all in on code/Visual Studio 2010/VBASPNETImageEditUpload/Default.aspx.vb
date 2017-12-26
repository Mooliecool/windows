'****************************** Module Header ******************************\
' Module Name:    Default.aspx.cs
' Project:        VBASPNETImageEditUpload
' Copyright (c) Microsoft Corporation
'
' The project shows up how to insert,edit or update an image and store
' it into Sql database.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'***************************************************************************/

Imports System.Collections.Generic
Imports System.Web.UI.WebControls
Imports System.Data.SqlClient
Imports System.IO


Partial Public Class _Default
    Inherits System.Web.UI.Page

    ' Static types of common images for checking.
    Private Shared imgytpes As New List(Of String)() From { _
        ".BMP", _
        ".GIF", _
        ".JPG", _
        ".PNG" _
    }

    ''' <summary>
    ''' Read all records into GridView.
    ''' If has records, select the first record to be shown in the FormView
    ''' as default; otherwise, change the formview to insert mode so as to let
    ''' data to be inserted.
    ''' </summary>
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        If Not IsPostBack Then

            gvPersonOverView.DataBind()

            If gvPersonOverView.Rows.Count > 0 Then
                gvPersonOverView.SelectedIndex = 0
                fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
                fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
            Else
                fvPersonDetails.ChangeMode(FormViewMode.Insert)
                fvPersonDetails.DefaultMode = FormViewMode.Insert
            End If
        End If
    End Sub

    ''' <summary>
    ''' Validate whether data satisfies the type of image.
    ''' </summary>
    Protected Sub CustomValidator1_ServerValidate(ByVal source As Object, ByVal args As ServerValidateEventArgs)
        If args.Value IsNot Nothing AndAlso args.Value <> "" Then
            args.IsValid = imgytpes.IndexOf(System.IO.Path.GetExtension(args.Value).ToUpper()) >= 0
        End If
    End Sub

    ''' <summary>
    ''' After checking the validation of the image type,
    ''' assign the image type and the image byte collection through
    ''' the e.Values argument and do the insert.
    ''' </summary>
    Protected Sub fvPersonDetails_ItemInserting(ByVal sender As Object, ByVal e As FormViewInsertEventArgs)
        Dim obj As Object = Session("insertstate")
        If obj Is Nothing OrElse CBool(obj) Then
            Dim cv As CustomValidator = TryCast(fvPersonDetails.FindControl("cmvImageType"), CustomValidator)

            cv.Validate()
            e.Cancel = Not cv.IsValid

            Dim fup As FileUpload = DirectCast(fvPersonDetails.FindControl("fupInsertImage"), FileUpload)

            If cv.IsValid AndAlso fup.PostedFile.FileName.Trim() <> "" Then
                e.Values("PersonImage") = File.ReadAllBytes(fup.PostedFile.FileName)
                e.Values("PersonImageType") = fup.PostedFile.ContentType

            End If
        Else
            e.Cancel = True
            gvPersonOverView.DataBind()
            fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
            fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
        End If
    End Sub

    ''' <summary>
    ''' After checking the validation of the image type,
    ''' assign the image type and the image byte collection through
    ''' the e.Values argument and do the update.
    ''' </summary>
    Protected Sub fvPersonDetails_ItemUpdating(ByVal sender As Object, ByVal e As FormViewUpdateEventArgs)
        Dim cv As CustomValidator = TryCast(fvPersonDetails.FindControl("cmvImageType"), CustomValidator)

        cv.Validate()
        e.Cancel = Not cv.IsValid

        Dim fup As FileUpload = DirectCast(fvPersonDetails.FindControl("fupEditImage"), FileUpload)

        If cv.IsValid AndAlso fup.PostedFile.FileName.Trim() <> "" Then
            e.NewValues("PersonImage") = File.ReadAllBytes(fup.PostedFile.FileName)
            e.NewValues("PersonImageType") = fup.PostedFile.ContentType
        End If
    End Sub

    ''' <summary>
    ''' After updated, re-databind data and select the first one as default.
    ''' </summary>
    Protected Sub fvPersonDetails_ItemUpdated(ByVal sender As Object, ByVal e As FormViewUpdatedEventArgs)
        gvPersonOverView.DataBind()
        gvPersonOverView.SelectedIndex = gvPersonOverView.SelectedRow.RowIndex
    End Sub

    ''' <summary>
    ''' After inserted successfully, re-databind data,select the first one as default,
    ''' Change the FormView mode to ReadOnly (for viewing).
    ''' </summary>
    Protected Sub fvPersonDetails_ItemInserted(ByVal sender As Object, ByVal e As FormViewInsertedEventArgs)
        gvPersonOverView.DataBind()
        gvPersonOverView.SelectedIndex = gvPersonOverView.Rows.Count - 1
        fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
        fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
    End Sub

    ''' <summary>
    ''' After deleted successfully, re-databind data.
    ''' </summary>
    Protected Sub fvPersonDetails_ItemDeleted(ByVal sender As Object, ByVal e As FormViewDeletedEventArgs)
        gvPersonOverView.DataBind()

        If gvPersonOverView.Rows.Count > 0 Then
            Dim delindex As Integer = CInt(ViewState("delindex"))
            If delindex = 0 Then
                gvPersonOverView.SelectedIndex = 0
            ElseIf delindex = gvPersonOverView.Rows.Count Then
                gvPersonOverView.SelectedIndex = gvPersonOverView.Rows.Count - 1
            Else
                gvPersonOverView.SelectedIndex = delindex

            End If
        Else
            fvPersonDetails.ChangeMode(FormViewMode.Insert)
            fvPersonDetails.DefaultMode = FormViewMode.Insert
        End If
    End Sub

    ''' <summary>
    ''' To show detail image and information in the FormView when GridView's
    ''' SelectedRowIndex Changed.
    ''' </summary>
    Protected Sub gvPersonOverView_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs)
        fvPersonDetails.ChangeMode(FormViewMode.[ReadOnly])
        fvPersonDetails.DefaultMode = FormViewMode.[ReadOnly]
    End Sub

    ''' <summary>
    ''' Keep the row index into ViewState for the usage of Item_Deleted.
    ''' </summary>
    Protected Sub fvPersonDetails_ItemDeleting(ByVal sender As Object, ByVal e As FormViewDeleteEventArgs)
        ViewState("delindex") = gvPersonOverView.SelectedIndex
    End Sub

    ''' <summary>
    ''' Keep the insertState into Session to avoid the duplicated inserting
    ''' after refreshing page.
    ''' </summary>
    Protected Sub fvPersonDetails_ModeChanging(ByVal sender As Object, ByVal e As FormViewModeEventArgs)
        Session("insertstate") = (e.NewMode = FormViewMode.Insert)
    End Sub

End Class
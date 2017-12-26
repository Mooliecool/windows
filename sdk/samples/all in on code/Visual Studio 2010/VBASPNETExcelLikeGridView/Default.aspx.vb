'***************************** Module Header ******************************\
'* Module Name:    DBProcess.vb
'* Project:        CSExcelLikeGridView
'* Copyright (c) Microsoft Corporation
'*
'* This is a UI module which lets which lets you do a batch inserting, updating
'* as well as deleting.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'\****************************************************************************


Imports System.Web.UI.WebControls
Imports System.Data
Imports System.Text

Partial Public Class _Default
    Inherits System.Web.UI.Page
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        If Not IsPostBack Then
            Dim p As New DBProcess()
            Dim dt As DataTable = p.GetDataTable()
            ViewState("dt") = dt
            GridView1.DataSource = dt
            GridView1.DataBind()
        End If
    End Sub

    ''' <summary>
    ''' This function will confirm the last modification
    ''' and do a batch save.
    ''' </summary>
    Protected Sub btnSaveAll_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSaveAll.Click

        'Default the value is false, meaning that the db isn't saved
        Dim flag As Boolean = False

        Dim p As New DBProcess()
        Dim dt As DataTable = CType(ViewState("dt"), DataTable)

        ' Change states and do a batch update
        For i As Integer = 0 To GridView1.Rows.Count - 1
            If TryCast(GridView1.Rows(i).FindControl("chkDelete"), CheckBox).Checked Then
                dt.Rows(i).Delete()
				flag = True
            Else
                If dt.Rows(i)("PersonName").ToString() <> TryCast(GridView1.Rows(i).FindControl("tbName"), TextBox).Text Then
                    If dt.Rows(i).RowState = DataRowState.Unchanged Then
                        dt.Rows(i).BeginEdit()
                    End If
                    dt.Rows(i)("PersonName") = TryCast(GridView1.Rows(i).FindControl("tbName"), TextBox).Text
                    If dt.Rows(i).RowState = DataRowState.Unchanged Then
                        dt.Rows(i).EndEdit()
                    End If
                    flag = True
                End If
                If dt.Rows(i)("PersonAddress").ToString() <> TryCast(GridView1.Rows(i).FindControl("tbAddress"), TextBox).Text Then
                    If dt.Rows(i).RowState = DataRowState.Unchanged Then
                        dt.Rows(i).BeginEdit()
                    End If
                    dt.Rows(i)("PersonAddress") = TryCast(GridView1.Rows(i).FindControl("tbAddress"), TextBox).Text
                    If dt.Rows(i).RowState = DataRowState.Unchanged Then
                        dt.Rows(i).EndEdit()
                    End If
                    flag = True
                End If
            End If
        Next

        p.BatchSave(dt)

        ' Save data into db, so no need to maintain the state colors
        HidState.Value = "[]"

        dt = p.GetDataTable()
        GridView1.DataSource = dt
        GridView1.DataBind()

        If (flag) Then
            ClientScript.RegisterStartupScript([GetType](), "js", "alert('Save All Changes successfully!');", True)
        End If

    End Sub

    ''' <summary>
    '''  This function processes with the following aspect:
    '''  Record every row and every cell's state into HidState.
    ''' </summary>
    Protected Sub btnAdd_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim i As Integer = 0
        Dim p As New DBProcess()
        Dim dt As DataTable = CType(ViewState("dt"), DataTable)
        Dim tb1 As TextBox = Nothing
        Dim tb2 As TextBox = Nothing
        Dim sbu As New StringBuilder()
        Dim chkbox As CheckBox = Nothing

        ' To maintain the JSON state.
        sbu.Append("[")

        While i < dt.Rows.Count
            ' Loop the cells
            For j As Integer = 2 To GridView1.HeaderRow.Cells.Count - 1
                tb1 = TryCast(GridView1.Rows(i).FindControl("tbName"), TextBox)
                tb2 = TryCast(GridView1.Rows(i).FindControl("tbAddress"), TextBox)

                sbu.Append("{'Index':'" & (i * GridView1.HeaderRow.Cells.Count + j))

                'Not added, meaning unchanged or added
                If dt.Rows(i).RowState <> DataRowState.Added Then
                    ' Decide whether the 1st cell in db changed or not
                    If j = 2 Then
                        If Not tb1.Text.Equals(dt.Rows(i)(j - 1, DataRowVersion.Original)) Then
                            dt.Rows(i).BeginEdit()
                            sbu.Append("','Color':'blue',")
                            dt.Rows(i)(j - 1) = tb1.Text
                        Else
                            sbu.Append("','Color':'',")
                        End If
                    Else
                        ' Decide whether the 2nd cell in db changed or not
                        If Not tb2.Text.Equals(dt.Rows(i)(j - 1, DataRowVersion.Original)) Then
                            dt.Rows(i).BeginEdit()
                            sbu.Append("','Color':'blue',")
                            dt.Rows(i)(j - 1) = tb2.Text
                        Else
                            sbu.Append("','Color':'',")
                        End If
                    End If
                    dt.Rows(i).EndEdit()
                Else

                    ' Add row should be marked as green
                    If dt.Rows(i).RowState = DataRowState.Added Then
                        sbu.Append("','Color':'green',")
                    Else
                        ' Other rows should keep the original color
                        sbu.Append("','Color':'',")
                    End If
                End If

                ' Keep the Delete Statement
                chkbox = TryCast(GridView1.Rows(i).FindControl("chkDelete"), CheckBox)
                sbu.Append("'Deleted':'" & chkbox.Checked & "'},")
            Next
            i += 1
        End While

        Dim r As DataRow = dt.NewRow()
        r("PersonName") = TryCast(GridView1.FooterRow.FindControl("tbNewName"), TextBox).Text
        r("PersonAddress") = TryCast(GridView1.FooterRow.FindControl("tbNewAddress"), TextBox).Text
        dt.Rows.Add(r)
        sbu.Append("{'Index':'" & (i * GridView1.HeaderRow.Cells.Count + 2) & "','Color':'green','Deleted':'false'},")
        sbu.Append("{'Index':'" & (i * GridView1.HeaderRow.Cells.Count + 3) & "','Color':'green','Deleted':'false'}")
        sbu.Append("]")
        HidState.Value = sbu.ToString()
        GridView1.DataSource = dt
        GridView1.DataBind()
    End Sub
End Class

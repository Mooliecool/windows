'****************************** Module Header *******************************
' Module Name:  Default.aspx.vb
' Project:      VBASPNETFormViewUpload
' Copyright (c) Microsoft Corporation.
' 
' This page populates a FromView control with data from a SQL Server 
' database and provides UI for data manipulation.
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

Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' The Page is accessed for the first time.
        If Not IsPostBack Then
            ' Enable the FormView paging option and 
            ' specify the PageButton count.
            fvPerson.AllowPaging = True
            fvPerson.PagerSettings.PageButtonCount = 15

            ' Populate the FormView control.
            BindFormView()
        End If
    End Sub

    Private Sub BindFormView()
        ' Get the connection string from Web.config. 
        ' When we use Using statement, 
        ' we don't need to explicitly dispose the object in the code, 
        ' the using statement takes care of it.
        Using conn As New SqlConnection(ConfigurationManager.ConnectionStrings("SQLServer2005DBConnectionString").ToString())
            ' Create a DataSet object.
            Dim dsPerson As New DataSet()

            ' Create a SELECT query.
            Dim strSelectCmd As String = "SELECT PersonID,LastName,FirstName FROM Person"

            ' Create a SqlDataAdapter object
            ' SqlDataAdapter represents a set of data commands and a 
            ' database connection that are used to fill the DataSet and 
            ' update a SQL Server database. 
            Dim da As New SqlDataAdapter(strSelectCmd, conn)

            ' Open the connection
            conn.Open()

            ' Fill the DataTable named "Person" in DataSet with the rows
            ' returned by the query.
            da.Fill(dsPerson, "Person")

            ' Bind the FormView control.
            fvPerson.DataSource = dsPerson
            fvPerson.DataBind()
        End Using
    End Sub

    ' FormView.ItemDeleting Event
    Protected Sub fvPerson_ItemDeleting(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.FormViewDeleteEventArgs) Handles fvPerson.ItemDeleting
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
            cmd.CommandText = "DELETE FROM Person WHERE PersonID = @PersonID"

            ' Set the command type
            ' CommandType.Text for ordinary SQL statements; 
            ' CommandType.StoredProcedure for stored procedures.
            cmd.CommandType = CommandType.Text

            ' Get the PersonID from the ItemTemplate of the FormView 
            ' control.
            Dim strPersonID As String = DirectCast(fvPerson.Row.FindControl("lblPersonID"), Label).Text

            ' Append the parameter to the SqlCommand and set value.
            cmd.Parameters.Add("@PersonID", SqlDbType.Int).Value = strPersonID

            ' Open the connection.
            conn.Open()

            ' Execute the command.
            cmd.ExecuteNonQuery()
        End Using

        ' Rebind the FormView control to show data after deleting.
        BindFormView()
    End Sub

    ' FormView.ItemInserting Event
    Protected Sub fvPerson_ItemInserting(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.FormViewInsertEventArgs) Handles fvPerson.ItemInserting
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
            cmd.CommandText = "INSERT INTO Person ( LastName, FirstName, Picture ) VALUES ( @LastName, @FirstName, @Picture )"

            ' Set the command type
            ' CommandType.Text for ordinary SQL statements; 
            ' CommandType.StoredProcedure for stored procedures.
            cmd.CommandType = CommandType.Text

            ' Get the first name and last name from the 
            ' InsertItemTemplate of the FormView control.
            Dim strLastName As String = DirectCast(fvPerson.Row.FindControl("tbLastName"), TextBox).Text
            Dim strFirstName As String = DirectCast(fvPerson.Row.FindControl("tbFirstName"), TextBox).Text

            ' Append the parameters to the SqlCommand and set values.
            cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = strLastName
            cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = strFirstName

            Dim uploadPicture As FileUpload = DirectCast(fvPerson.FindControl("uploadPicture"), FileUpload)

            If uploadPicture.HasFile Then
                ' Append the Picture parameter to the SqlCommand.
                ' If a picture is specified, set the parameter with 
                ' the value of bytes in the specified picture file. 
                cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = uploadPicture.FileBytes
            Else
                ' Append the Picture parameter to the SqlCommand.
                ' If no picture is specified, set the parameter's 
                ' value to NULL.
                cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = DBNull.Value
            End If

            ' Open the connection.
            conn.Open()

            ' Execute the command.
            cmd.ExecuteNonQuery()
        End Using

        ' Switch FormView control to the ReadOnly display mode. 
        fvPerson.ChangeMode(FormViewMode.ReadOnly)

        ' Rebind the FormView control to show data after inserting.
        BindFormView()
    End Sub

    ' FormView.ItemUpdating Event
    Protected Sub fvPerson_ItemUpdating(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.FormViewUpdateEventArgs) Handles fvPerson.ItemUpdating
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
            cmd.CommandText = "UPDATE Person SET LastName = @LastName, FirstName = @FirstName, Picture = ISNULL(@Picture,Picture) WHERE PersonID = @PersonID"

            ' Set the command type
            ' CommandType.Text for ordinary SQL statements; 
            ' CommandType.StoredProcedure for stored procedures.
            cmd.CommandType = CommandType.Text

            ' Get the person ID, first name and last name from the 
            ' EditItemTemplate of the FormView control.
            Dim strPersonID As String = DirectCast(fvPerson.Row.FindControl("lblPersonID"), Label).Text
            Dim strLastName As String = DirectCast(fvPerson.Row.FindControl("tbLastName"), TextBox).Text
            Dim strFirstName As String = DirectCast(fvPerson.Row.FindControl("tbFirstName"), TextBox).Text

            ' Append the parameters to the SqlCommand and set values.
            cmd.Parameters.Add("@PersonID", SqlDbType.Int).Value = strPersonID
            cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = strLastName
            cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = strFirstName

            ' Find the FileUpload control in the EditItemTemplate of 
            ' the FormView control.
            Dim uploadPicture As FileUpload = DirectCast(fvPerson.FindControl("uploadPicture"), FileUpload)

            If uploadPicture.HasFile Then
                ' Append the Picture parameter to the SqlCommand.
                ' If a picture is specified, set the parameter with 
                ' the value of bytes in the specified picture file. 
                cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = uploadPicture.FileBytes
            Else
                ' Append the Picture parameter to the SqlCommand.
                ' If no picture is specified, set the parameter's 
                ' value to NULL.
                cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = DBNull.Value
            End If

            ' Open the connection.
            conn.Open()

            ' Execute the command.
            cmd.ExecuteNonQuery()
        End Using

        ' Switch FormView control to the ReadOnly display mode. 
        fvPerson.ChangeMode(FormViewMode.ReadOnly)

        ' Rebind the FormView control to show data after updating.
        BindFormView()
    End Sub

    ' FormView.PageIndexChanging Event
    Protected Sub fvPerson_PageIndexChanging(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.FormViewPageEventArgs) Handles fvPerson.PageIndexChanging
        ' Set the index of the new display page. 
        fvPerson.PageIndex = e.NewPageIndex

        ' Rebind the FormView control to show data in the new page.
        BindFormView()
    End Sub

    ' FormView.ModeChanging Event
    Protected Sub fvPerson_ModeChanging(ByVal sender As Object, ByVal e As System.Web.UI.WebControls.FormViewModeEventArgs) Handles fvPerson.ModeChanging
        ' Switch FormView control to the new mode
        fvPerson.ChangeMode(e.NewMode)

        ' Rebind the FormView control to show data in new mode.
        BindFormView()
    End Sub
End Class
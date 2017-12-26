'****************************** Module Header ******************************\
' Module Name:  RelatedTableUpdateForm.vb
' Project:      VBUseADONET
' Copyright (c) Microsoft Corporation.
' 
' The RelatedTableUpdateForm example demonstrates the Microsoft ADO.NET 
' technology to update related data tables in the databases using Visual Basic. 
' It shows hot to use TableAdapterManager to update two related data tables
' and how to write codes manually to update two related data tables.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports VBUseADONET
Imports System.Windows.Forms
#End Region

Public Class RelatedTableUpdateForm

    ''' <summary>
    ''' Fill the data tables in Form's load event
    ''' </summary>
    ''' 

    Private Sub RelatedTableUpdateForm_Load(ByVal sender As System.Object, _
                                            ByVal e As System.EventArgs) _
                                            Handles MyBase.Load
        ' TODO: This line of code loads data into the
        ' 'SQLServer2005DBDataSet.Course' table. You can move, or remove it,
        ' as needed.
        Me.CourseTableAdapter.Fill(Me.SQLServer2005DBDataSet.Course)
        'TODO: This line of code loads data into the 
        ' 'SQLServer2005DBDataSet.Department' table. You can move, or remove it,
        ' as needed.
        Me.DepartmentTableAdapter.Fill(Me.SQLServer2005DBDataSet.Department)
    End Sub

    Private Sub CourseBindingNavigatorSaveItem_Click(ByVal sender As System.Object, _
                                                     ByVal e As System.EventArgs)
        Me.Validate()
        Me.CourseBindingSource.EndEdit()
        Me.TableAdapterManager.UpdateAll(Me.SQLServer2005DBDataSet)

    End Sub

    ''' <summary>
    ''' Update the related data tables by TableAdapterManager
    ''' </summary>
    ''' 

    Private Sub DepartmentBindingNavigatorSaveItem_Click(ByVal sender As System.Object, _
                                                         ByVal e As System.EventArgs) _
                                                         Handles DepartmentBindingNavigatorSaveItem.Click

        Me.Validate()
        Me.DepartmentBindingSource.EndEdit()

        ' Add this line of code in the codes automatically created
        Me.CourseBindingSource.EndEdit()

        ' Use TableAdapterManager to update the related data tables
        Me.TableAdapterManager.UpdateAll(Me.SQLServer2005DBDataSet)

    End Sub

    ''' <summary>
    ''' Update the related data tables by writing codes manually
    ''' </summary>
    ''' 

    Private Sub DepartmentBindingNavigatorSaveItem2_Click(ByVal sender As System.Object, _
                                                          ByVal e As System.EventArgs) _
                                                          Handles DepartmentBindingNavigatorSaveItem2.Click
        Me.Validate()
        Me.DepartmentBindingSource.EndEdit()

        ' Add this line of code in the codes automatically created
        Me.CourseBindingSource.EndEdit()

        ' Writing codes manually to update the related data tables

        ' Get all the deleted data rows in the Course table
        Dim deletedCourses As SQLServer2005DBDataSet.CourseDataTable = _
        DirectCast(SQLServer2005DBDataSet.Course.GetChanges(DataRowState.Deleted),  _
        SQLServer2005DBDataSet.CourseDataTable)

        ' Get all the new data rows in the Course table
        Dim newCourses As SQLServer2005DBDataSet.CourseDataTable = _
        DirectCast(SQLServer2005DBDataSet.Course.GetChanges(DataRowState.Added),  _
        SQLServer2005DBDataSet.CourseDataTable)

        ' Get all the modified data rows in the Course table
        Dim modifiedCourses As SQLServer2005DBDataSet.CourseDataTable = _
        DirectCast(SQLServer2005DBDataSet.Course.GetChanges(DataRowState.Modified),  _
        SQLServer2005DBDataSet.CourseDataTable)

        Try
            ' Remove all deleted coures from the Course table.
            If deletedCourses IsNot Nothing Then
                CourseTableAdapter.Update(deletedCourses)
            End If

            ' Update the Department table.
            DepartmentTableAdapter.Update(SQLServer2005DBDataSet.Department)

            ' Add new courses to the Course table.
            If newCourses IsNot Nothing Then
                CourseTableAdapter.Update(newCourses)
            End If

            ' Update all modified courses.
            If modifiedCourses IsNot Nothing Then
                CourseTableAdapter.Update(modifiedCourses)
            End If

            ' Accept all the changes of the Strong Typed DataSet

            SQLServer2005DBDataSet.AcceptChanges()
        Catch ex As System.Exception
            MessageBox.Show("Update failed for " & ex.Message)
        Finally
            ' Cleanup the data rows objects
            If deletedCourses IsNot Nothing Then
                deletedCourses.Dispose()
            End If
            If newCourses IsNot Nothing Then
                newCourses.Dispose()
            End If
            If modifiedCourses IsNot Nothing Then
                modifiedCourses.Dispose()
            End If
        End Try
    End Sub

    ''' <summary>
    ''' Commit parent records in the DataSet before adding
    ''' new child records. 
    ''' (This method should be added when using 
    ''' TableAdapterManager to update the related data 
    ''' tables)
    ''' </summary>
    ''' 

    Private Sub CourseBindingSource_AddingNew(ByVal sender As System.Object, _
                                              ByVal e As System.ComponentModel.AddingNewEventArgs) _
                                              Handles CourseBindingSource.AddingNew
        Me.DepartmentBindingSource.EndEdit()
    End Sub
End Class
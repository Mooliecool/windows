'****************************** Module Header ******************************\
' Module Name:    LinqToCSVForm.vb
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' This LinqToCSVForm creates a CSV document based on some of the 
' All-In-One Code Framework project information and uses the simple 
' LINQ to CSV library to read and display the CSV file contents.  
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
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports System.IO
#End Region

Public Class LinqToCSVForm

    Private Sub LinqToCSVForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Create the CSV file
        CreateCSVFile()
    End Sub

    ' The file name of the CSV file
    Private filePath As String = "Projects.csv"

#Region "Load and Display CSV File"

    ' Use the simple LINQ to CSV library to load and display the CSV
    ' file contents.

    Private Sub btnLoad_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLoad.Click
        Try
            ' Read the CSV file using StreamReader
            Using reader As New StreamReader(filePath)
                ' Use the extension method Lines of the StreamReader
                ' to load the contents into anonymous type objects

                Dim projects = From p In reader.Lines() _
                    Select New With {.ProjectName = p(0), .Language = p(1), .Description = p(2), .Owner = p(3)}

                ' Bind the data to DataGridView
                dgvLinqToCSV.DataSource = projects.ToList()
            End Using
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region


#Region "Create CSV File"

    ''' <summary>
    ''' This method creates a CSV file based on some of the All-In-One Code Framework project
    ''' information.
    ''' </summary>
    Private Sub CreateCSVFile()
        ' Create the CodoFx projects objects
        Dim projects As Project() = New Project(1) _
            {New Project() With {.ProjectName = "CSDllCOMServer", .Language = "C#", .Description = "An in-process COM server in C#", .Owner = "Jialiang Ge"}, _
             New Project() With {.ProjectName = "CSLinqToDataSets", .Language = "C#", .Description = "Use LINQ to DataSets in C#", .Owner = "Lingzhi Sun"}}

        ' Create the CSV file using StreamWriter
        Try
            Using writer As New StreamWriter(filePath)
                ' Write comment and data structure in CSV file
                writer.WriteLine("#All-In-One Code Framework Projects (format: Project Name, Program Language, Description, Owner)")

                ' Write the data
                For Each proj In projects
                    writer.WriteLine([String].Format(proj.ProjectName & "," & proj.Language & "," & proj.Description & "," & proj.Owner))
                Next
            End Using

            MessageBox.Show("Projects.csv created successfully!")
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region
End Class

#Region "The All-In-One Code Framework project class"

Public Class Project

    ' The project name
    Private _ProjectName As String
    Public Property ProjectName() As String
        Get
            Return (_ProjectName)
        End Get
        Set(ByVal value As String)
            _ProjectName = value
        End Set
    End Property

    ' The project programme language
    Private _Language As String
    Public Property Language() As String
        Get
            Return (_Language)
        End Get
        Set(ByVal value As String)
            _Language = value
        End Set
    End Property

    ' The project description
    Private _Description As String
    Public Property Description() As String
        Get
            Return (_Description)
        End Get
        Set(ByVal value As String)
            _Description = value
        End Set
    End Property

    ' The project owner
    Private _Owner As String
    Public Property Owner() As String
        Get
            Return (_Owner)
        End Get
        Set(ByVal value As String)
            _Owner = value
        End Set
    End Property
End Class
#End Region


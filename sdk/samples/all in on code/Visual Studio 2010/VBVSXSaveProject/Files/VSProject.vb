'********************************* Module Header *********************************\
' Module Name:        VSProject.vb
' Project :           VBVSXSaveProject
' Copyright (c)       Microsoft Corporation
' 
' This class represents a project in the solution. It supplies method to get the all
' the files included in the project. 
' 
' The source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************

Imports System.IO
Imports EnvDTE

Namespace Files

    Public Class VSProject
        ''' <summary>
        ''' A project of a solution.
        ''' </summary>
        Private privateProject As Project
        Public Property Project() As Project
            Get
                Return privateProject
            End Get
            Private Set(ByVal value As Project)
                privateProject = value
            End Set
        End Property

        ''' <summary>
        ''' The folder that contains the project file.
        ''' </summary>
        Private privateProjectFolder As DirectoryInfo
        Public Property ProjectFolder() As DirectoryInfo
            Get
                Return privateProjectFolder
            End Get
            Private Set(ByVal value As DirectoryInfo)
                privateProjectFolder = value
            End Set
        End Property

        ''' <summary>
        ''' Initialize the Project and ProjectFolder properties.
        ''' </summary>
        Public Sub New(ByVal proj As Project)
            ' Initial the project object.
            Me.Project = proj

            ' Get the current project directory.
            Me.ProjectFolder = New FileInfo(Project.FullName).Directory
        End Sub

        ''' <summary>
        ''' Get all the files included in the project. 
        ''' </summary>
        Public Function GetIncludedFiles() As List(Of ProjectFileItem)
            Dim files = New List(Of ProjectFileItem)()

            ' Add the project file (*.csproj or *.vbproj...) to the list of files.
            files.Add(New ProjectFileItem With _
                      {.Fileinfo = New FileInfo(Project.FullName),
                       .NeedCopy = True,
                       .IsUnderProjectFolder = True})

            ' Add the files included in the project.
            For Each item As ProjectItem In Project.ProjectItems
                GetProjectItem(item, files)
            Next item

            Return files
        End Function

        ''' <summary>
        ''' Get all the files included in the project. 
        ''' </summary>
        Private Sub GetProjectItem(ByVal item As ProjectItem, ByVal files As List(Of ProjectFileItem))
            ' Gets the files associated with a ProjectItem.
            ' Most project items consist of only one file, but some can have more than
            ' one, as with forms in Visual Basic that are saved as both .frm (text) and
            ' .frx (binary) files.
            ' See http://msdn.microsoft.com/en-us/library/envdte.projectitem.filecount.aspx
            For i As Short = 0 To item.FileCount - 1
                If File.Exists(item.FileNames(i)) Then
                    Dim fileItem As New ProjectFileItem()

                    fileItem.Fileinfo = New FileInfo(item.FileNames(i))

                    If fileItem.FullName.StartsWith(Me.ProjectFolder.FullName,
                                                    StringComparison.OrdinalIgnoreCase) Then
                        fileItem.IsUnderProjectFolder = True
                        fileItem.NeedCopy = True
                    End If

                    files.Add(fileItem)
                End If
            Next i

            ' Get the files of sub node under this node.
            For Each subItem As ProjectItem In item.ProjectItems
                GetProjectItem(subItem, files)
            Next subItem
        End Sub
    End Class
End Namespace
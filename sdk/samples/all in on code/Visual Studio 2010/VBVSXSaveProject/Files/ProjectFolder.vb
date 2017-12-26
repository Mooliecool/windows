'********************************* Module Header *********************************\
' Module Name:        ProjectFolder.vb
' Project :           VBVSXSaveProject
' Copyright (c)       Microsoft Corporation
'
' This class is used to get the files in the project folder.
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

Namespace Files

    Public NotInheritable Class ProjectFolder
        ''' <summary>
        ''' Get files in the project folder.
        ''' </summary>
        ''' <param name="projectFilePath">
        ''' The path of the project file. 
        ''' </param>
        Private Sub New()
        End Sub
        Public Shared Function GetFilesInProjectFolder(ByVal projectFilePath As String) As List(Of ProjectFileItem)
            ' Get the folder that includes the project file.
            Dim projFile As New FileInfo(projectFilePath)
            Dim projFolder As DirectoryInfo = projFile.Directory

            If projFolder.Exists Then
                ' Get all files information in project folder.
                Dim files = projFolder.GetFiles("*", SearchOption.AllDirectories)

                Return files.Select(Function(f) New ProjectFileItem With _
                                                {.Fileinfo = f,
                                                 .IsUnderProjectFolder = True}).ToList()
            Else
                ' The project folder does not exist.
                Return Nothing
            End If
        End Function

    End Class
End Namespace
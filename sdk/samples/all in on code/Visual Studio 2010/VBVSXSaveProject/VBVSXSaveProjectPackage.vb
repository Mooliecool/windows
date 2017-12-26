'********************************* Module Header *********************************\
' Module Name:        VBVSXSaveProjectPackage.vb
' Project :           VBVSXSaveProject
' Copyright (c)       Microsoft Corporation
'
' This package adds menus to IDE, which supply following features 
' 1. Copy the whole project to a new location.
' 2. Open the new project in the current IDE.
' 
'
' The source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************

Imports System.ComponentModel.Design
Imports System.Globalization
Imports System.Linq
Imports System.Runtime.InteropServices
Imports System.Windows.Forms
Imports EnvDTE
Imports Microsoft.VisualStudio.Shell

''' <summary>
''' This is the class that implements the package exposed by this assembly.
'''
''' The minimum requirement for a class to be considered a valid package for Visual
''' Studio is to implement the IVsPackage interface and register itself with the 
''' shell. This package uses the helper classes defined inside the Managed Package 
''' Framework (MPF) to do it: it derives from the Package class that provides the 
''' implementation of the IVsPackage interface and uses the registration attributes
''' defined in the framework to register itself and its components with the shell.
''' </summary>
' This attribute tells the PkgDef creation utility (CreatePkgDef.exe) that this
' class is a package.
' This attribute is used to register the information needed to show the this
' package in the Help/About dialog of Visual Studio.
' This attribute is needed to let the shell know that this package exposes some 
' menus.
<PackageRegistration(UseManagedResourcesOnly:=True),
InstalledProductRegistration("#110", "#112", "1.0", IconResourceID:=400),
 ProvideMenuResource("Menus.ctmenu", 1),
 Guid(GuidList.guidVBVSXSaveProjectPkgString)>
Public NotInheritable Class CSVSXSaveProjectPackage
    Inherits Package

    ''' <summary>
    ''' Specify a DTE Object in this application.
    ''' </summary>
    Private dte As DTE
    Friend ReadOnly Property DTEObject() As DTE
        Get
            If dte Is Nothing Then
                dte = TryCast(Me.GetService(GetType(DTE)), DTE)
            End If
            Return dte
        End Get
    End Property

    ''' <summary>
    ''' Default constructor of the package.
    ''' Inside this method you can place any initialization code that does not 
    ''' require any Visual Studio service because at this point the package object
    ''' is created but not sited yet inside Visual Studio environment. The place 
    ''' to do all the other initialization is the Initialize method.
    ''' </summary>
    Public Sub New()
        Trace.WriteLine(String.Format(
                        CultureInfo.CurrentCulture,
                        "Entering constructor for: {0}",
                        Me.ToString()))
    End Sub

    '///////////////////////////////////////////////////////////////////////////////
    ' Overriden Package Implementation
#Region "Package Members"

    ''' <summary>
    ''' Initialization of the package; this method is called right after the
    ''' package is sited, so this is the place where you can put all the
    ''' initialization code that rely on services provided by VisualStudio.
    ''' </summary>
    Protected Overrides Sub Initialize()
        Trace.WriteLine(String.Format(CultureInfo.CurrentCulture,
                                      "Entering Initialize() of: {0}",
                                      Me.ToString()))
        MyBase.Initialize()

        ' Add our command handlers for menu (commands must exist in the .vsct file)
        Dim mcs As OleMenuCommandService =
            TryCast(GetService(GetType(IMenuCommandService)), OleMenuCommandService)
        If Nothing IsNot mcs Then
            ' Create the command for the menu item.
            Dim menuCommandID As New CommandID(GuidList.guidVBVSXSaveProjectCmdSet,
                                               CInt(Fix(PkgCmdIDList.cmdidVBVSXSaveProjectCommandID)))
            Dim menuItem As New MenuCommand(AddressOf MenuItemCallback, menuCommandID)
            mcs.AddCommand(menuItem)

            ' Create the command for the VSXSaveProjectCmdSet menu item.
            Dim vbVSXSaveProjectContextCommandID As New CommandID(GuidList.guidVBVSXSaveProjectContextCmdSet,
                                                                  CInt(Fix(PkgCmdIDList.cmdidVBVSXSaveProjectContextCommandID)))
            Dim vbVSXSaveProjectMenuContextCommand As New OleMenuCommand(AddressOf MenuItemCallback, vbVSXSaveProjectContextCommandID)
            mcs.AddCommand(vbVSXSaveProjectMenuContextCommand)
        End If
    End Sub
#End Region

    ''' <summary>
    ''' This function is the callback used to execute a command when the a menu 
    ''' item is clicked. See the Initialize method to see how the menu item is
    ''' associated to this function using the OleMenuCommandService service and 
    ''' the MenuCommand class.
    ''' </summary>
    Private Sub MenuItemCallback(ByVal sender As Object, ByVal e As EventArgs)
        Try
            ' Get current active project object.
            Dim proj = Me.GetActiveProject()

            If proj IsNot Nothing Then
                ' Get the project information.
                Dim vsProj = New Files.VSProject(proj)

                ' Get the files included in the project.
                Dim includedFiles = vsProj.GetIncludedFiles()

                ' Get the files under the project folder.
                Dim projfolderFiles =
                    Files.ProjectFolder.GetFilesInProjectFolder(proj.FullName)

                ' Add the other files such as documents under the project folder, so
                ' the user can choose them.
                Dim totalItems = New List(Of Files.ProjectFileItem)(includedFiles)
                For Each fileItem As Files.ProjectFileItem In projfolderFiles
                    If includedFiles.Where(Function(f) f.FullName.Equals(
                                               fileItem.FullName,
                                               StringComparison.OrdinalIgnoreCase)).Count() = 0 Then
                        totalItems.Add(fileItem)
                    End If
                Next fileItem

                ' Display the user interface.
                Using dialog As New SaveProjectDialog()
                    ' Display the all the files.
                    dialog.FilesItems = totalItems
                    dialog.OriginalFolderPath = vsProj.ProjectFolder.FullName

                    Dim result = dialog.ShowDialog()

                    ' Open the new project.
                    If result = DialogResult.OK AndAlso dialog.OpenNewProject Then
                        Dim newProjectPath As String = String.Format(
                            "{0}\{1}",
                            dialog.NewFolderPath,
                            proj.FullName.Substring(vsProj.ProjectFolder.FullName.Length))

                        Dim cmd As String = String.Format(
                            "File.OpenProject ""{0}""", newProjectPath)

                        Me.DTEObject.ExecuteCommand(cmd)
                    End If
                End Using
            End If
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

    ''' <summary>
    ''' Get the active project object.
    ''' </summary>
    Friend Function GetActiveProject() As Project
        Dim activeProject As Project = Nothing

        ' Get all project in Solution Explorer.
        Dim activeSolutionProjects As Array =
            TryCast(Me.DTEObject.ActiveSolutionProjects, Array)
        If activeSolutionProjects IsNot Nothing _
            AndAlso activeSolutionProjects.Length > 0 Then
            ' Get the active project.
            activeProject = TryCast(activeSolutionProjects.GetValue(0), Project)
        End If
        Return activeProject
    End Function
End Class
'************************** Module Header ******************************'
' Module Name:  CustomPropertyPageProjectFactory.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' This is the project factory for our project flavor.
' 
' How Project SubType Work:
' 
' First, we have to register our CustomPropertyPageProjectFactory to Visual Studio.
' 
' Second, we need a Project Template, which is created by the VBVSXProjectSubTypeTemplate
' project.
' 
'' The ProjectTemplate.vbproj in VBVSXProjectSubTypeTemplate contains following script 
'    <ProjectTypeGuids>
'        {9E1605FB-8DEB-462D-986B-B8866D9CDE60};
'        {F184B08F-C81C-45F6-A57F-5ABD9991F28F}
'    </ProjectTypeGuids>
'
'    {9E1605FB-8DEB-462D-986B-B8866D9CDE60} is the Guid of the CustomPropertyPageProjectFactory.
'    {F184B08F-C81C-45F6-A57F-5ABD9991F28F} means Visual Basic project. 
' 
' At last, When Visual Studio is creating or opening a Visual Basic project with above ProjectTypeGuids,
' 1. The environment calls the base project (Visual Basic Project)'s CreateProject, and while the 
'    project parses its project file it discovers that the aggregate project type GUIDs list
'    is not null. The project discontinues directly creating its project.
' 
' 2. If there are multiple project type GUIDs, the environment makes recursive function calls to 
'    your implementations of PreCreateForOuter, 
'    Microsoft.VisualStudio.Shell.Interop.IVsAggregatableProject.SetInnerProject(System.Object) 
'    and InitializeForOuter methods while it is walking the list of project type GUIDs, 
'    starting with the outermost project subtype.
' 
' 3. In the PreCreateForOuter method of the ProjectFactory, we can return our ProjectFlavor object,
'    which can customize the Property Page. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Runtime.InteropServices
Imports Microsoft.VisualStudio.Shell.Flavor

Namespace ProjectFlavor

    ''' <summary>
    ''' The project factory for our project flavor.
    ''' </summary>
    <Guid("9E1605FB-8DEB-462D-986B-B8866D9CDE60")>
    Public Class CustomPropertyPageProjectFactory
        Inherits FlavoredProjectFactoryBase

        Public Const CustomPropertyPageProjectFactoryGuidString As String =
            "9E1605FB-8DEB-462D-986B-B8866D9CDE60"

        Public Shared ReadOnly CustomPropertyPageProjectFactoryGuid As _
            New Guid(CustomPropertyPageProjectFactoryGuidString)

        ' With this package, we can get access to VS services.
        Private _package As VSXProjectSubTypePackage

        Public Sub New(ByVal package As VSXProjectSubTypePackage)
            MyBase.New()
            Me._package = package
        End Sub

#Region "IVsAggregatableProjectFactory"

        ''' <summary>
        ''' Create an instance of CustomPropertyPageProjectFlavor. 
        ''' The initialization will be done later when Visual Studio calls
        ''' InitalizeForOuter on it.
        ''' </summary>
        ''' <param name="outerProjectIUnknown">
        ''' This value points to the outer project. It is useful if there is a 
        ''' Project SubType of this Project SubType.
        ''' </param>
        ''' <returns>
        ''' An CustomPropertyPageProjectFlavor instance that has not been initialized.
        ''' </returns>
        Protected Overrides Function PreCreateForOuter(ByVal outerProjectIUnknown As IntPtr) As Object
            Dim newProject As New CustomPropertyPageProjectFlavor()
            newProject.Package = Me._package
            Return newProject
        End Function

#End Region
    End Class
End Namespace

'************************** Module Header ******************************'
' Module Name:  CustomPropertyPageProjectFlavor.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' A Project SubType, or called ProjectFlavor,  is a flavor of an inner project.
' The default behavior of all methods is to delegate to the inner project. 
' For any behavior you want to change, simply handle the request yourself,
' and delegate to the base class any case you don't want to handle.
' 
' In this CustomPropertyPageProjectFlavor, we demonstrate 2 features
' 1. Add our custom Property Page.
' 2. Remove the default Service Property Page.
' 
' By overriding GetProperty method and using propId parameter containing one of 
' the values of the __VSHPROPID2 enumeration, we can filter, add or remove project
' properties. 
' 
' For example, to add a page to the configuration-dependent property pages, we
' need to filter configuration-dependent property pages and then add a new page 
' to the existing list. 
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
Imports Microsoft.VisualStudio
Imports Microsoft.VisualStudio.Shell.Flavor
Imports Microsoft.VisualStudio.Shell.Interop

Namespace ProjectFlavor

    Public Class CustomPropertyPageProjectFlavor
        Inherits FlavoredProjectBase
        Implements IVsProjectFlavorCfgProvider

        ' Internal so that our config can have easy access to it. 
        ' With this package, we can get access to VS services.
        Friend Property Package() As VSXProjectSubTypePackage

        ' The IVsProjectFlavorCfgProvider of the inner project.
        ' Because we are flavoring the base project directly, it is always null.
        Protected _innerVsProjectFlavorCfgProvider As IVsProjectFlavorCfgProvider = Nothing

        Public Sub New()
        End Sub

#Region "Overriden Methods"

        ''' <summary>
        ''' This is were all QI for interface on the inner object should happen. 
        ''' Then set the inner project wait for InitializeForOuter to be called to do
        ''' the real initialization
        ''' </summary>
        ''' <param name="innerIUnknown"></param>
        Protected Overrides Sub SetInnerProject(ByVal innerIUnknown As IntPtr)
            Dim objectForIUnknown As Object = Nothing
            objectForIUnknown = Marshal.GetObjectForIUnknown(innerIUnknown)
            If MyBase.serviceProvider Is Nothing Then
                MyBase.serviceProvider = Me.Package
            End If
            MyBase.SetInnerProject(innerIUnknown)
            Me._innerVsProjectFlavorCfgProvider =
                TryCast(objectForIUnknown, IVsProjectFlavorCfgProvider)
        End Sub

        ''' <summary>
        ''' Release the innerVsProjectFlavorCfgProvider when closed.
        ''' </summary>
        Protected Overrides Sub Close()
            MyBase.Close()
            If _innerVsProjectFlavorCfgProvider IsNot Nothing Then
                If Marshal.IsComObject(_innerVsProjectFlavorCfgProvider) Then
                    Marshal.ReleaseComObject(_innerVsProjectFlavorCfgProvider)
                End If
                _innerVsProjectFlavorCfgProvider = Nothing
            End If
        End Sub

        ''' <summary>
        '''  By overriding GetProperty method and using propId parameter containing one of 
        '''  the values of the __VSHPROPID2 enumeration, we can filter, add or remove project
        '''  properties. 
        '''  
        '''  For example, to add a page to the configuration-dependent property pages, we
        '''  need to filter configuration-dependent property pages and then add a new page 
        '''  to the existing list. 
        ''' </summary>
        Protected Overrides Function GetProperty(ByVal itemId As UInteger,
                                                 ByVal propId As Integer,
                                                 <Out()> ByRef [property] As Object) _
                                             As Integer

            If propId = CInt(__VSHPROPID2.VSHPROPID_CfgPropertyPagesCLSIDList) Then

                ' Get a semicolon-delimited list of clsids of the configuration-dependent
                ' property pages.
                ErrorHandler.ThrowOnFailure(MyBase.GetProperty(itemId, propId, [property]))

                ' Add the CustomPropertyPage property page.
                [property] += ";" + GetType(CustomPropertyPage).GUID.ToString("B")

                Return VSConstants.S_OK

            End If

            If propId = CInt(__VSHPROPID2.VSHPROPID_PropertyPagesCLSIDList) Then

                ' Get the list of priority page guids from the base project system.
                ErrorHandler.ThrowOnFailure(MyBase.GetProperty(itemId, propId, [property]))
                Dim pageList As String = CStr([property])

                ' Remove the Services page from the project designer.
                Dim servicesPageGuidString As String = "{43E38D2E-43B8-4204-8225-9357316137A4}"

                RemoveFromCLSIDList(pageList, servicesPageGuidString)
                [property] = pageList
                Return VSConstants.S_OK
            End If

            Return MyBase.GetProperty(itemId, propId, [property])
        End Function

        Private Sub RemoveFromCLSIDList(ByRef pageList As String, ByVal pageGuidString As String)
            ' Remove the specified page guid from the string of guids.
            Dim index As Integer = pageList.IndexOf(pageGuidString,
                                                    StringComparison.OrdinalIgnoreCase)

            If index <> -1 Then

                ' Guids are separated by ';', so we need to ensure we remove the ';' 
                ' when removing the last guid in the list.
                Dim index2 As Integer = index + pageGuidString.Length + 1

                If index2 >= pageList.Length Then
                    pageList = pageList.Substring(0, index).TrimEnd(";"c)
                Else
                    pageList = pageList.Substring(0, index) & pageList.Substring(index2)
                End If

            Else
                Throw New ArgumentException(
                    String.Format("Cannot find the Page {0} in the Page List {1}",
                                  pageGuidString, pageList))
            End If
        End Sub

#End Region

#Region "IVsProjectFlavorCfgProvider Members"

        ''' <summary>
        ''' Allows the base project to ask the project subtype to create an 
        ''' IVsProjectFlavorCfg object corresponding to each one of its 
        ''' (project subtype's) configuration objects.
        ''' </summary>
        ''' <param name="pBaseProjectCfg">
        ''' The IVsCfg object of the base project.
        ''' </param>
        ''' <param name="ppFlavorCfg">
        ''' The IVsProjectFlavorCfg object of the project subtype.
        ''' </param>
        ''' <returns></returns>
        Public Function CreateProjectFlavorCfg(ByVal pBaseProjectCfg As IVsCfg,
                                               <Out()> ByRef ppFlavorCfg As IVsProjectFlavorCfg) _
                                           As Integer _
                                           Implements IVsProjectFlavorCfgProvider.CreateProjectFlavorCfg

            Dim cfg As IVsProjectFlavorCfg = Nothing

            If _innerVsProjectFlavorCfgProvider IsNot Nothing Then

                _innerVsProjectFlavorCfgProvider.CreateProjectFlavorCfg(
                    pBaseProjectCfg, cfg)

            End If

            Dim configuration As New CustomPropertyPageProjectFlavorCfg()

            configuration.Initialize(Me, pBaseProjectCfg, cfg)
            ppFlavorCfg = CType(configuration, IVsProjectFlavorCfg)

            Return VSConstants.S_OK

        End Function

#End Region

    End Class
End Namespace

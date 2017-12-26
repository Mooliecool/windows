'************************** Module Header ******************************'
' Module Name:  CustomPropertyPage.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The CustomPropertyPage Class inherits the PropertyPage Class and overrides 
' the GetNewPageView and GetNewPropertyStore methods. For more detailed 
' description, see the PropertyPage Class.
' 
' This class will be provided by this Package.
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
Imports VBVSXProjectSubType.PropertyPageBase

Namespace ProjectFlavor

    <Guid("DC8276CC-05FE-4584-B5EA-F778DF4BC655")>
    Friend Class CustomPropertyPage
        Inherits PropertyPage

#Region "Overriden Properties and Methods"

        ''' <summary>
        ''' Help keyword that should be associated with the page
        ''' </summary>
        Protected Overrides ReadOnly Property HelpKeyword() As String
            Get
                Return String.Empty
            End Get
        End Property

        ''' <summary>
        ''' Title of the property page.
        ''' </summary>
        Public Overrides ReadOnly Property Title() As String
            Get
                Return "Custom"
            End Get
        End Property

        ''' <summary>
        ''' Provide the view of our properties.
        ''' </summary>
        ''' <returns></returns>
        Protected Overrides Function GetNewPageView() As IPageView
            Return New CustomPropertyPageView(Me)
        End Function

        ''' <summary>
        ''' Use a store implementation designed for flavors.
        ''' </summary>
        ''' <returns>Store for our properties</returns>
        Protected Overrides Function GetNewPropertyStore() As IPropertyStore
            Return New CustomPropertyPagePropertyStore()
        End Function

#End Region
    End Class
End Namespace
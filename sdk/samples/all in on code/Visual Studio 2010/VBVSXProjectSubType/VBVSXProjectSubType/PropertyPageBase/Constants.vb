'************************** Module Header ******************************'
' Module Name:  IPageView.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' This class defines the constants used in this project.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Namespace PropertyPageBase
    Public NotInheritable Class Constants
#Region "Following constants are used in IPropertyPage::Show Method."

        Public Const SW_SHOW As Integer = 5
        Public Const SW_SHOWNORMAL As Integer = 1
        Public Const SW_HIDE As Integer = 0

#End Region

#Region "Following constants are used in IPropertyPageSite::OnStatusChange Method."

        ''' <summary>
        ''' The values in the pages have changed, so the state of the
        ''' Apply button should be updated.
        ''' </summary>
        Public Const PROPPAGESTATUS_DIRTY As Integer = &H1

        ''' <summary>
        ''' Now is an appropriate time to apply changes.
        ''' </summary>
        Public Const PROPPAGESTATUS_VALIDATE As Integer = &H2

#End Region
    End Class
End Namespace
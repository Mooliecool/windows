'************************** Module Header ******************************'
' Module Name:  CustomPropertyPageView.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The CustomPropertyPageView Class inherits the PageView Class and overrides 
' the PropertyControlTable Property, to which this class adds two Property 
' Name / Control KeyValuePairs. For more detailed description, see the 
' PageView Class.
' 
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
    <Guid("582FD7A4-8476-4550-8912-D9138FB4A927")>
   Partial Friend Class CustomPropertyPageView
        Inherits PageView
        Public Const StringPropertyTag As String = "StringProperty"
        Public Const BooleanPropertyTag As String = "BooleanProperty"

#Region "Constructors"
        ''' <summary>
        ''' This is the runtime constructor.
        ''' </summary>
        ''' <param name="site">Site for the page.</param>
        Public Sub New(ByVal site As IPageViewSite)
            MyBase.New(site)
            InitializeComponent()
        End Sub
        ''' <summary>
        ''' This constructor is only to enable winform designers
        ''' </summary>
        Public Sub New()
            InitializeComponent()
        End Sub
#End Region

        Private _propertyControlTable As PropertyControlTable

        ''' <summary>
        ''' This property is used to map the control on a PageView object to a property
        ''' in PropertyStore object.
        ''' 
        ''' This property will be called in the base class's constructor, which means that
        ''' the InitializeComponent has not been called and the Controls have not been
        ''' initialized.
        ''' </summary>
        Protected Overrides ReadOnly Property PropertyControlTable() As PropertyControlTable
            Get
                If _propertyControlTable Is Nothing Then
                    ' This is the list of properties that will be persisted and their
                    ' assciation to the controls.
                    _propertyControlTable = New PropertyControlTable()

                    ' This means that this CustomPropertyPageView object has not been
                    ' initialized.
                    If String.IsNullOrEmpty(MyBase.Name) Then
                        Me.InitializeComponent()
                    End If

                    ' Add two Property Name / Control KeyValuePairs. 
                    _propertyControlTable.Add(StringPropertyTag, tbStringProperty)
                    _propertyControlTable.Add(BooleanPropertyTag, chkBooleanProperty)
                End If
                Return _propertyControlTable
            End Get
        End Property
    End Class
End Namespace
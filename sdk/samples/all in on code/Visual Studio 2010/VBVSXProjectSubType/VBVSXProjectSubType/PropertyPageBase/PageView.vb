'************************** Module Header ******************************'
' Module Name:  PageView.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The PageView Class inherits from the UserControl and implements the IPageView
' and IPropertyPageUI interfaces. It is used to display the properties in a 
' PropertyStore object that belongs to a PropertyPage object. 
' 
' Through the IPageView interface, the PageView object can be shown, hidden 
' or moved. 
' 
' The PropertyControlTable property is used to map the control on a PageView object
' to a property in PropertyStore object. And through the IPropertyPageUI interface, 
' the values of its controls could be accessed. 
' 
' NOTE: 1. This UserControl cannot be used directly, you have to design your own
'          page view that inherits from it.
' 
'       2. You can only access the values of its TextBox and CheckBox controls.  
'          If you want other controls, such as ComboBox, you have to override
'          the methods of the IPropertyPageUI interface.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.ComponentModel
Imports Microsoft.VisualStudio
Imports System.Windows.Forms
Imports System.Drawing

Namespace PropertyPageBase
    Public Class PageView
        Inherits UserControl
        Implements IPageView, IPropertyPageUI

        Private _propertyControlMap As PropertyControlMap

        ''' <summary>
        ''' This property is used to map the control on a PageView object to a property
        ''' in PropertyStore object.
        ''' This property must be overriden.
        ''' </summary>
        Protected Overridable ReadOnly Property PropertyControlTable() As PropertyControlTable
            Get
                Throw New NotImplementedException()
            End Get
        End Property

        ''' <summary>
        ''' Occur if the value of a control changed.
        ''' </summary>
        Public Event UserEditComplete As UserEditCompleteHandler _
            Implements IPropertyPageUI.UserEditComplete


        <EditorBrowsable(EditorBrowsableState.Never)>
        Public Sub New()
        End Sub

        Public Sub New(ByVal pageViewSite As IPageViewSite)
            Me._propertyControlMap =
                New PropertyControlMap(pageViewSite, Me, Me.PropertyControlTable)
        End Sub

#Region "IPageView members"

        ''' <summary>
        ''' Get the size of this UserControl.
        ''' </summary>
        Public ReadOnly Property ViewSize() As Size _
            Implements IPageView.ViewSize
            Get
                Return MyBase.Size
            End Get
        End Property

        ''' <summary>
        ''' Make the PageView hide.
        ''' </summary>
        Public Sub HideView() Implements IPageView.HideView
            MyBase.Hide()
        End Sub

        ''' <summary>
        ''' Initialize this PageView object.
        ''' </summary>
        ''' <param name="parentControl">
        ''' The parent control of this PageView object.
        ''' </param>
        ''' <param name="rectangle">
        ''' The position of this PageView object.
        ''' </param>
        Public Overridable Sub Initialize(ByVal parentControl As Control,
                                          ByVal rectangle As Rectangle) _
                                      Implements IPageView.Initialize

            MyBase.SetBounds(rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height)
            MyBase.Parent = parentControl

            ' Initialize the value of the Controls on this PageView object. 
            Me._propertyControlMap.InitializeControls()

            ' Register the event when the value of a Control changed.
            For Each control As Control In Me.PropertyControlTable.GetControls()
                Dim tb As TextBox = TryCast(control, TextBox)
                If tb IsNot Nothing Then
                    AddHandler tb.TextChanged, AddressOf TextBox_TextChanged
                Else
                    Dim chk As CheckBox = TryCast(control, CheckBox)
                    If chk IsNot Nothing Then
                        AddHandler chk.CheckedChanged, AddressOf CheckBox_CheckedChanged
                    End If
                End If
            Next control
            Me.OnInitialize()
        End Sub

        ''' <summary>
        ''' Move to new position.
        ''' </summary>
        Public Sub MoveView(ByVal rectangle As Rectangle) Implements IPageView.MoveView
            MyBase.Location = New Point(rectangle.X, rectangle.Y)
            MyBase.Size = New Size(rectangle.Width, rectangle.Height)
        End Sub

        ''' <summary>
        ''' Pass a keystroke to the property page for processing.
        ''' </summary>
        Public Function ProcessAccelerator(ByRef keyboardMessage As Message) _
            As Integer Implements IPageView.ProcessAccelerator

            If Control.FromHandle(keyboardMessage.HWnd).PreProcessMessage(keyboardMessage) Then
                Return VSConstants.S_OK
            End If
            Return VSConstants.S_FALSE
        End Function

        ''' <summary>
        ''' Refresh the UI.
        ''' </summary>
        Public Sub RefreshPropertyValues() Implements IPageView.RefreshPropertyValues
            Me._propertyControlMap.InitializeControls()
        End Sub

        ''' <summary>
        ''' Show this PageView object.
        ''' </summary>
        Public Sub ShowView() Implements IPageView.ShowView
            MyBase.Show()
        End Sub

#End Region

#Region "IPropertyPageUI"

        ''' <summary>
        ''' Get the value of a Control on this PageView object.
        ''' </summary>
        Public Overridable Function GetControlValue(ByVal control As Control) _
            As String Implements IPropertyPageUI.GetControlValue

            Dim chk As CheckBox = TryCast(control, CheckBox)
            If chk IsNot Nothing Then
                Return chk.Checked.ToString()
            End If

            Dim tb As TextBox = TryCast(control, TextBox)
            If tb Is Nothing Then
                Throw New ArgumentOutOfRangeException()
            End If
            Return tb.Text
        End Function

        ''' <summary>
        ''' Set the value of a Control on this PageView object.
        ''' </summary>
        Public Overridable Sub SetControlValue(ByVal control As Control,
                                               ByVal value As String) _
                                           Implements IPropertyPageUI.SetControlValue
            Dim chk As CheckBox = TryCast(control, CheckBox)
            If chk IsNot Nothing Then
                Dim flag As Boolean
                If Not Boolean.TryParse(value, flag) Then
                    flag = False
                End If
                chk.Checked = flag
            Else
                Dim tb As TextBox = TryCast(control, TextBox)
                If tb IsNot Nothing Then
                    tb.Text = value
                End If
            End If
        End Sub


#End Region

        ''' <summary>
        ''' Raise the UserEditComplete event.
        ''' </summary>
        Private Sub CheckBox_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs)
            Dim chk As CheckBox = TryCast(sender, CheckBox)
            RaiseEvent UserEditComplete(chk, chk.Checked.ToString())
        End Sub

        ''' <summary>
        ''' Raise the UserEditComplete event.
        ''' </summary>
        Private Sub TextBox_TextChanged(ByVal sender As Object, ByVal e As EventArgs)
            Dim tb As TextBox = TryCast(sender, TextBox)
            RaiseEvent UserEditComplete(tb, tb.Text)
        End Sub

        Protected Overridable Sub OnInitialize()
        End Sub

    End Class
End Namespace

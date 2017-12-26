'************************** Module Header ******************************'
' Module Name:  PropertyPage.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' A PropertyPage object contains a PropertyStore object which stores the Properties,
' and a PageView object which is a UserControl used to display the Properties.
' 
' The IPropertyPage and IPropertyPage2 Interfaces provide the main features of 
' a property page object that manages a particular page within a property sheet.
' 
' A property page implements at least IPropertyPage and can optionally implement
' IPropertyPage2 if selection of a specific property is supported. See
' http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.ole.interop.ipropertypage2.aspx
' http://msdn.microsoft.com/en-us/library/ms683996(VS.85).aspx
' 
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
Imports Microsoft.VisualStudio
Imports Microsoft.VisualStudio.OLE.Interop
Imports System.Windows.Forms
Imports System.Drawing

Namespace PropertyPageBase

    Public MustInherit Class PropertyPage
        Implements IPropertyPage2, IPropertyPage, IPageViewSite

        Private _propertyPageSite As IPropertyPageSite

        ''' <summary>
        ''' Use a site object with this interface to set up communications between the
        ''' property frame and the property page object.
        ''' http://msdn.microsoft.com/en-us/library/ms690583(VS.85).aspx
        ''' </summary>
        Public Property PropertyPageSite() As IPropertyPageSite
            Get
                Return _propertyPageSite
            End Get
            Set(ByVal value As IPropertyPageSite)
                _propertyPageSite = value
            End Set
        End Property

        Private _propertyStore As IPropertyStore

        ''' <summary>
        ''' A PropertyStore object is used to store the Properties of a 
        ''' PropertyPage object.
        ''' </summary>
        Public Property PropertyStore() As IPropertyStore
            Get
                Return _propertyStore
            End Get
            Set(ByVal value As IPropertyStore)
                _propertyStore = value
            End Set
        End Property

        Protected _myPageView As IPageView

        ''' <summary>
        ''' A PageView is a UserControl that is used to display the Properties of a 
        ''' PropertyPage object.
        ''' </summary>
        Public Property MyPageView() As IPageView
            Get
                If _myPageView Is Nothing Then
                    Dim concretePageView As IPageView = GetNewPageView()
                    Me.MyPageView = concretePageView
                End If
                Return _myPageView
            End Get
            Set(ByVal value As IPageView)
                _myPageView = value
            End Set
        End Property

        ' The changed Property Name / Value KeyValuePair. 
        Private _propertyToBePersisted As Nullable(Of KeyValuePair(Of String, String))

        ''' <summary>
        ''' The Property Page Title that is displayed in Visual Studio.
        ''' </summary>
        Public MustOverride ReadOnly Property Title() As String

        ''' <summary>
        ''' The HelpKeyword if F1 is pressed. By default, it will return String.Empty.
        ''' </summary>
        Protected MustOverride ReadOnly Property HelpKeyword() As String

        Protected MustOverride Function GetNewPageView() As IPageView

        Protected MustOverride Function GetNewPropertyStore() As IPropertyStore

        Protected Sub New()
        End Sub

#Region "IPropertyPage2 Members"

        ''' <summary>
        ''' Initialize a property page and provides the page with a pointer to the 
        ''' IPropertyPageSite interface through which the property page communicates
        ''' with the property frame.
        ''' </summary>
        Public Sub SetPageSite(ByVal pPageSite As IPropertyPageSite) _
            Implements IPropertyPage2.SetPageSite, IPropertyPage.SetPageSite

            PropertyPageSite = pPageSite

        End Sub

        ''' <summary>
        ''' Create the dialog box window for the property page.
        ''' The dialog box is created without a frame, caption, or system menu/controls. 
        ''' </summary>
        ''' <param name="hWndParent">
        ''' The window handle of the parent of the dialog box that is being created.
        ''' </param>
        ''' <param name="pRect">
        ''' The RECT structure containing the positioning information for the dialog box. 
        ''' This method must create its dialog box with the placement and dimensions
        ''' described by this structure.
        ''' </param>
        ''' <param name="bModal">
        ''' Indicates whether the dialog box frame is modal (TRUE) or modeless (FALSE).
        ''' </param>
        Public Sub Activate(ByVal hWndParent As IntPtr,
                            ByVal pRect() As RECT,
                            ByVal bModal As Integer) _
                        Implements IPropertyPage2.Activate, IPropertyPage.Activate

            If (pRect Is Nothing) OrElse (pRect.Length = 0) Then
                Throw New ArgumentNullException("pRect")
            End If

            Dim parentControl As Control = Control.FromHandle(hWndParent)
            Dim rect As RECT = pRect(0)
            Me.MyPageView.Initialize(
                parentControl, Rectangle.FromLTRB(rect.left, rect.top, rect.right, rect.bottom))

        End Sub

        ''' <summary>
        ''' Destroy the window created in IPropertyPage::Activate.
        ''' </summary>
        Public Sub Deactivate() _
            Implements IPropertyPage2.Deactivate, IPropertyPage.Deactivate

            If Me._myPageView IsNot Nothing Then
                Me._myPageView.Dispose()
                Me._myPageView = Nothing
            End If
        End Sub

        ''' <summary>
        ''' Retrieve information about the property page.
        ''' </summary>
        ''' <param name="pPageInfo"></param>
        Public Sub GetPageInfo(ByVal pPageInfo() As PROPPAGEINFO) _
            Implements IPropertyPage2.GetPageInfo, IPropertyPage.GetPageInfo

            Dim proppageinfo As PROPPAGEINFO
            If (pPageInfo Is Nothing) OrElse (pPageInfo.Length = 0) Then
                Throw New ArgumentNullException("pPageInfo")
            End If
            proppageinfo.cb = CUInt(Marshal.SizeOf(GetType(PROPPAGEINFO)))
            proppageinfo.dwHelpContext = 0
            proppageinfo.pszDocString = Nothing
            proppageinfo.pszHelpFile = Nothing
            proppageinfo.pszTitle = Me.Title
            proppageinfo.SIZE.cx = Me.MyPageView.ViewSize.Width
            proppageinfo.SIZE.cy = Me.MyPageView.ViewSize.Height
            pPageInfo(0) = proppageinfo
        End Sub

        ''' <summary>
        ''' Provide the property page with an array of pointers to objects associated 
        ''' with this property page. 
        ''' When the property page receives a call to IPropertyPage::Apply, it must send
        ''' value changes to these objects through whatever interfaces are appropriate.
        ''' The property page must query for those interfaces. This method can fail if 
        ''' the objects do not support the interfaces expected by the property page.
        ''' </summary>
        ''' <param name="cObjects">
        ''' The number of pointers in the array pointed to by ppUnk. 
        ''' If this parameter is 0, the property page must release any pointers previously
        ''' passed to this method.
        ''' </param>
        ''' <param name="ppunk"></param>
        Public Sub SetObjects(ByVal cObjects As UInteger, ByVal ppunk() As Object) Implements IPropertyPage2.SetObjects, IPropertyPage.SetObjects

            ' If cObjects ==0 or ppunk == null, release the PropertyStore.
            If (ppunk Is Nothing) OrElse (cObjects = 0) Then
                If Me.PropertyStore IsNot Nothing Then
                    Me.PropertyStore.Dispose()
                    Me.PropertyStore = Nothing
                End If
            Else

                ' Initialize the PropertyStore using the provided objects.
                Dim flag As Boolean = False
                If Me.PropertyStore IsNot Nothing Then
                    flag = True
                End If
                Me.PropertyStore = Me.GetNewPropertyStore()
                Me.PropertyStore.Initialize(ppunk)

                ' If PropertyStore is not null, which means that the PageView UI has been
                ' initialized, then it needs to be refreshed.
                If flag Then
                    Me.MyPageView.RefreshPropertyValues()
                End If
            End If
        End Sub

        ''' <summary>
        ''' Make the property page dialog box visible or invisible. 
        ''' If the page is made visible, the page should set the focus 
        ''' to itself, specifically to the first property on the page.
        ''' </summary>
        ''' <param name="nCmdShow">
        ''' A command describing whether to become visible (SW_SHOW or SW_SHOWNORMAL) or 
        ''' hidden (SW_HIDE). No other values are valid for this parameter.
        ''' </param>
        Public Sub Show(ByVal nCmdShow As UInteger) Implements IPropertyPage2.Show, IPropertyPage.Show
            Select Case nCmdShow
                Case PropertyPageBase.Constants.SW_HIDE
                    Me.MyPageView.HideView()
                    Return

                Case PropertyPageBase.Constants.SW_SHOW, PropertyPageBase.Constants.SW_SHOWNORMAL
                    Me.MyPageView.ShowView()
                    Return
            End Select
        End Sub

        ''' <summary>
        ''' Positions and resizes the property page dialog box within the frame.
        ''' </summary>
        ''' <param name="pRect"></param>
        Public Sub Move(ByVal pRect() As RECT) Implements IPropertyPage2.Move, IPropertyPage.Move
            Me.MyPageView.MoveView(Rectangle.FromLTRB(pRect(0).left, pRect(0).top, pRect(0).right, pRect(0).bottom))
        End Sub

        ''' <summary>
        ''' Indicate whether the property page has changed since it was activated 
        ''' or since the most recent call to Apply.
        ''' </summary>
        ''' <returns>
        ''' This method returns S_OK to indicate that the property page has changed. 
        ''' Otherwise, it returns S_FALSE.
        ''' </returns>
        Public Function IsPageDirty() As Integer Implements IPropertyPage2.IsPageDirty, IPropertyPage.IsPageDirty
            If Not Me._propertyToBePersisted.HasValue Then
                Return VSConstants.S_FALSE
            End If
            Return VSConstants.S_OK
        End Function

        ''' <summary>
        ''' Apply the current values to the underlying objects associated with the 
        ''' property page as previously passed to IPropertyPage::SetObjects.
        ''' </summary>
        Public Sub Apply() Implements IPropertyPage2.Apply

            ' Save the changed value to PropertyStore.
            If Me._propertyToBePersisted.HasValue Then
                Me.PropertyStore.Persist(Me._propertyToBePersisted.Value.Key, Me._propertyToBePersisted.Value.Value)

                Me._propertyToBePersisted = Nothing
            End If
        End Sub

        ''' <summary>
        ''' Invoke the property page help in response to an end-user request.
        ''' </summary>
        ''' <param name="pszHelpDir"></param>
        Public Sub Help(ByVal pszHelpDir As String) Implements IPropertyPage2.Help, IPropertyPage.Help
            Dim iPropertyPageSite As System.IServiceProvider =
                TryCast(Me.PropertyPageSite, System.IServiceProvider)

            If iPropertyPageSite IsNot Nothing Then
                Dim service As Microsoft.VisualStudio.VSHelp.Help =
                    TryCast(iPropertyPageSite.GetService(GetType(Microsoft.VisualStudio.VSHelp.Help)), Microsoft.VisualStudio.VSHelp.Help)

                If service IsNot Nothing Then
                    service.DisplayTopicFromF1Keyword(Me.HelpKeyword)
                End If
            End If
        End Sub

        ''' <summary>
        ''' Pass a keystroke to the property page for processing.
        ''' </summary>
        ''' <param name="pMsg">
        ''' A pointer to the MSG structure describing the keystroke to be processed.
        ''' </param>
        ''' <returns></returns>
        Public Function TranslateAccelerator(ByVal pMsg() As MSG) As Integer Implements IPropertyPage2.TranslateAccelerator, IPropertyPage.TranslateAccelerator

            ' Pass the message to the PageView object.
            Dim message As Message = message.Create(pMsg(0).hwnd, CInt(pMsg(0).message), pMsg(0).wParam, pMsg(0).lParam)
            Dim hr As Integer = Me.MyPageView.ProcessAccelerator(message)
            pMsg(0).lParam = message.LParam
            pMsg(0).wParam = message.WParam
            Return hr

        End Function

        ''' <summary>
        ''' Specifies which field is to receive the focus when the property page is activated. 
        ''' </summary>
        ''' <param name="DISPID">
        ''' The property that is to receive the focus.
        ''' </param>
        Public Sub EditProperty(ByVal DISPID As Integer) Implements IPropertyPage2.EditProperty

        End Sub

#End Region

#Region "IPropertyPage Members"

        ''' <summary>
        ''' Applies the current values to the underlying objects associated with the 
        ''' property page as previously passed to IPropertyPage::SetObjects.
        ''' </summary>
        Private Function IPropertyPage_Apply() As Integer Implements IPropertyPage.Apply
            Me.Apply()
            Return VSConstants.S_OK
        End Function

#End Region

#Region "IPageViewSite Members"

        ''' <summary>
        ''' Tis method is called if the value of a Control changed on a PageView object.
        ''' </summary>
        ''' <param name="propertyName">
        ''' The Property Name mapped to the Control.
        ''' </param>
        ''' <param name="propertyValue">
        ''' The new value.
        ''' </param>
        Public Sub PropertyChanged(ByVal propertyName As String, ByVal propertyValue As String) Implements IPageViewSite.PropertyChanged
            If Me.PropertyStore IsNot Nothing Then
                Me._propertyToBePersisted = New KeyValuePair(Of String, String)(propertyName, propertyValue)

                If Me.PropertyPageSite IsNot Nothing Then

                    ' Informs the frame that the property page managed by this site has                                                          
                    ' changed its state, that is, one or more property values have been 
                    ' changed in the page.
                    Me.PropertyPageSite.OnStatusChange(PropertyPageBase.Constants.PROPPAGESTATUS_DIRTY Or PropertyPageBase.Constants.PROPPAGESTATUS_VALIDATE)
                End If
            End If
        End Sub

        ''' <summary>
        ''' Get the value of a Property which is stored in the PropertyStore,
        ''' </summary>
        ''' <param name="propertyName"></param>
        ''' <returns></returns>
        Public Function GetValueForProperty(ByVal propertyName As String) As String Implements IPageViewSite.GetValueForProperty
            If Me.PropertyStore IsNot Nothing Then
                Return Me.PropertyStore.PropertyValue(propertyName)
            End If
            Return Nothing
        End Function

#End Region
    End Class
End Namespace

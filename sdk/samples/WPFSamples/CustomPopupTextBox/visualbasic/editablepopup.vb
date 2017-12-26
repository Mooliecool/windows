
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Controls.Primitives
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Shapes


' <summary>
' This control is a TextBox that acts like a Popup.  To get a control
' to behave like a Popup, you should define the IsOpen, Placement, 
' PlacementRectangle, PlacementTarget, HorizontalOffset, and
' VerticalOffset properties and then call Popup.CreateRoot to
' bind these six properties to their counterparts on the 
' internal Popup.
' </summary>

Partial Class EditablePopup
    Inherits System.Windows.Controls.TextBox '

    Private _parentPopup As Popup

    Public Sub New()

    End Sub 'New


    Shared Sub New()
        'This OverrideMetadata call tells the system that this element 
        'wants to provide a style that is different than its base class.
        'This style is defined in themes\generic.xaml
        DefaultStyleKeyProperty.OverrideMetadata(GetType(EditablePopup), _
                        New FrameworkPropertyMetadata(GetType(EditablePopup)))

    End Sub 'New

#Region "Properties to implement Popup Behavior"

    'Placement
    Public Shared PlacementProperty As DependencyProperty = _
                        Popup.PlacementProperty.AddOwner(GetType(EditablePopup))


    Public Property Placement() As PlacementMode
        Get
            Return CType(GetValue(PlacementProperty), PlacementMode)
        End Get
        Set(ByVal value As PlacementMode)
            SetValue(PlacementProperty, Value)
        End Set
    End Property

    'PlacementTarget
    Public Shared PlacementTargetProperty As DependencyProperty = _
                        Popup.PlacementTargetProperty.AddOwner(GetType(EditablePopup))


    Public Property PlacementTarget() As UIElement
        Get
            Return CType(GetValue(PlacementTargetProperty), UIElement)
        End Get
        Set(ByVal value As UIElement)
            SetValue(PlacementTargetProperty, Value)
        End Set
    End Property


    'PlacementRectangle
    Public Shared PlacementRectangleProperty As DependencyProperty = _
                        Popup.PlacementRectangleProperty.AddOwner(GetType(EditablePopup))


    Public Property PlacementRectangle() As Rect
        Get
            Return CType(GetValue(PlacementRectangleProperty), Rect)
        End Get
        Set(ByVal value As Rect)
            SetValue(PlacementRectangleProperty, Value)
        End Set
    End Property

    'HorizontalOffset
    Public Shared HorizontalOffsetProperty As DependencyProperty = _
                        Popup.HorizontalOffsetProperty.AddOwner(GetType(EditablePopup))


    Public Overloads Property HorizontalOffset() As Double
        Get
            Return System.Convert.ToDouble(GetValue(HorizontalOffsetProperty))
        End Get
        Set(ByVal value As Double)
            SetValue(HorizontalOffsetProperty, value)
        End Set
    End Property

    'VerticalOffset
    Public Shared VerticalOffsetProperty As DependencyProperty = _
                        Popup.VerticalOffsetProperty.AddOwner(GetType(EditablePopup))

    Public Overloads Property VerticalOffset() As Double
        Get
            Return System.Convert.ToDouble(GetValue(VerticalOffsetProperty))
        End Get
        Set(ByVal value As Double)
            SetValue(VerticalOffsetProperty, value)
        End Set
    End Property

    Public Shared IsOpenProperty As DependencyProperty = _
                Popup.IsOpenProperty.AddOwner(GetType(EditablePopup), _
                                     New FrameworkPropertyMetadata(False, _
                                     FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, _
                                     New PropertyChangedCallback(AddressOf OnIsOpenChanged)))


    Public Property IsOpen() As Boolean
        Get
            Return CBool(GetValue(IsOpenProperty))
        End Get
        Set(ByVal value As Boolean)
            SetValue(IsOpenProperty, value)
        End Set
    End Property

    Private Shared Sub OnIsOpenChanged(ByVal d As DependencyObject, _
                                       ByVal e As DependencyPropertyChangedEventArgs)

        Dim ctrl As EditablePopup = CType(d, EditablePopup)

        If CBool(e.NewValue) Then
            If ctrl._parentPopup Is Nothing Then
                ctrl.HookupParentPopup()
            End If
        End If

    End Sub 'OnIsOpenChanged


    'Create the Popup and attach the CustomControl to it.
    Private Sub HookupParentPopup()

        _parentPopup = New Popup()

        _parentPopup.AllowsTransparency = True

        Popup.CreateRootPopup(_parentPopup, Me)

    End Sub 'HookupParentPopup

#End Region
End Class 'EditablePopup 

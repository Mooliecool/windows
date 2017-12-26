Imports System
Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Markup

Namespace DPContentStateSampleVB

    Public Class DPStatePage
        Inherits Page

        Shared Sub New()
            ' Register the local property with the journalable dependency property
            DPStatePage.FocusedControlNameProperty = DependencyProperty.Register("FocusedControlName", GetType(String), GetType(DPStatePage), New FrameworkPropertyMetadata(Nothing, FrameworkPropertyMetadataOptions.Journal))
        End Sub

        Public Sub New()
            Me.InitializeComponent()
            AddHandler MyBase.PreviewLostKeyboardFocus, New KeyboardFocusChangedEventHandler(AddressOf Me.DPStatePage_PreviewLostKeyboardFocus)
            AddHandler MyBase.Loaded, New RoutedEventHandler(AddressOf Me.DPStatePage_Loaded)
        End Sub

        Private Sub DPStatePage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

            ' Set focus on last element to have the focus when this page was last browsed to
            If (Not Me.FocusedControlName Is Nothing) Then
                Dim element As IInputElement = DirectCast(LogicalTreeHelper.FindLogicalNode(Me, Me.FocusedControlName), IInputElement)
                Keyboard.Focus(element)
            End If

        End Sub

        Private Sub DPStatePage_PreviewLostKeyboardFocus(ByVal sender As Object, ByVal e As KeyboardFocusChangedEventArgs)

            ' Remember the newly focused control, unless it is the hyperlink that is
            ' used for navigation.
            Dim element As IInputElement = IIf((Not e.NewFocus Is Me.navHyperlink), e.NewFocus, e.OldFocus)

            ' Get name (can't object as most aren't serializable, eg TextBox, Hyperlink, which is 
            ' a requirement for journaling.
            Dim dp As DependencyProperty = IIf(TypeOf element Is FrameworkElement, FrameworkElement.NameProperty, FrameworkContentElement.NameProperty)
            Me.FocusedControlName = CStr(DirectCast(element, DependencyObject).GetValue(dp))

        End Sub

        ' Property to register with the journalable dependency property
        Public Property FocusedControlName() As String
            Get
                Return CStr(MyBase.GetValue(DPStatePage.FocusedControlNameProperty))
            End Get
            Set(ByVal value As String)
                MyBase.SetValue(DPStatePage.FocusedControlNameProperty, value)
            End Set
        End Property

        ' Journalable dependency property to remember control focus
        Public Shared ReadOnly FocusedControlNameProperty As DependencyProperty

    End Class

End Namespace
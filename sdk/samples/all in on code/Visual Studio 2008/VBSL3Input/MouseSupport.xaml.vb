'****************************** Module Header ******************************'
' Module Name:  MouseSupport.xaml.vb
' Project:      VBSL3Input
' Copyright (c) Microsoft Corporation.
' 
' The MouseSupport UserControl's codeb-heind file, implemented functions: 
' 
'   Register Mouse event
'   Mouse drag and drop
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 7/28/2009 6:07 PM Mog Liang Created
' * 7/29/2009 6:00 PM Jialiang Ge Reviewed
'***************************************************************************'

Imports System.Collections.ObjectModel


Public Class MouseSupport
    Inherits UserControl

    ' Methods
    Public Sub New()
        Me.InitializeComponent()
        AddHandler MyBase.Loaded, New RoutedEventHandler(AddressOf Me.MouseSupport_Loaded)
    End Sub


    Private Sub Button_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

        DirectCast(sender, Button).AddHandler(UIElement.MouseLeftButtonDownEvent, _
                                              New MouseButtonEventHandler( _
                                              AddressOf Me.Button_MouseLeftButtonDown), True)

        DirectCast(sender, Button).AddHandler(UIElement.MouseLeftButtonUpEvent, _
                                              New MouseButtonEventHandler( _
                                              AddressOf Me.Button_MouseLeftButtonUp), True)

        AddHandler DirectCast(sender, Button).MouseMove, _
        New MouseEventHandler(AddressOf Me.Button_MouseMove)

    End Sub


    Private Sub Button_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me.isMouseCaptured = True

        ' Record mouse start draging position
        Me.mouseVerticalPosition = e.GetPosition(Nothing).Y
        Me.mouseHorizontalPosition = e.GetPosition(Nothing).X
        DirectCast(sender, Button).CaptureMouse()
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & _
                                 ": (Button)Mouse left button down"))

    End Sub


    Private Sub Button_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)

        Me.isMouseCaptured = False
        DirectCast(sender, Button).ReleaseMouseCapture()
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & _
                                 ": (Button)Mouse left button up"))

    End Sub


    Private Sub Button_MouseMove(ByVal sender As Object, ByVal e As MouseEventArgs)

        If Me.isMouseCaptured Then
            Dim item As Button = DirectCast(sender, Button)
            ' Calculate the current position of the object.
            Dim deltaV As Double = (e.GetPosition(Nothing).Y - Me.mouseVerticalPosition)
            Dim deltaH As Double = (e.GetPosition(Nothing).X - Me.mouseHorizontalPosition)
            Dim newTop As Double = (deltaV + CDbl(item.GetValue(Canvas.TopProperty)))
            Dim newLeft As Double = (deltaH + CDbl(item.GetValue(Canvas.LeftProperty)))

            ' Set new position of object.
            item.SetValue(Canvas.TopProperty, newTop)
            item.SetValue(Canvas.LeftProperty, newLeft)

            ' Update position global variables.
            Me.mouseVerticalPosition = e.GetPosition(Nothing).Y
            Me.mouseHorizontalPosition = e.GetPosition(Nothing).X
        End If

    End Sub


    Private Sub Canvas_MouseEnter(ByVal sender As Object, ByVal e As MouseEventArgs)

        ' ObservableCollection notify any collection changes to listbox, then
        ' listbox will update the view.
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse entered"))

    End Sub


    Private Sub Canvas_MouseLeave(ByVal sender As Object, ByVal e As MouseEventArgs)
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse left"))
    End Sub


    Private Sub Canvas_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse left button down"))
    End Sub


    Private Sub Canvas_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse left button up"))
    End Sub


    Private Sub Canvas_MouseMove(ByVal sender As Object, ByVal e As MouseEventArgs)
        Me.positionTB.Text = ("Mouse Position:" & e.GetPosition(TryCast(sender, UIElement)).ToString())
    End Sub


    Private Sub Canvas_MouseWheel(ByVal sender As Object, ByVal e As MouseWheelEventArgs)
        Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse wheel rolled"))
    End Sub


    Private Sub MouseSupport_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' Set string list to listbox as itemssource.
        Me._eventlist = New ObservableCollection(Of String)
        Me.eventlb1.ItemsSource = Me._eventlist

    End Sub


    ' Use string list for recording mouse event.
    Private _eventlist As ObservableCollection(Of String)

    ' Global variables used to keep track of the mouse position and whether 
    ' the object is captured by the mouse.
    Private isMouseCaptured As Boolean
    Private mouseHorizontalPosition As Double
    Private mouseVerticalPosition As Double

End Class


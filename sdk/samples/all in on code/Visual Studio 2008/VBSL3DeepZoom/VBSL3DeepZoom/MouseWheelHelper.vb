Imports System.Windows.Browser

Public Class MouseWheelHelper
	' Events
	Public Event Moved As EventHandler(Of MouseWheelEventArgs)

	' Methods
	Public Sub New(ByVal element As FrameworkElement)
		If (MouseWheelHelper.workerInstance Is Nothing) Then
			MouseWheelHelper.workerInstance = New Worker
		End If
		AddHandler MouseWheelHelper.workerInstance.Moved, New EventHandler(Of MouseWheelEventArgs)(AddressOf Me.HandleMouseWheel)
		AddHandler element.MouseEnter, New MouseEventHandler(AddressOf Me.HandleMouseEnter)
		AddHandler element.MouseLeave, New MouseEventHandler(AddressOf Me.HandleMouseLeave)
		AddHandler element.MouseMove, New MouseEventHandler(AddressOf Me.HandleMouseMove)
	End Sub

	Private Sub HandleMouseEnter(ByVal sender As Object, ByVal e As EventArgs)
		Me.isMouseOver = True
	End Sub

	Private Sub HandleMouseLeave(ByVal sender As Object, ByVal e As EventArgs)
		Me.isMouseOver = False
	End Sub

	Private Sub HandleMouseMove(ByVal sender As Object, ByVal e As EventArgs)
		Me.isMouseOver = True
	End Sub

	Private Sub HandleMouseWheel(ByVal sender As Object, ByVal args As MouseWheelEventArgs)
		If Me.isMouseOver Then
			RaiseEvent Moved(Me, args)
		End If
	End Sub


	' Fields
	Private isMouseOver As Boolean = False
	Private Shared workerInstance As Worker

	' Nested Types
	Private Class Worker
		' Events
		Public Event Moved As EventHandler(Of MouseWheelEventArgs)

		' Methods
		Public Sub New()
			If HtmlPage.IsEnabled Then
				HtmlPage.Window.AttachEvent("DOMMouseScroll", New EventHandler(Of HtmlEventArgs)(AddressOf Me.HandleMouseWheel))
				HtmlPage.Window.AttachEvent("onmousewheel", New EventHandler(Of HtmlEventArgs)(AddressOf Me.HandleMouseWheel))
				HtmlPage.Document.AttachEvent("onmousewheel", New EventHandler(Of HtmlEventArgs)(AddressOf Me.HandleMouseWheel))
			End If
		End Sub

		Private Sub HandleMouseWheel(ByVal sender As Object, ByVal args As HtmlEventArgs)
			Dim delta As Double = 0
			Dim eventObject As ScriptObject = args.EventObject
			If (Not eventObject.GetProperty("wheelDelta") Is Nothing) Then
				delta = (CDbl(eventObject.GetProperty("wheelDelta")) / 120)
				If (Not HtmlPage.Window.GetProperty("opera") Is Nothing) Then
					delta = -delta
				End If
			ElseIf (Not eventObject.GetProperty("detail") Is Nothing) Then
				delta = (-CDbl(eventObject.GetProperty("detail")) / 3)
				If (HtmlPage.BrowserInformation.UserAgent.IndexOf("Macintosh") <> -1) Then
					delta = (delta * 3)
				End If
			End If
			If (delta <> 0) Then
				Dim e As New MouseWheelEventArgs(delta)
				RaiseEvent Moved(Me, e)
				If e.Handled Then
					args.PreventDefault()
				End If
			End If
		End Sub

	End Class
End Class

Public Class MouseWheelEventArgs
	Inherits EventArgs
	' Methods
	Public Sub New(ByVal delta As Double)
		Me.deltaField = delta
	End Sub


	' Properties
	Public ReadOnly Property Delta() As Double
		Get
			Return Me.deltaField
		End Get
	End Property

	Public Property Handled() As Boolean
		Get
			Return Me.handledField
		End Get
		Set(ByVal value As Boolean)
			Me.handledField = value
		End Set
	End Property


	' Fields
	Private deltaField As Double
	Private handledField As Boolean = False
End Class


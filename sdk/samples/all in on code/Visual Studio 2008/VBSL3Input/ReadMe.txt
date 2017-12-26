========================================================================
    SILVERLIGHT APPLICATION : VBSL3Input Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to handle mouse and keyboard event in 
Silverlight 3. It demonstrate following functionalities:

	Handle mouse/keyboard event
	Implement mouse drag function


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1.To Handle mouse/keyboard event. The normal way is creating an event handler 
and register handler on target control.

	<Canvas Margin="20" Background="AliceBlue" MouseEnter="Canvas_MouseEnter">
        
	Private Sub Canvas_MouseEnter(ByVal sender As Object, ByVal e As MouseEventArgs)
		Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": Mouse entered"))
	End Sub

Another way to handle control event, is using AddHandler method. The API 
AddHandler provides a technique whereby you can attach a handler that will 
always be invoked for the route, even if some other handler earlier in the 
route has set Handled to true. For example, handling Button's 
MouseLeftButtonDown/MouseLeftButtonUp event

	DirectCast(sender, Button).AddHandler(UIElement.MouseLeftButtonUpEvent,
		New MouseButtonEventHandler(AddressOf Me.Button_MouseLeftButtonUp), True)

	Private Sub Button_MouseLeftButtonUp(ByVal sender As Object, _
		ByVal e As MouseButtonEventArgs)
		'handle event
	End Sub

2.To implement mouse drag function, we need combine the usage of 
MouseLeftButtonDown, MouseLeftButtonUp and MouseMove event.

	Private Sub Button_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		Me.isMouseCaptured = True
		Me.mouseVerticalPosition = e.GetPosition(Nothing).Y
		Me.mouseHorizontalPosition = e.GetPosition(Nothing).X
		DirectCast(sender, Button).CaptureMouse()
		Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": (Button)Mouse left button down"))
	End Sub

	Private Sub Button_MouseLeftButtonUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
		Me.isMouseCaptured = False
		DirectCast(sender, Button).ReleaseMouseCapture()
		Me._eventlist.Insert(0, (DateTime.Now.ToLongTimeString & ": (Button)Mouse left button up"))
	End Sub

	Private Sub Button_MouseMove(ByVal sender As Object, ByVal e As MouseEventArgs)
		If Me.isMouseCaptured Then
			Dim item As Button = DirectCast(sender, Button)
			Dim deltaV As Double = (e.GetPosition(Nothing).Y - Me.mouseVerticalPosition)
			Dim deltaH As Double = (e.GetPosition(Nothing).X - Me.mouseHorizontalPosition)
			Dim newTop As Double = (deltaV + CDbl(item.GetValue(Canvas.TopProperty)))
			Dim newLeft As Double = (deltaH + CDbl(item.GetValue(Canvas.LeftProperty)))
			item.SetValue(Canvas.TopProperty, newTop)
			item.SetValue(Canvas.LeftProperty, newLeft)
			Me.mouseVerticalPosition = e.GetPosition(Nothing).Y
			Me.mouseHorizontalPosition = e.GetPosition(Nothing).X
		End If
	End Sub
        
3.To put UserControl in another UserControl, first we need register the 
usercontrol's namespace, then use usercontrol with this name 
"prefix_name:usercontrol_name"

	<cssinput:KeyboardSupport  xmlns:cssinput="clr-namespace:CSSL3Input" />

4.To bind list to listbox, first set ListBox.ItemsSource to list, then set 
databinding on ListBox.ItemTemplate. For details, please check out
http://msdn.microsoft.com/en-us/library/cc265158(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
References:

Mouse Support
http://msdn.microsoft.com/en-us/library/cc189029(VS.95).aspx

Keyboard Support
http://msdn.microsoft.com/en-us/library/cc189015(VS.95).aspx

How to: Drag and Drop Objects in UI Layout
http://msdn.microsoft.com/en-us/library/cc189066(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////

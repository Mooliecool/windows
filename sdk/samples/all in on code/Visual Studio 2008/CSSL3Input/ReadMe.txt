========================================================================
    SILVERLIGHT APPLICATION : CSSL3Input Project Overview
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

	private void Canvas_MouseEnter(object sender, MouseEventArgs e)
	{
		//handle event
	}

Another way to handle control event, is using AddHandler method. The API 
AddHandler provides a technique whereby you can attach a handler that will 
always be invoked for the route, even if some other handler earlier in the 
route has set Handled to true. For example, handling Button's 
MouseLeftButtonDown / MouseLeftButtonUp event

	((Button)sender).AddHandler(Button.MouseLeftButtonUpEvent,
		new MouseButtonEventHandler(Button_MouseLeftButtonUp),true);

	private void Button_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
	{
		//handle event
	}

2.To implement mouse drag function, we need combine the usage of 
MouseLeftButtonDown, MouseLeftButtonUp and MouseMove event.

	void Button_MouseMove(object sender, MouseEventArgs e)
	{
		if (isMouseCaptured)
		{
			var item= ((Button)sender);
			// Calculate the current position of the object.
			double deltaV = e.GetPosition(null).Y - mouseVerticalPosition;
			double deltaH = e.GetPosition(null).X - mouseHorizontalPosition;
			double newTop = deltaV + (double)item.GetValue(Canvas.TopProperty);
			double newLeft = deltaH + (double)item.GetValue(Canvas.LeftProperty);

			// Set new position of object.
			item.SetValue(Canvas.TopProperty, newTop);
			item.SetValue(Canvas.LeftProperty, newLeft);

			// Update position global variables.
			mouseVerticalPosition = e.GetPosition(null).Y;
			mouseHorizontalPosition = e.GetPosition(null).X;
		}
	}

	private void Button_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
	{
		isMouseCaptured = true;
		mouseVerticalPosition = e.GetPosition(null).Y;
		mouseHorizontalPosition = e.GetPosition(null).X;
		((Button)sender).CaptureMouse();
	}

	private void Button_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
	{
		isMouseCaptured = false;
		((Button)sender).ReleaseMouseCapture();
	}
        
3.To put UserControl in another UserControl, first we need register the 
usercontrol's namespace, then use usercontrol with this name 
"prefix_name:usercontrol_name"

	<cssinput:KeyboardSupport  xmlns:cssinput="clr-namespace:CSSL3Input" />
        
4.To bind list to listbox, first set ListBox.ItemsSource to list, then set 
databinding on ListBox.ItemTemplate. For details, please check out
http://msdn.microsoft.com/en-us/library/cc265158(VS.95).aspx.


/////////////////////////////////////////////////////////////////////////////
References:

Mouse Support
http://msdn.microsoft.com/en-us/library/cc189029(VS.95).aspx

Keyboard Support
http://msdn.microsoft.com/en-us/library/cc189015(VS.95).aspx

How to: Drag and Drop Objects in UI Layout
http://msdn.microsoft.com/en-us/library/cc189066(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////

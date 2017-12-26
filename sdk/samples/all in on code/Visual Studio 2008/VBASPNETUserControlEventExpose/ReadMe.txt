=============================================================================
               VBASPNETuserControlEventsExpose Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The project illustrates how to bubble an event from Web User Control to the 
web page that may display something on Web Page according to which event is 
fired in this User control.


/////////////////////////////////////////////////////////////////////////////
Demo

Click the VBASPNETuserControlEventsExpose.sln directly and open the 
VBASPNETuserControlEventsExpose website to test the page directly.

If you want to have a further test, please follow the demonstration steps 
below.

Step 1: View the Default.aspx,you will find some Controls,select the 
dropdownlist value.

Step 2: When click the the button control whose value is 'I am inside User 
Control',

The webpage custom event will fire, display the dropdownlist selected value 
and inform user the usercontrol's button click is clicked.


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Declare a delagate and a event in the user control code behind,
add one button to this User Control.

Step 2. Add the following lines of code in the click event of button of 
usercontrol.

    RaiseEvent MyEvent(sender, e)
    Response.Write("User Control¡¯s Button Click <BR/>")

Step 3. In the Default.aspx.cs file,Load the usercontrol, and subscribe the 
event of the user control in the page load event.

Step 4. Define a suitable event handler to display the dropdownlist selected 
value.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Events and Delegates
http://msdn.microsoft.com/en-us/library/17sde2xt(VS.71).aspx

FAQs for Web Forms
http://forums.asp.net/t/1360420.aspx

Catch event from Web User Control in Webpage
http://devcandy.blogspot.com/2008/06/catch-event-from-web-user-control-in.html


/////////////////////////////////////////////////////////////////////////////
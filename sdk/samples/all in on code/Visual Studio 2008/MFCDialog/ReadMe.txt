================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : MFCDialog Project Overview
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The MFCDialog example demonstrates the creation of modal and modeless dialog 
boxes in MFC.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Creating Modal Dialog Boxes  
To create a modal dialog box, call either of the two public constructors 
declared in CDialog. Next, call the dialog object's DoModal member function 
to display the dialog box and manage interaction with it until the user 
chooses OK or Cancel. This management by DoModal is what makes the dialog 
box modal. For modal dialog boxes, DoModal loads the dialog resource.

B. Creating Modeless Dialog Boxes
For a modeless dialog box, you must provide your own public constructor in 
your dialog class. To create a modeless dialog box, call your public 
constructor and then call the dialog object's Create member function to load 
the dialog resource. You can call Create either during or after the 
constructor call. If the dialog resource has the property WS_VISIBLE, the 
dialog box appears immediately. If not, you must call its ShowWindow member 
function.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Modal Dialog Boxes  
http://msdn.microsoft.com/en-us/library/b8tas481.aspx

MSDN: Creating Modeless Dialog Boxes  
http://msdn.microsoft.com/en-us/library/hf0yazk7.aspx


/////////////////////////////////////////////////////////////////////////////

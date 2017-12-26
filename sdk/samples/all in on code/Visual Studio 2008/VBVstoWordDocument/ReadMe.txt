========================================================================
    Word Document Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This Sample demonstrates how to manipulate Word 2007 Content Controls 
in a VSTO document-level project

/////////////////////////////////////////////////////////////////////////////
Demo:
Run the project, you'll find three buttons in your Actions Pane, click the button, 
will insert different kinds of Content Control in the document.

Please note that the Text Content Control also demonstarte the XMLMapping feature.

And all the events of the content control are hoooked in ThisDocument.vb, and they will
 output some texts in Visual Studio Output windows while debugging.

/////////////////////////////////////////////////////////////////////////////

Creation:
	Visual Studio Side:
		1.Create a Word 2007 Document project.
		2.Add a new user control in the project.
		3.Drag three buttons in the user control.
		4.Add code to insert all types of content controls at runtime.

/////////////////////////////////////////////////////////////////////////////
References:

Working with Content Controls
http://msdn.microsoft.com/en-us/library/bb243344.aspx

/////////////////////////////////////////////////////////////////////////////
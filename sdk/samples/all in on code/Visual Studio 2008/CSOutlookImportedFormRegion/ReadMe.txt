========================================================================
    Outlook Imported Form Region Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This Sample demonstrates how to build a Outlook look&feel custom form via
 Outlook Frorm Region in Visual Studio 2008


/////////////////////////////////////////////////////////////////////////////

Creation:
	Outlook Side:
		1.Create a Form Region
		 a.Go to "Tools" menu,select "Form", select "Design a Form".
		 b.Select "Message" at "Design From" dialog
		 c.On the Active Inspector,go to "Developer" tab, Click "Form Region", Select "New Form Region"
		 d.Add Controls into the Form Region.
		 e.Save the Form Region as "CodeFxFormRegion.ofs".

	Visual Studio Side:
		1.Create a Outlook 2007 AddIn project
		2.Add a New Item
		3.At "Add New Item" dialog select Outlook Form Region
		4.At "Select how you want to create the form region" dialog select "Import an Outlook Form Storage file"
		5.At "Select the type of form region you want to create"dialog choose "Replace-all"
		6.Next Name the Form Retion as ImportedFormRegion, and check "Inspectors that are in compose mode"
		  "Inspectors that are in read mode", "Reading Pane" check boxes.
		7.Go to ImportedFormRegion.cs file and add event handler for all the controls.




/////////////////////////////////////////////////////////////////////////////
References:

Walkthrough: Importing a Form Region That Is Designed in Outlook
http://msdn.microsoft.com/en-us/library/bb608611.aspx

/////////////////////////////////////////////////////////////////////////////
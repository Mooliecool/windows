========================================================================
    SILVERLIGHT APPLICATION : VBSL4COMInterop Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project created a simple application, which could interoperate with COM components,
exporting data to notepad or Microsoft Excel applications.


/////////////////////////////////////////////////////////////////////////////
Demo:

To test the silverlight4 COM interaction feature, please try the following steps:
1. Install the silverlight application to local machine with evaluated permission.
	a. Open VBSL4COMInterop solution and compile.
	b. Run the project.
	c. Right click the silverlight application in browser, select "Install VBSL4COMInterop
	   application on to this computer...", then click "Install" button in the popup panel.
2. Test the application in OOB mode
    a. double click "VBSL4COMInterop Application" shortcut on desktop to start the
	   application.
	b. edit data by manipulating the datagrid.
	c. click the buttons to export data to different applications.

/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 4 Tools RC2 for Visual Studio 2010
http://www.microsoft.com/downloads/details.aspx?FamilyID=bf5ab940-c011-4bd1-ad98-da671e491009&displaylang=en

Silverilght 4 runtime
http://silverlight.net/getstarted/

Microsoft Office 2007 or higher
http://office.microsoft.com/en-us/default.aspx


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. What's the precondition of interoperating COM in silverlight?
    Interoperate with COM is the new feature in Silverlight4, to develop the COM interop 
	silverlight	application, we need use Silverlight4 SDK. Also, to interop with COM, the silverlight
	application need to be running under elevated trust OOB mode.

	For details about Trusted Application, please refer to
	http://msdn.microsoft.com/en-us/library/ee721083(VS.95).aspx

	To determine if COM interop is available, we could use this code:
	    System.Runtime.InteropServices.Automation.AutomationFactory.IsAvailable

2. How to manipulate the Microsoft Word automation in silverlight?
    1. Use AutomationFactory to create word application automation object.
		Dim word = AutomationFactory.CreateObject("Word.Application")

	2. Create new word document, and then write Text and apply format.
	    Dim doc = word.Documents.Add()
        Dim range1 = doc.Paragraphs[1].Range
        range1.Text = "Silverlight4 Word Automation Sample" & vbLf
        range1.Font.Size = 24
        range1.Font.Bold = true

	3. Print document
	    doc.PrintOut()

3. How to manipulate the Windows Notepad in silverlight?
	1. Use AutomationFactory to create WSHShell object.
		Dim shell = AutomationFactory.CreateObject("WScript.Shell")
	
	2. Run Notepad.exe
		shell.Run(@"%windir%\notepad", 5)

	3. Send Keys to Notepad application
		shell.SendKeys("Name{Tab}Age{Tab}Gender{Enter}")
    
/////////////////////////////////////////////////////////////////////////////
References:

AutomationFactory Class
http://msdn.microsoft.com/en-us/library/system.runtime.interopservices.automation.automationfactory(VS.95).aspx

How to: Use Automation in Trusted Applications
http://msdn.microsoft.com/en-us/library/ff457794(VS.95).aspx

/////////////////////////////////////////////////////////////////////////////

========================================================================
    Visual Studio Automation : CSVSAutomationOutsideIDE Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Demostrate how to automate Visual Studio outside the IDE. One can use 
existing Visual Studio instance or create a new one to execute specified 
command or automate by DTE object, like macro or add-in.

In this sample, we first find an existing Visual Studio instance or create 
a new one and navigate its browser to All-In-One homepage.


/////////////////////////////////////////////////////////////////////////////
Steps:

Step1. Reference EnvDTE object which is located at GAC and Visual Studio
PublicAssembly folder.

Step2. Find an existing IDE instance and obtain its DTE object.

Step3. If there is no existing one, create a new one.

Step4. Display the UI of the IDE by setting its visiblity property.

Step5. Using DTE object to execute a command: View.URL to navigate Visual 
Studio embeded browser to All-In-One homepage.


/////////////////////////////////////////////////////////////////////////////
References:

How to start Visual Studio programmatically
http://blogs.msdn.com/kirillosenkov/archive/2009/03/03/how-to-start-visual-studio-programmatically.aspx

How to: Get References to the DTE and DTE2 Objects
http://msdn.microsoft.com/en-us/library/68shb4dw.aspx


/////////////////////////////////////////////////////////////////////////////
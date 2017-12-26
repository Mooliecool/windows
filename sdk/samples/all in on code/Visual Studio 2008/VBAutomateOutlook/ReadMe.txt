========================================================================
    CONSOLE APPLICATION : VBAutomateOutlook Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The VBAutomateOutlook example demonstrates the use of Visual Basic.NET codes  
to automate Microsoft Outlook to enumerate contacts, send a mail, close the 
Microsoft Outlook application and then clean up unmanaged COM resources. 


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this code sample on a computer that has Microsoft Outlook 2007 
installed.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the Outlook automation 
sample that starts a Microsoft Outlook instance, logs on with your profile, 
enumerates the contact items, creates and sends a new mail item, logs off and 
quits the Microsoft Outlook application cleanly.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get the application: VBAutomateOutlook.exe.

Step2. Open Windows Task Manager (Ctrl+Shift+Esc) to confirm that no 
outlook.exe is running. 

Step3. Run the application. It should print the following content in the 
console window if no error is thrown.

  Outlook.Application is started
  User logs on ...
  Please press ENTER to continue when Outlook is ready.

Outlook would ask you to input your profile and password. When Outlook is 
ready, press ENTER in the console window of VBAutomateOutlook. The 
application will then enumerate your contacts and print the contacts:

  Enumerate the contact items
  <the email address of your contacts and the name of your discussion lists>

Next, VBAutomateOutlook automates Outlook to create and display or send a 
new mail item. 

  Create and send a new mail item

In the new mail item, the To line is set as codefxf@microsoft.com, which is 
the feedback channel of All-In-One Code Framework. The Subject is set to 
"Feedback of All-In-One Code Framework" and the email body shows "Feedback:" 
in bold.

After you input your feedback and click the Send button, the mail item is 
sent and VBAutomateOutlook automates Outlook to log off the current profile 
and quit itself.

  Log off and quit the Outlook application

Step4. In Windows Task Manager, confirm that the outlook.exe process does not 
exist, i.e. the Microsoft Outlook intance was closed and cleaned up properly.


/////////////////////////////////////////////////////////////////////////////
Creation:

VBAutomateOutlook - CSAutomateOutlook - CppAutomateOutlook

These examples automate Microsoft Outlook to do the same thing in different 
programming languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Console application and reference the Outlook Primary Interop 
Assembly (PIA). To reference the Outlook PIA, right-click the project file
and click the "Add Reference..." button. In the Add Reference dialog, 
navigate to the .NET tab, find Microsoft.Office.Interop.Outlook 12.0.0.0 and 
click OK.

Step2. Import and rename the Outlook interop namepace:

	Imports Outlook = Microsoft.Office.Interop.Outlook

Step3. Initialize the current thread as STA

	<STAThread()> _
	Sub Main()

	End Sub

Step4. Start up an Outlook application by creating an Outlook.Application 
object.

	oOutlook = New Outlook.Application()

In Vista with UAC enabled, if the automation client is run as administrator, 
the application may throw the error 0x80010001. Akash well explained the  
problem in the blog:
http://blogs.msdn.com/akashb/archive/2008/11/03/unable-to-instantiate-outlook-object-from-visual-studio-2008-on-vista-with-uac-on.aspx

Step5. Get the namespace and the logon.

	' Get the namespace and the logon.
	oNS = oOutlook.GetNamespace("MAPI")
	
	' Log on by using a dialog box to choose the profile.
	oNS.Logon(missing, missing, True, True)

	' Alternative logon method that uses a specific profile.
	' If you use this logon method, change the profile name to an 
	' appropriate value. The second parameter of Logon is the password 
	' (if any) associated with the profile. This parameter exists only 
	' for backwards compatibility and for security reasons, and it is 
	' not recommended for use.
	'oNS.Logon("YourValidProfile", missing, False, True)

Step6. Enumerate the contact items.

	oCtFolder = oNS.GetDefaultFolder( _
	Outlook.OlDefaultFolders.olFolderContacts)
	oCts = oCtFolder.Items

	' Enumerate the contact items. 
	For i As Integer = 1 To oCts.Count
		Dim oItem As Object = oCts(i)

		If (TypeOf oItem Is Outlook.ContactItem) Then
			Dim oCt As Outlook.ContactItem = oItem
			Console.WriteLine(oCt.Email1Address)
			' Do not need to Marshal.ReleaseComObject oCt because 
			' Dim oCt As Outlook.ContactItem = oItem is a simple .NET 
			' type casting, instead of a COM QueryInterface.
		ElseIf (TypeOf oItem Is Outlook.DistListItem) Then
			Dim oDl As Outlook.DistListItem = oItem
			Console.WriteLine(oDl.DLName)
			' Do not need to Marshal.ReleaseComObject oDl because 
			' Dim oDl As Outlook.DistListItem = oItem is a simple .NET 
			' type casting, instead of a COM QueryInterface.
		End If

		' Release the COM object of the Outlook item.
		Marshal.FinalReleaseComObject(oItem)
		oItem = Nothing
	Next

Be careful with foreach loops. See: http://tiny.cc/uXw8S.

Step7. Create and send a new mail item.

	oMail = oOutlook.CreateItem(Outlook.OlItemType.olMailItem)
	oMail.Display(True)
	' [-or-]
	'oMail.Send()

Step8. User logs off and quits Outlook.

	oNS.Logoff()
	oOutlook.Quit()

Step9. Clean up the unmanaged COM resources. To get Outlook terminated 
rightly, we need to call Marshal.FinalReleaseComObject() on each COM object 
we used. We can either explicitly call Marshal.FinalReleaseComObject on all 
accessor objects:

	If Not oMail Is Nothing Then
		Marshal.FinalReleaseComObject(oMail)
		oMail = Nothing
	End If
	If Not oCts Is Nothing Then
		Marshal.FinalReleaseComObject(oCts)
		oCts = Nothing
	End If
	If Not oCtFolder Is Nothing Then
		Marshal.FinalReleaseComObject(oCtFolder)
		oCtFolder = Nothing
	End If
	If Not oNS Is Nothing Then
		Marshal.FinalReleaseComObject(oNS)
		oNS = Nothing
	End If
	If Not oOutlook Is Nothing Then
		Marshal.FinalReleaseComObject(oOutlook)
		oOutlook = Nothing
	End If

and/or force a garbage collection as soon as the calling function is off the 
stack (at which point these objects are no longer rooted) and then call 
GC.WaitForPendingFinalizers.

	' Force a garbage collection as soon as the calling function is off 
	' the stack (at which point these objects are no longer rooted) and 
	' then call GC.WaitForPendingFinalizers.
	GC.Collect()
	GC.WaitForPendingFinalizers()
	' GC needs to be called twice in order to get the Finalizers called 
	' - the first time in, it simply makes a list of what is to be 
	' finalized, the second time in, it actually is finalizing. Only 
	' then will the object do its automatic ReleaseComObject.
	GC.Collect()
	GC.WaitForPendingFinalizers()

For more readings, please refer to this blog:

Outlook Item Leaks
http://blogs.msdn.com/mstehle/archive/2007/12/07/oom-net-part-2-outlook-item-leaks.aspx


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Outlook 2007 Developer Reference
http://msdn.microsoft.com/en-us/library/bb177050.aspx

How to use the Microsoft Outlook Object Library to create an Outlook contact 
in Visual Basic .NET
http://support.microsoft.com/kb/313787

How to use the Microsoft Outlook Object Library to force a Send/Receive 
action by using Visual Basic .NET
http://support.microsoft.com/kb/313793

Programming samples that can reference items and folders in Outlook by using 
Visual Basic .NET
http://support.microsoft.com/kb/313800

Writing .NET Code for Outlook
http://www.outlookcode.com/article.aspx?ID=43


/////////////////////////////////////////////////////////////////////////////
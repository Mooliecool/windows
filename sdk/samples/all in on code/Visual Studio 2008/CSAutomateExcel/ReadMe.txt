========================================================================
    CONSOLE APPLICATION : CSAutomateExcel Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CSAutomateExcel example demonstrates how to use Visual C# codes to create 
a Microsoft Excel instance, create a workbook, fill data into a specific 
range, save the workbook, close the Microsoft Excel application and then 
clean up unmanaged COM resources.

Office automation is based on Component Object Model (COM). When you call a 
COM object of Office from managed code, a Runtime Callable Wrapper (RCW) is 
automatically created. The RCW marshals calls between the .NET application 
and the COM object. The RCW keeps a reference count on the COM object. If 
all references have not been released on the RCW, the COM object of Office 
does not quit and may cause the Office application not to quit after your 
automation. In order to make sure that the Office application quits cleanly, 
the sample demonstrates two solutions.

Solution1.AutomateExcel demonstrates automating Microsoft Excel application 
by using Microsoft Excel Primary Interop Assembly (PIA) and explicitly 
assigning each COM accessor object to a new varaible that you would 
explicitly call Marshal.FinalReleaseComObject to release it at the end.

Solution2.AutomateExcel demonstrates automating Microsoft Excel application 
by using Microsoft Excel PIA and forcing a garbage collection as soon as the 
automation function is off the stack (at which point the RCW objects are no 
longer rooted) to clean up RCWs and release COM objects.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this code sample on a computer that has Microsoft Excel 2007 
installed.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the Excel automation 
sample that starts a Microsoft Excel instance, creates a workbook, fills 
data into a specified range, saves the workbook, and quits the Microsoft 
Excel application cleanly.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get the application: CSAutomateExcel.exe.

Step2. Open Windows Task Manager (Ctrl+Shift+Esc) to confirm that no 
Excel.exe is running. 

Step3. Run the application. It should print the following content in the 
console window if no error is thrown.

  Excel.Application is started
  A new workbook is created
  The active worksheet is renamed as Report
  Filling data into the worksheet ...
  Save and close the workbook
  Quit the Excel application

  Excel.Application is started
  A new workbook is created
  The active worksheet is renamed as Report
  Filling data into the worksheet ...
  Save and close the workbook
  Quit the Excel application

Then, you will see two new workbooks in the directory of the application: 
Sample1.xlsx and Sample2.xlsx. Both workbooks have a worksheet named "Report". 
The worksheet has the following data in the range A1:C6.

  First Name   Last Name   Full Name
  John         Smith       John Smith
  Tom          Brown       Tom Brown
  Sue          Thomas      Sue Thomas
  Jane         Jones       Jane Jones
  Adam         Johnson     Adam Johnson

Step4. In Windows Task Manager, confirm that the Excel.exe process does not 
exist, i.e. the Microsoft Excel intance was closed and cleaned up properly.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSAutomateExcel - VBAutomateExcel - CppAutomateExcel

These examples automate Microsoft Excel to do the same thing in different 
programming languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Console application and reference the Excel Primary Interop 
Assembly (PIA). To reference the Excel PIA, right-click the project file
and click the "Add Reference..." button. In the Add Reference dialog, 
navigate to the .NET tab, find Microsoft.Office.Interop.Excel 12.0.0.0 and 
click OK.

Step2. Import and rename the Excel interop namepace:

	using Excel = Microsoft.Office.Interop.Excel;

Step3. Start up an Excel application by creating an Excel.Application object.

	Excel.Application oXL = new Excel.Application();

Step4. Get the Workbooks collection from Application.Workbooks and call its 
Add function to create a new workbook. The Add function returns a Workbook 
object.

	oWBs = oXL.Workbooks;
	oWB = oWBs.Add(missing);

Step5. Get the active worksheet by calling Workbook.ActiveSheet and set the
sheet's Name.

	oSheet = oWB.ActiveSheet as Excel.Worksheet;
	oSheet.Name = "Report";

Step6. Construct a two-dimensional array containing some first name and last 
name data and assign it to the Value2 property of a worksheet range. The 
array's content will appear in the range.

	string[,] saNames = new string[,] {
		{"John", "Smith"}, 
		{"Tom", "Brown"}, 
		{"Sue", "Thomas"}, 
		{"Jane", "Jones"}, 
		{"Adam", "Johnson"}};

	oRng1 = oSheet.get_Range("A2", "B6");
	oRng1.Value2 = saNames;

Step7. Use formula to generate Full Name column from first name and last name 
by setting range's Formula property.

	oRng2 = oSheet.get_Range("C2", "C6");
	oRng2.Formula = "=A2 & \" \" & B2";

Step8. Call workbook.SaveAs method to save the workbook as a local file. 
Then, call workbook.Close to close the workbook and call application.Quit to 
quit the application.

	oWB.SaveAs(fileName, Excel.XlFileFormat.xlOpenXMLWorkbook,
		missing, missing, missing, missing,
		Excel.XlSaveAsAccessMode.xlNoChange,
		missing, missing, missing, missing, missing);
	oWB.Close(missing, missing, missing);

	oXL.UserControl = true;
	oXL.Quit();

Step9. Clean up the unmanaged COM resource. To get Excel terminated rightly,
we need to call Marshal.FinalReleaseComObject() on each COM object we used.
We can either explicitly call Marshal.FinalReleaseComObject on all accessor 
objects:

	// See Solution1.AutomateExcel
	if (oRng2 != null)
	{
		Marshal.FinalReleaseComObject(oRng2);
		oRng2 = null;
	}
	if (oRng1 != null)
	{
		Marshal.FinalReleaseComObject(oRng1);
		oRng1 = null;
	}
	if (oCells != null)
	{
		Marshal.FinalReleaseComObject(oCells);
		oCells = null;
	}
	if (oSheet != null)
	{
		Marshal.FinalReleaseComObject(oSheet);
		oSheet = null;
	}
	if (oWB != null)
	{
		Marshal.FinalReleaseComObject(oWB);
		oWB = null;
	}
	if (oWBs != null)
	{
		Marshal.FinalReleaseComObject(oWBs);
		oWBs = null;
	}
	if (oXL != null)
	{
		Marshal.FinalReleaseComObject(oXL);
		oXL = null;
	}

and/or force a garbage collection as soon as the calling function is off the 
stack (at which point these objects are no longer rooted) and then call 
GC.WaitForPendingFinalizers.

	// See Solution2.AutomateExcel
	GC.Collect();
	GC.WaitForPendingFinalizers();
	// GC needs to be called twice in order to get the Finalizers called 
	// - the first time in, it simply makes a list of what is to be 
	// finalized, the second time in, it actually is finalizing. Only 
	// then will the object do its automatic ReleaseComObject.
	GC.Collect();
	GC.WaitForPendingFinalizers();


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Excel 2007 Developer Reference
http://msdn.microsoft.com/en-us/library/bb149067.aspx

How to automate Microsoft Excel from Microsoft Visual C# .NET
http://support.microsoft.com/kb/302084

How to terminate Excel process after automation
http://blogs.msdn.com/geoffda/archive/2007/09/07/the-designer-process-that-would-not-terminate-part-2.aspx

How To Use Automation to Get and to Set Office Document Properties with 
Visual C# .NET
http://support.microsoft.com/kb/303296


/////////////////////////////////////////////////////////////////////////////
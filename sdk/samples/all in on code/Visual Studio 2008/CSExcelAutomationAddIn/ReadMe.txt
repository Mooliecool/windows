========================================================================
    OFFICE ADD-IN : CSExcelAutomationAddIn Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSExcelAutomationAddIn project is a class library project written in C#.
It illustrates how to write a managed COM component which can be used as an
Automation AddIn in Excel. The Automation AddIn can provide user defined 
functions for Excel.


/////////////////////////////////////////////////////////////////////////////
Deployment:

Step1: Copy the output assembly to the target machine.

Step2: Run regasm with /CodeBase flag to register the assembly as COM interop.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Visual C# class library project.

Step2. Import the following namepaces:

	using Microsoft.Win32;
	using System.Runtime.InteropServices;

Step3. Use the following three attributes to decorate your class

    [Guid("7127696E-AB87-427a-BC85-AB3CBA301CF3")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]

You can generate an Guid using the integrated tool from Tools->Create GUID

Step4. Write public functions that will be exported as user defined functions 
(UDFs) in Excel. For example, 

	public double NumberOfCells(object range)
	{
		Excel.Range r = range as Excel.Range;
		return r.Cells.Count;
	}

Step5. Write two functions decorated with these attributes respectively

	[ComRegisterFunctionAttribute]
	[ComUnregisterFunctionAttribute]

In the two functions, write registry keys that register / unregister the 
assembly as Excel automation add-in.

Step6. Register the output assembly as COM component.
To do this, click Project->Project Properties... button. And in the project
properties page, navigate to Build tab and check the box "Register for COM
interop".

Step7. Build your solution.

Step8. Open Excel, click the Office button->Excel Options. In the Excel
Options dialog, navigate to Add-Ins tab, and choose the Excel Add-ins in the
comboBox, click Go.

Step9. In Add-Ins dialog, click Automation button. In the Automation Servers
dialog, find CSExcelAutomationAddIn.MyFunctions. Select it and click OK for
twice. 

Step10. Use the UDFs in the Excel workbook.


/////////////////////////////////////////////////////////////////////////////
References:

Excel COM add-ins and Automation add-ins
http://support.microsoft.com/kb/291392

Writing user defined functions for Excel in .NET
http://blogs.msdn.com/eric_carter/archive/2004/12/01/273127.aspx

Create an Automation Add-In for Excel using .NET
http://www.codeproject.com/KB/COM/excelnetauto.aspx


/////////////////////////////////////////////////////////////////////////////
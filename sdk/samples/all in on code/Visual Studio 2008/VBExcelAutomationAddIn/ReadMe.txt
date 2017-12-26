========================================================================
    CLASS LIBRARY APPLICATION : VBExcelAutomationAddIn Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The VBExcelAutomationAddIn project is a class library project written in VB.
It illustrates how to write a managed COM component which can be used as an
Automation AddIn in Excel. The Automation AddIn can provide user defined 
functions for Excel.


/////////////////////////////////////////////////////////////////////////////
Deployment:

Step1: Copy the output assembly to the target machine.

Step2: Run regasm with /CodeBase flag to register the assembly as COM interop.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create an VB.NET class library project

Step2. Import the following namepace:

	Imports System.Runtime.InteropServices
	Imports System.Text
	Imports Microsoft.Win32

Step3. Use the following three attributes to decorate your class

	<ComVisible(True), _
	ClassInterface(ClassInterfaceType.AutoDual), _
	Guid("83111578-8F0D-4821-835A-714DD2AACE3B")> _

You can generate an Guid using the integrated tool from Tools->Create GUID

Step4. Write public functions that will be exported as user defined functions 
(UDFs) in Excel. For example, 

	Public Function NumberOfCells(ByVal range As Object) As Double
        Dim r As Excel.Range = TryCast(range, Excel.Range)
        Return CDbl(r.get_Cells.get_Count)
    End Function

Step5. Write two functions decorated with these attributes respectively:

	<ComRegisterFunction()> _
	<ComUnregisterFunction()> _

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
dialog, find VBExcelAutomationAddIn.MyFunctions. Select it and click OK for
twice. 

Step10. Use the UDFs in the Excel workbook.


/////////////////////////////////////////////////////////////////////////////
References:

Excel COM add-ins and Automation add-ins
http://support.microsoft.com/kb/291392

Writing user defined functions for Excel in .NET
http://blogs.msdn.com/eric_carter/archive/2004/12/01/273127.aspx

CCreate an Automation Add-In for Excel using .NET
http://www.codeproject.com/KB/COM/excelnetauto.aspx


/////////////////////////////////////////////////////////////////////////////
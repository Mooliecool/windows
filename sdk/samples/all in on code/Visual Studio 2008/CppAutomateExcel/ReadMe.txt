========================================================================
    CONSOLE APPLICATION : CppAutomateExcel Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CppAutomateExcel example demonstrates how to write VC++ codes to create a  
Microsoft Excel instance, create a workbook, fill data into a specific 
range, save the workbook, close the Microsoft Excel application and then 
clean up unmanaged COM resources.

There are three basic ways you can write VC++ automation codes:

1. Automating Excel using the #import directive and smart pointers 

The code in Solution1.h/cpp demonstrates the use of #import to automate Excel. 
#import (http://msdn.microsoft.com/en-us/library/8etzzkb6.aspx), a new  
directive that became available with Visual C++ 5.0, creates VC++ "smart 
pointers" from a specified type library. It is very powerful, but often not 
recommended because of reference-counting problems that typically occur when 
used with the Microsoft Office applications. Unlike the direct API approach 
in Solution2.h/cpp, smart pointers enable us to benefit from the type info to 
early/late bind the object. #import takes care of adding the messy guids to 
the project and the COM APIs are encapsulated in custom classes that the 
#import directive generates.

2. Automating Excel using C++ and the COM APIs

The code in Solution2.h/cpp demontrates the use of C/C++ and the COM APIs to 
automate Excel. The raw automation is much more difficult, but it is sometimes 
necessary to avoid the overhead with MFC, or problems with #import. Basically, 
you work with such APIs as CoCreateInstance(), and COM interfaces such as 
IDispatch and IUnknown.

3. Automating Excel with MFC

With MFC, Visual C++ Class Wizard can generate "wrapper classes" from the 
type libraries. These classes simplify the use of the COM servers. Automating 
Excel with MFC is not covered in this sample.


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
you will get the application: CppAutomateExcel.exe.

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
Sample1.xlsx and Sample2.xlsx. Both workbooks contain a worksheet named 
"Report". The worksheet has the following data in the range A2:B6.

  John         Smith
  Tom          Brown
  Sue          Thomas
  Jane         Jones
  Adam         Johnson

Step4. In Windows Task Manager, confirm that the Excel.exe process does not 
exist, i.e. the Microsoft Excel intance was closed and cleaned up properly.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppAutomateExcel - CSAutomateExcel - VBAutomateExcel

These examples automate Microsoft Excel to do the same thing in different 
programming languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Automating Excel using the #import directive and smart pointers 
(Solution1.h/cpp)

Step1. Import the type library of the target COM server using the #import 
directive. 

    #import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
        rename("RGB", "MSORGB")
        rename("DocumentProperties", "MSODocumentProperties")
    // [-or-]
    //#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
    //    rename("RGB", "MSORGB")
    //    rename("DocumentProperties", "MSODocumentProperties")

    using namespace Office;

    #import "libid:0002E157-0000-0000-C000-000000000046"
    // [-or-]
    //#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

    using namespace VBIDE;

    #import "libid:00020813-0000-0000-C000-000000000046" \
        rename("DialogBox", "ExcelDialogBox") \
        rename("RGB", "ExcelRGB") \
        rename("CopyFile", "ExcelCopyFile") \
        rename("ReplaceText", "ExcelReplaceText") \
        no_auto_exclude
    // [-or-]
    //#import "C:\\Program Files\\Microsoft Office\\Office12\\EXCEL.EXE" \
    //    rename("DialogBox", "ExcelDialogBox") \
    //    rename("RGB", "ExcelRGB") \
    //    rename("CopyFile", "ExcelCopyFile") \
    //    rename("ReplaceText", "ExcelReplaceText") \
    //    no_auto_exclude

Step2. Build the project. If the build is successful, the compiler generates 
the .tlh and .tli files that encapsulate the COM server based on the type 
library specified in the #import directive. It serves as a class wrapper we
can now use to create the COM class and access its properties, methods, etc.

Step3. Initializes the COM library on the current thread and identifies the 
concurrency model as single-thread apartment (STA) by calling CoInitializeEx, 
or CoInitialize.

Step4. Create the Excel.Application COM object using the smart pointer. The 
class name is the original interface name (i.e. Excel::_Application) with a 
"Ptr" suffix. We can use either the constructor of the smart pointer class or 
its CreateInstance method to create the COM object.

Step5. Automate the Excel COM object through the smart pointers. In this 
example, you can find the basic operations in Excel automation like 

    Create a new Workbook. (i.e. Application.Workbooks.Add)
    Get the active Worksheet, and rename it to be "Report".
    Fill data into the worksheet's cells.
    Save the workbook as a xlsx file and close it.

Step6. Quit the Excel application. (i.e. Application.Quit())

Step7. It is said that the smart pointers are released automatically, so we 
do not need to manually release the COM object.

Step8. It is necessary to catch the COM errors if the type library was  
imported without raw_interfaces_only and when the raw interfaces 
(e.g. raw_Quit) are not used. For example:

    #import "XXXX.tlb"
    try
    {
        spXlApp->Quit();
    }
    catch (_com_error &err)
    {
    }

Step9. Uninitialize COM for this thread by calling CoUninitialize.

-----------------------------------------------------------------------------

B. Automating Excel using C++ and the COM APIs (Solution2.h/cpp)

Step1. Add the automation helper function, AutoWrap.

Step2. Initializes the COM library on the current thread and identifies the 
concurrency model as single-thread apartment (STA) by calling CoInitializeEx, 
or CoInitialize.

Step3. Get CLSID of the Excel COM server using the API CLSIDFromProgID.

Step4. Start the Excel COM server and get the IDispatch interface using the 
API CoCreateInstance.

Step5. Automate the Excel COM object with the help of AutoWrap. In this 
example, you can find the basic operations in Excel automation like 

    Create a new Workbook. (i.e. Application.Workbooks.Add)
    Get the active Worksheet, and rename it to be "Report".
    Fill data into the worksheet's cells.
    Save the workbook as a xlsx file and close it.

Step6. Quit the Excel application. (i.e. Application.Quit())

Step7. Release the COM objects.

Step8. Uninitialize COM for this thread by calling CoUninitialize.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Excel 2007 Developer Reference
http://msdn.microsoft.com/en-us/library/bb149067.aspx

Office Automation Using Visual C++
http://support.microsoft.com/kb/196776/

How to automate Excel from C++ without using MFC or #import
http://support.microsoft.com/kb/216686


/////////////////////////////////////////////////////////////////////////////
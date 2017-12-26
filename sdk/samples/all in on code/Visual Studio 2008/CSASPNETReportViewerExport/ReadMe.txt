========================================================================
    Web APPLICATION : CSASPNETReportViewerExport Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Sometimes we may have to hide the toolbar of the ReportViewer control, or the 
users only want to view the report in the PDF or EXCEL directly, then we must
achieve the export function programmatically. This sample demonstrates how to
export the rdlc as PDF and EXCEL programmatically. 


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

ReportViewer installation:
Microsoft Report Viewer 2008 SP1 Redistributable
http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=bb196d5d-76c2-4a0e-9458-267d22b6aac6


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1.Create a export function which accepts a file type parameter.

2.In order to export the RDLC report into other forms, We have to get the 
byte data of the RDLC report with LocalReport.Render Method. 

3.Use a FileStream to create a PDF file or EXCEL file.

4.Write the aboved byte data to the FileStream, then the report records will be 
exported into the PDF file or the Excel file.


/////////////////////////////////////////////////////////////////////////////
Assemble reference:

Microsoft.ReportViewer.WebForms 9.0


/////////////////////////////////////////////////////////////////////////////
References:

LocalReport.Render Method 
http://msdn.microsoft.com/en-us/library/ms251839(VS.80).aspx

FileStream.Write Method
http://msdn.microsoft.com/en-us/library/system.io.filestream.write.aspx


/////////////////////////////////////////////////////////////////////////////
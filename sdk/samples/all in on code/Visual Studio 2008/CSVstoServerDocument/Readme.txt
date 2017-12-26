========================================================================
    VSTO : CSVstoServerDocument Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSVstoServerDocument project demonstrates how to use the ServerDocument
class to extract information from a VSTO customized Word document or Excel
Workbook; and also how to programmatically add / remove VSTO customizations.

/////////////////////////////////////////////////////////////////////////////

Note: The project must be compiled as x86 for it to work.
The 2003 Office version of ServerDocument calls VSTOStorageWrapper.dll which
is an x86 COM component, so the calling application must be an x86 process.

There are two versions of ServerDocument class, one for 2003 Office and the
other for 2007 Office.

The 2003 Office version can be found in assembly (part of VSTO 2005 SE):
Microsoft.VisualStudio.Tools.Applications.Runtime.dll

The 2007 Office version can be found in assembly (part of VSTO 3.0):
Microsoft.VisualStudio.Tools.Applications.ServerDocument.v9.0.dll

To add/remove the VSTO customization, call the static methods AddCustomization
and RemoveCustomization of the ServerDocument class.

To get detailed VSTO customization info from a document/workbook, construct
an instance of ServerDocument and then access its properties and methods.

/////////////////////////////////////////////////////////////////////////////
References:

ServerDocument Class (2003 System)
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.tools.applications.runtime.serverdocument(VS.89).aspx

ServerDocument Class (2007 System)
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.tools.applications.serverdocument.aspx

/////////////////////////////////////////////////////////////////////////////
========================================================================
    OFFICE ADD-IN : CSOutlookRibbonXml Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSOutlookRibbonXml provides the examples on how to customize Office UI 
using the Ribbon XML. This sample also shows a way on how to keep & track the 
same control's property status (e.g. Checked) in different inspectors.


/////////////////////////////////////////////////////////////////////////////
Creation:

1. We need to create an XML file containing description of our customized
   Ribbon contents. (See Ribbon.xml)
2. Create a class that implements the 
   Microsoft.Office.Core.IRibbonExtensibility class. (See Ribbon.cs)
3. In Ribbon.cs, implement the GetCustomUI (memeber of IRibbonExtensibility)
   method. In this method, we return Ribbon XML according to the RibbonID
   passed in.
4. In Ribbon.cs, implement the callback methods.


/////////////////////////////////////////////////////////////////////////////
References:

Customizing the Ribbon in Outlook 2007
http://msdn.microsoft.com/en-us/library/bb226712.aspx


/////////////////////////////////////////////////////////////////////////////
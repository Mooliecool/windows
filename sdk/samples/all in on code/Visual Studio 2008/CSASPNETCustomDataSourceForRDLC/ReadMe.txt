========================================================================
    Web APPLICATION : CSASPNETCustomDataSourceForRDLC Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:
This sample demonstrates how to use custom class as the datasource for the RDLC.
In order to explain the sample more clearly, we will simulate some customers'
information.


/////////////////////////////////////////////////////////////////////////////

ReportViewer installation:
Microsoft Report Viewer 2008 SP1 Redistributable
http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=bb196d5d-76c2-4a0e-9458-267d22b6aac6

/////////////////////////////////////////////////////////////////////////////

The principle of the sample:

1.Create a class "Customer" as the template to store the customers' information,
like:ID,Name and country.

2.Then create a GetPersons class which includes a generic list to store all the 
instances of the Customers as the datasource for RDLC.

3.Add a report to the solution and use a table control to show the customers' 
information.

4.Add a report parameter "CountryName" to filter the customers' information.

5. Go to the datasource property of the report, then set the filter with the
following expression.

Expression:
=iif(Parameters!CountryName.Value="",Parameters!CountryName.Value,Fields!Country.Value)

Operation: "="

Value: =Parameters!CountryName.Value

/////////////////////////////////////////////////////////////////////////////

Assemble reference:
Microsoft.ReportViewer.WebForms 9.0

/////////////////////////////////////////////////////////////////////////////
References:
LocalReport.SetParameters Method
 
http://msdn.microsoft.com/en-us/library/ms251790(VS.80).aspx




/////////////////////////////////////////////////////////////////////////////
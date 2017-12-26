========================================================================
    Web APPLICATION : CSASPNETRDLCMainSubReport Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Introduction:

This sample demonstrates how to build a sub-report which get data from
SQL Server based on a primary key passed from the main-report, then use 
the ReportViewer control to show that Main/Sub report in the web page.
 

/////////////////////////////////////////////////////////////////////////////
Creation steps:

1.Create the main report with the report wizard, follow these steps: 
  a.In the Solution Explorer of the Visual Studio, add a new item, 
    then select the report wizard template and name it "MainReport". 
  b.In the Data Source configuration wizard, connect to the SQLServer2005DB
    database and create DepDataSet which includes the Department table. 
  c.Select the tabular as the report type.
  d.Drag the fields: DepartmentID and Name to the Details columns.
  e.Finish the report wizard.

2.Create the sub report with the report wizard, follow these steps: 
  a.In the Solution Explorer of the Visual Studio, add a new item, 
    then select the report wizard template and name it ¡°SubReport¡±. 
  b.In the Data Source configuration wizard, connect to the SQLServer2005DB 
    database and create CourseDataSet which includes the Course table.
  c.Select the tabular as the report type.
  d.Drag the fields: CourseID and Title to the Details columns.
  e.Finish the report wizard.

3.Filter the sub report with the Department ID and create a parameter 
  to incept the Department ID from the main report, follow these steps: 
  a.Switch to sub report, select the report menu in the report designer interface,
    and then select report parameter item, in the popup window, add a new
    parameter named DepartmentID, change its type into Integer, then click ok.
  b.Select the report menu in the report designer interface, 
    and then select Data sources item  in the popup window, click the property 
    of the sub report datasource. Switch to the filter tab, 
    use  "=Fields! DepartmentID.Value"  for the expression
    and "=Parameters! DepartmentID.Value" for value, and the operator is "=".

4.Add the sub report control into the main report, follow these steps: 
  a.Select the detail row of the table in main report,
    right click and insert a new detail row below. 
  b.Merge the cells of the second detail row.
  c.Drag a sub report control from the tool box into the second detail row. 
  d.Right click the sub report control, then select the property. 
    In the general tab of the popup window, select the sub report name, 
    then switch to the parameters tab, use "DepartmentID" for the parameter name
    and "=Fields! DepartmentID.Value" for parameter value.
  e.Select the report menu in the report designer interface, 
    and then select Data sources item,  in the popup window, 
    select the sub report datasource and add it into the main report. 
    Otherwise we will get the error message "Error:Subreport could not be shown" 
    during the report running.

5.Use the Report Viewer control to display the report, follow these steps: 
  a.In the Solution Explorer of the Visual Studio, add a new item, 
    then select the web form template and name it "CSASPNETRDLCMainSubReport.aspx".  
  b.From the Reporting node of the ToolBox, add a ReportViewer control to the webpage, 
    then in the report tasks, select MainReport.rdlc.
  c.Use the following code to supply the data for the sub report. 
  protected void Page_Load(object sender, EventArgs e)
  {
   ReportViewer1.LocalReport.SubreportProcessing += new SubreportProcessingEventHandler(SetSubDataSource);
   this.ReportViewer1.LocalReport.Refresh();
  }
  public void SetSubDataSource(object sender, SubreportProcessingEventArgs e)
  {
   e.DataSources.Add(new ReportDataSource("CourseDataSet_Course", "ObjectDataSource2"));
  }






/////////////////////////////////////////////////////////////////////////////
Database usage:

We use Course and Department tables that belong to SQLServer2005DB database as the datasource.

The sample data:

Department table

DepartmentID	Name
1	            Computer Department 
2	            Mathematics

Course table

courseid	title
1	        ASPNET
2	        JAVA
3	        linear algebra
4	        higher mathematics

/////////////////////////////////////////////////////////////////////////////
References:

LocalReport.SubreportProcessing Event 
http://msdn.microsoft.com/en-us/library/microsoft.reporting.webforms.localreport.subreportprocessing.aspx


/////////////////////////////////////////////////////////////////////////////
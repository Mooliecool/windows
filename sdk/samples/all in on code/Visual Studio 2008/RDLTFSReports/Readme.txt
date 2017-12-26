===============================================================================
    SSRS Reports : RDLTFSReports Project Overview
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

There are a lot of reports included in Process Template and they meet out most requirement. 
But sometime we need more data or reports from TfsWarehouse.

The Work Item Age Chart Report demonstrates how to create and deploy a Custom Report that 
can show a chat of work item age range.


/////////////////////////////////////////////////////////////////////////////

Prerequisite:

This Report Server Project sample works in machines where SQL Server Business Intelligence
Development Studio 2008 is installed, and SQL Server 2008 Reporting Service is also 
required to deploy this report.


////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the deploy the report.

Step1. Use IE to open a Reports Site of a Team Project(i.e. ProjectA), and the site url is 
like http://<server>/Reports/Pages/Folder.aspx?ItemPath=%2fProjectA

Step2. Click "Upload File". In the new page, click "Browse" of "File to upload"
and select  "Work Item Age Chart Report.rdl" include in this sample project 
folder.

Step3. Check "Overwrite Item if it exists" and then click "OK"

Step4. After IE is back to http://<server>/Reports/Pages/Folder.aspx?ItemPath
=%2fProjectA, click "Work Item Age Chart Report" to view the detail of this 
report.

Step5. In "Work Item Age Chart Report" page, click "Properties">Data Source.
check "a shared data source" and browse to select "/Home/TfsOlapReportDS"

Step6. Click "Apply" 

Step7. Click "View"


/////////////////////////////////////////////////////////////////////////////
Deployment:

Way1: follow this steps in Demo

way2: Open the property page of this sample project, set following properties

TargetDataSourceFolder:  /
TargetFolder:            <ProjectName>
TargetDataSourceFolder:  http://<Server>/reportserver

Then right click project=>Deploy. After deploying, please check whether the report
exists in Report Server and then modify the data source.


/////////////////////////////////////////////////////////////////////////////
 Code Logic:


A.Using VS to create a Report Server Project "CSTFSReports" 


B. Right click Shared Data Sources and add following data source

   TfsOlapReportDS.rds:

    Type:Microsoft SQL Server Analysis Services

    Connection String: Data source=<server name>; Initial Catalog=TfsWarehouse

    Credentials: Use Windows Authentication(intergraded security)


C. Right Click Reports and then add a report 
	1 Select Shared data source TfsOlapReportDS as Data Source
	2 In the query designer, click "Query Parameter", add ProjectName with default 
	value ProjectA.(We will modify it later)
	3 Use following MDX script as Query
	
	with 
member [Measures].[Age] 
As 
IIF([Measures].[Current Work Item Count]>0,
DateDiff("d",cdate([System_CreatedDate].[Date].CurrentMember .name),Now()),null)

member [Measures].[AgeRange]
as 
case
when [Measures].[Age] >70 then 70
when [Measures].[Age] >60 then 60
when [Measures].[Age] >50 then 50
when [Measures].[Age] >40 then 40
when [Measures].[Age] >15 then 15
when [Measures].[Age] >10 then 10
when [Measures].[Age] >5 then 5
when [Measures].[Age] >0 then 0
when [Measures].[Age] <0 then -1
else null
end
 SELECT 
 non empty  
 {[Measures].[Current Work Item Count],[Measures].[Age] ,[Measures].[AgeRange]} 
 ON COLUMNS,
 [System_CreatedDate].[Date].[Date].AllMembers 
 on Rows

 FROM

( SELECT ( STRTOSET(@ProjectName, CONSTRAINED) ) ON COLUMNS FROM [Current Work Item])
 WHERE ( IIF( STRTOSET(@ProjectName, CONSTRAINED).Count = 1, STRTOSET(@ProjectName, CONSTRAINED), 
 [Team Project].[Team Project].currentmember ) )

	4 Select Tabular as Report Type (We will delete it because we want to use chart)

	5 Add all field to detail and then next,next¡­

D. Modify the default value of  parameter ProjectName to =Globals!ReportFolder.Substring(1) and 
   Visible to Hidden
   
E. Delete the table in Design mode, drop a Chart from Tool Box, set the chart type to column

F. Drop Current_work_Item_count from Report Data Window to Data Fields of the chart, and AgeRange 
   to catagory fields.

G. Set the deploy property of the project

  Right click project and then select  property, in the Property dialog, set OverwriteDataSources 
  to False, TargetDataSourceFolder to /, TargetReportFolder to <projectname>, TargetServerURL 
  to http://<Servername>/Reportserver

H. Set the data source of the report in report server.

    Go to http://<server>/Reports/Pages/Report.aspx?ItemPath=%2f<project>%2f<Report1Name>, 
    select properties->DataSource, select correct source ¡°/TfsOlapReportDS¡±


/////////////////////////////////////////////////////////////////////////////




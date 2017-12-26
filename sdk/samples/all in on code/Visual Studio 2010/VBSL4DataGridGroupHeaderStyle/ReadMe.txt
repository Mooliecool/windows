=============================================================================
  SILVERLIGHT APPLICATION : VBSL4DataGridGroupHeaderStyle Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This project created a sample application, which illustrates how to define 
group header style of different levels and define group header style 
according to group header content.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 4 Tools RTM for Visual Studio 2010
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=b3deb194-ca86-4fb6-a716-b67c2604a139&displaylang=en

Silverlight 4 Toolkit for Visual Studio 2010
http://silverlight.codeplex.com/


/////////////////////////////////////////////////////////////////////////////
Implementation:

1.	How to define group header style of different levels. 
Group header style can be defined using DataGrid.RowGroupHeaderStyle Property 
and the styles are applied to different levels based on the order of style tag. 
The first one is applied to the most top level, the second for the second level 
and so on. 

2.	How to vary group header style in one level. 
In this sample, StackPanel is defined within the DataGrid.RowGroupHeaderStyle 
using Control.Template. The Background of StackPanel is binded to 
GroupHeaderName, and the background value is set based on GroupHeaderName 
using IValueConverter. 

    
/////////////////////////////////////////////////////////////////////////////
References:

DataGrid.RowGroupHeaderStyles Property
http://msdn.microsoft.com/en-us/library/system.windows.controls.datagrid.rowgroupheaderstyles.aspx

Control.Template Property
http://msdn.microsoft.com/en-us/library/system.windows.controls.control.template.aspx

Style.TargetType Property
http://msdn.microsoft.com/en-us/library/system.windows.style.targettype.aspx


/////////////////////////////////////////////////////////////////////////////
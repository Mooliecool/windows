========================================================================
    SILVERLIGHT APPLICATION : VBSL4DataFormCancelButton Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This project demonstrates how to show Cancel Button in DataForm when editting

/////////////////////////////////////////////////////////////////////////////
Demo:

To test this sample, please try the following steps:
	1. Make sure you have Silverlight 4 Tools for Visual Studio 2010 and 
	   Silverlight Toolkit installed.
	   This demo uses Silverlight 4 Toolkit - April 2010 
	   (http://silverlight.codeplex.com/releases/view/43528)
	   
	   It is recommended to install Silverlight_4_Toolkit_April_2010.msi. 
	   After installation, all related dlls will be added to GAC. 
	   Otherwise, you need to unzip Silverlight_4_Toolkit_April_2010.zip 
	   and replace the reference to 
	   System.Windows.Controls.Data.DataForm.Toolkit.dll.
	   
	   You can find it under 
	   April 2010 Silverlight Toolkit\April 2010 Silverlight Toolkit\Bin.
	   (Right click on your project-->Add Reference -->choose the dll)
	2. Open CSSL4DataFormCancelButton solution and compile.
	3. Run the project.
    4. The Cancel Button will be disabled by default and will be enabled 
	   when you modify any of the fields or add a new record.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 4 Tools RTM for Visual Studio 2010
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=b3deb194-ca86-4fb6-a716-b67c2604a139&displaylang=en


Silverlight 4 Toolkit for Visual Studio 2010
http://silverlight.codeplex.com/releases/view/43528


/////////////////////////////////////////////////////////////////////////////
Code Logic:

How to achieve the Cancel function?
    1. Create a class which implements IEditableObject Interface and 
	   BeginEdit,CancelEdit,EndEdit three methods.
	2. Copy the current item when BeginEdit.
	3. Restore the original value When CancelEdit.
    
/////////////////////////////////////////////////////////////////////////////
References:

IEditableObject Interface
http://msdn.microsoft.com/en-us/library/system.componentmodel.ieditableobject_members%28v=VS.95%29.aspx

DataAnnotation Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.dataannotations%28VS.95%29.aspx

RangeAttribute Class
http://msdn.microsoft.com/en-us/library/system.componentmodel.dataannotations.rangeattribute%28v=VS.95%29.aspx

INotifyPropertyChanged Interface
http://msdn.microsoft.com/en-us/library/system.componentmodel.inotifypropertychanged%28VS.95%29.aspx


/////////////////////////////////////////////////////////////////////////////

=============================================================================
           Visual Studio Package Project: VBCustomizeVSToolboxItem
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

If you add a new item to Visual Studio 2010 toolbox, the display name and 
tooltip of the new item is the same by default.  The sample demonstrates how 
to add an item with custom tooltip to Visual Studio Toolbox.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Visual Studio 2010 Premium or Visual Studio 2010 Ultimate. Visual Studio 2010 SDK.

NOTE: VS2010 SP1 should be applied after the installation of VS SDK.


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this project in Visual Studio 2010 Professional or better. 

Step2. Open the property page of this project and choose Debug tab. Set the Start Option
       to Start external program and browse the devenv.exe (The default location is 
       C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe), and add
       "/rootsuffix Exp" (no quote) to the Command line arguments.
        
Step3. Build the solution. 

Step4. Press F5, and the Experimental Instance of Microsoft Visual Studio 2010 will 
       be launched.

Step5. In the VS Experimental Instance, open a new or existing solution and start to edit 
       a .txt file. 

	     
Step6. Open the toolbox window, you will find a new tab called "VB Custom Toolbox Tab" and
       a new item "VB Custom Toolbox Item". Move mouse over this item, and you can see following 
	   tooltip:
	   
	   VB Custom Toolbox Tooltip
	   VB Custom Toolbox Description 

Step7. Drag the new toolbox item and drop it to the .txt file, "VB Hello world" will be added 
       to the file.
	   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Create a VSIX project.

1. Select the Visual Studio Package project template. In the Name box, type a name for the 
   solution and then click OK.

   The Visual Studio Package project template is available in these locations in the New 
   Project dialog box:
       Under Visual Basic Extensibility. By default, the language of the project is Visual Basic.
       Under C# Extensibility. By default, the language of the project is C#.
       Under Other Project Types Extensibility. By default, the language of the project is C++.

2. On the Select a Programming Language page, select either Visual C#. Have the template 
   generate a key.snk file to sign the assembly. Alternatively, click Browse to select your
   own key file. The template makes a copy of your key file and names it key.snk. 

3. On the Basic VSPackage Information page, specify details about your VSPackage. 

4. Click Finish to complete the creation. 

5. Optionally, select Integration Test Project and Unit Test Project to create test
   projects for your solution.

   For more information, see 
   Walkthrough: Creating a Menu Command By Using the Visual Studio Package Template.
   http://msdn.microsoft.com/en-us/library/bb164725.aspx

   NOTE: Command menu does not have to be selected.


B. Override the Initialize method in VBCustomizeVSToolboxItemPackage.

   In this method, get the IVsToolbox2 and IVsActivityLog services.


C. Add custom toolbox item. 
   
   Verify whether the ToolboxTab and ToolboxItem exist, if not, add custom toolbox item.

   using (var stream = SaveStringToStreamRaw(FormatTooltipData(toolboxTooltipString, toolboxDescriptionString)))
   {
       var toolboxData = new Microsoft.VisualStudio.Shell.OleDataObject();
       toolboxData.SetData("VSToolboxTipInfo", stream);
       toolboxData.SetData(DataFormats.Text, "Hello world");

       TBXITEMINFO[] itemInfo = new TBXITEMINFO[1];
       itemInfo[0].bstrText = toolboxItemString;
       itemInfo[0].hBmp = IntPtr.Zero;
       itemInfo[0].dwFlags = (uint)__TBXITEMINFOFLAGS.TBXIF_DONTPERSIST;

       ErrorHandler.ThrowOnFailure(vsToolbox2.AddItem(toolboxData, itemInfo, toolboxTabString));
   }


/////////////////////////////////////////////////////////////////////////////
References:

Walkthrough: Customizing ToolboxItem Configuration Dynamically
http://msdn.microsoft.com/en-us/library/bb165910.aspx

Walkthrough: Autoloading Toolbox Items
http://msdn.microsoft.com/en-us/library/bb166237.aspx

VSIX Deployment
http://msdn.microsoft.com/en-us/library/ff363239.aspx


/////////////////////////////////////////////////////////////////////////////
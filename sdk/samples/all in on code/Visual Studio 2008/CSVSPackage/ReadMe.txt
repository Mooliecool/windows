=============================================================================
            VSPackage Module: CSVSPackage Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VSPackages are software modules that make up and extend the Visual Studio 
integrated development environment (IDE) by providing UI elements, services, 
projects, editors, and designers. VSPackages are the principal architectural 
unit of Visual Studio, and are the unit of deployment, licensing, and security 
also. Visual Studio itself is written mostly as a collection of VSPackages. 
This sample demonstrate how to use the the Visual Studio Integration Package 
Wizard to create a simple VSPackage.


//////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the project
Setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

NOTE: The Package Load Failure Dialog occurs because there is no PLK(Package Load Key)
      Specified in this package. To obtain a PLK, please to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info
      http://msdn.microsoft.com/en-us/library/bb165395.aspx

/////////////////////////////////////////////////////////////////////////////
Creation:

A: How to Create a VSPackage 

	1. Create a new project using Visual Studio Integration Package as template
	   (New Project dialog box -> Other Project Types -> Extensibility). 

	2. In the Location box, type the file path for your VSPackage. 

	3. In the Name box, type the name for the solution and then click OK to start
	   the wizard. 

	3. On the Select a Programming Language page, select Visual C# and have the 
	   wizard generate a key.snk file to sign the assembly, then click Next.

	4. In the Basic VSPackage Information page, specify details about your 
	   VSPackage(Brand the VSPackage) and click Next.

	5. Select the Menu Command option to create a new command for the VSPackage
	   and click Next. The new command is put at the top of the Tools menu.

	6. Type the name(CSVSPackageDemo) and ID(cmdidCSVSPackageDemo) for the new 
	   menu command in the text boxes, then click Next. The command ID is the name
	   of a constant that represents this menu command in the generated code.

	7. Uncheck the Intergration Test Project and Unit Test Project, then click
	   Finish.

B: Customize the Menu Command Handler

	1. In Solution Explorer, open CSVSPackagePackage.cs. 
	
	2. Find the CSVSPackagePackage class.
	
	3. Find the code for the menu handler, which is implemented by the MenuItemCallback
	   method. This method executes when the user click the menu itmer. By default, it 
	   shows a message box.
	   
	   private void MenuItemCallback(object sender, EventArgs e)
        {
            // Show a Message Box to prove we were here
            IVsUIShell uiShell = (IVsUIShell)GetService(typeof(SVsUIShell));
            Guid clsid = Guid.Empty;
            int result;
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(uiShell.ShowMessageBox(
                       0,
                       ref clsid,
                       "CSVSPackageDemo",
                       string.Format(CultureInfo.CurrentCulture, 
                                     "Inside {0}.MenuItemCallback()", this.ToString()),
                       string.Empty,
                       0,
                       OLEMSGBUTTON.OLEMSGBUTTON_OK,
                       OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST,
                       OLEMSGICON.OLEMSGICON_INFO,
                       0,        // false
                       out result));
        }

C: Add a Keyboard Shortcut

	1. In Solution Explorer, open CSVSPackage.vsct.
	   VSCT file stands for Visual Studio Command Table. This is an XML based file that 
	   describes the layout and appearance of command items for a VSPackage. Command 
	   items include buttons, combo boxes, menus, toolbars, and groups of command items.
	
	2. Find the end of the Commands element, which is indicated by the </Commands> tag. 
	
	3. Add the following lines between the </Commands> tag and the <Symbols> tag. 
	   <KeyBindings>
          <KeyBinding guid="guidCSVSPackageCmdSet" 
                      id="cmdidCSVSPackageDemo"  
                      editor="guidVSStd97" 
                      key1="N" 
                      mod1="Control Shift"/>
       </KeyBindings>
	
D: Registration:

    1. PackageRegistration:
       The regpkg.exe utility scans types for this attribute to recognize that the type 
       should be registered as a package. Adding this attribute to our class, regpkg.exe 
       will handle it as a package and looks for other attributes to register the class 
       according to our intention. In our example this attribute sets the 
       UseManagedResourcesOnly flag to tell that all resources used by our package are 
       described in the managed package and not in a satellite .dll.
       http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.packageregistrationattribute.aspx
       
    2. DefaultRegistryRoot:
       This attribute specifier the default registration root for the package. The default
       value registers the package to in the Experimental hive.
       http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.defaultregistryrootattribute.aspx
    
    3. InstalledProductRegistration:
       This attribute is responsible to provide information to be displayed by the 
       Help|About function in the VS IDE. 
       http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.installedproductregistrationattribute.aspx
       
    4. ProvideLoadKey:
       hie attribute specifies that the VSPackage has a load key embedded in it and 
       provides some general information about the pacakge. Each VS component should 
       be signed with a so-called package load key (PLK) that is used by Visual Studio 
       to check if the package is the one it says about itself it is. 
       http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.provideloadkeyattribute.aspx
    
    5. ProvideMenuResource:
       This attribute tells Visual Studio that the package has some menus inside.       
       http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.providemenuresourceattribute.aspx
    
    6. Guid:
       This attribute defines the GUID of our package. This GUID is the unique identifier 
       of our package. This is used for the COM class registration, referencing to our 
       package within the IDE, and so on.
       http://msdn.microsoft.com/en-us/library/system.runtime.interopservices.guidattribute.aspx


/////////////////////////////////////////////////////////////////////////////
References:

How to: Create VSPackages (C# and Visual Basic)
http://msdn.microsoft.com/en-us/library/bb164725.aspx

How to: Register a VSPackage (C#)
http://msdn.microsoft.com/en-us/library/bb166544.aspx

How to: Brand a VSPackage (C#)
http://msdn.microsoft.com/en-us/library/bb165769.aspx

VSPackage Tutorial 1: How to Create a VSPackage
http://msdn.microsoft.com/en-us/library/cc138589.aspx

Designing XML Command Table (.Vsct) Files
http://msdn.microsoft.com/en-us/library/bb166366.aspx

KeyBindings Element
http://msdn.microsoft.com/en-us/library/bb165085.aspx

Creating an empty package
http://dotneteers.net/blogs/divedeeper/archive/2008/01/03/LernVSXNowPart2.aspx

Creating a package with a simple command
http://dotneteers.net/blogs/divedeeper/archive/2008/01/06/LearnVSXNowPart3.aspx

Menus and comands in VS IDE
http://dotneteers.net/blogs/divedeeper/archive/2008/02/22/LearnVSXNowPart13.aspx


/////////////////////////////////////////////////////////////////////////////
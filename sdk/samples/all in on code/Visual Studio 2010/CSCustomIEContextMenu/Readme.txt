=============================================================================
       Windows APPLICATION: CSCustomIEContextMenu Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to open an image in a new Tab using customized IE 
Context Menu. This sample supplies the following features:

1. Adding Entries to the IE Standard Context Menu. 
2. Overriding the IE Standard Context Menu using Browser Helper Object.
3. Deploy the custom IE Context Menu.

NOTE:
1. In IE8, there is a new approach to add a entry to IE context menu, which is called
   Accelerator or Activity. Accelerators make it easier to copy information from one 
   Web page to another, and "Adding Entries to the IE Standard Context Menu" is still
   a good approach to do work locally.

2. If you override the IE Standard Context Menu in a BHO, make sure only one add-on 
   at a time can override IDocHostUIHandler and multiple add-ons can easily conflict 
   with each other. You can also create your own web browser to set this handler.

3. On Windows Server 2008 or Windows Server 2008 R2, the Internet Explorer Enhanced 
   Security Configuration (IE ESC) is set to On by default, which means that this 
   Extension can not be loaded by IE. You have to turn off IE ESC for you. 


/////////////////////////////////////////////////////////////////////////////
Setup and Removal:

A. Setup

For 32bit IE on x86 or x64 OS, installCSCustomIEContextMenuSetup(x86).msi, the output
of the CSCustomIEContextMenuSetup(x86) setup project.

For 64bit IE on x64 OS, install CSCustomIEContextMenuSetup(x64).msi outputted by the 
CSCustomIEContextMenuSetup(x64) setup project.

B. Removal

For 32bit IE on x86 or x64 OS, uninstall CSCustomIEContextMenuSetup(x86).msi, the 
output of the CSCustomIEContextMenuSetup(x86) setup project. 

For 64bit IE on x64 OS, uninstall CSCustomIEContextMenuSetup(x64).msi, the output of
the CSCustomIEContextMenuSetup(x64) setup project.



/////////////////////////////////////////////////////////////////////////////
Demo:
 
Step1. Open this project in VS2010 and set the platform of the solution to x86. Make
       sure that the projects CSCustomIEContextMenu and CSCustomIEContextMenuSetup(x86)
	   are selected to build in Configuration Manager.

	   NOTE: If you want to run this sample in 64bit IE, set the platform to x64 and 
	         select CSCustomIEContextMenu and CSCustomIEContextMenuSetup(x64) to build.
        
Step2. Build the solution.
 
Step3. Right click the project CSCustomIEContextMenuSetup(x86) in Solution Explorer, 
       and choose "Install".

	   After the installation, open 32bit IE and click Tools=>Manage Add-ons, in the 
	   Manage Add-ons dialog, you can find the item "CSCustomIEContextMenu.BHOIEContextMenu".

Demo Adding Entries to the IE Standard Context Menu. 

Step4. Open 32bit IE and click Tools=>Manage Add-ons, and disable 
       "CSCustomIEContextMenu.BHOIEContextMenu". You may have to restart IE to make it 
	   take effect. 

Step5. Visit http://www.microsoft.com/. Right click an image on the web page, in the context
       menu, you will see the item "Open image in new tab". Click this item, and IE will
	   open a new tab to open the image.

Demo Overriding the IE Standard Context Menu.
 
Step6. Open 32bit IE and click Tools=>Manage Add-ons, and enable 
       "CSCustomIEContextMenu.BHOIEContextMenu". You may have to restart IE to make it 
	   take effect. 

Step7. Visit http://www.microsoft.com/. Right click an image on the web page, in the context
       menu, you will only see one item called "Open image in new tab". Click this item, and IE will
	   open a new tab to open the image.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Create the project and add references

In Visual Studio 2010, create a Visual C# / Windows / Class Library project 
named "CSCustomIEContextMenu". 

Right click the project in Solution Explorer and choose "Add Reference". Add
"Microsoft HTML Object Library" and "Microsoft Internet Controls" in COM tab.
-----------------------------------------------------------------------------

B. Add Entries to the IE Standard Context Menu.

To add a entry to IE Standard Context Menu, you can add a key under 
"HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\MenuExt\".  The default 
value of the key is the html file path that will handle the event. See 
http://msdn.microsoft.com/en-us/library/aa753589(VS.85).aspx for detailed information.

The class OpenImageMenuExt supplies 2 methods to add / remove entries in the registry.
And the html page Resource\OpenImage.htm is used to open a image in a new tab.

-----------------------------------------------------------------------------

C. Override the IE Standard Context Menu using Browser Helper Object.

1. The class OpenImageHandler implements the interface IDocHostUIHandler, and it will
   override the default context menu.

2. The class OpenImageBHO is a Browser Helper Object that will set the OpenImageHandler
   as the UIHandler of the html document. For how to create / deploy BHO, see the sample
   "CSBrowserHelperObject" in Microsoft All-In-One Code Framework.   

-----------------------------------------------------------------------------

D. Deploying the custom context menu with a setup project.

  (1) In CSCustomIEContextMenu, add an Installer class (named CustomIEContextMenuInstaller 
      in this code sample) to define the custom actions in the setup. The class derives from
      System.Configuration.Install.Installer. We use the custom actions to add/remove the IE 
	  Context Menu entries in registry and register/unregister the COM-visible classes in
	  the current managed assembly when user installs/uninstalls the component. 

    [RunInstaller(true), ComVisible(false)]
    public partial class CustomIEContextMenuInstaller : System.Configuration.Install.Installer
    {
        public CustomIEContextMenuInstaller()
        {
            InitializeComponent();
        }
  
        public override void Install(System.Collections.IDictionary stateSaver)
        {
            base.Install(stateSaver);

            OpenImageMenuExt.RegisterMenuExt();

            RegistrationServices regsrv = new RegistrationServices();
            if (!regsrv.RegisterAssembly(this.GetType().Assembly,
            AssemblyRegistrationFlags.SetCodeBase))
            {
                throw new InstallException("Failed To Register for COM");
            }
        }

        public override void Uninstall(System.Collections.IDictionary savedState)
        {
            base.Uninstall(savedState);

            OpenImageMenuExt.UnRegisterMenuExt();

            RegistrationServices regsrv = new RegistrationServices();
            if (!regsrv.UnregisterAssembly(this.GetType().Assembly))
            {
                throw new InstallException("Failed To Unregister for COM");
            }
        }
    }


  (2) To add a deployment project, on the File menu, point to Add, and then 
  click New Project. In the Add New Project dialog box, expand the Other 
  Project Types node, expand the Setup and Deployment Projects, click Visual 
  Studio Installer, and then click Setup Project. In the Name box, type 
  CSCustomIEContextMenuSetup(x86). Click OK to create the project. 
  In the Properties dialog of the setup project, make sure that the 
  TargetPlatform property is set to x86. This setup project is to be deployed 
  for 32bit IE on x86 or x64 Windows operating systems. 

  Right-click the setup project, and choose Add / Project Output ... 
  In the Add Project Output Group dialog box, CSCustomIEContextMenu will  
  be displayed in the Project list. Select Primary Output and click OK. VS
  will detect the dependencies of the CSCustomIEContextMenu, including .NET
  Framework 4.0 (Client Profile).

  Right-click the setup project, and choose View / Custom Actions. 
  In the Custom Actions Editor, right-click the root Custom Actions node. On 
  the Action menu, click Add Custom Action. In the Select Item in Project 
  dialog box, double-click the Application Folder. Select Primary output from 
  CSCustomIEContextMenu. This adds the custom actions that we defined 
  in BHOInstaller of CSCustomIEContextMenu. 
  
  Right-click the setup project again, and choose View / File System. In the 
  Application Folder, add a folder named "Resource", and add OpenImage.htm to 
  this folder.

  Build the setup project. If the build succeeds, you will get a .msi file 
  and a Setup.exe file. You can distribute them to your users to install or 
  uninstall this BHO. 

  (3) To deploy the BHO for 64bit IE on a x64 operating system, you 
  must create a new setup project (e.g. CSCustomIEContextMenuSetup(x64) 
  in this code sample), and set its TargetPlatform property to x64. 

  Although the TargetPlatform property is set to x64, the native shim 
  packaged with the .msi file is still a 32-bit executable. The Visual Studio 
  embeds the 32-bit version of InstallUtilLib.dll into the Binary table as 
  InstallUtil. So the custom actions will be run in the 32-bit, which is 
  unexpected in this code sample. To workaround this issue and ensure that 
  the custom actions run in the 64-bit mode, you either need to import the 
  appropriate bitness of InstallUtilLib.dll into the Binary table for the 
  InstallUtil record or - if you do have or will have 32-bit managed custom 
  actions add it as a new record in the Binary table and adjust the 
  CustomAction table to use the 64-bit Binary table record for 64-bit managed 
  custom actions. This blog article introduces how to do it manually with 
  Orca http://blogs.msdn.com/b/heaths/archive/2006/02/01/64-bit-managed-custom-actions-with-visual-studio.aspx

  In this code sample, we automate the modification of InstallUtil by using a 
  post-build javascript: Fix64bitInstallUtilLib.js. You can find the script 
  file in the CSCustomIEContextMenuSetup(x64) project folder. To 
  configure the script to run in the post-build event, you select the 
  CSCustomIEContextMenuSetup(x64) project in Solution Explorer, and 
  find the PostBuildEvent property in the Properties window. Specify its 
  value to be 
  
	"$(ProjectDir)Fix64bitInstallUtilLib.js" "$(BuiltOuputPath)" "$(ProjectDir)"

  Repeat the rest steps in (2) to add the project output, set the custom 
  actions, configure the prerequisites, and build the setup project.


/////////////////////////////////////////////////////////////////////////////
Diagnostic:

To debug IE Context Menu, you can attach to iexplorer.exe. 


/////////////////////////////////////////////////////////////////////////////
References:

Adding Entries to the Standard Context Menu
http://msdn.microsoft.com/en-us/library/aa753589(VS.85).aspx

Context Menus and Extensions
http://msdn.microsoft.com/en-us/library/aa770042(VS.85).aspx#wbc_ctxmenus

Browser Helper Objects: The Browser the Way You Want It
http://msdn.microsoft.com/en-us/library/ms976373.aspx


/////////////////////////////////////////////////////////////////////////////


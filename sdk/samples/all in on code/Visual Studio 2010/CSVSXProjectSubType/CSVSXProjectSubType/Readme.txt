================================================================================
        Visual Studio Package Project: CSVSXProjectSubType                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

 A Project SubType, or called ProjectFlavor, let you customize or flavor the 
 behavior of the project systems of Visual Studio. Customizations include 
 1. Saving additional data in the project file.
 2. Adding or filtering items in the Add New Item dialog box.
 3. Controlling how assemblies are debugged and deployed.
 4. Extending the project Property Pages dialog box.

 In this sample, we demonstrate how to create a Project SubType with following 
 features:
 1. Removing the Services Property Page.
 2. Adding a custom Property Page.
 3. Saving the data on the custom Property Page to project file.

For more detailed information about Project SubTypes, please check 
http://msdn.microsoft.com/en-us/library/bb166488.aspx

/////////////////////////////////////////////////////////////////////////////
How the Project SubTypes Work:

First, we have to register our CustomPropertyPageProjectFactory to Visual Studio.

Second, we need a Project Template, which is created by the CSVSXProjectSubTypeTemplate
project.

The ProjectTemplate.csproj in CSVSXProjectSubTypeTemplate contains following script 
    <ProjectTypeGuids>
        {3C53C28F-DC44-46B0-8B85-0C96B85B2042};
        {FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}
    </ProjectTypeGuids>

    {3C53C28F-DC44-46B0-8B85-0C96B85B2042} is the Guid of the CustomPropertyPageProjectFactory.
    {FAE04EC0-301F-11D3-BF4B-00C04F79EFBC} means CSharp project. 

At last, When Visual Studio is creating or opening a CSharp project with above ProjectTypeGuids,
1. The environment calls the base project (CSharp Project)'s CreateProject, and while the 
   project parses its project file it discovers that the aggregate project type GUIDs list
   is not null. The project discontinues directly creating its project.

2. If there are multiple project type GUIDs, the environment makes recursive function calls to 
   your implementations of PreCreateForOuter, 
   Microsoft.VisualStudio.Shell.Interop.IVsAggregatableProject.SetInnerProject(System.Object) 
   and InitializeForOuter methods while it is walking the list of project type GUIDs, 
   starting with the outermost project subtype.

3. In the PreCreateForOuter method of the ProjectFactory, we can return our ProjectFlavor object,
   which can customize the Property Page. 

/////////////////////////////////////////////////////////////////////////////
Prerequisite

Visual Studio 2010 Professional or Visual Studio 2010 Ultimate. Visual Studio 2010 SDK.


////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this solution in Visual Studio 2010 Professional or better. 

Step2. Set the CSVSXProjectSubTypeVSIX as the StartUp Project, and open its property pages. 

       1. Select the Debug tab. Set the Start Option to Start external program and browse 
          the devenv.exe (The default location is C:\Program Files\Microsoft Visual Studio
          10.0\Common7\IDE\devenv.exe), and add "/rootsuffix Exp" (no quote) to the Command
          line arguments.
       
       2. Select the VSIX tab, make sure "Create VSIX Container during build" and 
          "Deploy VSIX content to Experimental Instance for debugging" are checked.
        
Step3. Build the solution.  

Step4. Press F5, and the Experimental Instance of Microsoft Visual Studio 2010 will 
       be launched.
       
       In the VS Experimental Instance, click Tool=>Extension Manager, you will find 
       CSVSXProjectSubTypeVSIX is loaded.

Step5. In the VS Experimental Instance, click File=>New=>Project. In the "New Project"
       dialog, you will find "CSVSXProjectSubTypeTemplate" in the Visual C# templates.

       Use the CSVSXProjectSubTypeTemplate to create a new project, for example, 
       CSVSXProjectSubTypeTemplate1.
	     
Step6. Open the property page of CSVSXProjectSubTypeTemplate1, you will find that 
       1. The "Service" Property Page is removed.
       2. A new "Custom" Property Page is added.
	   
Step7. Select the "Custom" Property Page of CSVSXProjectSubTypeTemplate1, you will 
       see 3 controls: a Label, a TextBox and a CheckBox. 
       
       Type some text in the TextBox and check the CheckBox, save it. And then open 
       CSVSXProjectSubTypeTemplate1.csproj with NotePad, you will find the text that
       you typed.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Create a CSharp Project Template.

   VS SDK supplies a Project Template called "C# Project Template", with it you can create a 
   CSharp Project Template. For more detailed steps, see http://msdn.microsoft.com/en-us/library/dd885241.aspx

   Then open the ProjectTemplate.csproj in the CSVSXProjectSubTypeTemplate project, add 
   ProjectTypeGuids property to <Project><PropertyGroup> element.

       <?xml version="1.0" encoding="utf-8"?>
       <Project ToolsVersion="4.0" DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
	      <PropertyGroup>
	    	 ...
             <ProjectTypeGuids>{3C53C28F-DC44-46B0-8B85-0C96B85B2042};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}</ProjectTypeGuids>
             ...
   
   {3C53C28F-DC44-46B0-8B85-0C96B85B2042} is the Guid of our Project Factory.
   {FAE04EC0-301F-11D3-BF4B-00C04F79EFBC} means CSharp project. 


B. Create a VSPackage project to Create and Register our Project Factory.

   Follow the steps in http://msdn.microsoft.com/en-us/library/bb164725.aspx to create
   an Empty VS Package, NOT to check the "Menu Command" option in step5, and skip the
   step6.

   1. The code files under the PropertyPageBase folder are from VS2005 SDK of April 2006.

      A PropertyPage object contains a PropertyStore object which stores the Properties,
      and a PageView object which is a UserControl used to display the Properties.

      The PropertyControlTable class stores the Control / Property Name KeyValuePairs, 
      and The PropertyControlMap class is used to initialize the Controls on a PageView
      Object by the PropertyPage object. 

   2. The CustomPropertyPage, CustomPropertyPagePropertyStore and CustomPropertyPageView
      classed under the ProjectFlavor folder inherit the classes or implement the 
      interfaces of the PropertyPageBase.
      
   3. The CustomPropertyPageProjectFlavor class is our ProjectFlavor, and the 
      CustomPropertyPageProjectFlavorCfg class give the project subtype access to various 
      configuration interfaces.

      By overriding GetProperty method and using propId parameter containing one of 
      the values of the __VSHPROPID2 enumeration, we can filter, add or remove project
      properties. 
      
      For example, to add a page to the configuration-dependent property pages, we
      need to filter configuration-dependent property pages and then add a new page 
      to the existing list. 


            protected override int GetProperty(uint itemId, int propId, out object property)
            {
                if (propId == (int)__VSHPROPID2.VSHPROPID_CfgPropertyPagesCLSIDList)
                {
                    // Get a semicolon-delimited list of clsids of the configuration-dependent
                    // property pages.
                    ErrorHandler.ThrowOnFailure(base.GetProperty(itemId, propId, out property));
            
                    // Add the CustomPropertyPage property page.
                    property += ';' + typeof(CustomPropertyPage).GUID.ToString("B");
            
                    return VSConstants.S_OK;
                }
            
                if (propId == (int)__VSHPROPID2.VSHPROPID_PropertyPagesCLSIDList)
                {
            
                    // Get the list of priority page guids from the base project system.
                    ErrorHandler.ThrowOnFailure(base.GetProperty(itemId, propId, out property));
                    string pageList = (string)property;
            
                    // Remove the Services page from the project designer.
                    string servicesPageGuidString = "{43E38D2E-43B8-4204-8225-9357316137A4}";
            
                    RemoveFromCLSIDList(ref pageList, servicesPageGuidString);
                    property = pageList;
                    return VSConstants.S_OK;
                }
            
                return base.GetProperty(itemId, propId, out property);
            }
   
   4. The CustomPropertyPageProjectFactory class  is the project factory for our project flavor.
      You can read the "How the Project SubTypes Work:" section to learn the code logic of this
      Project Factory.

           #region IVsAggregatableProjectFactory
            
            /// <summary>
            /// Create an instance of CustomPropertyPageProjectFlavor. 
            /// The initialization will be done later when Visual Studio calls
            /// InitalizeForOuter on it.
            /// </summary>
            /// <param name="outerProjectIUnknown">
            /// This value points to the outer project. It is useful if there is a 
            /// Project SubType of this Project SubType.
            /// </param>
            /// <returns>
            /// An CustomPropertyPageProjectFlavor instance that has not been initialized.
            /// </returns>
            protected override object PreCreateForOuter(IntPtr outerProjectIUnknown)
            {
                CustomPropertyPageProjectFlavor newProject = new CustomPropertyPageProjectFlavor();
                newProject.package = this.package;
                return newProject;
            }
          
            #endregion


   5. The VSXProjectSubTypePackage class is a VSPackage that registers our Project Factory 
      and Property Page.

           [PackageRegistration(UseManagedResourcesOnly = true)]
           // Register the PropertyPage.
           [ProvideObject(
               typeof(CustomPropertyPage),
               RegisterUsing = RegistrationMethod.CodeBase)]
           // Register the project (note that we do not specify the extension as we use the
           // same one as the base project).
           [ProvideProjectFactory(typeof(CustomPropertyPageProjectFactory),
               "Task Project",
               null,
               null,
               null,
               @"..\Templates\Projects")]
           [Guid(GuidList.guidCSVSXProjectSubTypePkgString)]
           public sealed class VSXProjectSubTypePackage : Package
           {
               ...
          
               protected override void Initialize()
               {
                   Trace.WriteLine(string.Format("Entering Initialize() of: {0}",
                       this.ToString()));
                   base.Initialize();
                   this.RegisterProjectFactory(new CustomPropertyPageProjectFactory(this));
               }
               
               ...
          
           }

C. Create a VSIX project to package up the VSPackage and Project Template. 
   
   Follow the steps in http://msdn.microsoft.com/en-us/library/dd393742.aspx to create an 
   empty VSIX project.

   Double click source.extension.vsixmanifest of CSVSXProjectSubTypeVSIX to open it. Click
   "Add Content" button to add the VSPackage of the CSVSXProjectSubType project, and 
   Project Template of the CSVSXProjectSubTypeTemplate.



   

/////////////////////////////////////////////////////////////////////////////
References:

Project Subtypes
http://msdn.microsoft.com/en-us/library/bb166488.aspx

IPropertyPage Interface
http://msdn.microsoft.com/en-us/library/ms691246(VS.85).aspx

FlavoredProjectFactoryBase Class
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.flavor.flavoredprojectfactorybase.aspx

FlavoredProjectBase Class
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.flavor.flavoredprojectbase.aspx

IVsProjectFlavorCfgProvider Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.ivsprojectflavorcfgprovider.aspx

IVsProjectFlavorCfg Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.ivsprojectflavorcfg.aspx

IPersistXMLFragment Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.ipersistxmlfragment.aspx

VSIX Deployment
http://msdn.microsoft.com/en-us/library/ff363239.aspx
/////////////////////////////////////////////////////////////////////////////
=============================================================================
            VSX Module: CSVSPackageToolbox Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VSPackages are software modules that make up and extend the Visual Studio 
integrated development environment (IDE) by providing UI elements, services, 
projects, editors, and designers. VSPackages are the principal architectural 
unit of Visual Studio, and are the unit of deployment, licensing, and security 
also. Visual Studio itself is written mostly as a collection of VSPackages. 
This sample demonstrate how to use the the Visual Studio Integration Package 
Wizard to create a simple VSPackage and automatically load all the ToolboxItem 
items provided by its own assembly.


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
	   Then click Next.The new command is put at the top of the Tools menu.

	6. Type the name(InitializeToolbox) and ID(cmdidInitializeToolbox) for the new 
	   menu command in the text boxes, then click Next. The command ID is the name
	   of a constant that represents this menu command in the generated code.

	7. Uncheck the Intergration Test Project and Unit Test Project, then click
	   Finish.
	 	  
B: Creating a Toolbox Control

	1. Right-Click the Project and select "Add"-> "Add new Item", then select 
	   UserControl, type name "MyControl"
	
	2. In the UserControl designer, add a TextBox and a Button, double-click the 
	   Button, add the click event handler.
	   
	    private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog opg = new OpenFileDialog();
            opg.Multiselect = false;
            if (opg.ShowDialog() == DialogResult.OK)
                this.textBox1.Text = opg.FileName;
        }

	3. Add attributes to the file to enable the VSPackage to query the supplied 
	ToolboxItem class:
	
		// Set the display name and custom bitmap to use for this item.
		// The build action for the bitmap must be "Embedded Resource".
		[DisplayName("ToolboxMemberDemo")]
		[Description("Custom toolbox item from package LoadToolboxMembers.")]
		[ToolboxItem(true)]
		[ToolboxBitmap(typeof(MyControl), "MyControl.bmp")]
		public partial class MyControl : UserControl
		{
			......
		}
		
	4. Save the file. 

	5. Right-Click the Project and select "Add"-> "Add new Item", then select 
	   Bitmap add a 16*16 bitmap with the name MyControl.bmp
	   
C: Modifying the VSPackage Implementation

	1. Open the CSVSToolboxPackage.cs and add the following namespace directives  
	
		using System.Collections;
		using System.ComponentModel;
		using System.Drawing.Design;
		using System.Reflection;
		
	2. Register the VSPackage as a ToolboxItem class by adding an instance of 
	   ProvideToolboxItemsAttribute
	   
	       [ProvideToolboxItems(1)]
           public sealed class CSVSToolboxPackage : Package
           {
               ......
           }
	
    3. Add the following two new private fields to the CSVSToolboxPackage class: 

            // List for the toolbox items provided by this package.
            private ArrayList ToolboxItemList;

			// Name for the Toolbox category tab for the package's toolbox items.
			private string CategoryTab = "LoadToolboxMemberDemo";

	4. Modify the Initialize method in CSVSToolboxPackage to do the following 
	   things: 

            // Add our command handlers for menu (commands must exist in the .vsct file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                // Create the command for the menu item.
                CommandID menuCommandID = new CommandID(GuidList.guidCSVSToolboxCmdSet, 
                (int)PkgCmdIDList.cmdidInitializeToolbox);
                MenuCommand menuItem = new MenuCommand(MenuItemCallback, menuCommandID );
                mcs.AddCommand( menuItem );

                // Subscribe to the toolbox intitialized and upgraded events.
                ToolboxInitialized += new EventHandler(OnRefreshToolbox);
                ToolboxUpgraded += new EventHandler(OnRefreshToolbox);
            }

            // Use reflection to get the toolbox items provided in this assembly.
            ToolboxItemList = CreateItemList(this.GetType().Assembly);
            if (null == ToolboxItemList)
            {
                // Unable to generate the list.
                // Add error handling code here.
            }
	
    5. Add two methods, CreateItemList and CreateToolboxItem, to construct, by 
       using metadata, instances of the ToolboxItem objects that are available 
       in the CSVSToolbox assembly, as follows: 

        // Scan for toolbox items in the assembly and return the list of
        // toolbox items.
        private ArrayList CreateItemList(Assembly assembly)
        {
            ArrayList list = new ArrayList();
            foreach (Type possibleItem in assembly.GetTypes())
            {
                ToolboxItem item = CreateToolboxItem(possibleItem);
                if (item != null)
                {
                    list.Add(item);
                }
            }
            return list;
        }

        // If the type represents a toolbox item, return an instance of the type;
        // otherwise, return null.
        private ToolboxItem CreateToolboxItem(Type possibleItem)
        {
            // A toolbox item must implement IComponent and must not be abstract.
            if (!typeof(IComponent).IsAssignableFrom(possibleItem) ||
                possibleItem.IsAbstract)
            {
                return null;
            }

            // A toolbox item must have a constructor that takes a parameter of
            // type Type or a constructor that takes no parameters.
            if (null == possibleItem.GetConstructor(new Type[] { typeof(Type) }) &&
                null == possibleItem.GetConstructor(new Type[0]))
            {
                return null;
            }

            ToolboxItem item = null;

            // Check the custom attributes of the candidate type and attempt to
            // create an instance of the toolbox item type.
            AttributeCollection attribs =
                TypeDescriptor.GetAttributes(possibleItem);
            ToolboxItemAttribute tba =
                attribs[typeof(ToolboxItemAttribute)] as ToolboxItemAttribute;
            if (tba != null && !tba.Equals(ToolboxItemAttribute.None))
            {
                if (!tba.IsDefaultAttribute())
                {
                    // This type represents a custom toolbox item implementation.
                    Type itemType = tba.ToolboxItemType;
                    ConstructorInfo ctor =
                        itemType.GetConstructor(new Type[] { typeof(Type) });
                    if (ctor != null && itemType != null)
                    {
                        item = (ToolboxItem)ctor.Invoke(new object[] { possibleItem });
                    }
                    else
                    {
                        ctor = itemType.GetConstructor(new Type[0]);
                        if (ctor != null)
                        {
                            item = (ToolboxItem)ctor.Invoke(new object[0]);
                            item.Initialize(possibleItem);
                        }
                    }
                }
                else
                {
                    // This type represents a default toolbox item.
                    item = new ToolboxItem(possibleItem);
                }
            }
            if (item == null)
            {
                throw new ApplicationException("Unable to create a ToolboxItem " +
                    "object from " + possibleItem.FullName + ".");
            }

            // Update the display name of the toolbox item and add the item to
            // the list.
            DisplayNameAttribute displayName =
                attribs[typeof(DisplayNameAttribute)] as DisplayNameAttribute;
            if (displayName != null && !displayName.IsDefaultAttribute())
            {
                item.DisplayName = displayName.DisplayName;
            }

            return item;
        }
        
	6. Implement the OnRefreshToolbox method to handle the ToolboxInitialized 
	   and ToolboxUpgraded events. 
	   
	    void OnRefreshToolbox(object sender, EventArgs e)
        {
            // Add new instances of all ToolboxItems contained in ToolboxItemList.
            IToolboxService service =
                GetService(typeof(IToolboxService)) as IToolboxService;
            IVsToolbox toolbox = GetService(typeof(IVsToolbox)) as IVsToolbox;

            //Remove target tab and all controls under it.
            foreach (ToolboxItem oldItem in service.GetToolboxItems(CategoryTab))
            {
                service.RemoveToolboxItem(oldItem);
            }
            toolbox.RemoveTab(CategoryTab);

            foreach (ToolboxItem itemFromList in ToolboxItemList)
            {
                service.AddToolboxItem(itemFromList, CategoryTab);
            }
            service.SelectedCategory = CategoryTab;

            service.Refresh();
        }
	
	7. Change the menu item command handler method, MenuItemCallBack, as follows. 
	
	    private void MenuItemCallback(object sender, EventArgs e)
        {
            IVsPackage pkg = GetService(typeof(Package)) as Package;
            pkg.ResetDefaults((uint)__VSPKGRESETFLAGS.PKGRF_TOOLBOXITEMS);
        }
	
	8. Save the file.
	
E: Testing the Toolbox

    1. Press F5 to open a new instance of the Visual Studio experimental build.
       
    2. Create a new Windows Form project.
    
    3. Click the "Tools" -> "InitializeToolbox".
    
    4. Drag and drop the Control to the Form and press F5 to test.

    
/////////////////////////////////////////////////////////////////////////////
References:

How to: Create VSPackages (C# and Visual Basic)
http://msdn.microsoft.com/en-us/library/bb164725.aspx

How to: Register a VSPackage (C#)
http://msdn.microsoft.com/en-us/library/bb166544.aspx

VSPackage Tutorial 1: How to Create a VSPackage
http://msdn.microsoft.com/en-us/library/cc138589.aspx

Designing XML Command Table (.Vsct) Files
http://msdn.microsoft.com/en-us/library/bb166366.aspx

Managing the Toolbox
http://msdn.microsoft.com/en-us/library/bb165325.aspx

Toolbox (Visual Studio SDK)
http://msdn.microsoft.com/en-us/library/bb166364.aspx

Walkthrough: Autoloading Toolbox Items
http://msdn.microsoft.com/en-us/library/bb166364.aspx

Walkthrough: Adding a Menu Controller to a Toolbar
http://msdn.microsoft.com/en-us/library/bb165748.aspx

Creating a package with a simple command
http://dotneteers.net/blogs/divedeeper/archive/2008/01/06/LearnVSXNowPart3.aspx

Menus and comands in VS IDE
http://dotneteers.net/blogs/divedeeper/archive/2008/02/22/LearnVSXNowPart13.aspx


/////////////////////////////////////////////////////////////////////////////

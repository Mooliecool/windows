=============================================================================
            CSVSToolBars Module: CSVSToolBars Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VSPackages are software modules that make up and extend the Visual Studio 
integrated development environment (IDE) by providing UI elements, services, 
projects, editors, and designers. VSPackages are the principal architectural 
unit of Visual Studio, and are the unit of deployment, licensing, and security 
also. Visual Studio itself is written mostly as a collection of VSPackages. 
This sample demonstrate how to use the the Visual Studio Integration Package 
Wizard to create a simple VSPackage with a toolbar.


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

	5. Select the Menu Command option to create a new command for the VSPackage. Then 
	   click Next.

	6. Type the name(All-In-One) and ID(cmdidMyCommand) for the new 
	   menu command in the text boxes, then click Next. The command ID is the name
	   of a constant that represents this menu command in the generated code.
	   
	7. Uncheck the Intergration Test Project and Unit Test Project, then click
	   Finish.
	
B: Add a Toolbar to VS IDE

	1. In Solution Explorer, open CSVSToolbars.vsct.
	   VSCT file stands for Visual Studio Command Table. This is an XML based file that 
	   describes the layout and appearance of command items for a VSPackage. Command 
	   items include buttons, combo boxes, menus, toolbars, and groups of command items.
	
	2. In the <Symbols> section, find the <GuidSymbol> node whose name attribute is 
	   guidCSVSToolbarsCmdSet. Add the following two <IDSymbol> elements to the list of 
	   <IDSymbol> elements in this node to define a toolbar and a toolbar group. 
 
	   <IDSymbol name="ToolbarID" value="0x1000" />
       <IDSymbol name="ToolbarGroupID" value="0x1001" />
	
	3. Just above the <Groups> section, create an empty <Menus> section, and create the 
	   following <Menu> element to define the toolbar that you declared in step 2.
	   
	   <Menus>
        <Menu guid="guidCSVSToolbarsCmdSet" id="ToolbarID"
              priority="0x0000" type="Toolbar">
          <Parent guid="guidCSVSToolbarsCmdSet" id="ToolbarID" />
          <Strings>
            <ButtonText>My Toolbar</ButtonText>
            <CommandName>My Toolbar</CommandName>
          </Strings>
        </Menu>
       </Menus> 
	   
    4. Add a new <Group> element to the <Groups> section to define the group that you 
       declared in the <Symbols> section. 
       
       <Group guid="guidCSVSToolbarsCmdSet" id="ToolbarGroupID"
              priority="0x0000">
         <Parent guid="guidCSVSToolbarsCmdSet" id="ToolbarID"/>
       </Group>
      
    5. In the Buttons Element, change the parent of the existing Button Elementto the 
       toolbar group so that the toolbar will be displayed. 
          
       <Parent guid="guidCSVSToolbarsCmdSet" id="ToolbarGroupID" />
      
    6. Save CSVSToolbars.vsct.
	
	
C: Add a icon to the command:

    1. Create a bitmap with a color depth of 32-bits. An icon is always 16 x 16 so this
       bitmap must be 16 pixels high and a multiple of 16 pixels wide.In this sample, it
       is camera.png, which is a 32bits png image.      
     
    2. Add the bitmap to the resource of this project
    
    3. Open the .vsct file in the editor. 
       
    4. In the Symbols Element, find the GuidSymbol Element that contains your existing 
       bitmap entries. By default, it is named guidImages. And add new Symbols Element next
       to the existing one.

			<GuidSymbol name="guidMyImages" value="{4056cc26-4a2f-432c-a816-e6694e673abb}">
			  <IDSymbol name="bmpMyPic" value="1" />
			</GuidSymbol>
           
    5. Add an IDSymbol Element for each icon in your bitmap. The name attribute is the 
       icon's ID, and the value indicates its position on the strip. 
       
       		  <IDSymbol name="bmpMyPic" value="1" />
       
    6. Create a Bitmap Element in the <Bitmaps> section of the .vsct file to represent the 
       bitmap containing the icons. 
        
            <Bitmap guid="guidMyImages" href="Resources\camera.png"/>
            
    7.Change the Toolbar button to use the new icon
    
            <Icon guid="guidMyImages" id="bmpMyPic" />
           
    8. Save all the files.
  
    
 D: Add a menu controller
    
    1. Open the .vsct file in the editor. 
    
    2. In the Symbols Element, in the GuidSymbol Element named "guidCSVSToolbarsCmdSet", 
       declare your menu controller, menu controller group, and three menu items
       
          <IDSymbol name="TestMenuController" value="0x1300"/>
		  <IDSymbol name="TestMenuControllerGroup" value="0x1060"/>
		  <IDSymbol name="cmdidMCItem1" value="0x0130"/>
		  <IDSymbol name="cmdidMCItem2" value="0x0131"/>
		  <IDSymbol name="cmdidMCItem3" value="0x0132"/>
		  
	3.In Menus Element,after the last menu entry, define the menu controller as a menu.
	
		  <Menu guid="guidCSVSToolbarsCmdSet" id="TestMenuController"
			  priority="0x0100" type="MenuController">
			<Parent guid="guidCSVSToolbarsCmdSet" id="ToolbarGroupID"/>
			<CommandFlag>IconAndText</CommandFlag>
			<CommandFlag>TextChanges</CommandFlag>
			<CommandFlag>TextIsAnchorCommand</CommandFlag>
			<Strings>
			  <ButtonText>Test Menu Controller</ButtonText>
			  <CommandName>Test Menu Controller</CommandName>
			</Strings>
		  </Menu>
	
	4. In the Groups Element, after the last group entry, add the menu controller group
	
		  <Group guid="guidCSVSToolbarsCmdSet" id="TestMenuControllerGroup"
		   priority="0x000">
			<Parent guid="guidCSVSToolbarsCmdSet" id="TestMenuController"/>
		  </Group>
		  
	5. In the Buttons Element, after the last button entry, add a Button Element for each of 
	   your menu items.
	   
		  <Button guid="guidCSVSToolbarsCmdSet" id="cmdidMCItem1"
		  priority="0x0000" type="Button">
			<Parent guid="guidCSVSToolbarsCmdSet" id="TestMenuControllerGroup"/>
			<Icon guid="guidImages" id="bmpPic1"/>
			<CommandFlag>IconAndText</CommandFlag>
			<Strings>
			  <ButtonText>MC Item 1</ButtonText>
			  <CommandName>MC Item 1</CommandName>
			</Strings>
		  </Button>
		  <Button guid="guidCSVSToolbarsCmdSet" id="cmdidMCItem2"
				priority="0x0100" type="Button">
			<Parent guid="guidCSVSToolbarsCmdSet" id="TestMenuControllerGroup"/>
			<Icon guid="guidImages" id="bmpPic2"/>
			<CommandFlag>IconAndText</CommandFlag>
			<Strings>
			  <ButtonText>MC Item 2</ButtonText>
			  <CommandName>MC Item 2</CommandName>
			</Strings>
		  </Button>
		  <Button guid="guidCSVSToolbarsCmdSet" id="cmdidMCItem3"
				priority="0x0200" type="Button">
			<Parent guid="guidCSVSToolbarsCmdSet" id="TestMenuControllerGroup"/>
			<Icon guid="guidImages" id="bmpPicSearch"/>
			<CommandFlag>IconAndText</CommandFlag>
			<Strings>
			  <ButtonText>MC Item 3</ButtonText>
			  <CommandName>MC Item 3</CommandName>
			</Strings>
		  </Button>	
	
	6. Save the .vsct file.
	
 E: Implement the menu controller
    
    1. Open PkgCmdID.cs and add the following lines 
    
        public const int cmdidMCItem1 = 0x130;
        public const int cmdidMCItem2 = 0x131;
        public const int cmdidMCItem3 = 0x132;
    
    2. At the top of the CSVSToolbarsPackage class, add the following:
       
        private int currentMCCommand; // The currently selected menu controller command
		  
	3. In the Initialize method, immediately after the last call to the AddCommand method, 
	   add code to route the events for each command through the same handlers.
	
                for (int i = PkgCmdIDList.cmdidMCItem1; i <=
                   PkgCmdIDList.cmdidMCItem3; i++)
                {
                    CommandID cmdID = new
                    CommandID(GuidList.guidCSVSToolbarsCmdSet, i);
                    OleMenuCommand mc = new OleMenuCommand(new
                      EventHandler(OnMCItemClicked), cmdID);
                    mc.BeforeQueryStatus += new EventHandler(OnMCItemQueryStatus);
                    mcs.AddCommand(mc);
                    // The first item is, by default, checked.
                    if (PkgCmdIDList.cmdidMCItem1 == i)
                    {
                        mc.Checked = true;
                        this.currentMCCommand = i;
                    }
                }
	
	4. Add the following two event handler defined above
	
	    private void OnMCItemClicked(object sender, EventArgs e)
        {
            OleMenuCommand mc = sender as OleMenuCommand;
            if (null != mc)
            {
                string selection;
                switch (mc.CommandID.ID)
                {
                    case PkgCmdIDList.cmdidMCItem1:
                        selection = "Menu controller Item 1";
                        break;

                    case PkgCmdIDList.cmdidMCItem2:
                        selection = "Menu controller Item 2";
                        break;

                    case PkgCmdIDList.cmdidMCItem3:
                        selection = "Menu controller Item 3";
                        break;

                    default:
                        selection = "Unknown command";
                        break;
                }
                this.currentMCCommand = mc.CommandID.ID;

                IVsUIShell uiShell =
                  (IVsUIShell)GetService(typeof(SVsUIShell));
                Guid clsid = Guid.Empty;
                int result;
                uiShell.ShowMessageBox(
                           0,
                           ref clsid,
                           "Test Tool Window Toolbar Package",
                           string.Format(CultureInfo.CurrentCulture,
                                         "You selected {0}", selection),
                           string.Empty,
                           0,
                           OLEMSGBUTTON.OLEMSGBUTTON_OK,
                           OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST,
                           OLEMSGICON.OLEMSGICON_INFO,
                           0,
                           out result);
            }
        }
	
	5. Save the all files.
    
 E: Testing the Tool Window

    1. Press F5 to open a new instance of the Visual Studio experimental build.
       
    2. On the View menu, point to Toolbars and then click My Toolbar.
    
    3. Click the Load File button on the Tool Window and Select a media file.

    
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

Commands Element
http://msdn.microsoft.com/en-us/library/bb165399.aspx

KeyBindings Element
http://msdn.microsoft.com/en-us/library/bb165085.aspx

Walkthrough: Adding a Toolbar to the IDE 
http://msdn.microsoft.com/en-us/library/bb164715.aspx

How to: Add Icons to Commands on Toolbars
http://msdn.microsoft.com/en-us/library/bb165158.aspx

How to: Add Menu Controllers to Toolbars
http://msdn.microsoft.com/en-us/library/bb166443.aspx

Walkthrough: Adding a Menu Controller to a Toolbar
http://msdn.microsoft.com/en-us/library/bb165748.aspx

Creating a package with a simple command
http://dotneteers.net/blogs/divedeeper/archive/2008/01/06/LearnVSXNowPart3.aspx

Menus and comands in VS IDE
http://dotneteers.net/blogs/divedeeper/archive/2008/02/22/LearnVSXNowPart13.aspx


/////////////////////////////////////////////////////////////////////////////


          
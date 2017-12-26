=============================================================================
            CSVSToolWindow Module: CSVSToolWindow Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VSPackages are software modules that make up and extend the Visual Studio 
integrated development environment (IDE) by providing UI elements, services, 
projects, editors, and designers. VSPackages are the principal architectural 
unit of Visual Studio, and are the unit of deployment, licensing, and security 
also. Visual Studio itself is written mostly as a collection of VSPackages. 
This sample demonstrate how to use the the Visual Studio Integration Package 
Wizard to create a simple VSPackage with a tool window that has a Windows Media
control on it.


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

	5. Select the Menu Command option to create a new command for the VSPackage and
	   select Tool Window to create a tool window in this VSPackage. Then click Next. 
	   The new command is put at the top of the Tools menu.

	6. Type the name(All-In-One) and ID(cmdidToolWindow) for the new 
	   menu command in the text boxes, then click Next. The command ID is the name
	   of a constant that represents this menu command in the generated code.
	   
	7. Type the name(My Tool Window) and ID(cmdidMyTool) for new tool window in the 
	   text boxes, and click Next.

	8. Uncheck the Intergration Test Project and Unit Test Project, then click
	   Finish.

B: Customize the ToolWindow

	1. In Solution Explorer, MyControls.cs in the WinForm Designer. 
	
	2. Delete the ClickMe button and add the Windows Media control to MyControls. 
	
	3. Set the Dock property to Fill.
	
	
C: Add a Toolbar to the Tool Window

	1. In Solution Explorer, open CSVSToolWindow.vsct.
	   VSCT file stands for Visual Studio Command Table. This is an XML based file that 
	   describes the layout and appearance of command items for a VSPackage. Command 
	   items include buttons, combo boxes, menus, toolbars, and groups of command items.
	
	2. In the <Symbols> section, find the <GuidSymbol> node whose name attribute is 
	   guidCSVSToolWindowCmdSet. Add the following two <IDSymbol> elements to the list of 
	   <IDSymbol> elements in this node to define a toolbar and a toolbar group. 
 
	   <IDSymbol name="ToolbarID" value="0x1000" />
       <IDSymbol name="ToolbarGroupID" value="0x1001" />
	
	3. Just above the <Groups> section, create an empty <Menus> section, and create the 
	   following <Menu> element to define the toolbar that you declared in step 2.
	   
	   <Menus>
        <Menu guid="guidCSVSToolWindowCmdSet" id="ToolbarID"
              priority="0x0000" type="ToolWindowToolbar">
          <Parent guid="guidCSVSToolWindowCmdSet" id="ToolbarID" />
          <Strings>
            <ButtonText>Tool Window Toolbar</ButtonText>
            <CommandName>Tool Window Toolbar</CommandName>
          </Strings>
        </Menu>
       </Menus> 
	   
    4. Add a new <Group> element to the <Groups> section to define the group that you 
       declared in the <Symbols> section. 
       
       <Group guid="guidCSVSToolWindowCmdSet" id="ToolbarGroupID"
              priority="0x0000">
         <Parent guid="guidCSVSToolWindowCmdSet" id="ToolbarID"/>
       </Group>
      
    5. Save CSVSToolWindow.vsct.
	
	
D: Add a Toolbar to the Tool Window

	1. In Solution Explorer, open CSVSToolWindow.vsct.
	   VSCT file stands for Visual Studio Command Table. This is an XML based file that 
	   describes the layout and appearance of command items for a VSPackage. Command 
	   items include buttons, combo boxes, menus, toolbars, and groups of command items.
	
	2. In the <Symbols> section, declare the following commands just after the toolbar
	   and toolbar group declarations. 
	   
	   <IDSymbol name="cmdidWindowsMediaOpen" value="0x132" /> 
	
	3. In the <Buttons> section, a <Button> element is already present and it contains 
	   a definition forthe cmdidToolWindow command. Add one more <Button> element to 
	   define the cmdidWindowsMedia and cmdidWindowsMediaOpen commands. 
	   
        <Button guid="guidCSVSToolWindowCmdSet"
                id="cmdidWindowsMediaOpen" priority="0x0101"
                type="Button">
          <Parent guid="guidCSVSToolWindowCmdSet"
                id="ToolbarGroupID"/>
          <Icon guid="guidImages" id="bmpPic1" />
          <CommandFlag>IconAndText</CommandFlag>
          <Strings>
            <CommandName>cmdidWindowsMediaOpen</CommandName>
            <ButtonText>Load File</ButtonText>
          </Strings>
        </Button>
        
    4. Save and close CSVSToolWindow.vsct, open PkgCmdID.cs and add the following lines 
       in the class just after the existing members
              
        public const int cmdidWindowsMediaOpen = 0x132;
        public const int ToolbarID = 0x1000;
       
    5. Save PkgCmdID.cs
	
E: Implement the Commands:

    1. In Solution Explorer, open MyToolWindow.cs, which contains the class for the tool 
       window itself. Add the following code just after the existing using statements. 
       
       using System.ComponentModel.Design;
       
    2. Add the following code to the constructor, just before the line that says 
       control = new MyControl(). 
       
            // Create the toolbar.
            this.ToolBar = new CommandID(GuidList.guidCSVSToolWindowCmdSet,
                PkgCmdIDList.ToolbarID);
            this.ToolBarLocation = (int)VSTWT_LOCATION.VSTWT_TOP;

            // Create the handlers for the toolbar commands.
            var mcs = GetService(typeof(IMenuCommandService))
                as OleMenuCommandService;
            if (null != mcs)
            {
                var toolbarbtnCmdID = new CommandID(
                    GuidList.guidCSVSToolWindowCmdSet,
                    PkgCmdIDList.cmdidWindowsMediaOpen);
                var menuItem = new MenuCommand(new EventHandler(
                    ButtonHandler), toolbarbtnCmdID);
                mcs.AddCommand(menuItem);
            }
           
    3. In Solution Explorer, right-click MyControl.cs, click View Code, and add the following
       code to the end of the file, just before the final two closing braces.Save and close 
       MyControl.cs. 
       
        public AxWMPLib.AxWindowsMediaPlayer MediaPlayer
		{
			get { return axWindowsMediaPlayer1; }
		}

       
    4. Open to MyToolWindow.cs, and add the following handler at the end of the class, just before 
       the two final closing braces. 
       
        private void ButtonHandler(object sender, EventArgs arguments)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.Filter = "media files |*.wmv;*.asf";
            fd.Multiselect = false;
            if (DialogResult.OK == fd.ShowDialog())
            {
                control.MediaPlayer.URL = fd.FileName;
            }
        }
           
    5. Save all the files.

F: Testing the Tool Window

    1. Press F5 to open a new instance of the Visual Studio experimental build.
       
    2. On the View menu, point to Other Windows and then click My Tool Window.
    
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

VSPackage Tutorial 2: How to Create a Tool Window
http://msdn.microsoft.com/en-us/library/cc138567.aspx

Creating a package with a simple command
http://dotneteers.net/blogs/divedeeper/archive/2008/01/06/LearnVSXNowPart3.aspx

Menus and comands in VS IDE
http://dotneteers.net/blogs/divedeeper/archive/2008/02/22/LearnVSXNowPart13.aspx


/////////////////////////////////////////////////////////////////////////////


          
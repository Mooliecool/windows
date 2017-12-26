=============================================================================
            VSX Module: CSVSPackageWPFToolWindow Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demostrate how to host a WPF control into Visual Studio 
ToolWindow. 

To use the sample
1. Run the sample
2. Open tool window by View / Other Windows / WPFToolWindow
3. A tool window will be docked at place of solution explorer
4. The tool window hosts a WPF control which represents MyDocuments folder
structure


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

1. Creating the Project
Create a Visual Studio Integration Package project that provides a tool window. 
Later, you add a WPF control to this tool window.

To create the Visual Studio project

Create a Visual Studio package that provides a tool window.

In the Visual Studio Integration Package Wizard, use the following settings:
- Set the programming language to Visual Basic or Visual C#.
- Use the default values on the Basic Package Information page.
- Add a tool window that is named Hosted WPF Clock Control.
- Click Finish.

The wizard generates a project that contains a WinForms user control, 
MyControl, for the tool window.

2. Adding the WPF User Control
Add a WPF user control to your project. The user control presented here 
represents MyDocuments folder structure by tree view. 
Then, add this WPF control to the control for the tool window in your package.

To create the WPF user control

In Solution Explorer, right-click the tool window project, 
point to Add, and then click New Item.

In the Add New Item dialog box, select the User Control (WPF) template, 
name it WPFControl, and click Add.

Open the WPFControl.xaml file for editing, and replace the child element 
of the UserControl element by using the following markup. 

XAML
<UserControl x:Class="Company.VSPackageWPFToolWindow.WPFControl"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    Height="300" Width="300">
    <Grid>
        <TreeView Name="treeView" HorizontalAlignment="Stretch" 
			HorizontalContentAlignment="Left" />
    </Grid>
</UserControl>
To open the code-behind file for editing, right-click in the XAML editor, 
and then click View Code.

Add the following code:
public TreeView WPFTreeView
{
    get
    {
        return this.treeView;
    }
}

Save all files and verify that the solution builds.

To add the WPF user control to the tool window
a. Open the MyControl user control in design mode.
b. Remove the default button, button1, from the control.
c. In the Toolbox, directly drag WPFControl into the Windows Form

Click Dock in parent container to set the Dock property to Fill.

Right-click the design surface, and then click View Code to open the MyControl class in the code editor.

Delete the button1_Click method that the wizard generated.

Add the methods shown in the following example. 
public WPFControl WPFControl
{
    get
    {
        return wpfControl1;
    }
}

Open the MyToolWindow.cs file, add following code:
/// <summary>
/// Standard constructor for the tool window.
/// </summary>
public MyToolWindow() :
    base(null)
{
    // Set the window title reading it from the resources.
    this.Caption = Resources.ToolWindowTitle;
    // Set the image that will appear on the tab of the window frame
    // when docked with an other window
    // The resource ID correspond to the one defined in the resx file
    // while the Index is the offset in the bitmap strip. Each image in
    // the strip being 16x16.
    this.BitmapResourceID = 301;
    this.BitmapIndex = 1;


    control = new MyControl();
    wpfControl = control.WPFControl;
}

public override void OnToolWindowCreated()
{
    base.OnToolWindowCreated();
    InitializeTreeViewContent();

}

private void InitializeTreeViewContent()
{
    DirectoryInfo myDocInfo =
        new DirectoryInfo(
            Environment.GetFolderPath(
                Environment.SpecialFolder.MyDocuments
            )
        );
    IntializeTreeViewContentRecursively(myDocInfo, wpfControl.WPFTreeView.Items);
}

private void IntializeTreeViewContentRecursively(
    DirectoryInfo myDocInfo, ItemCollection itemCollection)
{            
    if (myDocInfo == null)
        return;

    try
    {
        TreeViewItem item = new TreeViewItem();
        item.Header = myDocInfo.Name;
        itemCollection.Add(item);

        DirectoryInfo[] subDirs = myDocInfo.GetDirectories();
        if (subDirs != null)
        {
            foreach (DirectoryInfo dir in subDirs)
                IntializeTreeViewContentRecursively(dir, item.Items);
        }

        FileInfo[] files = myDocInfo.GetFiles();
        if (files != null)
        {
            foreach (FileInfo file in files)
            {
                TreeViewItem fileItem = new TreeViewItem();
                fileItem.Header = file.Name;
                fileItem.Tag = file.FullName;
                item.Items.Add(fileItem);
            }
        }
    }
    catch
    {
        return;
    }
}

3. Add regsitry attribute to the package

Open CSVSPackageWPFToolWindowPackage.cs, add following attribute:

[ProvideToolWindow(typeof(MyToolWindow), Style = VsDockStyle.Tabbed, 
	Window = "3ae79031-e1bc-11d0-8f78-00a0c9110057")]

The attribute indicates that the tool window will be docked at the same place
as solution explorer.
    
/////////////////////////////////////////////////////////////////////////////
References:

Walkthrough: Hosting a WPF User Control in a Tool Window
http://msdn.microsoft.com/en-us/library/cc826120.aspx

/////////////////////////////////////////////////////////////////////////////

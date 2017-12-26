========================================================================
    Visual Studio Add-In : CSVSAddInCommandBarUI Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSVSAddInCommandBarUI project demostrates how to display Visual Studio
add-in in toolbar or menubar. 

Visual Studio offers three kinds of CommandBar objects:

1. Toolbars — Contain one or more menu bars.

2. Menu bars — Commands on toolbars, such as File, Edit, and View.

3. Shortcut menus (also known as context or popup menus.) — Menus that appear 
on the screen when you right-click a menu or object (such as a file or project). 
Submenus cascade off menu commands or off shortcut menus. Shortcut menus are 
similar to other menus in Visual Studio. However, you access them by pointing 
to an arrow in a menu bar, or by right-clicking an item in the integrated 
development environment (IDE).

This sample displays three commandbars:

1. In Menubar -> Tool menu -> All-In-One Command
2. In Toolbar -> All-In-One command
3. In Task List toolbox -> right click content menu -> All-In-One command


/////////////////////////////////////////////////////////////////////////////
Deployment:

1. Open CSVSAddInCommandBarUI.AddIn file and change the Assembly element to
the path of the CSVSAddInCommandBarUI.dll file.

2. Copy the CSVSAddInCommandBarUI.AddIn file to directory:
%userprofile%\Documents\Visual Studio 2008\Addins\


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create Visual Studio Add-in project from File -> New -> Project 
-> Other Project Types -> Extensibility -> Visual Studio Add-In

Step2. When you create an add-in by using the Add-In Wizard and select 
the option to display it as a command, the command is on the Tools 
menu by default. 

Step3. In the add-in's Connect class and OnConnection() procedure,
Creates Command with name CSVSAddInCommandBarUI and set its
icon ID as 6743, which is red star instead of its default smiley
face icon.

Step4. Add command to Tool menubar by getting CommandBar with name "MenuBar"
and its child CommandBarPopup control "Tools" and adding command into it. 

Step5. Add command to toolbar by getting CommandBar "Standard" and adding
command into its last item, then set its caption.

Step6. Add command to shortcut menu by getting CommandBar "Task List" and
adding command into its last item, then set its caption 

Step7. When any of those three button or menu item is clicked, execute 
command "View.URL" to navigate Visual Studio web browser to All-In-One
home page.


/////////////////////////////////////////////////////////////////////////////
References:

Displaying Add-ins on Toolbars and Menus
http://msdn.microsoft.com/en-us/library/ms165623.aspx


/////////////////////////////////////////////////////////////////////////////
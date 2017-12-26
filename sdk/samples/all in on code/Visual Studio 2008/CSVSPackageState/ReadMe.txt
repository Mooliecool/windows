================================================================================
       VSX application : CSVSPackageState Project Overview
       
            Visual Studio Package State Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Visual Studio package state sample demostrate the state persisting for
application options and show object states in properties window.

The sample doesn't include the state management for solution and project,
which will be included in project package sample.

*Tools/Options Page*

1. Open menu [Tools]/[Options]
2. Select [CSVSPackageState Category]
3. Change the value of [Option Integer Property]
4. Open registry editor
5. Find key: "HKEY_CURRENT_USER\Software\Microsoft\VisualStudio\
   9.0Exp\UserSettings\DialogPage\
   AllInOne.CSVSPackageState.OptionPageGrid"
   The [OptionInteger] value stores user settings.
   
*Property Window*
In this sample, we use ToolWindow to demostrate property window.

1. Open menu [View]/[Other Windows]/[All-In-One Tool Window]
2. Open menu [View]/[Properties Window]
3. In the All-In-One Tool Window, change the value of 
   PropertyInteger or PropertyString
4. Change the focus by clicking Properties window
5. The PropertyInteger or PropertyString will be changed 
   according to the value set in the tool window.
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
Concepts:

*Tools/Options Page*
Selecting Options on the Tools menu opens the Options 
dialog box. The options in this dialog box are collectively 
referred to as Tools/Options pages. The tree control in the 
navigation pane includes option categories, and each category 
has option pages. When you select a page, its options appear 
in the right pane. These pages let you change the values of 
the options that determine the state of a VSPackage.

The DialogPage class implements IProfileManager, which provides 
persistence for both options pages and user settings. The default 
implementations of the LoadSettingsFromStorage and SaveSettingsToStorage
methods persist property changes into a user's section of the registry 
if the property can be converted to and from a string.

*Property Window*
When you select an object in Visual Studio, the public properties 
of that object appear in the Properties window. To select an object 
programmatically, you add the object to a list of selectable and 
selected objects in a selection container. You use the STrackSelection 
service to notify Visual Studio of the selection.

/////////////////////////////////////////////////////////////////////////////
Creation:

*Tools/Options Page*
1. Create a new Visual Studio Integration Package project.
2. On the Select a Programming Language page, select Visual Basic or Visual C#.
3. On the Select VSPackage Options page, select Menu Command.
4. On the Command Options page, change the Command name to Get internal 
option VB or Get internal option CS for Visual Basic or Visual C# respectively, 
and then click Finish.
5. Declare an OptionPageGrid class and derive it from DialogPage.
6. Apply a System.Runtime.InteropServices.ClassInterfaceAttribute 
to the OptionPageGrid class.
This creates a COM dual interface that lets Visual Studio Automation use 
GetAutomationObject to access the public members of the class programmatically.
7. Apply a ProvideOptionPageAttribute to the VSPackage class to assign to the 
class an options category and options page name for the OptionPageGrid.
8. Add an OptionInteger property to the OptionPageGrid class.

*Property Window*
1. Create a new Visual Studio Integration Package project.
2. In the Select a Programming Language page, select Visual C#.
3. In the Select VSPackage Options page, select Tool Window.
4. In the Tool Window Options page, change the Window Name to 
"All-In-One Tool Window", and then click Finish.
5. The wizard creates the managed project.
6. Open the file, MyToolWindow.cs, and add the following fields to 
the MyToolWindow class:
a. ITrackSelection service 
The TrackSelection property uses GetService to obtain an STrackSelection 
service, which provides an ITrackSelection interface. 
b. OnToolWindowCreated event
c. SelectList method
The OnToolWindowCreated event handler and SelectList method together create a list 
of selected objects that contains only the tool window pane object itself.
d. UpdateSelection method
The UpdateSelection method tells the Properties window to display the public 
properties of the tool window pane.
/////////////////////////////////////////////////////////////////////////////
References:

VSPackage State
http://msdn.microsoft.com/en-us/library/bb165693.aspx

Support for Options Pages
http://msdn.microsoft.com/en-us/library/bb166553.aspx

Support for the Property Browser
http://msdn.microsoft.com/en-us/library/bb165050.aspx
/////////////////////////////////////////////////////////////////////////////








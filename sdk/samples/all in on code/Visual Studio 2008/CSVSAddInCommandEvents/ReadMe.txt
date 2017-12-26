========================================================================
    Visual Studio Add-In : CSVSAddInCommandEvents Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to subscribe to the shell command executing and
how to change the menu item's caption dynamically.

The EnvDTE Automation has provided CommandEvents interface to represent the
specific command events in the shell.
The DTE.Events.get_CommandEvents() method gives the way to get the specific
command events, then you could add your personal actions before/after the
command executing.

To change the text of menu item in AddIn, you need to get the CommandBar
which contains the menu item controls firstly, then use
CommandBarControl.Caption property to specify the caption of menu item.


/////////////////////////////////////////////////////////////////////////////
Deployment:

1. Open CSVSAddInCommandEvents.AddIn file and change the Assembly element to
the path of the CSVSAddInCommandEvents.dll file.

2. Copy the CSVSAddInCommandEvents.AddIn file to directory:
%userprofile%\Documents\Visual Studio 2008\Addins\


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create Visual Studio Add-in project from File -> New -> Project
-> Other Project Types -> Extensibility -> Visual Studio Add-In.

Step2. When you create an add-in by using the Add-In Wizard and select
the option to display it as a command, the command is on the Tools
menu by default.

Step3. In the add-in's Connect class and OnConnection() procedure,
modify the command with name CSVSAddInCommandEvents and button text
Add CommandEvent Subscription:

Command command
    = commands.AddNamedCommand2(_addInInstance,
    "CSVSAddInCommandEvents",
    "Add CommandEvent Subscription",
    "Executes the command for CSVSAddInCommandEvents",
    true, 59, ref contextGUIDS,
    (int)vsCommandStatus.vsCommandStatusSupported
    + (int)vsCommandStatus.vsCommandStatusEnabled,
    (int)vsCommandStyle.vsCommandStylePictAndText,
    vsCommandControlType.vsCommandControlTypeButton);

Step4. Add variables for the CommandEvents and menu item's state:

private CommandEvents addReferenceEvents; // Command events.
private bool isSubscribe; // Flag to indicate the menu item's current work.

Step5. Initialize the isSubscribe to true in constructor:
isSubscribe = true;

Step6. Define a method to add subscription of the Project.AddReference
command with below contents:

public void AddSubscription()
{
    // "{1496A755-94DE-11D0-8C3F-00C04FC2AAE2}, 1113" Guid-ID pair refer to
    // Project.AddReference command.
    // About how to get the Guid and ID of the specific command, please take
    // a look at this link on Dr.eX's blog:
    // http://blogs.msdn.com/dr._ex/archive/2007/04/17/using-
    // enablevsiplogging-to-identify-menus-and-commands-with-vs-2005-sp1.aspx
    try
    {
        addReferenceEvents
            = _applicationObject.Events.get_CommandEvents(
            "{1496A755-94DE-11D0-8C3F-00C04FC2AAE2}",
            1113);
        addReferenceEvents.BeforeExecute
            += new _dispCommandEvents_BeforeExecuteEventHandler
                (addReferenceEvents_BeforeExecute);
        addReferenceEvents.AfterExecute
            += new _dispCommandEvents_AfterExecuteEventHandler
                (addReferenceEvents_AfterExecute);
    }
    catch (Exception e)
    {
        System.Windows.Forms.MessageBox.Show(e.Message);
    }
}

Step7. Define a method to remove subscription of the Project.AddReference
command with below contents:

public void RemoveSubscription()
{
    try
    {
        addReferenceEvents.BeforeExecute
            -= new _dispCommandEvents_BeforeExecuteEventHandler
                (addReferenceEvents_BeforeExecute);
        addReferenceEvents.AfterExecute
            -= new _dispCommandEvents_AfterExecuteEventHandler
                (addReferenceEvents_AfterExecute);
    }
    catch (Exception e)
    {
        System.Windows.Forms.MessageBox.Show(e.Message);
    }
}

Step8. Implement the BeforeExecute/AfterExecute event handlers:

public void addReferenceEvents_BeforeExecute(string Guid,
    int ID,
    object CustomIn,
    object CustomOut,
    ref bool CancelDefault)
{
    System.Windows.Forms.MessageBox.Show("Before adding reference.");

    // If you want to cancel the default handler for this command, specify
    // the CancelDefault to true.

    //CancelDefault = true;
}

public void addReferenceEvents_AfterExecute(string Guid,
    int ID,
    object CustomIn,
    object CustomOut)
{
    System.Windows.Forms.MessageBox.Show("After adding reference.");
}

Step9. Modify the Exec() procedure.
Call the AddSubscription() or RemoveSubscription() method, change the caption
of menu item depends on the flag isSubscribe:

public void Exec(string commandName,
    vsCommandExecOption executeOption,
    ref object varIn,
    ref object varOut,
    ref bool handled)
{
    handled = false;
    if (executeOption == vsCommandExecOption.vsCommandExecOptionDoDefault)
    {
        if (commandName
            == "CSVSAddInCommandEvents.Connect.CSVSAddInCommandEvents")
        {
            // Get the Tools command bar.
            Microsoft.VisualStudio.CommandBars.CommandBar menuBarCommandBar
                = ((Microsoft.VisualStudio.CommandBars.CommandBars)
                _applicationObject.CommandBars)["MenuBar"];
            CommandBarControl toolsControl
                = menuBarCommandBar.Controls["Tools"];
            CommandBarPopup toolsPopup = (CommandBarPopup)toolsControl;
            CommandBar toolsCommandBar = toolsPopup.CommandBar;
            if (isSubscribe)
            {
                // Subscribe to the command events.
                AddSubscription();
                // Next clicking will remove the subscription.
                isSubscribe = false;
                // Get the menu item control.
                CommandBarControl menuItemControl
                    = toolsCommandBar.Controls["Add Subscription"];
                // Change its caption to Remove Subscription.
                menuItemControl.Caption = "Remove CommandEvent Subscription";
            }
            else
            {
                // Remove the subscription.
                RemoveSubscription();
                // Next clicking will add the subscription.
                isSubscribe = true;
                // Get the menu item control.
                CommandBarControl menuItemControl
                    = toolsCommandBar.Controls["Remove Subscription"];
                // Change its caption to Add Subscription.
                menuItemControl.Caption = "Add CommandEvent Subscription";
            }

            handled = true;
            return;
        }
    }
}

Step10. Compile the project.


/////////////////////////////////////////////////////////////////////////////
References:

Using EnableVSIPLogging to identify menus and commands with VS 2005 + SP1:
http://blogs.msdn.com/dr._ex/archive/2007/04/17/using-enablevsiplogging-to-
identify-menus-and-commands-with-vs-2005-sp1.aspx

CommandEvents Interface:
http://msdn.microsoft.com/en-us/library/envdte.commandevents.aspx

_DTE.Events Property:
http://msdn.microsoft.com/en-us/library/envdte._dte.events.aspx 


/////////////////////////////////////////////////////////////////////////////
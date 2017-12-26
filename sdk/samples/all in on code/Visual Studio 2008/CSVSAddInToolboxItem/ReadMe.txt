========================================================================
    Visual Studio Add-In : CSVSAddInToolboxItem Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSVSAddInToolboxItem project demostrates how to customize Toolbox items
in DTE automation model or by Toolbox service.

In this sample, a add-in command button named "Add Customized Toolbox Item" 
will be registered in the Tool menu. By clicking the menu, sample code will
do follow two things:
1. Use DTE automation model to add an item under CustomTab tab
2. Use VS Toolbox service to add an item under CustomTab tab

Both items are HTML content, they can be used by dragging and dropping to a 
web page designer.


/////////////////////////////////////////////////////////////////////////////
Background:

In VS automation model, it provides functions:
1. Add a tab to the Toolbox (Add method).
2. Activate a tab in the Toolbox (Activate method).
3. Delete a tab from the Toolbox (Delete method).
4. Add an item to the Toolbox (Add method).
5. Select an item in the Toolbox (Select method).
6. Delete an item from a tab in the Toolbox (Delete method).
For more detail, please refer to:
http://msdn.microsoft.com/en-us/library/6xs853ft.aspx

VS Toolbox service provides richer access to toolbox functionality.
Its functions can be described by IVSToolbox interface:
Name	Description
AddActiveXItem	 Allows ActiveX controls to be programmatically added to the Toolbox.
AddItem	 Adds a data object to the specified tab in the Toolbox.
AddItemFromFile	 Allows files to be programmatically added to the Toolbox.
AddTab	 Adds a new tab to the Toolbox.
AddTabEx	 Adds a new tab to the Toolbox and specifies how the tabs are viewed.
DataUsed	 Called by a designer to inform the Toolbox that the item has been pasted onto the form.
EnumItems	 Enumerates items on a given tab in the Toolbox, or all items in the Toolbox.
EnumTabs	 Enumerates through tabs in the Toolbox.
GetData	 Returns the data object for the selected item in the Toolbox.
GetFrame	 Returns the window frame for the Toolbox.
GetTab	 Returns the name of the selected tab.
GetTabView	 Returns the tab view for the specified tab.
IsCurrentUser	 Determines whether the specified Toolbox user is the current user.
RegisterDataProvider	 Registers a Toolbox data provider.
RemoveItem	 Removes an item from the Toolbox.
RemoveTab	 Removes a tab from the Toolbox.
SelectItem	 Programmatically selects an item.
SelectTab	 Programmatically selects the referenced Toolbox tab.
SetCursor	 Sets the drop cursor for the current, enabled Toolbox item.
SetItemInfo	 Sets information for the referenced item.
SetTabView	 Sets the type of view for the specified tab.
UnregisterDataProvider	 Unregisters the specified data provider.
UpdateToolboxUI	 Forces a redraw of the Toolbox UI.

/////////////////////////////////////////////////////////////////////////////
Deployment:

1. Open CSVSAddInToolboxItem.AddIn file and change the Assembly element to
the path of the CSVSAddInToolboxItem.dll file.

2. Copy the CSVSAddInToolboxItem.AddIn file to directory:
%userprofile%\Documents\Visual Studio 2008\Addins\


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create Visual Studio Add-in project from File -> New -> Project 
-> Other Project Types -> Extensibility -> Visual Studio Add-In

Step2. When you create an add-in by using the Add-In Wizard and select 
the option to display it as a command, the command is on the Tools 
menu by default. 

Step3. In Connect class, add two methods:AddItemByDTE and AddItemByVsToolboxService 
with following code:

/// <summary>
/// This method adds a Toolbox item by DTE automation model
/// </summary>
private void AddItemByDTE()
{
    // Get tabs from automation model
    ToolBoxTabs tabs = _applicationObject.ToolWindows.ToolBox.ToolBoxTabs;
    IEnumerator e = tabs.GetEnumerator();

    ToolBoxTab tab = null;
    while (e.MoveNext())
    {
        ToolBoxTab ct = e.Current as ToolBoxTab;
        if (ct.Name == "CustomTab")
        {
            tab = ct;
            break;
        }
    }

    // If there is no CustomTab, add one
    if (tab == null)
        tab = tabs.Add("CustomTab");

    // Add Toolbox Item, but we can't customize other information for 
    // Toolbox item like icon and transparency.
    tab.ToolBoxItems.Add(
        "DTE Added HTML Content", 
        "<input id=\"Button1\" type=\"button\" value=\"button\" />", 
        vsToolBoxItemFormat.vsToolBoxItemFormatHTML);
}

/// <summary>
/// This method adds a Toolbox item by VS Toolbox service.
/// This way provides more flexibilities than DTE way.
/// </summary>
private void AddItemByVsToolboxService()
{
    // Get shell service provider.
    ServiceProvider sp =
        new ServiceProvider((Microsoft.VisualStudio.OLE.Interop.IServiceProvider)_applicationObject);

    // Get the IVsToolbox interface.
    IVsToolbox tbs = sp.GetService(typeof(SVsToolbox)) as IVsToolbox;

    // Toolbox Item Info data 
    TBXITEMINFO[] itemInfo = new TBXITEMINFO[1];
    Bitmap bitmap =
        new Bitmap(this.GetType().Assembly.GetManifestResourceStream("CSVSAddInToolboxItem.Demo.bmp"));
    itemInfo[0].bstrText = "Service Added HTML Content";
    itemInfo[0].hBmp = bitmap.GetHbitmap(); // Specify the bitmap.
    itemInfo[0].dwFlags = (uint)__TBXITEMINFOFLAGS.TBXIF_DONTPERSIST;

    // OleDataObject to host toolbox data
    OleDataObject tbItem = new OleDataObject();
    tbItem.SetText(
        ConvertToClipboardFormat("<input id=\"Button1\" type=\"button\" value=\"button\" />", null, null),
        TextDataFormat.Html);

    // Add a new toolbox item to MyCustomTab tab
    tbs.AddItem(tbItem, itemInfo, "CustomTab");
}

Step4. Add ConvertToClipboardFormat method. Most of the method code is referenced 
from http://blogs.msdn.com/jmstall/pages/sample-code-html-clipboard.aspx

Step5. In the Exec method, invoke both AddItemByDTE and AddItemByVsToolboxService 
methods


/////////////////////////////////////////////////////////////////////////////
References:

IVsToolbox Members
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.ivstoolbox_members.aspx

VSSDK IDE Sample: Editor with Toolbox
http://code.msdn.microsoft.com/EditorwithToolbox

How do I manipulate ToolBox Items?
http://blogs.msdn.com/dr._ex/archive/2004/03/23/94991.aspx

Toolbox (Visual Studio SDK)
http://msdn.microsoft.com/en-us/library/bb166364.aspx

How to: Control the Toolbox
http://msdn.microsoft.com/en-us/library/6xs853ft.aspx

Sample code for copying Html to Clipboard
http://blogs.msdn.com/jmstall/pages/sample-code-html-clipboard.aspx

Forum Threads
http://social.msdn.microsoft.com/Forums/en-NZ/vsx/thread/da5d7d40-72e7-4c99-a51a-5b085c696174
http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/3ab660c2-f270-43c5-80f8-f3561a71c565


/////////////////////////////////////////////////////////////////////////////
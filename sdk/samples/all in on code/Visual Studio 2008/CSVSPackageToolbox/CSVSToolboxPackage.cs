/****************************** Module Header ******************************\
* Module Name:  CSVSToolboxPackage.cs
* Project:      CSVSPackageToolbox
* Copyright (c) Microsoft Corporation.
* 
* Implementation of CSVSToolbox
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using Microsoft.Win32;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;
using System.Collections;
using System.ComponentModel;
using System.Drawing.Design;
using System.Reflection;


namespace MyCompany.CSVSToolbox
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    ///
    /// The minimum requirement for a class to be considered a valid package for Visual Studio
    /// is to implement the IVsPackage interface and register itself with the shell.
    /// This package uses the helper classes defined inside the Managed Package Framework (MPF)
    /// to do it: it derives from the Package class that provides the implementation of the 
    /// IVsPackage interface and uses the registration attributes defined in the framework to 
    /// register itself and its components with the shell.
    /// </summary>
    // This attribute tells the registration utility (regpkg.exe) that this class needs
    // to be registered as package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // A Visual Studio component can be registered under different regitry roots; for instance
    // when you debug your package you want to register it in the experimental hive. This
    // attribute specifies the registry root to use if no one is provided to regpkg.exe with
    // the /root switch.
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]
    // This attribute is used to register the informations needed to show the this package
    // in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration(false, "#110", "#112", "1.0", IconResourceID = 400)]
    // In order be loaded inside Visual Studio in a machine that has not the VS SDK installed, 
    // package needs to have a valid load key (it can be requested at 
    // http://msdn.microsoft.com/vstudio/extend/). This attributes tells the shell that this 
    // package has a load key embedded in its resources.
    [ProvideLoadKey("Standard", "1.0", "CSVSToolboxDemo", "MyCompany", 1)]
    // This attribute is needed to let the shell know that this package exposes some menus.
    [ProvideMenuResource(1000, 1)]
    [Guid(GuidList.guidCSVSToolboxPkgString)]
    [ProvideToolboxItems(1)]
    public sealed class CSVSToolboxPackage : Package
    {
        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public CSVSToolboxPackage()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture, "Entering constructor for: {0}", this.ToString()));
        }

        // List for the toolbox items provided by this package.
        private ArrayList ToolboxItemList;

        // Name for the Toolbox category tab for the package's toolbox items.
        private string CategoryTab = "LoadToolboxMemberDemo";
        

        /////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initilaization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Trace.WriteLine (string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add our command handlers for menu (commands must exist in the .vsct file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                // Create the command for the menu item.
                CommandID menuCommandID = new CommandID(GuidList.guidCSVSToolboxCmdSet, (int)PkgCmdIDList.cmdidInitializeToolbox);
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

        }
        #endregion

        #region Tools Menu callback function
        /// <summary>
        /// This function is the callback used to execute a command when the a menu item is clicked.
        /// See the Initialize method to see how the menu item is associated to this function using
        /// the OleMenuCommandService service and the MenuCommand class.
        /// </summary>
        private void MenuItemCallback(object sender, EventArgs e)
        {
            IVsPackage pkg = GetService(typeof(Package)) as Package;
            pkg.ResetDefaults((uint)__VSPKGRESETFLAGS.PKGRF_TOOLBOXITEMS);
        }

        #endregion

        #region Get the Toolbox items with reflection
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

        #endregion

        #region The event handlers for the ToolboxInitialized and ToolboxUpgraded
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

        #endregion

    }
}
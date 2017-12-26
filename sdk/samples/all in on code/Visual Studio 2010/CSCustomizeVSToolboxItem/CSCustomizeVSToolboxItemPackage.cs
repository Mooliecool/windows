/****************************** Module Header ******************************\
* Module Name:  CSCustomizeVSToolboxItemPackage.cs
* Project:	    CSCustomizeVSToolboxItem
* Copyright (c) Microsoft Corporation.
* 
* The class CSCustomizeVSToolboxItemPackage inherits the class 
* Microsoft.VisualStudio.Shell.Package class. It overrides the Initialize method.
* 
* If you add a new item to Vs2010 toolbox, the display name and tooltip of the new
* item are the same by default. The sample demonstrates how to add an item with 
* custom tooltip to Visual Studio Toolbox. 
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
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace Microsoft.CSCustomizeVSToolboxItem
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
    [DefaultRegistryRoot(@"Software\Microsoft\VisualStudio\10.0")]
    // This attribute tells the PkgDef creation utility (CreatePkgDef.exe) that this class is
    // a package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // This package will be auto loaded if there is a solution in VS.
    [ProvideAutoLoad(VSConstants.UICONTEXT.SolutionExists_string)]
    // This attribute is used to register the information needed to show the this package
    // in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)]
    [Guid(GuidList.guidCSCustomizeVSToolboxItemPkgString)]
    // Register the VSPackage as providing ToolboxItem objects that have the 
    // ProvideToolboxItemsAttribute class.
    [ProvideToolboxItems(1)]
    public sealed class CSCustomizeVSToolboxItemPackage : Package
    {
        // Define the tab, item, tooltip, description and drag-drop text.

        const string toolboxTabString = "CS Custom Toolbox Tab";
        const string toolboxItemString = "CS Custom Toolbox Item";
        const string toolboxTooltipString = "CS Custom Toolbox Tooltip";
        const string toolboxDescriptionString = "CS Custom Toolbox Description";
        const string toolboxItemTextString = "CS Hello world!";

        // The IVsToolbox2 service.
        IVsToolbox2 vsToolbox2;

        // The IVsActivityLog service.
        IVsActivityLog vsActivityLog;

        // Memory stream to store the tooltip data.
        Stream tooltipStream;

        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not require 
        /// any Visual Studio service because at this point the package object is created but 
        /// not sited yet inside Visual Studio environment. The place to do all the other 
        /// initialization is the Initialize method.
        /// </summary>
        public CSCustomizeVSToolboxItemPackage()
        {
            Trace.WriteLine("Entering constructor for: {0}", this.ToString());
        }



        /////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initialization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            base.Initialize();

            ToolboxInitialized += new EventHandler(ToolboxItemPackage_ToolboxInitialized);
            this.ToolboxUpgraded += new EventHandler(ToolboxItemPackage_ToolboxUpgraded);
            // Initialize the services.
            vsActivityLog = GetService(typeof(SVsActivityLog)) as IVsActivityLog;
            vsToolbox2 = GetService(typeof(SVsToolbox)) as IVsToolbox2;

            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
                string.Format("Entering initializer for: {0}", this.ToString()));



            // Add the toolbox item if it does not exist.
            try
            {
                if (!VerifyToolboxTabExist())
                {
                    AddToolboxTab();
                }

                if (!VerifyToolboxItemExist())
                {
                    AddToolboxItem();
                }

                // vsToolbox2.UpdateToolboxUI();
            }
            catch (Exception ex)
            {
                LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_ERROR, ex.Message);
                LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_ERROR, ex.StackTrace);
            }
        }

        void ToolboxItemPackage_ToolboxUpgraded(object sender, EventArgs e)
        {
            //throw new NotImplementedException();
        }

        void ToolboxItemPackage_ToolboxInitialized(object sender, EventArgs e)
        {
            // throw new NotImplementedException();
        }

        /// <summary>
        /// Log the VS activity using the IVsActivityLog service.
        /// </summary>
        void LogEntry(__ACTIVITYLOG_ENTRYTYPE type, string message)
        {

            if (vsActivityLog != null)
            {
                int hr = vsActivityLog.LogEntry((UInt32)type, this.ToString(), message);
                ErrorHandler.ThrowOnFailure(hr);
            }
        }

        /// <summary>
        /// Verify whether the toolbox tab exists.
        /// </summary>
        bool VerifyToolboxTabExist()
        {
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
              string.Format("Entering VerifyToolboxTabExist for: {0}", this.ToString()));

            bool exist = false;

            IEnumToolboxTabs tabs;
            uint num;

            ErrorHandler.ThrowOnFailure(vsToolbox2.EnumTabs(out tabs));
            string[] rgelt = new string[1];
            for (int i = tabs.Next(1, rgelt, out num);
                (ErrorHandler.Succeeded(i) && (num > 0)) && (rgelt[0] != null);
                i = tabs.Next(1, rgelt, out num))
            {
                if (rgelt[0] == toolboxTabString)
                {
                    exist = true;
                    break;
                }
            }

            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
             string.Format("VerifyToolboxTabExist {0}: {1}", toolboxTabString, exist));

            return exist;
        }

        /// <summary>
        /// Add the toolbox tab. 
        /// </summary>
        void AddToolboxTab()
        {
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
             string.Format("Entering AddToolboxTab for: {0}", toolboxTabString));

            ErrorHandler.ThrowOnFailure(vsToolbox2.AddTab(toolboxTabString));
        }

        /// <summary>
        /// Verify whether the toolbox item exists.
        /// </summary>
        bool VerifyToolboxItemExist()
        {
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
             string.Format("Entering VerifyToolboxItemExist for: {0}", this.ToString()));

            bool exist = false;

            IEnumToolboxItems items;
            uint num;
            ErrorHandler.ThrowOnFailure(vsToolbox2.EnumItems(toolboxTabString, out items));
            var rgelt = new Microsoft.VisualStudio.OLE.Interop.IDataObject[1];
            for (int i = items.Next(1, rgelt, out num);
                (ErrorHandler.Succeeded(i) && (num > 0)) && (rgelt[0] != null);
                i = items.Next(1, rgelt, out num))
            {
                string displayName;
                var hr = (vsToolbox2 as IVsToolbox3).GetItemDisplayName(rgelt[0], out displayName);
                ErrorHandler.ThrowOnFailure(hr);

                if (displayName.Equals(toolboxItemString, StringComparison.OrdinalIgnoreCase))
                {
                    exist = true;
                    break;
                }
            }

            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
          string.Format("VerifyToolboxItemExist {0}: {1}", toolboxItemString, exist));

            return exist;
        }

        /// <summary>
        /// Add the toolbox item. 
        /// </summary>
        void AddToolboxItem()
        {
            LogEntry(__ACTIVITYLOG_ENTRYTYPE.ALE_INFORMATION,
            string.Format("Entering AddToolboxItem for: {0}", toolboxItemString));


            var toolboxData = new Microsoft.VisualStudio.Shell.OleDataObject();

            tooltipStream = FormatTooltipData(toolboxTooltipString, toolboxDescriptionString);

            // Set the tooltip.
            toolboxData.SetData("VSToolboxTipInfo", tooltipStream);

            // Set the Drag-Drop text.
            toolboxData.SetData(DataFormats.Text, toolboxItemTextString);

            TBXITEMINFO[] itemInfo = new TBXITEMINFO[1];
            itemInfo[0].bstrText = toolboxItemString;
            itemInfo[0].hBmp = IntPtr.Zero;
            itemInfo[0].dwFlags = (uint)__TBXITEMINFOFLAGS.TBXIF_DONTPERSIST;

            ErrorHandler.ThrowOnFailure(
                vsToolbox2.AddItem(toolboxData, itemInfo, toolboxTabString));
        }

        /// <summary>
        /// Format the tooltip.
        /// </summary>
        private Stream FormatTooltipData(string tooltip, string description)
        {
            const string NameHeader = "Name:";
            const string DescriptionHeader = "Description:";

            char ch = (char)(1 + NameHeader.Length + tooltip.Length);
            string str = ch.ToString() + NameHeader + tooltip;

            if (!string.IsNullOrEmpty(description))
            {
                ch = (char)(1 + DescriptionHeader.Length + description.Length);

                str += ch.ToString() + DescriptionHeader + description;
            }
            str += '\0';

            return SaveStringToStream(str);
        }

        private Stream SaveStringToStream(string value)
        {
            byte[] bytes = new UnicodeEncoding().GetBytes(value);
            MemoryStream stream = null;
            if ((bytes != null) && (bytes.Length > 0))
            {
                stream = new MemoryStream((int)bytes.Length);
                stream.Write(bytes, 0, (int)bytes.Length);
                stream.Flush();
            }
            else
            {
                stream = new MemoryStream();
            }
            stream.WriteByte((byte)0);
            stream.WriteByte((byte)0);
            stream.Flush();
            stream.Position = 0L;
            return stream;

        }
        #endregion

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing && this.tooltipStream != null)
            {
                this.tooltipStream.Dispose();
            }
        }

    }
}

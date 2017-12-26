/************************************* Module Header **************************************\
* Module Name:  Ribbon.cs
* Project:      CSOutlookRibbonXml
* Copyright (c) Microsoft Corporation.
* 
* The CSOutlookRibbonXml sample demonstrates how to use Ribbon XML to create customized 
* Ribbon for Outlook 2007 inspectors.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using System.Text;
using System.IO;
using System.Linq;
using System.Drawing;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Windows.Forms;
using Microsoft.Office.Interop.Outlook;
using Office = Microsoft.Office.Core;
using System.Diagnostics;
#endregion


namespace CSOutlookRibbonXml
{
    [ComVisible(true)]
    public class Ribbon : Office.IRibbonExtensibility
    {
        #region Private members
        /// <summary>
        /// The reference to the IRibbonUI interface.
        /// </summary>
        private Office.IRibbonUI ribbon;

        #endregion

        #region Private inner class

        private class Settings
        {
            /// <summary>
            /// Inspector - Settings pair.
            /// </summary>
            private static Dictionary<Inspector, Settings> RibbonSettings = 
                new Dictionary<Inspector, Settings>();

            public bool GroupThreeVisible = true;
            public bool MySecondTabVisible = false;
            public string TxtEditValue = "This is a label.";

            /// <summary>
            /// Get the Inspector specific ribbon item setting.
            /// </summary>
            /// <param name="inspector">The target Inspector.</param>
            /// <returns>Setting associated with the Inspector.</returns>
            public static Settings GetSettings(Inspector inspector)
            {
                if (!RibbonSettings.ContainsKey(inspector))
                {
                    Settings s = new Settings();
                    RibbonSettings.Add(inspector, s);
                    ((InspectorEvents_10_Event)inspector).Close +=
                        new InspectorEvents_10_CloseEventHandler(Inspector_Close);
                    return s;
                }

                return RibbonSettings[inspector];
            }

            public static void Inspector_Close()
            {
                foreach (Inspector inspector in RibbonSettings.Keys)
                {
                    int i;

                    for (i = 1; i <= inspector.Application.Inspectors.Count; i++)
                    {
                        if (inspector.Application.Inspectors[i].Equals(inspector))
                        {
                            break;
                        }
                    }

                    if (i > inspector.Application.Inspectors.Count)
                    {
                        RibbonSettings.Remove(inspector);
                        Marshal.ReleaseComObject(inspector);
                        GC.Collect();
                        break;
                    }
                }
            }
        }

        #endregion

        #region .ctor
        public Ribbon()
        {
        }
        #endregion

        #region IRibbonExtensibility Members

        /// <summary>
        /// Outlook will call this method to get the Ribbon XML.
        /// </summary>
        public string GetCustomUI(string ribbonID)
        {
            // We will show our customized Ribbon on the following types of
            // inspectors
            if (ribbonID == "Microsoft.Word.Document" ||
                ribbonID == "Microsoft.Outlook.Mail.Read" ||
                ribbonID == "Microsoft.Outlook.Mail.Compose" ||
                ribbonID == "Microsoft.Outlook.MeetingRequest.Read" ||
                ribbonID == "Microsoft.Outlook.MeetingRequest.Send" ||
                ribbonID == "Microsoft.Outlook.Appointment" ||
                ribbonID == "Microsoft.Outlook.Contact" ||
                ribbonID == "Microsoft.Outlook.Task")
                return GetResourceText("CSOutlookRibbonXml.Ribbon.xml");
            else
                return null;
        }

        #endregion

        #region Ribbon Callbacks
        // Create callback methods here. For more information about adding callback methods, 
        // select the Ribbon XML item in Solution Explorer and then press F1

        /// <summary>
        /// Outlook will call this method when the ribbon is being loaded.
        /// </summary>
        /// <param name="ribbonUI">Reference to the IRibbonUI interface.</param>
        public void Ribbon_Load(Office.IRibbonUI ribbonUI)
        {
            this.ribbon = ribbonUI;
        }

        /// <summary>
        /// Outlook will call this method to get the content XML for dynamic menus.
        /// We can generate different XML contents at runtime so the menu can be *dynamic*.
        /// </summary>
        /// <param name="control">The control whose content is being retrieved.</param>
        /// <returns>Content XML for the control.</returns>
        public string Ribbon_GetContent(Office.IRibbonControl control)
        {
            StringBuilder sb = new StringBuilder();

            switch (control.Id)
            {
                case "mnuSample":
                    sb.Append("<menu xmlns=\"http://schemas.microsoft.com/office/2006/01/customui\">");
                    sb.Append("<dynamicMenu id=\"mnuSubMenu\" label=\"Sub Menu\" getContent=\"Ribbon_GetContent\"/>");
                    sb.Append("<button id=\"btnItem1\" label=\"Item 1\" />");
                    sb.Append("<menuSeparator id=\"separator2\" />");
                    sb.Append("<button id=\"btnItem2\" label=\"Item 2\" />");
                    sb.Append("</menu>");
                    break;

                case "mnuSubMenu":
                    sb.Append("<menu xmlns=\"http://schemas.microsoft.com/office/2006/01/customui\">");
                    sb.Append("<button id=\"btnSubItem1\" label=\"Sub Item 1\"/>");
                    sb.Append("<button id=\"btnSubItem2\" label=\"Sub Item 2\"/>");
                    sb.Append("<button id=\"btnSubItem3\" label=\"Sub Item 3\"/>");
                    sb.Append("</menu>");
                    break;
            }

            return sb.ToString();
        }

        /// <summary>
        /// Outlook will call this method to get the item image.
        /// </summary>
        /// <param name="imageName">Name of the image.</param>
        /// <returns>The Bitmap object linked to the imageName.</returns>
        public Bitmap LoadImage(string imageName)
        {
            Bitmap bmp = null;

            switch (imageName.ToLowerInvariant())
            {
                case "globe":
                    bmp = Properties.Resources.Globe;
                    break;

                case "audiocd":
                    bmp = Properties.Resources.AudioCD;
                    break;

                case "blankcd":
                    bmp = Properties.Resources.blank_cd;
                    break;

                case "audiodvd":
                    bmp = Properties.Resources.audiodvd;
                    break;

                case "bdmovie":
                    bmp = Properties.Resources.Blu_RayMovieDisk;
                    break;

                case "burncd":
                    bmp = Properties.Resources.BurnCD;
                    break;

                case "audiocdplus":
                    bmp = Properties.Resources.AudioCDPlus;
                    break;

                case "vcd":
                    bmp = Properties.Resources.CD_V;
                    break;
            }

            return bmp;
        }

        /// <summary>
        /// Outlook will call this method to get the item's Visible value.
        /// </summary>
        /// <param name="control"></param>
        /// <returns></returns>
        public bool GetVisible(Office.IRibbonControl control)
        {
            bool visible = false;
            Inspector inspector = (Inspector)control.Context;

            switch (control.Id)
            {
                case "grpThree":
                    visible = Settings.GetSettings(inspector).GroupThreeVisible;
                    break;

                case "mySecondTab":
                    visible = Settings.GetSettings(inspector).MySecondTabVisible;
                    break;

                case "grpMail":
                    visible = inspector.CurrentItem is MailItem;
                    break;

                case "grpAppointmentItem":
                    visible = inspector.CurrentItem is AppointmentItem;
                    break;

                case "grpTaskItem":
                    visible = inspector.CurrentItem is TaskItem;
                    break;

                case "grpContactItem":
                    visible = inspector.CurrentItem is ContactItem;
                    break;
            }

            return visible;
        }

        /// <summary>
        /// Outlook will call this method to get the "Pressed" status of the chkShowGroup
        /// control. See Ribbon.xml for the chkShowGroup control.
        /// </summary>
        public bool chkShowGroup_GetPressed(Office.IRibbonControl control)
        {
            return Settings.GetSettings((Inspector)control.Context).GroupThreeVisible;
        }

        /// <summary>
        /// Outlook will call this method when the chkShowGroup is clicked.
        /// </summary>
        public void chkShowGroup_OnAction(Office.IRibbonControl control, bool pressed)
        {
            Settings.GetSettings((Inspector)control.Context).GroupThreeVisible = pressed;
            this.ribbon.InvalidateControl("grpThree");
        }

        /// <summary>
        /// Outlook will call this method when Pressed status of the tbSecondTab is needed.
        /// </summary>
        public bool tbSecondTab_GetPressed(Office.IRibbonControl control)
        {
            return Settings.GetSettings((Inspector)control.Context).MySecondTabVisible;
        }

        /// <summary>
        /// Outlook will call this method when the tbSecondTab is clicked.
        /// </summary>
        public void tbSecondTab_OnAction(Office.IRibbonControl control, bool pressed)
        {
            Settings.GetSettings((Inspector)control.Context).MySecondTabVisible = pressed;
            this.ribbon.InvalidateControl("mySecondTab");
        }

        /// <summary>
        /// Outlook will call this method when the content of txtEdit is changed.
        /// </summary>
        public void txtEdit_OnChange(Office.IRibbonControl control, string text)
        {
            Settings.GetSettings((Inspector)control.Context).TxtEditValue = text;
            this.ribbon.InvalidateControl("lblSample");
        }

        /// <summary>
        /// Outlook will call this method when the label text of lblSample is needed.
        /// </summary>
        public string lblSample_GetLabel(Office.IRibbonControl control)
        {
            return Settings.GetSettings((Inspector)control.Context).TxtEditValue;
        }

        /// <summary>
        /// Outlook will call this method when the btnWeb button is clicked.
        /// </summary>
        public void btnWeb_OnAction(Office.IRibbonControl control)
        {
            Process.Start("http://cfx.codeplex.com");
        }

        /// <summary>
        /// Outlook will call this method when the split button itself is clicked.
        /// </summary>
        public void splitButton_Click(Office.IRibbonControl control)
        {
            // SplitButton itself clicked
            MessageBox.Show("SplitButton itself clicked",
                            "Event",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
        }

        /// <summary>
        /// Outlook will call this method when the btnAlign* buttons are clicked.
        /// </summary>
        /// <param name="control"></param>
        public void btnAlign_Click(Office.IRibbonControl control)
        {
            string lbl = null;

            switch (control.Id)
            {
                case "btnAlignLeft":
                    lbl = "Left";
                    break;

                case "btnAlignCenter":
                    lbl = "Center";
                    break;

                case "btnAlignRight":
                    lbl = "Right";
                    break;
            }

            MessageBox.Show(lbl,
                            "SplitButton Item Clicked",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Information);
        }

        /// <summary>
        /// Outlook will call this method when the dialog launcher in group one is clicked.
        /// </summary>
        public void grpOneDlgLauncher_OnAction(Office.IRibbonControl control)
        {
            using (ColorDialog colorDlg = new ColorDialog())
            {
                if (colorDlg.ShowDialog() == DialogResult.OK)
                    MessageBox.Show(colorDlg.Color.ToString(),
                                    "Selected Color",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// Outlook will call this method when the lblMainMode label is needed.
        /// </summary>
        public string lblMainMode_GetLabel(Office.IRibbonControl control)
        {
            MailItem m = ((Inspector)control.Context).CurrentItem as MailItem;

            if (m != null)
            {
                return string.Format("Current Mode: {0}",
                    m.EntryID == null ? "Drafting" : "Reading");
            }

            return string.Empty;
        }

        #endregion

        #region Helpers

        /// <summary>
        /// Extract text from resource.
        /// </summary>
        /// <param name="resourceName">The name of the resource.</param>
        /// <returns>Resource text.</returns>
        private static string GetResourceText(string resourceName)
        {
            Assembly asm = Assembly.GetExecutingAssembly();
            string[] resourceNames = asm.GetManifestResourceNames();
            for (int i = 0; i < resourceNames.Length; ++i)
            {
                if (string.Compare(resourceName, resourceNames[i], StringComparison.OrdinalIgnoreCase) == 0)
                {
                    using (StreamReader resourceReader = new StreamReader(asm.GetManifestResourceStream(resourceNames[i])))
                    {
                        if (resourceReader != null)
                        {
                            return resourceReader.ReadToEnd();
                        }
                    }
                }
            }
            return null;
        }

        #endregion
    }
}

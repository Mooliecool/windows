/****************************** Module Header ******************************\
* Module Name:  ReferencePageDialog.cs
* Project:      CSVSPackageAddReferenceTab
* Copyright (c) Microsoft Corporation.
* 
* Visual Studio supports to extend the Add Reference dialog and add custom
* tab page into the dialog.
*
* This sample demostrate you how to add a custom .NET user control as a tab 
* page into the add reference dialog, and how to enable select button and
*  handle item selection events.
*
* All the sample code is based on MPF.
*
* The sample is initiated by the thread on the forum:
* http://social.msdn.microsoft.com/Forums/en-US/vsx/thread/ddb0f935-b8ac-400d-9e3d-64d74be85031
* 
* History:
* * 1/12/2010 1:00 AM Hongye Sun Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.VisualStudio.Shell.Interop;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio;
using System.Reflection;

namespace Microsoft.CSVSPackageAddReferenceTab
{
    /// <summary>
    /// A user control acting as a reference tab page
    /// </summary>
    [ComVisible(true)]
    [Guid(GuidList.guidReferencePageString)]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public partial class ReferencePageDialog : UserControl, IVsComponentSelectorData, IVsComponentUser,
                                               IVsComponentSelectorDlg2
    {
        /// <summary>
        /// The message of size change
        /// </summary>
        const int WM_SIZE = 5;
        bool initialized = false;

        private static ReferencePageDialog _dialog = null;
        /// <summary>
        /// The handle of the ReferencePageDialog
        /// </summary>
        public static IntPtr DialogPointer
        {
            get
            {
                if (_dialog == null)
                {
                    _dialog = new ReferencePageDialog();
                }
                return _dialog.Handle;
            }
        }

        public ReferencePageDialog()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Initialize the items in the list box
        /// </summary>
        protected virtual void InitializeItems() 
        {
            if (initialized) return;


            ListViewItem lvItem = 
                new ListViewItem(this.GetType().Assembly.GetName().Name);
            lvItem.SubItems.Add(this.GetType().Assembly.Location);

            this.ReferenceListView.Items.Add(lvItem);

            AssemblyName[] assemblies =
                this.GetType().Assembly.GetReferencedAssemblies();
            
            foreach (AssemblyName assembly in assemblies)
            {
                Assembly a = Assembly.Load(assembly);
                lvItem = new ListViewItem(assembly.Name);
                lvItem.SubItems.Add(a.Location);
                this.ReferenceListView.Items.Add(lvItem);
            }

            initialized = true;
        }

        /// <summary>
        /// Indicates whether or not the select button should be enabled
        /// </summary>
        /// <returns></returns>
        protected virtual bool GetCanSelect() { return true; }

        /// <summary>
        /// Set the selection mode of the tab page, single or multiple.
        /// </summary>
        /// <param name="multiSelect"></param>
        protected virtual void SetSelectionMode(bool multiSelect) 
        {
            this.ReferenceListView.MultiSelect = multiSelect;
        }

        /// <summary>
        /// Clear the slection on the tab page
        /// </summary>
        protected virtual void ClearSelection() 
        {
            this.ReferenceListView.Clear();
        }

        /// <summary>
        /// Get the user selected items from the list box
        /// </summary>
        /// <returns>
        /// The list of SCOMPONENTSELECTORDATA of selected items
        /// </returns>
        protected virtual VSCOMPONENTSELECTORDATA[] GetSelection() 
        {
            if (this.ReferenceListView.SelectedItems.Count == 0)
                return null;

            List<VSCOMPONENTSELECTORDATA> items = new List<VSCOMPONENTSELECTORDATA>();
            foreach (ListViewItem lvItem in this.ReferenceListView.SelectedItems)
            {
                VSCOMPONENTSELECTORDATA item = new VSCOMPONENTSELECTORDATA();
                item.bstrTitle = lvItem.SubItems[0].Text;
                item.bstrFile = lvItem.SubItems[1].Text;
                items.Add(item);
            }
            return items.ToArray(); ; 
        
        }

        /// <summary>
        /// Process the message which is sent from parent dialog
        /// </summary>
        /// <param name="m">Message</param>
        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case VSConstants.CPPM_INITIALIZELIST:
                    HandleInitializeItemsMessage(m);
                    break;
                case VSConstants.CPPM_QUERYCANSELECT:
                    HandleQueryCanSelectMessage(m);
                    break;
                case VSConstants.CPPM_SETMULTISELECT:
                    HandleSetMultiSelectMessage(m);
                    break;
                case VSConstants.CPPM_CLEARSELECTION:
                    HandleClearSelectionMessage(m);
                    break;
                case VSConstants.CPPM_GETSELECTION:
                    HandleGetSelectionMessage(m);
                    break;
                case WM_SIZE:
                    HandleSizeMessage(m);
                    break;
                default:
                    base.WndProc(ref m);
                    break;
            }
        }

        /// <summary>
        /// Handle the initialize message
        /// </summary>
        /// <param name="m"></param>
        void HandleInitializeItemsMessage(Message m)
        {
            InitializeItems();
        }

        /// <summary>
        /// Handle the QueryCanSelect message
        /// </summary>
        /// <param name="m"></param>
        void HandleQueryCanSelectMessage(Message m)
        {
            Marshal.StructureToPtr(
                Convert.ToByte(GetCanSelect()),
                m.LParam,
                false);
        }

        /// <summary>
        /// Handle the multiple select message
        /// </summary>
        /// <param name="m"></param>
        void HandleSetMultiSelectMessage(Message m)
        {
            SetSelectionMode(Convert.ToBoolean((byte)m.LParam));
        }

        /// <summary>
        /// Handle the clear selection message
        /// </summary>
        /// <param name="m"></param>
        void HandleClearSelectionMessage(Message m)
        {
            ClearSelection();
        }

        /// <summary>
        /// Handle the get selection message
        /// </summary>
        /// <param name="m"></param>
        void HandleGetSelectionMessage(Message m)
        {
            VSCOMPONENTSELECTORDATA[] items = GetSelection();
            int count = items != null ? items.Length : 0;

            // Write item's count to m.WParam
            Marshal.WriteInt32(m.WParam, count);
            if (count > 0)
            {
                // Allocate the memory for VSCOMPONENTSELECTORDATA pointers' array
                IntPtr ppItems = Marshal.AllocCoTaskMem(
                  count * Marshal.SizeOf(typeof(IntPtr)));
                for (int i = 0; i < count; i++)
                {
                    // Allocate memory for VSCOMPONENTSELECTORDATA data 
                    IntPtr pItem = Marshal.AllocCoTaskMem(
                            Marshal.SizeOf(typeof(VSCOMPONENTSELECTORDATA)));

                    // Write the pointer into pointers' array in sequence
                    Marshal.WriteIntPtr(
                        ppItems, 
                        i * Marshal.SizeOf(typeof(IntPtr)),
                        pItem);

                    // Write VSCOMPONENTSELECTORDATA into pointer
                    Marshal.StructureToPtr(items[i], pItem, false);
                }

                // Write the VSCOMPONENTSELECTORDATA** into m.LParam
                Marshal.WriteIntPtr(m.LParam, ppItems);
            }
        }

        // Handle Size message to resize the tab page
        private void HandleSizeMessage(Message m)
        {
            IntPtr parentHwnd = NativeMethods.GetParent(Handle);

            if (parentHwnd != IntPtr.Zero)
            {
                IntPtr grandParentHwnd = NativeMethods.GetParent(parentHwnd);

                if (grandParentHwnd != IntPtr.Zero)
                {
                    NativeMethods.Rect parentClientRect;
                    NativeMethods.Rect grandParentClientRect;

                    NativeMethods.GetClientRect(parentHwnd, out parentClientRect);
                    NativeMethods.GetClientRect(grandParentHwnd, out grandParentClientRect);

                    int width = grandParentClientRect.Width;
                    int height = grandParentClientRect.Height;

                    if ((parentClientRect.Width != width) || (parentClientRect.Height != height))
                    {
                        NativeMethods.MoveWindow(parentHwnd, 0, 0, width, height, true);
                        NativeMethods.MoveWindow(Handle, 0, 0, width, height, true);
                    }
                }
            }

        }

        #region IVsComponentSelectorData Members

        public int GetData(VSCOMPONENTSELECTORDATA[] pData)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region IVsComponentUser Members

        public int AddComponent(VSADDCOMPOPERATION dwAddCompOperation, uint cComponents, IntPtr[] rgpcsdComponents, IntPtr hwndPickerDlg, VSADDCOMPRESULT[] pResult)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region IVsComponentSelectorDlg2 Members

        public int ComponentSelectorDlg2(uint grfFlags, IVsComponentUser pUser, uint cComponents, IntPtr[] rgpcsdComponents, string lpszDlgTitle, string lpszHelpTopic, ref uint pxDlgSize, ref uint pyDlgSize, uint cTabInitializers, VSCOMPONENTSELECTORTABINIT[] rgcstiTabInitializers, ref Guid pguidStartOnThisTab, string pszBrowseFilters, ref string pbstrBrowseLocation)
        {
            throw new NotImplementedException();
        }

        #endregion


        private void ReferenceListView_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            IntPtr parentHwnd = NativeMethods.GetParent(Handle);
            IntPtr grandParentHwnd = NativeMethods.GetParent(parentHwnd);
            IntPtr greatGrandParentHwnd = NativeMethods.GetParent(grandParentHwnd);

            // Send CPDN_SELCHANGED message to the great grandparent dialog to notify that 
            // selection is changed.
            NativeMethods.SendMessage(greatGrandParentHwnd, VSConstants.CPDN_SELCHANGED, IntPtr.Zero, Handle);
        }

        private void ReferenceListView_DoubleClick(object sender, EventArgs e)
        {
            IntPtr parentHwnd = NativeMethods.GetParent(Handle);
            IntPtr grandParentHwnd = NativeMethods.GetParent(parentHwnd);
            IntPtr greatGrandParentHwnd = NativeMethods.GetParent(grandParentHwnd);

            // Send CPDN_SELDBLCLICK message to the great grandparent dialog to notify that
            // user double click on the selected item.
            NativeMethods.SendMessage(greatGrandParentHwnd, VSConstants.CPDN_SELDBLCLICK, IntPtr.Zero, Handle);
        }
    }
}

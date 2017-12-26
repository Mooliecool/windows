/****************************** Module Header ******************************\
* Module Name:  OpenImageHandler.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class OpenImageHandler implements the interface IDocHostUIHandler.ShowContextMenu
* method. For other methods in the interface IDocHostUIHandler, just return 1 which means
* the default handler will be used.
*  
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
using System.Drawing;
using System.Windows.Forms;
using CSCustomIEContextMenu.NativeMethods;
using mshtml;
using SHDocVw;

namespace CSCustomIEContextMenu
{
    class OpenImageHandler : NativeMethods.IDocHostUIHandler, IDisposable
    {

        private bool disposed = false;

        // The IE instance that hosts this WebBrowser Control.
        public InternetExplorer host;

        // The custom context menu. 
        private ContextMenuStrip contextMenu;
        private ToolStripMenuItem menuItem;


        /// <summary>
        /// Initialize the handler.
        /// </summary>
        public OpenImageHandler(InternetExplorer host)
        {
            this.host = host;

            contextMenu = new ContextMenuStrip();
            menuItem = new ToolStripMenuItem();

            menuItem.Size = new Size(180, 100);
            menuItem.Text = "Open image in new tab";
            menuItem.Click += new EventHandler(menuItem_Click);

            contextMenu.Items.Add(menuItem);
        }

        void menuItem_Click(object sender, EventArgs e)
        {

            try
            {
                (host.Document as HTMLDocument).parentWindow.open(contextMenu.Tag as string);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        #region IDocHostUIHandler

        /// <summary>
        /// Show custom Context Menu for Image.
        /// </summary>
        /// <param name="dwID">
        /// A DWORD that specifies the identifier of the shortcut menu to be displayed. 
        /// See NativeMethods.CONTEXT_MENU_CONST.
        /// </param>
        /// <param name="pt">
        /// The screen coordinates for the menu.
        /// </param>
        /// <param name="pcmdtReserved"></param>
        /// <param name="pdispReserved">
        /// The object at the screen coordinates specified in ppt. This enables a host to
        /// pass particular objects, such as anchor tags and images, to provide more 
        /// specific context.
        /// </param>
        /// <returns>
        /// Return 0 means that host displayed its UI. MSHTML will not attempt to display its UI. 
        /// </returns>
        public int ShowContextMenu(int dwID, POINT pt, object pcmdtReserved, object pdispReserved)
        {
            if (dwID == NativeMethods.CONTEXT_MENU_CONST.CONTEXT_MENU_IMAGE)
            {
                var img = pdispReserved as IHTMLImgElement;
                if (img != null)
                {
                    contextMenu.Tag = img.src;
                    contextMenu.Show(pt.x, pt.y);
                    return 0;
                }
            }
            return 1;
        }

        public int GetHostInfo(DOCHOSTUIINFO info)
        {
            return 1;
        }

        public int ShowUI(int dwID, IOleInPlaceActiveObject activeObject, IOleCommandTarget commandTarget, IOleInPlaceFrame frame, IOleInPlaceUIWindow doc)
        {
            return 1;
        }

        public int HideUI()
        {
            return 1;
        }

        public int UpdateUI()
        {
            return 1;
        }

        public int EnableModeless(bool fEnable)
        {
            return 1;
        }

        public int OnDocWindowActivate(bool fActivate)
        {
            return 1;
        }

        public int OnFrameWindowActivate(bool fActivate)
        {
            return 1;
        }

        public int ResizeBorder(COMRECT rect, IOleInPlaceUIWindow doc, bool fFrameWindow)
        {
            return 1;
        }

        public int TranslateAccelerator(ref MSG msg, ref Guid group, int nCmdID)
        {
            return 1;
        }

        public int GetOptionKeyPath(string[] pbstrKey, int dw)
        {
            return 1;
        }

        public int GetDropTarget(IOleDropTarget pDropTarget, out IOleDropTarget ppDropTarget)
        {
            ppDropTarget = null;
            return 1;
        }

        public int GetExternal(out object ppDispatch)
        {
            ppDispatch = null;
            return 1;
        }

        public int TranslateUrl(int dwTranslate, string strURLIn, out string pstrURLOut)
        {
            pstrURLOut = string.Empty;
            return 1;
        }

        public int FilterDataObject(System.Runtime.InteropServices.ComTypes.IDataObject pDO, out System.Runtime.InteropServices.ComTypes.IDataObject ppDORet)
        {
            ppDORet = null;
            return 1;
        }
        #endregion

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed) return;

            if (disposing)
            {
                // Clean up all managed resources.
                if (contextMenu != null)
                {
                    contextMenu.Dispose();
                }

                if (menuItem != null)
                {
                    menuItem.Dispose();
                }
            }
            disposed = true;
        }
    }
}

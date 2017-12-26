/************************************* Module Header **************************************\
* Module Name:	ThisDocument.cs
* Project:		CSVstoWordDocument
* Copyright (c) Microsoft Corporation.
* 
* The CSWordDocument project provides the examples on how manipulate Word 2007 Content Controls 
* in a VSTO document-level project
 
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
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml.Linq;
using Microsoft.VisualStudio.Tools.Applications.Runtime;
using Office = Microsoft.Office.Core;
using Word = Microsoft.Office.Interop.Word;
using System.Diagnostics;
#endregion


namespace CSVstoWordDocument
{
    public partial class ThisDocument
    {
        private void ThisDocument_Startup(object sender, System.EventArgs e)
        {

            this.ActionsPane.Visible = true;
            PaneControl p = new PaneControl();
            this.ActionsPane.Controls.Add(p);
        }

        private void ThisDocument_Shutdown(object sender, System.EventArgs e)
        {
        }

        #region VSTO Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InternalStartup()
        {
            this.ContentControlBeforeStoreUpdate += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlBeforeStoreUpdateEventHandler(this.ThisDocument_ContentControlBeforeStoreUpdate);
            this.ContentControlAfterAdd += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlAfterAddEventHandler(this.ThisDocument_ContentControlAfterAdd);
            this.ContentControlOnEnter += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlOnEnterEventHandler(this.ThisDocument_ContentControlOnEnter);
            this.Shutdown += new System.EventHandler(this.ThisDocument_Shutdown);
            this.ContentControlOnExit += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlOnExitEventHandler(this.ThisDocument_ContentControlOnExit);
            this.ContentControlBeforeDelete += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlBeforeDeleteEventHandler(this.ThisDocument_ContentControlBeforeDelete);
            this.Startup += new System.EventHandler(this.ThisDocument_Startup);
            this.ContentControlBeforeContentUpdate += new Microsoft.Office.Interop.Word.DocumentEvents2_ContentControlBeforeContentUpdateEventHandler(this.ThisDocument_ContentControlBeforeContentUpdate);
        }

        #endregion

        #region Event Handlers

        private void ThisDocument_ContentControlAfterAdd(Microsoft.Office.Interop.Word.ContentControl NewContentControl, bool InUndoRedo)
        {
            Debug.Print("ThisDocument_ContentControlAfterAdd :" + NewContentControl.Title);
        }

        private void ThisDocument_ContentControlBeforeContentUpdate(Microsoft.Office.Interop.Word.ContentControl ContentControl, ref string Content)
        {
            Debug.Print("ThisDocument_ContentControlBeforeContentUpdate :" + ContentControl.Title);
        }

        private void ThisDocument_ContentControlBeforeDelete(Microsoft.Office.Interop.Word.ContentControl OldContentControl, bool InUndoRedo)
        {
            Debug.Print("ThisDocument_ContentControlBeforeDelete :" + OldContentControl.Title);
        }

        private void ThisDocument_ContentControlBeforeStoreUpdate(Microsoft.Office.Interop.Word.ContentControl ContentControl, ref string Content)
        {
            Debug.Print("ThisDocument_ContentControlBeforeStoreUpdate :" + ContentControl.Title);
        }

        private void ThisDocument_ContentControlOnEnter(Microsoft.Office.Interop.Word.ContentControl ContentControl)
        {
            Debug.Print("ThisDocument_ContentControlOnEnter :" + ContentControl.Title);
        }

        private void ThisDocument_ContentControlOnExit(Microsoft.Office.Interop.Word.ContentControl ContentControl, ref bool Cancel)
        {
            Debug.Print("ThisDocument_ContentControlOnExit :" + ContentControl.Title);
        }

        #endregion
    }
}

/************************************* Module Header **************************************\
* Module Name:  MyToolWindow.cs
* Project:      CSVSPackageState
* Copyright (c) Microsoft Corporation.
* 
* The Visual Studio package state sample demostrate the state persisting for
* application options and show object states in properties window.
*
* The sample doesn't include the state management for solution and project,
* which will be included in project package sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/6/2009 1:00 PM Hongye Sun Created
\******************************************************************************************/

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;

namespace AllInOne.CSVSPackageState
{
    /// <summary>
    /// This class implements the tool window exposed by this package and hosts a user control.
    ///
    /// In Visual Studio tool windows are composed of a frame (implemented by the shell) and a pane, 
    /// usually implemented by the package implementer.
    ///
    /// This class derives from the ToolWindowPane class provided from the MPF in order to use its 
    /// implementation of the IVsWindowPane interface.
    /// </summary>
    [Guid("37fdbfbb-be1a-4e66-93a1-30ab34e5865b")]
    public class MyToolWindow : ToolWindowPane
    {
        // This is the user control hosted by the tool window; it is exposed to the base class 
        // using the Window property. Note that, even if this class implements IDispose, we are
        // not calling Dispose on this object. This is because ToolWindowPane calls Dispose on 
        // the object returned by the Window property.
        private MyControl control;

        private ITrackSelection trackSel;

        private SelectionContainer selContainer;



        /// <summary>
        /// Standard constructor for the tool window.
        /// </summary>
        public MyToolWindow() :
            base(null)
        {
            this.Caption = Resources.ToolWindowTitle;

            control = new MyControl();
        }

        /// <summary>
        /// This property returns the handle to the user control that should
        /// be hosted in the Tool Window.
        /// </summary>
        override public IWin32Window Window
        {
            get
            {
                return (IWin32Window)control;
            }
        }

        /// <summary>
        /// The TrackSelection property uses GetService to obtain an STrackSelection 
        /// service, which provides an ITrackSelection interface. 
        /// </summary>
        private ITrackSelection TrackSelection
        {
            get
            {
                if (trackSel == null)
                    trackSel =
                       GetService(typeof(STrackSelection)) as ITrackSelection;
                return trackSel;
            }
        }

        /// <summary>
        /// The UpdateSelection method tells the Properties window to display the public 
        /// properties of the tool window pane.
        /// </summary>
        public void UpdateSelection()
        {
            ITrackSelection track = TrackSelection;
            if (track != null)
                track.OnSelectChange((ISelectionContainer)selContainer);
        }

        /// <summary>
        /// The OnToolWindowCreated event handler and SelectList method together create a list 
        /// of selected objects that contains only the tool window pane object itself.
        /// </summary>
        public void SelectList(ArrayList list)
        {
            selContainer = new SelectionContainer(true, false);
            selContainer.SelectableObjects = list;
            selContainer.SelectedObjects = list;
            UpdateSelection();
        }

        /// <summary>
        /// The OnToolWindowCreated event handler and SelectList method together create a list 
        /// of selected objects that contains only the tool window pane object itself.
        /// </summary>
        public override void OnToolWindowCreated()
        {
            PropertyObject obj = new PropertyObject();
            this.control.Object = obj;

            ArrayList listObjects = new ArrayList();
            listObjects.Add(obj);
            SelectList(listObjects);
        }

    }
}

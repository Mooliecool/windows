/*******************************************************************************
 * File: FetchTimerForm.cs
 *
 * Description: 
 * 
 * The sample demonstrates how to cache (prefetch) and retrieve properties, and 
 * is also a simple tool that enables you to see performance data when retrieving
 *  current and cached properties.
 * 
 * To use the application, run it, set options, and leave it in the foreground. 
 * Then move the cursor over any element on the screen and press Alt+G. The 
 * application displays information about the element, along with performance data. 
 * 
 * Note that if you select Descendants in the scope, it may take some time to 
 * cache properties for complex elements such as the toolbar docking area in 
 * Microsoft Visual Studio.
 * 
 * It is recommended that you try testing each element several times. Initial runs
 * may take longer, and you may obtain anomalous results as the Windows scheduler 
 * interrupts the caching process.
 * 
 * Try caching for both Win32 controls and WPF controls, and note that the 
 * advantage is generally much greater for WPF controls.
 *
 *     
 *  This file is part of the Microsoft Windows SDK Code Samples.
 * 
 *  Copyright (C) Microsoft Corporation.  All rights reserved.
 * 
 * This source code is intended only as a supplement to Microsoft
 * Development Tools and/or on-line documentation.  See these other
 * materials for detailed information regarding Microsoft code samples.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 ******************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Automation;
using System.Diagnostics;
using System.Threading;
using System.Security.Permissions;

namespace FetchTimer
{
    public partial class FetchTimerForm : Form
    {
        // Member variables.
        System.Windows.Point targetPoint;
        TreeScope cacheScope;
        AutomationElementMode elementMode;
        Thread workerThread;
        UIAutomationFetcher fetcher;

        public delegate void OutputDelegate(string results);
        public OutputDelegate outputMethodInstance;

        /// <summary>
        /// The entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FetchTimerForm());
        }

        /// <summary>
        /// Constructor.
        /// </summary>
        public FetchTimerForm()
        {
            InitializeComponent();
            outputMethodInstance = new OutputDelegate(OutputResults);
        }


        /// <summary>
        /// Enables/disables Children check box when Descendants changed, since
        /// Descendants includes Children.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cbDescendants_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox box = sender as CheckBox;
            if (box.Checked)
            {
                cbChildren.Enabled = false;
            }
            else
            {
                cbChildren.Enabled = true;
            }
        }

        /// <summary>
        /// Prints information to the text box.
        /// </summary>
        /// <param name="output">String to print.</param>
        public void OutputResults(string output)
        {
            tbOutput.AppendText(output);
        }

        /// <summary>
        /// Clears the output box.
        /// </summary>
        /// <param name="sender">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        private void btnClear_Click(object sender, EventArgs e)
        {
            tbOutput.Clear();
        }

        /// <summary>
       /// Responds to button click; saves options and starts 
       /// the UI Automation worker thread.
       /// </summary>
       /// <param name="sender">Object that raised the event.</param>
       /// <param name="e">Event arguments.</param>
        private void btnProps_Click(object sender, EventArgs e)
        {
            // Convert Drawing.Point to Windows.Point.
            System.Drawing.Point drawingPoint = Cursor.Position;
            targetPoint = new System.Windows.Point(drawingPoint.X, drawingPoint.Y);

            // Save caching settings in member variables so UI isn't accessed 
            // directly by the other thread.

            if (rbFull.Checked)
            {
                elementMode = AutomationElementMode.Full;
            }
            else
            {
                elementMode = AutomationElementMode.None;
            }

            // For simplicity, always include Element in scope.
            cacheScope = TreeScope.Element;
            if (cbDescendants.Checked)
            {
                cacheScope |= TreeScope.Descendants;  
            }
            // Note: if descendants are specified, children 
            // are automatically included.
            else if (cbChildren.Checked)
            {
                cacheScope |= TreeScope.Children;
            }

            fetcher = new UIAutomationFetcher(this, targetPoint, 
                cacheScope, elementMode);

            // Start another thread to do the UI Automation work.
            ThreadStart threadDelegate = new ThreadStart(StartWorkerThread);
            workerThread = new Thread(threadDelegate);
            workerThread.Priority = ThreadPriority.Highest;
            workerThread.Start();
            OutputResults("Wait..." + Environment.NewLine);
        }

        /// <summary>
        /// Delegated method for ThreadStart.
        /// </summary>
        /// <remarks>
        /// UI Automation must be called on a separate thread if the client application
        /// itself might be a target.
        /// </remarks>
        void StartWorkerThread()
        {
            fetcher.DoWork();
        }

    
    }  // Form class

} // Namespace

/******************************************************************************
 * File: ExecuteScriptClient.cs
 *
 * Description: 
 * This is the client application used to start the target application 
 * and execute the generated script. The target application is the same one
 * used by the script generator.
 * 
 * See Client.cs in the ScriptGeneratorClient project for a full description 
 * of the sample code.
 *      
 * This file is part of the Microsoft Windows SDK Code Samples.
 * 
 * Copyright (C) Microsoft Corporation.  All rights reserved.
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
 *****************************************************************************/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows.Automation;
using System.Diagnostics;
using System.Threading;

namespace ExecuteScript
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// Interaction logic for the ExecuteScriptClient form.
    /// </summary>
    ///--------------------------------------------------------------------
    public partial class ExecuteScriptClient : Form
    {
        AutomationElement targetApp;

        ///--------------------------------------------------------------------
        /// <summary>
        /// The various permutations of client controls.
        /// </summary>
        /// <remarks>
        /// The state of the client application impacts which client controls 
        /// are available.
        /// </remarks>
        ///--------------------------------------------------------------------
        public enum ControlState
        {
            /// <summary>
            /// Initial state of the client.
            /// </summary>
            Initial,
            /// <summary>
            /// Target is available.
            /// </summary>
            TargetStarted
        };
        
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            ExecuteScriptClient executeScriptClient = new ExecuteScriptClient();
            executeScriptClient.StartPosition = FormStartPosition.Manual;
            executeScriptClient.Location = new Point(50, 50);
            Application.Run(executeScriptClient);
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Constructor
        /// </summary>
        /// <remarks>
        /// Initializes components.
        /// </remarks>
        ///--------------------------------------------------------------------
        public ExecuteScriptClient()
        {
            InitializeComponent();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Creates an instance of the Script class.
        /// </summary>
        /// <remarks>
        /// The Script.cs file is our generated script.
        /// It uses the support functions contained in ScriptFunctions.cs.
        /// </remarks>
        /// <param name="src">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void ExecuteScript_Click(object src, EventArgs e)
        {
            new Script(this);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Starts our target application.
        /// </summary>
        /// <param name="src">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void StartTarget_Click(object src, EventArgs e)
        {
            targetApp = StartTargetApp();
            if (targetApp == null)
            {
                throw new ApplicationException(
                    "Unable to start target application.");
            }
            OutputFeedback(
                "Target started; execute script." + Environment.NewLine);

            RegisterTargetCloseEventListener();

            int clientLocationTop = this.Top;
            int clientLocationRight = this.Right + 100;
            TransformPattern transformPattern =
                targetApp.GetCurrentPattern(TransformPattern.Pattern)
                as TransformPattern;
            transformPattern.Move(clientLocationRight, clientLocationTop);

            SetControls(ControlState.TargetStarted);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Register a window closed event listener for the target application.
        /// </summary>
        ///--------------------------------------------------------------------
        private void RegisterTargetCloseEventListener()
        {
            AutomationEventHandler targetCloseListener = 
                new AutomationEventHandler(OnTargetClosed);
            Automation.AddAutomationEventHandler(
                WindowPattern.WindowClosedEvent, 
                targetApp, 
                TreeScope.Element, 
                targetCloseListener);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Our window closed event handler for the target application.
        /// </summary>
        ///--------------------------------------------------------------------
        private void OnTargetClosed(object src, AutomationEventArgs e)
        {
            SetControls(ControlState.Initial);
            OutputFeedback("Target shut down.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Starts the target application.
        /// </summary>
        /// <returns>The target automation element.</returns>
        ///--------------------------------------------------------------------
        private AutomationElement StartTargetApp()
        {
            try
            {
                // Start target application.
                ProcessStartInfo startInfo =
                new ProcessStartInfo(System.Windows.Forms.Application.StartupPath + "\\ScriptGeneratorTarget.exe");
                startInfo.WindowStyle = ProcessWindowStyle.Normal;
                startInfo.UseShellExecute = true;

                Process p = Process.Start(startInfo);

                // Give the target application some time to startup.
                // For Win32 applications, WaitForInputIdle can be used instead.
                // Another alternative is to listen for WindowOpened events.
                Thread.Sleep(2500);

                // Return the automation element
                IntPtr windowHandle = p.MainWindowHandle;
                return (AutomationElement.FromHandle(windowHandle));
            }
            catch (Win32Exception)
            {
                // To do: error handling
                return null;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Modifies the state of the controls on the form.
        /// </summary>
        /// <param name="controlState">
        /// The current state of the target application.
        /// </param>
        /// <remarks>
        /// Check thread safety for client updates based on target events.
        /// </remarks>
        ///--------------------------------------------------------------------
        public void SetControls(ControlState controlState)
        {
            // Check if we need to call BeginInvoke.
            if (this.InvokeRequired)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.BeginInvoke(new ClientControlsDelegate(SetControls),
                                                 new object[] { controlState });
                return;
            }
            switch (controlState)
            {
                case ControlState.Initial:
                    btnStartTarget.Enabled = true;
                    btnExecuteScript.Enabled = false;
                    break;
                case ControlState.TargetStarted:
                    btnStartTarget.Enabled = false;
                    btnExecuteScript.Enabled = true;
                    break;
            }
        }
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="controlState">
        /// The current state of the target application; 
        /// true = active, false = inactive.
        /// </param>
        public delegate void ClientControlsDelegate(ControlState controlState);

        ///--------------------------------------------------------------------
        /// <summary>
        /// Provides feedback on client form.
        /// </summary>
        /// <param name="message">
        /// The feedback message.
        /// </param>
        /// <remarks>
        /// Check thread safety for client updates based on target events.
        /// </remarks>
        ///--------------------------------------------------------------------
        public void OutputFeedback(string message)
        {
            // Check if we need to call BeginInvoke.
            if (this.InvokeRequired)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.BeginInvoke(new OutputFeedbackDelegate(OutputFeedback),
                                                 new object[] { message });

                return;
            }
            tbFeedback.AppendText(message);
            tbFeedback.ScrollToCaret();
        }
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="message">
        /// The feedback message.
        /// </param>
        public delegate void OutputFeedbackDelegate(string message);
    }
}
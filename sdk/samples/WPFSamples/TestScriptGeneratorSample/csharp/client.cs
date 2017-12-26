/*******************************************************************************
 * File: Client.cs
 *
 * Description: 
 * 
 * This sample demonstrates how to use UI Automation control patterns and events 
 * to generate test scripts for automated testing scenarios. The test script 
 * generator tracks user interaction with the UI of a target application, storing 
 * the elements and events of interest. This generated test script can then be 
 * used for later playback.  
 * 
 * Information about each element and associated event is saved to a general 
 * storage class (UIAEventRecord.cs). A subset of the information is also saved 
 * to a script class (ScriptKeeper.cs) used to generate the test script.
 * 
 * Both the event store and the script store can be dumped as plain text for 
 * subsequent analysis or modification by a tester.
 * 
 * NOTE:
 * UI Automation and the automated test scenario addressed in this sample 
 * require a contract between the application, or control developer, and 
 * a tester. In other words, the ability to consistently execute a generated 
 * script is dependent on a static, predictable target UI to operate on. 
 * In this regard, UI Automation should not be considered a solution for 
 * scenarios where the recording and subsequent playback of interactions 
 * with random applications and controls is required.
 * 
 * Two common issues that make "blind" recording and playback impractical 
 * with UI Automation are listed here.
 * 
 *      1)  Objects that do not have a unique identifier, such as an 
 *          AutomationID property, are very difficult to locate during 
 *          playback.
 *      2)  A dynamic UI also makes it difficult to locate specific objects 
 *          during playback. For example, the Windows Task Manager dialog 
 *          where items listed on the "Processes" or "Applications" tabs 
 *          are dependent on system state. 
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
using System.IO;
using System.Text;
using System.Drawing;

namespace TestScriptGeneratorSample
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// Interaction logic for the Client form.
    /// </summary>
    ///--------------------------------------------------------------------
    public partial class ScriptGeneratorClient : Form
    {
        private Thread workerThread;
        private UiaWorker uiaWorker;
        string automatedTestScript;
        private AutomationElement targetApp;
        private AutomationEventHandler targetCloseListener;

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
            TargetStarted, 
            /// <summary>
            /// The UI Automation worker has started.
            /// </summary>
            UIAStarted, 
            /// <summary>
            /// The UI Automation worker has stopped or target has been closed.
            /// </summary>
            UIAStopped, 
            /// <summary>
            /// Either the generated script or the event store has been dumped.
            /// </summary>
            StoreDumped 
        };

        ///--------------------------------------------------------------------
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        ///--------------------------------------------------------------------
        [STAThread]
        static void Main()
        {
            System.Windows.Forms.Application.EnableVisualStyles();
            System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);
            ScriptGeneratorClient scriptGeneratorClient = 
                new ScriptGeneratorClient();
            scriptGeneratorClient.StartPosition = FormStartPosition.Manual;
            scriptGeneratorClient.Location = new System.Drawing.Point(50, 50);
            System.Windows.Forms.Application.Run(scriptGeneratorClient);
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <remarks>
        /// Initializes components.
        /// </remarks>
        ///--------------------------------------------------------------------
        public ScriptGeneratorClient()
        {
            InitializeComponent();
            SetControls(ControlState.Initial);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Start target button click listener.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        /// <remarks>
        /// Starts the target application, initializes the queue, and 
        /// registers for events of interest.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void btnStartTarget_Click(object src, EventArgs e)
        {
            targetApp = StartTargetApp();
            if (targetApp == null)
            {
                throw new ApplicationException(
                    "Unable to start target application.");
            }
            OutputFeedback(
                "Target started; start recording." + Environment.NewLine);

            RegisterTargetCloseEventListener();

            int clientLocationTop = this.Top;
            int clientLocationRight = this.Right + 100;
            TransformPattern transformPattern = 
                targetApp.GetCurrentPattern(TransformPattern.Pattern) 
                as TransformPattern;
            if (transformPattern != null)
            {
                transformPattern.Move(clientLocationRight, clientLocationTop);
            }

            SetControls(ControlState.TargetStarted);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Register the target closed event listener.
        /// </summary>
        ///--------------------------------------------------------------------
        private void RegisterTargetCloseEventListener()
        {
            targetCloseListener = new AutomationEventHandler(OnTargetClosed);
            Automation.AddAutomationEventHandler(
                WindowPattern.WindowClosedEvent, 
                targetApp, 
                TreeScope.Element, 
                targetCloseListener);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// The target closed event handler.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        /// <remarks>
        /// Changes the state of client controls and shuts down the 
        /// UI Automation thread.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void OnTargetClosed(object src, AutomationEventArgs e)
        {
            if (uiaWorker != null)
            {
                uiaWorker.Shutdown();
            }

            SetControls(ControlState.Initial);
            OutputFeedback("Target shut down." + Environment.NewLine);

            Automation.RemoveAutomationEventHandler(
                WindowPattern.WindowClosedEvent, targetApp, targetCloseListener);
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
                new ProcessStartInfo(
                System.Windows.Forms.Application.StartupPath +
                "\\ScriptGeneratorTarget.exe");
                startInfo.WindowStyle = ProcessWindowStyle.Normal;
                startInfo.UseShellExecute = true;

                Process p = Process.Start(startInfo);

                // Give the target application some time to startup.
                // For Win32 applications, WaitForInputIdle can be used instead.
                // Another alternative is to listen for WindowOpened events.
                // Otherwise, an ArgumentException results when you try to
                // retrieve an automation element from the window handle.
                Thread.Sleep(3000);

                // Return the automation element
                IntPtr windowHandle = p.MainWindowHandle;
                return (AutomationElement.FromHandle(windowHandle));
            }
            catch (ArgumentException)
            {
                // To do: error handling
                return null;
            }
            catch (Win32Exception)
            {
                // To do: error handling
                return null;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Starts the UI Automation worker on its own thread.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        /// <remarks>
        /// Starts the target application, initializes the queue, and 
        /// registers for events of interest.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void btnStart_Click(object src, EventArgs e)
        {
            uiaWorker = new UiaWorker(this, targetApp);
            
            // Start another thread to do the UI Automation work.
            ThreadStart threadDelegate = new ThreadStart(StartUiaWorkerThread);
            workerThread = new Thread(threadDelegate);
            workerThread.Start();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Delegated method for ThreadStart. Creates a UIAutoWorker class that 
        /// does all UI Automation related work.
        /// </summary>
        /// <remarks>
        /// UI Automation must be called on a separate thread if the client 
        /// application itself could be a target for event handling.
        /// </remarks>
        ///--------------------------------------------------------------------
        public void StartUiaWorkerThread()
        {
            uiaWorker.StartWork();
            if (uiaWorker != null)
            {
                SetControls(ControlState.UIAStarted);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stops the UI Automation worker.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void btnStop_Click(object src, EventArgs e)
        {
            uiaWorker.Shutdown();
            if (uiaWorker.ElementQueue.Count == 0)
            {
                SetControls(ControlState.TargetStarted);
            }
            else
            {
                SetControls(ControlState.UIAStopped);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Dumps a plain text version of the event store to the client.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void GenerateTextOutput(object src, EventArgs e)
        {
            Button buttonSender = src as Button;
            switch (buttonSender.Name)
            {
                case "btnDumpStore":
                    StringBuilder storeDump = new StringBuilder();
                    foreach (UiaEventRecord storedItem in uiaWorker.ElementQueue)
                    {
                        storeDump.Append("AutomationID: ").AppendLine(storedItem.AutomationId);
                        storeDump.Append("ClassName: ").AppendLine(storedItem.ClassName);
                        storeDump.Append("ControlType: ").AppendLine(storedItem.ControlType);
                        storeDump.Append("EventID: ").AppendLine(storedItem.EventId);
                        storeDump.Append("EventTime: ").AppendLine(storedItem.EventTime.ToString());
                        storeDump.AppendLine("Supported Patterns:");
                        foreach (AutomationPattern pattern in storedItem.SupportedPatterns)
                        {
                            storeDump.AppendLine(pattern.ProgrammaticName);
                        }
                        storeDump.Append("Parent: ").AppendLine(storedItem.Parent);
                        storeDump.Append("Grandparent: ").AppendLine(storedItem.Grandparent);
                        storeDump.AppendLine();
                        tbFeedback.Text = storeDump.ToString();
                    }
                    SetControls(ControlState.StoreDumped);
                    break;
                case "btnGenerateScript":
                    automatedTestScript = GenerateTestScript();
                    tbFeedback.Text = automatedTestScript;
                    SetControls(ControlState.StoreDumped);
                    break;
                case "btnCopyToClipboard":
                    System.Windows.Forms.Clipboard.SetText(automatedTestScript);
                    StringBuilder scriptInstructions = new StringBuilder();
                    scriptInstructions.AppendLine(
                        "Script copied to clipboard; please follow these instructions.")
                        .AppendLine("1.\tClose the ScriptGenerator client and target applications")
                        .AppendLine("2.\tOpen Script.cs in the ExecuteScript project")
                        .AppendLine("3.\tSelect all and paste")
                        .AppendLine("4.\tBuild and run the ExecuteScript project.")
                        .Append("Note that the generated script will have function calls in the same ")
                        .Append("order as the event timestamps stored here. Since there is no way ")
                        .Append("to ensure the order of events, the tester may have to rearrange ")
                        .Append("the function calls in the script so controls are enabled as necessary ")
                        .Append("for playback. This behavior can be evident with interdependent controls that get ")
                        .Append("focus and perform an action such as the controls in the target application.");
                    tbFeedback.Text = scriptInstructions.ToString();
                    break;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Assembles the script with necessary comments, using statements,
        /// namespace specifier, etc.
        /// </summary>
        ///--------------------------------------------------------------------
        private string GenerateTestScript()
        {
            StringBuilder automatedTestScript = new StringBuilder();

            automatedTestScript.AppendLine(
                "//-------------------------------------------------------------");
            automatedTestScript.AppendLine(
                "// Description:\tAutomated test script");
            automatedTestScript.AppendLine(
                "// Generator:\tAutomatedScriptGenerator");
            automatedTestScript.Append(
                "// Date:\t").AppendLine(DateTime.Now.ToLocalTime().ToString());
            automatedTestScript.AppendLine(
                "//-------------------------------------------------------------");
            automatedTestScript.AppendLine("using System;");
            automatedTestScript.AppendLine("using System.Windows;");
            automatedTestScript.AppendLine("using System.Windows.Automation;");
            automatedTestScript.AppendLine("namespace ExecuteScript");
            automatedTestScript.AppendLine("{");
            automatedTestScript.AppendLine("\tclass Script");
            automatedTestScript.AppendLine("\t{");

            automatedTestScript.AppendLine(
                "\t\tpublic Script(ExecuteScriptClient clientApp)");
            automatedTestScript.AppendLine("\t\t{");
            automatedTestScript.AppendLine(
                "\t\t\tScriptFunctions scriptFunctions = new ScriptFunctions(\"" +
                targetApp.Current.AutomationId + "\", clientApp);");
            foreach (UiaEventRecord storedItem in uiaWorker.ElementQueue)
            {
                automatedTestScript.AppendLine("\t\t\t" + storedItem.ScriptAction);
            }
            automatedTestScript.AppendLine("\t\t}");
            automatedTestScript.AppendLine("\t}");
            automatedTestScript.AppendLine("}");
            return automatedTestScript.ToString();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Clears the output window.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void btnClear_Click(object src, EventArgs e)
        {
            tbFeedback.Clear();
        }

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
        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="message">
        /// The feedback message.
        /// </param>
        ///--------------------------------------------------------------------
        public delegate void OutputFeedbackDelegate(string message);

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
                this.BeginInvoke(new SetControlsDelegate(SetControls),
                                                 new object[] { controlState });
                return;
            }
            switch (controlState)
            {
                case ControlState.Initial:
                    btnStartTarget.Enabled = true;
                    btnDumpStore.Enabled = false;
                    btnStart.Enabled = false;
                    btnStop.Enabled = false;
                    btnGenerateScript.Enabled = false;
                    btnClear.Enabled = false;
                    btnCopyToClipboard.Enabled = false;
                    break;
                case ControlState.TargetStarted:
                    btnStartTarget.Enabled = false;
                    btnDumpStore.Enabled = false;
                    btnStart.Enabled = true;
                    btnStop.Enabled = false;
                    btnGenerateScript.Enabled = false;
                    btnClear.Enabled = false;
                    btnCopyToClipboard.Enabled = false;
                    break;
                case ControlState.StoreDumped:
                    btnStartTarget.Enabled = false;
                    btnDumpStore.Enabled = true;
                    btnStart.Enabled = true;
                    btnStop.Enabled = false;
                    btnGenerateScript.Enabled = true;
                    btnClear.Enabled = true;
                    btnCopyToClipboard.Enabled = true;
                    break;
                case ControlState.UIAStarted:
                    btnStartTarget.Enabled = false;
                    btnStart.Enabled = false;
                    btnStop.Enabled = true;
                    btnDumpStore.Enabled = false;
                    btnGenerateScript.Enabled = false;
                    btnClear.Enabled = false;
                    btnCopyToClipboard.Enabled = false;
                    break;
                case ControlState.UIAStopped:
                    btnStartTarget.Enabled = false;
                    btnDumpStore.Enabled = true;
                    btnStart.Enabled = true;
                    btnStop.Enabled = false;
                    btnGenerateScript.Enabled = true;
                    btnClear.Enabled = false;
                    btnCopyToClipboard.Enabled = false;
                    break;
            }
        }
        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="controlState">
        /// The current state of the target application.
        /// </param>
        ///--------------------------------------------------------------------
        public delegate void SetControlsDelegate(ControlState controlState);
    } 
} 

/*******************************************************************************
 *
 * File: UiaWorker.cs
 *
 * Description: A Class that implements UI Automation functionality
 *              on a separate thread.
 * 
 * For a full description of the sample, see Client.cs.
 *
 *     
 * This file is part of the Microsoft WinfFX SDK Code Samples.
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
 ******************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Windows.Automation;
using System.Diagnostics;
using System.Threading;
using System.Collections;
using System.Text.RegularExpressions;

namespace TestScriptGeneratorSample
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// UI Automation worker class.
    /// </summary>
    ///--------------------------------------------------------------------
    class UiaWorker
    {
        // The client form.
        private ScriptGeneratorClient clientApp;

        // The target application.
        private AutomationElement targetApp;

        // The desktop.
        private AutomationElement rootElement;

        // Storage for controls of interest found during initial 
        // scan of target application.
        private AutomationElementCollection elementCollection;

        // Storage for elements and events when generating the script.
        // The elements available represent the controls compiled during 
        // the initial scan of the target application.
        private Queue<UiaEventRecord> elementQueue;

        // Event listeners.
        private AutomationFocusChangedEventHandler focusHandler;
        private AutomationEventHandler invokeHandler;
        private AutomationPropertyChangedEventHandler rangevalueHandler;
        private AutomationEventHandler selectionHandler;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="client">The client application.</param>
        /// <param name="target">The target application.</param>
        /// <remarks>
        /// Initializes components.
        /// </remarks>
        ///--------------------------------------------------------------------
        public UiaWorker(ScriptGeneratorClient client, AutomationElement target)
        {
            // Initialize member variables.
            clientApp = client;
            targetApp = target;
            elementQueue = new Queue<UiaEventRecord>();
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Storage for elements and events when generating the script.
        /// </summary>
        ///--------------------------------------------------------------------
        public Queue<UiaEventRecord> ElementQueue
        {
            get
            {
                return elementQueue;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Start the UI Automation worker.
        /// </summary>
        ///--------------------------------------------------------------------
        internal void StartWork()
        {
            Feedback("Please wait for UIA worker to start.");

            // Get UI Automation root element.
            rootElement = AutomationElement.RootElement;

            try
            {
                CompileTargetControls();

                Feedback("UIA Worker started; target ready for interaction.");
            }
            catch (ElementNotAvailableException)
            {
                // Was target shut down before the UI Automation worker started?
                return;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Compile a collection of controls of interest and register for
        /// relevant events.
        /// </summary>
        ///--------------------------------------------------------------------
        private void CompileTargetControls()
        {
            // Create a CacheRequest.
            CacheRequest cacheRequest = new CacheRequest();

            // Specify scope and properties to cache.
            cacheRequest.TreeScope = TreeScope.Element;
            cacheRequest.Add(AutomationElement.IsInvokePatternAvailableProperty);
            cacheRequest.Add(AutomationElement.IsRangeValuePatternAvailableProperty);
            cacheRequest.Add(AutomationElement.IsSelectionPatternAvailableProperty);
            cacheRequest.Add(AutomationElement.AutomationIdProperty);
            cacheRequest.Add(AutomationElement.ClassNameProperty);
            cacheRequest.Add(AutomationElement.ControlTypeProperty);
            cacheRequest.Add(AutomationElement.NameProperty);

            // Get all immediate children of the target.
            using (cacheRequest.Activate())
            {
                elementCollection = 
                    targetApp.FindAll(
                    TreeScope.Children, Automation.ControlViewCondition);
                RegisterForEvents();
            }

        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Register for events of interest.
        /// </summary>
        ///--------------------------------------------------------------------
        private void RegisterForEvents()
        {
            // Focus changes are global; we'll get cached properties for 
            // all elements that receive focus.
            focusHandler =
               new AutomationFocusChangedEventHandler(OnFocusChange);
            Automation.AddAutomationFocusChangedEventHandler(focusHandler);

            // Register for events from descendants of the root element.
            // Only the events supported by a control will be registered.
            invokeHandler = new AutomationEventHandler(OnInvoke);
            Automation.AddAutomationEventHandler(
                InvokePattern.InvokedEvent, 
                targetApp, 
                TreeScope.Children, 
                invokeHandler);
            rangevalueHandler = 
                new AutomationPropertyChangedEventHandler(OnRangeValueChange);
            Automation.AddAutomationPropertyChangedEventHandler(
                targetApp, 
                TreeScope.Children, 
                rangevalueHandler, 
                RangeValuePattern.ValueProperty);
            selectionHandler = 
                new AutomationEventHandler(OnSelectionItemSelected);
            Automation.AddAutomationEventHandler(
                SelectionItemPattern.ElementSelectedEvent, 
                targetApp, 
                TreeScope.Descendants, 
                selectionHandler);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handle Invoke events of interest.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        /// <remarks>
        /// Some controls that have not implemented UI Automation correctly
        /// may fire spurious events. For example, a WinForms button will 
        /// fire an InvokedEvent on a mouse-down and then another series of 
        /// InvokedEvents on the subsequent mouse-up.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void OnInvoke(object src, AutomationEventArgs e)
        {
            AutomationElement invokedElement = src as AutomationElement;
            AutomationEvent invokeEvent = e.EventId;
            string scriptAction = "scriptFunctions.Invoke(\"" +
                EscapeChars(invokedElement.Cached.AutomationId) + "\");";

            if (invokedElement.Current.HasKeyboardFocus)
            {
                Feedback("Invoke event.");
                Feedback(invokedElement.Cached.Name);
                StoreElement(invokedElement, invokeEvent, scriptAction);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handle RangeValue events of interest.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void OnRangeValueChange(object src, AutomationEventArgs e)
        {
            AutomationElement rangevalueElement = src as AutomationElement;
            AutomationEvent rangevalueEvent = e.EventId;
            RangeValuePattern rangevaluePattern = 
                rangevalueElement.GetCurrentPattern(RangeValuePattern.Pattern) 
                as RangeValuePattern;
            string scriptAction = 
                "scriptFunctions.SetRangeValue(\"" +
                EscapeChars(rangevalueElement.Cached.AutomationId) +
                "\", " + rangevaluePattern.Current.Value + ");";
            
            if (rangevalueElement.Current.HasKeyboardFocus)
            {
                Feedback("RangeValue event.");
                Feedback(rangevalueElement.Cached.Name);
                StoreElement(rangevalueElement, rangevalueEvent, scriptAction);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handle Selection events of interest.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void OnSelectionItemSelected(object src, AutomationEventArgs e)
        {
            AutomationElement selectionitemElement = src as AutomationElement;
            AutomationEvent selectionEvent = e.EventId;
            string scriptAction = "scriptFunctions.Select(\"" +
                EscapeChars(selectionitemElement.Cached.AutomationId) + "\");";

            if (selectionitemElement.Current.HasKeyboardFocus)
            {
                Feedback("Selection event.");
                Feedback(selectionitemElement.Cached.Name);
                StoreElement(selectionitemElement, selectionEvent, scriptAction);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handle Focus events of interest.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void OnFocusChange(object src, AutomationFocusChangedEventArgs e)
        {
            AutomationElement focusedElement = src as AutomationElement;
            AutomationEvent focusEvent = e.EventId;
            string scriptAction = "scriptFunctions.SetFocus(\"" +
                EscapeChars(focusedElement.Cached.AutomationId) + "\");";

            AutomationElement topLevelWindow =
                GetTopLevelWindow(focusedElement);

            if (topLevelWindow != targetApp)
            {
                return;
            }

            Feedback("Focus changed.");
            Feedback(focusedElement.Cached.Name);
            StoreElement(focusedElement, focusEvent, scriptAction);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Store the element information for later script generation.
        /// </summary>
        /// <param name="element">The element of interest.</param>
        /// <param name="e">The event of interest.</param>
        /// <param name="scriptAction">
        /// The pre-defined function to call in the generated script.
        /// </param>
        /// <remarks>
        /// Note that the generated script will have function calls in the same 
        /// order as the event timestamps stored here. Since there is 
        /// no way to ensure the order of events, the tester may have to 
        /// rearrange the function calls in the script so controls are enabled 
        /// as necessary. This behavior can be evident with interdependent 
        /// controls that get focus and perform an action such as the controls in 
        /// the target application. 
        /// </remarks>
        ///--------------------------------------------------------------------
        private void StoreElement(
            AutomationElement element, AutomationEvent e, string scriptAction)
        {
            DateTime time = DateTime.Now;
            UiaEventRecord eventStore = new UiaEventRecord();
            try
            {
                eventStore.AutomationId = element.Cached.AutomationId;
                eventStore.ClassName = element.Cached.ClassName;
                eventStore.ControlType = 
                    element.Cached.ControlType.ProgrammaticName;
                eventStore.EventId = e.ProgrammaticName;
                eventStore.ScriptAction = scriptAction;
                GetAncestors(element, eventStore);
                eventStore.SupportedPatterns = element.GetSupportedPatterns();
                eventStore.EventTime = time;
                elementQueue.Enqueue(eventStore);
            }
            catch (NullReferenceException)
            {
                return;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Retrieve the parent and grandparent of the current element.
        /// </summary>
        /// <param name="element">The element of interest.</param>
        /// <param name="eventStore">
        /// The UI Automation event storage class.
        /// </param>
        ///--------------------------------------------------------------------
        private void GetAncestors(
            AutomationElement element, UiaEventRecord eventStore)
        {
            AutomationElement parent = 
                TreeWalker.ControlViewWalker.GetParent(element);
            if (parent != null)
            {
                eventStore.Parent = parent.Current.AutomationId;
                AutomationElement grandparent = 
                    TreeWalker.ControlViewWalker.GetParent(parent);
                if (grandparent != null)
                {
                    eventStore.Grandparent = grandparent.Current.AutomationId;
                }
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Retrieves the top-level window that contains the 
        /// UI Automation element of interest.
        /// </summary>
        /// <param name="element">The contained element.</param>
        /// <returns>The containing top-level window element.</returns>
        ///--------------------------------------------------------------------
        private AutomationElement GetTopLevelWindow(AutomationElement element)
        {
            TreeWalker walker = TreeWalker.ControlViewWalker;
            AutomationElement elementParent;
            AutomationElement node = element;
            if (node == rootElement)
            {
                return node;
            }
            while (true)
            {
                elementParent = walker.GetParent(node);
                if (elementParent == rootElement)
                {
                    break;
                }
                node = elementParent;
            }
            return node;
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Escape special characters.
        /// </summary>
        /// <param name="input">
        /// The string to escape.
        /// </param>
        ///--------------------------------------------------------------------
        private string EscapeChars(string input)
        {
            return Regex.Replace(input, "\"", "\\\"");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Destructor.
        /// </summary>
        ///--------------------------------------------------------------------
        ~UiaWorker()
        {
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Removes event handlers when user stops the UI Automation worker
        /// or shuts the target application down.
        /// </summary>
        ///--------------------------------------------------------------------
        public void Shutdown()
        {
            // Automation.RemoveAllEventHandlers is not used here since we don't
            // want to lose the window closed event listener for the target.
            Automation.RemoveAutomationFocusChangedEventHandler(OnFocusChange);
            foreach (AutomationElement element in elementCollection)
            {
                Automation.RemoveAutomationEventHandler(
                    InvokePattern.InvokedEvent, 
                    element, 
                    OnInvoke);
                Automation.RemoveAutomationEventHandler(
                    SelectionItemPattern.ElementSelectedEvent, 
                    element, 
                    OnSelectionItemSelected);
                Automation.RemoveAutomationPropertyChangedEventHandler(
                    element, 
                    OnRangeValueChange);
            }

            Feedback("UI Automation worker stopped.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Prints a line of text to the textbox.
        /// </summary>
        /// <param name="outputStr">The string to print.</param>
        ///--------------------------------------------------------------------
        private void Feedback(string outputStr)
        {
            clientApp.OutputFeedback(outputStr + Environment.NewLine);
        }
    }
}

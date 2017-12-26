/*******************************************************************************
 *
 * File: TargetHandler.cs
 *
 * Description: A Class that implements UI Automation functionality
 *              on a separate thread.
 * 
 * For a full description of the sample, see Client.xaml.cs.
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
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Threading;

namespace SelectionPatternClient
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// UI Automation worker class.
    /// </summary>
    ///--------------------------------------------------------------------
    class TargetHandler
    {
        private Client clientApp;
        private AutomationElement targetApp;
        private AutomationElement rootElement;
        private AutomationElementCollection targetControls;
        private StringBuilder feedbackText;
        private AutomationEventHandler targetCloseListener;

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
        internal TargetHandler(Client client, AutomationElement target)
        {
            // Initialize member variables.
            clientApp = client;
            targetApp = target;
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// The collection of Selection controls in the target.
        /// </summary>
        ///--------------------------------------------------------------------
        internal AutomationElementCollection TargetControls
        {
            get
            {
                return targetControls;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Start the UI Automation worker.
        /// </summary>
        ///--------------------------------------------------------------------
        internal void StartWork()
        {
            // Get UI Automation root element.
            rootElement = AutomationElement.RootElement;

            // Position the target relative to the client.
            double clientLocationTop = clientApp.Top;
            double clientLocationRight = clientApp.Width + clientApp.Left + 100;
            TransformPattern transformPattern =
                targetApp.GetCurrentPattern(TransformPattern.Pattern)
                as TransformPattern;
            if (transformPattern != null)
            {
                transformPattern.Move(clientLocationRight, clientLocationTop);
            }

            RegisterTargetClosedListener();

            // Get the target controls.
            CompileTargetControls();
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Register the target closed event listener.
        /// </summary>
        ///--------------------------------------------------------------------
        private void RegisterTargetClosedListener()
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
        /// Changes the state of client controls and removes event listeners.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void OnTargetClosed(object src, AutomationEventArgs e)
        {
            Feedback("Target has been closed. Please wait.");
            // Automation.RemoveAllEventHandlers is not used here since we don't
            // want to lose the window closed event listener for the target.
            RemoveTargetSelectionEventHandlers();

            Automation.RemoveAutomationEventHandler(
                WindowPattern.WindowClosedEvent, targetApp, targetCloseListener);
            clientApp.SetClientControlState(SelectionPatternClient.Client.ControlState.UIAStopped);

            Feedback(null);
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Finds the selection and selection item controls of interest 
        /// and initializes necessary event listeners.
        /// </summary>
        /// <remarks>
        /// Handles the special case of two Selection controls being returned
        /// for a ComboBox control.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void CompileTargetControls()
        {
            PropertyCondition condition =
                new PropertyCondition(
                AutomationElement.IsSelectionPatternAvailableProperty, true);
            // A ComboBox is an aggregate control containing a ListBox 
            // as a child. 
            // Both the ComboBox and its child ListBox support the 
            // SelectionPattern control pattern but all related 
            // functionality is delegated to the child.
            // For the purposes of this sample we can filter the child 
            // as we do not need to display the redundant information.
            AndCondition andCondition = new AndCondition(
                condition,
                new NotCondition(
                new PropertyCondition(
                AutomationElement.ClassNameProperty, "ComboLBox")));
            targetControls =
                targetApp.FindAll(TreeScope.Children |
                TreeScope.Descendants, andCondition);
            clientApp.EchoTargetControls();
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Gets the currently selected SelectionItem objects from target.
        /// </summary>
        /// <param name="selectionContainer">
        /// The target Selection container.
        /// </param>
        ///--------------------------------------------------------------------
        internal AutomationElement[] GetTargetCurrentSelection(
            AutomationElement selectionContainer)
        {
            try
            {
                SelectionPattern selectionPattern =
                    selectionContainer.GetCurrentPattern(
                    SelectionPattern.Pattern) as SelectionPattern;
                return selectionPattern.Current.GetSelection();
            }
            catch (InvalidOperationException exc)
            {
                Feedback(exc.Message);
                return null;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Subscribe to the selection events.
        /// </summary>
        /// <remarks>
        /// The events are raised by the SelectionItem elements, 
        /// not the Selection container.
        /// </remarks>
        ///--------------------------------------------------------------------
        internal void SetTargetSelectionEventHandlers()
        {
            foreach (AutomationElement control in targetControls)
            {
                AutomationEventHandler selectionHandler =
                    new AutomationEventHandler(TargetSelectionHandler);
                Automation.AddAutomationEventHandler(
                    SelectionItemPattern.ElementSelectedEvent,
                    control,
                    TreeScope.Element | TreeScope.Descendants,
                    selectionHandler);
                Automation.AddAutomationEventHandler(
                    SelectionItemPattern.ElementAddedToSelectionEvent,
                    control,
                    TreeScope.Element | TreeScope.Descendants,
                    selectionHandler);
                Automation.AddAutomationEventHandler(
                    SelectionItemPattern.ElementRemovedFromSelectionEvent,
                    control,
                    TreeScope.Element | TreeScope.Descendants,
                    selectionHandler);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Unsubscribe from the selection events.
        /// </summary>
        /// <remarks>
        /// The events are raised by the SelectionItem elements, 
        /// not the Selection container.
        /// </remarks>
        ///--------------------------------------------------------------------
        internal void RemoveTargetSelectionEventHandlers()
        {
            foreach (AutomationElement control in targetControls)
            {
                AutomationEventHandler selectionHandler =
                    new AutomationEventHandler(TargetSelectionHandler);
                Automation.RemoveAutomationEventHandler(
                    SelectionItemPattern.ElementSelectedEvent,
                    control,
                    selectionHandler);
                Automation.RemoveAutomationEventHandler(
                    SelectionItemPattern.ElementAddedToSelectionEvent,
                    control,
                    selectionHandler);
                Automation.RemoveAutomationEventHandler(
                    SelectionItemPattern.ElementRemovedFromSelectionEvent,
                    control,
                    selectionHandler);
            }
        }


        ///--------------------------------------------------------------------
        /// <summary>
        /// Handles user input in the target.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void TargetSelectionHandler(
            object src, AutomationEventArgs e)
        {
            feedbackText = new StringBuilder();

            // Get the name of the item, which is equivalent to its text.
            AutomationElement sourceItem = src as AutomationElement;

            SelectionItemPattern selectionItemPattern =
                sourceItem.GetCurrentPattern(SelectionItemPattern.Pattern)
                as SelectionItemPattern;

            AutomationElement sourceContainer;

            // Special case handling for composite controls.
            TreeWalker treeWalker = new TreeWalker(new PropertyCondition(
                    AutomationElement.IsSelectionPatternAvailableProperty,
                    true));
            sourceContainer =
                (treeWalker.GetParent(
                selectionItemPattern.Current.SelectionContainer) == null) ?
                selectionItemPattern.Current.SelectionContainer :
                treeWalker.GetParent(
                selectionItemPattern.Current.SelectionContainer);

            switch (e.EventId.ProgrammaticName)
            {
                case
                "SelectionItemPatternIdentifiers.ElementSelectedEvent":
                    feedbackText.Append(sourceItem.Current.Name)
                        .Append(" of the ")
                        .Append(sourceContainer.Current.AutomationId)
                        .Append(" control was selected.");
                    break;
                case
                "SelectionItemPatternIdentifiers.ElementAddedToSelectionEvent":
                    feedbackText.Append(sourceItem.Current.Name)
                        .Append(" of the ")
                        .Append(sourceContainer.Current.AutomationId)
                        .Append(" control was added to the selection.");
                    break;
                case
                "SelectionItemPatternIdentifiers.ElementRemovedFromSelectionEvent":
                    feedbackText.Append(sourceItem.Current.Name)
                        .Append(" of the ")
                        .Append(sourceContainer.Current.AutomationId)
                        .Append(" control was removed from the selection.");
                    break;
            }
            Feedback(feedbackText.ToString());

            for (int controlCounter = 0;
                controlCounter < targetControls.Count;
                controlCounter++)
            {
                clientApp.SetControlPropertiesText(controlCounter);
                clientApp.EchoTargetControlSelections(
                    targetControls[controlCounter],
                    controlCounter);
                clientApp.EchoTargetControlProperties(
                    targetControls[controlCounter],
                    controlCounter);
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Gets the selection items from each target control.
        /// </summary>
        /// <param name="selectionContainer">
        /// The target Selection container.
        /// </param>
        /// <returns>
        /// Automation elements that satisfy the specified conditions.
        /// </returns>
        ///--------------------------------------------------------------------
        internal AutomationElementCollection
            GetSelectionItemsFromTarget(AutomationElement selectionContainer)
        {
            PropertyCondition condition =
                new PropertyCondition(
                AutomationElement.IsSelectionItemPatternAvailableProperty,
                true);
            return selectionContainer.FindAll(TreeScope.Children |
                TreeScope.Descendants, condition);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Prints a line of text to the textbox.
        /// </summary>
        /// <param name="outputStr">The string to print.</param>
        ///--------------------------------------------------------------------
        private void Feedback(string outputStr)
        {
            clientApp.Feedback(outputStr);
        }
    }
}

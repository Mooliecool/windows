/******************************************************************************
 * File: Client.xaml.cs
 *
 * Description: 
 *    The sample consists of a Windows Forms target and a Windows Presentation 
 *    Foundation client that uses SelectionPattern and SelectionItemPattern to 
 *    interact with the collection of controls in the target. The functionality 
 *    demonstrated by the sample includes the dynamic construction of selection 
 *    controls in the client that map to the controls found in the target, 
 *    a report of important properties for each selection control, and the 
 *    ability to echo selection events from the target in the client and 
 *    vice versa.
 * 
 *    The target application, SelectionPatternTarget.exe, should automatically 
 *    be copied to the \bin\debug\ folder of the Visual Studio project when you 
 *    build the sample. The target is then started manually from the client. 
 *    You may have to copy the target executable to the bin/debug folder if 
 *    you receive an error stating that the file cannot be found.
 *
 * 
 * This file is part of the Microsoft .NET Framework SDK Code Samples.
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
using System.Windows;
using System.Diagnostics;
using System.Windows.Automation;
using System.Threading;
using System.IO;
using System.Text;
using System.ComponentModel;
using System.Windows.Threading;
using System.Windows.Controls;

namespace SelectionPatternClient
{
    /// <summary>
    /// Interaction logic for Client.xaml
    /// </summary>

    public partial class Client : Window
    {
        private AutomationElement targetApp;

        private TargetHandler targetHandler;

        private readonly string filePath =
            System.Windows.Forms.Application.StartupPath +
            "\\SelectionPatternTarget.exe";

        private ListBox[] clientListBox;
        private GroupBox[] clientGroupBox;
        private TextBlock[] clientTextBlockMultiple;
        private TextBlock[] clientTextBlockRequired;
        private TextBlock[] clientTextBlockSelected;

        private int selectionControlCounter;
        private StringBuilder feedbackText;

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
        };

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handle window loaded event.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event arguments</param>
        ///--------------------------------------------------------------------
        private void Client_OnLoad(object sender, RoutedEventArgs e)
        {
            SetClientControlState(ControlState.Initial);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handles the click event for the Start App button.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void buttonStartTarget_Click(object sender, RoutedEventArgs e)
        {
            targetApp = StartTargetApp(filePath);

            if (targetApp == null)
            {
                Feedback("Unable to start target application.");
                SetClientControlState(ControlState.UIAStopped);
                return;
            }
            Feedback("Target started.");

            targetHandler = new TargetHandler(this, targetApp);
            targetHandler.StartWork();

            SetClientControlState(ControlState.TargetStarted);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Echo the target selection controls in the client.
        /// </summary>
        ///--------------------------------------------------------------------
        internal void EchoTargetControls()
        {
            textControlsCounter.Text =
                targetHandler.TargetControls.Count.ToString();
            if (targetHandler.TargetControls.Count <= 0)
            {
                return;
            }

            clientGroupBox = new GroupBox[targetHandler.TargetControls.Count];
            clientListBox = new ListBox[targetHandler.TargetControls.Count];
            clientTextBlockMultiple = new TextBlock[targetHandler.TargetControls.Count];
            clientTextBlockRequired = new TextBlock[targetHandler.TargetControls.Count];
            clientTextBlockSelected = new TextBlock[targetHandler.TargetControls.Count];

            for (int controlCounter = 0;
                controlCounter < targetHandler.TargetControls.Count;
                controlCounter++)
            {
                // You can set the cache request tree scope to cache the 
                // children of the selection control at the same time.
                // However, this presents a problem with the combo box. 
                // Since the combo box is an aggregate control
                // the children include more than just the selection items.
                AutomationElementCollection selectionItemControls =
                    targetHandler.GetSelectionItemsFromTarget(
                    targetHandler.TargetControls[controlCounter]);

                // Initialize the control mappings.
                clientGroupBox[controlCounter] = new GroupBox();
                clientGroupBox[controlCounter].Padding = new Thickness(2);
                clientGroupBox[controlCounter].Margin = new Thickness(10);
                clientGroupBox[controlCounter].Header =
                    targetHandler.TargetControls[controlCounter].Current.AutomationId.ToString();
                stackControls.Children.Add(clientGroupBox[controlCounter]);

                clientTextBlockMultiple[controlCounter] = new TextBlock();
                clientTextBlockRequired[controlCounter] = new TextBlock();
                clientTextBlockSelected[controlCounter] = new TextBlock();
                SetControlPropertiesText(controlCounter);

                clientListBox[controlCounter] = new ListBox();
                EchoTargetControlProperties(targetHandler.TargetControls[controlCounter], controlCounter);

                StackPanel clientStackPanel = new StackPanel();
                clientStackPanel.Children.Add(clientTextBlockMultiple[controlCounter]);
                clientStackPanel.Children.Add(clientTextBlockRequired[controlCounter]);
                clientStackPanel.Children.Add(clientTextBlockSelected[controlCounter]);
                clientStackPanel.Children.Add(clientListBox[controlCounter]);
                clientGroupBox[controlCounter].Content = clientStackPanel;

                targetHandler.SetTargetSelectionEventHandlers();

                for (int selectionItem = 0;
                    selectionItem < selectionItemControls.Count;
                    selectionItem++)
                {
                    clientListBox[controlCounter].Items.Add(
                        selectionItemControls[selectionItem].Current.Name);
                }
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Display the relevant target control properties.
        /// </summary>
        /// <param name="controlCounter">
        /// Current selection control in target.
        /// </param>
        ///--------------------------------------------------------------------
        internal void SetControlPropertiesText(int controlCounter)
        {
            // Check if we need to call BeginInvoke.
            if (this.Dispatcher.CheckAccess() == false)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.Dispatcher.BeginInvoke(
                    DispatcherPriority.Send,
                    new SetControlPropertiesTextDelegate(SetControlPropertiesText),
                    controlCounter);
                return;
            }

            string propertyText;
            propertyText =
                "Can select multiple: " +
                targetHandler.TargetControls[controlCounter].GetCurrentPropertyValue(
                SelectionPattern.CanSelectMultipleProperty).ToString();

            clientTextBlockMultiple[controlCounter].Text = propertyText;
            clientTextBlockMultiple[controlCounter].Foreground =
               System.Windows.Media.Brushes.Black;
            clientTextBlockMultiple[controlCounter].Background =
                ((bool)targetHandler.TargetControls[controlCounter].GetCurrentPropertyValue(
                SelectionPattern.CanSelectMultipleProperty)) ?
                System.Windows.Media.Brushes.White :
                System.Windows.Media.Brushes.LightYellow;

            propertyText =
                "Is selection required: " +
                targetHandler.TargetControls[controlCounter].GetCurrentPropertyValue(
                SelectionPattern.IsSelectionRequiredProperty).ToString();
            clientTextBlockRequired[controlCounter].Text = propertyText;
            clientTextBlockRequired[controlCounter].Foreground =
                ((bool)targetHandler.TargetControls[controlCounter].GetCurrentPropertyValue(
                SelectionPattern.IsSelectionRequiredProperty)) ?
                System.Windows.Media.Brushes.Blue :
                System.Windows.Media.Brushes.Black;

            propertyText =
                "Items selected: " +
                targetHandler.GetTargetCurrentSelection(
                targetHandler.TargetControls[controlCounter]).Length.ToString();
            clientTextBlockSelected[controlCounter].Text = propertyText;
            clientTextBlockSelected[controlCounter].Foreground =
                System.Windows.Media.Brushes.Black;
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="controlCounter">
        /// Index of current selection control.
        /// </param>
        ///--------------------------------------------------------------------
        internal delegate void SetControlPropertiesTextDelegate(int controlCounter);

        ///--------------------------------------------------------------------
        /// <summary>
        /// Echo the target control properties in the client.
        /// </summary>
        /// <param name="targetSelectionControl">
        /// Current selection control in target.
        /// </param>
        /// <param name="controlCounter">
        /// Index of current selection control.
        /// </param>
        ///--------------------------------------------------------------------
        internal void EchoTargetControlProperties(
            AutomationElement targetSelectionControl,
            int controlCounter)
        {
            // Check if we need to call BeginInvoke.
            if (this.Dispatcher.CheckAccess() == false)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.Dispatcher.BeginInvoke(
                    DispatcherPriority.Send,
                    new EchoTargetControlPropertiesDelegate(EchoTargetControlProperties),
                    targetSelectionControl, controlCounter);
                return;
            }

            clientListBox[controlCounter].Name =
                targetSelectionControl.Current.AutomationId.ToString();

            clientListBox[controlCounter].SelectionMode =
                ((bool)targetSelectionControl.GetCurrentPropertyValue(
                SelectionPattern.CanSelectMultipleProperty)) ?
                SelectionMode.Multiple :
                SelectionMode.Single;

            clientListBox[controlCounter].Background =
                (clientListBox[controlCounter].SelectionMode ==
                SelectionMode.Single) ?
                System.Windows.Media.Brushes.LightYellow :
                System.Windows.Media.Brushes.White;

            clientListBox[controlCounter].Foreground =
                ((bool)targetSelectionControl.GetCurrentPropertyValue(
                SelectionPattern.IsSelectionRequiredProperty)) ?
                System.Windows.Media.Brushes.Blue :
                System.Windows.Media.Brushes.Black;
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="targetSelectionControl">
        /// Current selection control in target.
        /// </param>
        /// <param name="controlCounter">
        /// Index of current selection control.
        /// </param>
        ///--------------------------------------------------------------------
        internal delegate void EchoTargetControlPropertiesDelegate(
            AutomationElement targetSelectionControl,
            int controlCounter);

        ///--------------------------------------------------------------------
        /// <summary>
        /// Mirrors target selections in client list boxes.
        /// </summary>
        /// <param name="targetSelectionControl">
        /// Current target selection control.
        /// </param>
        /// <param name="controlCounter">
        /// Index of current selection control.
        /// </param>
        ///--------------------------------------------------------------------
        internal void EchoTargetControlSelections(
            AutomationElement targetSelectionControl,
            int controlCounter)
        {
            // Check if we need to call BeginInvoke.
            if (this.Dispatcher.CheckAccess() == false)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.Dispatcher.BeginInvoke(
                    DispatcherPriority.Send,
                    new EchoTargetControlSelectionsDelegate(EchoTargetControlSelections),
                    targetSelectionControl, controlCounter);
                return;
            }
            AutomationElementCollection targetSelectionItems =
                targetHandler.GetSelectionItemsFromTarget(targetSelectionControl);
            for (int itemCounter = 0;
                itemCounter < targetSelectionItems.Count;
                itemCounter++)
            {
                SelectionItemPattern selectionItemPattern =
                    targetSelectionItems[itemCounter].GetCurrentPattern(
                    SelectionItemPattern.Pattern) as SelectionItemPattern;
                ListBoxItem listboxItem =
                    GetClientItemFromIndex(clientListBox[controlCounter], itemCounter);
                listboxItem.IsSelected =
                    selectionItemPattern.Current.IsSelected;
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="targetSelectionControl">
        /// Current selection control in target.
        /// </param>
        /// <param name="controlCounter">
        /// Index of current selection control.
        /// </param>
        ///--------------------------------------------------------------------
        internal delegate void EchoTargetControlSelectionsDelegate(
            AutomationElement targetSelectionControl,
            int controlCounter);

        ///--------------------------------------------------------------------
        /// <summary>
        /// Gets an item from a WPF list box based on index.
        /// </summary>
        /// <param name="listBox">
        /// Current client list box.
        /// </param>
        /// <param name="index">
        /// Index of current list box item.
        /// </param>
        ///--------------------------------------------------------------------
        internal ListBoxItem GetClientItemFromIndex(ListBox listBox, int index)
        {
            return
                (ListBoxItem)(listBox.ItemContainerGenerator.ContainerFromIndex(index));
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handles the request to change the direction of user input from 
        /// client to target and vice versa.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        private void ChangeEchoDirection(object sender, RoutedEventArgs e)
        {
            Feedback("");

            string echoDirection = ((RadioButton)sender).Content.ToString();

            ResetSelectionHandlers(clientListBox, echoDirection);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Manages the selection handlers for client and target.
        /// </summary>
        /// <param name="clientListBox">
        /// The array of ListBox controls in the client.
        /// </param>
        /// <param name="echoDirection">
        /// The direction to echo selection events (that is, client or target).
        /// </param>
        ///--------------------------------------------------------------------
        internal void ResetSelectionHandlers(ListBox[] clientListBox, string echoDirection)
        {
            for (int controlCounter = 0;
                controlCounter < targetHandler.TargetControls.Count;
                controlCounter++)
            {
                targetHandler.RemoveTargetSelectionEventHandlers();

                clientListBox[controlCounter].SelectionChanged -=
                    new SelectionChangedEventHandler(ClientSelectionHandler);

                if (echoDirection == "Client")
                {
                    targetHandler.SetTargetSelectionEventHandlers();
                }
                else
                {
                    clientListBox[controlCounter].SelectionChanged +=
                        new SelectionChangedEventHandler(ClientSelectionHandler);
                }
            }
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Handles user input in the client.
        /// </summary>
        /// <param name="sender">The object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        ///--------------------------------------------------------------------
        internal void ClientSelectionHandler(
            object sender, SelectionChangedEventArgs e)
        {
            feedbackText = new StringBuilder();
            ListBox clientListBox = sender as ListBox;

            foreach (
                AutomationElement targetControl in
                targetHandler.TargetControls)
            {
                if (clientListBox.Name ==
                    targetControl.Current.AutomationId.ToString())
                {
                    SelectionPattern selectionPattern =
                        targetControl.GetCurrentPattern(
                        SelectionPattern.Pattern) as SelectionPattern;
                    AutomationElementCollection targetSelectionItems =
                        targetHandler.GetSelectionItemsFromTarget(targetControl);

                    if (e.AddedItems.Count > 0)
                    {
                        for (int itemCounter = 0;
                            itemCounter < clientListBox.Items.Count;
                            itemCounter++)
                        {
                            ListBoxItem listboxItem =
                                GetClientItemFromIndex(
                                clientListBox, itemCounter);
                            SelectionItemPattern selectionItemPattern =
                                targetSelectionItems[itemCounter].GetCurrentPattern(
                                SelectionItemPattern.Pattern) as
                                SelectionItemPattern;

                            if (!(selectionItemPattern.Current.IsSelected) &&
                                listboxItem.IsSelected)
                            {
                                if (selectionPattern.Current.GetSelection().Length < 1)
                                {
                                    try
                                    {
                                        selectionItemPattern.Select();
                                        feedbackText.Append(
                                        targetSelectionItems[itemCounter].Current.Name)
                                        .Append(" of the ")
                                        .Append(
                                        targetControl.Current.AutomationId)
                                        .Append(" control was selected.");
                                        Feedback(feedbackText.ToString());
                                    }
                                    catch (InvalidOperationException exc)
                                    {
                                        Feedback(exc.Message);
                                    }
                                }
                                else
                                {
                                    try
                                    {
                                        if (selectionPattern.Current.CanSelectMultiple)
                                        {
                                            selectionItemPattern.AddToSelection();
                                            feedbackText.Append(
                                                targetSelectionItems[itemCounter].Current.Name)
                                                .Append(" of the ")
                                                .Append(
                                                targetControl.Current.AutomationId)
                                                .Append(
                                                " control was added to the selection.");
                                            Feedback(feedbackText.ToString());
                                        }
                                        else
                                        {
                                            selectionItemPattern.Select();
                                            feedbackText.Append(
                                            targetSelectionItems[itemCounter].Current.Name)
                                            .Append(" of the ")
                                            .Append(
                                            targetControl.Current.AutomationId)
                                            .Append(" control was selected.");
                                            Feedback(feedbackText.ToString());
                                        }
                                    }
                                    catch (InvalidOperationException exc)
                                    {
                                        Feedback(exc.Message);
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else if (e.RemovedItems.Count > 0)
                    {
                        for (int itemCounter = 0;
                            itemCounter < clientListBox.Items.Count;
                            itemCounter++)
                        {
                            ListBoxItem listboxItem =
                                GetClientItemFromIndex(clientListBox, itemCounter);
                            SelectionItemPattern selectionItemPattern =
                                targetSelectionItems[itemCounter].GetCurrentPattern(
                                SelectionItemPattern.Pattern) as
                                SelectionItemPattern;

                            if ((selectionItemPattern.Current.IsSelected)
                                && !listboxItem.IsSelected)
                            {
                                try
                                {
                                    selectionItemPattern.RemoveFromSelection();
                                    feedbackText.Append(
                                    targetSelectionItems[itemCounter].Current.Name)
                                    .Append(" of the ")
                                    .Append(
                                    targetControl.Current.AutomationId)
                                    .Append(
                                    " control was removed from the selection.");
                                    Feedback(feedbackText.ToString());
                                    break;
                                }
                                catch (InvalidOperationException exc)
                                {
                                    Feedback(exc.Message);
                                }
                            }
                        }
                    }
                }
                for (int controlCounter = 0;
                    controlCounter < targetHandler.TargetControls.Count;
                    controlCounter++)
                {
                    SetControlPropertiesText(controlCounter);
                    EchoTargetControlProperties(
                        targetHandler.TargetControls[controlCounter],
                        controlCounter);
                }
            }
        }
        
        ///--------------------------------------------------------------------
        /// <summary>
        /// Starts the target application.
        /// </summary>
        /// <returns>The target automation element.</returns>
        ///--------------------------------------------------------------------
        private AutomationElement StartTargetApp(string target)
        {
            if (!File.Exists(target))
            {
                feedbackText.Append(target);
                feedbackText.Append(" not found.");
                Feedback(feedbackText.ToString());
                return null;
            }
            try
            {
                // Start target application.
                ProcessStartInfo startInfo = new ProcessStartInfo(target);
                startInfo.WindowStyle = ProcessWindowStyle.Normal;
                startInfo.UseShellExecute = true;

                Process p = Process.Start(startInfo);

                // Give the target application some time to startup.
                // For Win32 applications, WaitForInputIdle can be used instead.
                // Another alternative is to listen for WindowOpened events.
                // Otherwise, an ArgumentException results when you try to
                // retrieve an automation element from the window handle.
                Thread.Sleep(5000);

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
        /// Outputs information to the client window.
        /// </summary>
        /// <param name="message">The string to display.</param>
        ///--------------------------------------------------------------------
        internal void Feedback(string message)
        {
            // Check if we need to call BeginInvoke.
            if (this.Dispatcher.CheckAccess() == false)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.Dispatcher.BeginInvoke(
                    DispatcherPriority.Send, 
                    new FeedbackDelegate(Feedback),
                    message);
                return;
            }
            textFeedback.Text = message + Environment.NewLine;
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Thread-safe delegate.
        /// </summary>
        /// <param name="message">The string to display.</param>
        ///--------------------------------------------------------------------
        internal delegate void FeedbackDelegate(string message);

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
        internal void SetClientControlState(ControlState controlState)
        {
            // Check if we need to call BeginInvoke.
            if (this.Dispatcher.CheckAccess() == false)
            {
                // Pass the same function to BeginInvoke,
                // but the call would come on the correct
                // thread and InvokeRequired will be false.
                this.Dispatcher.BeginInvoke(
                    DispatcherPriority.Send,
                    new SetClientControlStateDelegate(SetClientControlState),
                    controlState);
                return;
            }
            switch (controlState)
            {
                case ControlState.Initial:
                    buttonStartTarget.IsEnabled = true;
                    textControlsCounter.Text = selectionControlCounter.ToString();
                    Echo.Visibility = Visibility.Hidden;
                    break;
                case ControlState.TargetStarted:
                    buttonStartTarget.IsEnabled = false;
                    Echo.Visibility = Visibility.Visible;
                    break;
                case ControlState.UIAStarted:
                    buttonStartTarget.IsEnabled = false;
                    textControlsCounter.Text = selectionControlCounter.ToString();
                    Echo.Visibility = Visibility.Visible;
                    break;
                case ControlState.UIAStopped:
                    buttonStartTarget.IsEnabled = true;
                    buttonStartTarget.Content = "Target has been closed - Restart";
                    echoClient.IsChecked = true;
                    selectionControlCounter = 0;
                    stackControls.Children.Clear();
                    textControlsCounter.Text = selectionControlCounter.ToString();
                    Echo.Visibility = Visibility.Hidden;
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
        internal delegate void SetClientControlStateDelegate(ControlState controlState);
    }
}

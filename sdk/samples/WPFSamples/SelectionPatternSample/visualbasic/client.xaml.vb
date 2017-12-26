'*****************************************************************************
' * File: Client.xaml.vb
' *
' * Description: 
' *    The sample consists of a Windows Forms target and a Windows Presentation 
' *    Foundation client that uses SelectionPattern and SelectionItemPattern to 
' *    interact with the collection of controls in the target. The functionality 
' *    demonstrated by the sample includes the dynamic construction of selection 
' *    controls in the client that map to the controls found in the target, 
' *    a report of important properties for each selection control, and the 
' *    ability to echo selection events from the target in the client and 
' *    vice versa.
' * 
' *    The target application, SelectionPatternTarget.exe, should automatically 
' *    be copied to the \bin\debug\ folder of the Visual Studio project when you 
' *    build the sample. The target is then started manually from the client. 
' *    You may have to copy the target executable to the bin/debug folder if 
' *    you receive an error stating that the file cannot be found.
' *
' * 
' * This file is part of the Microsoft .NET Framework SDK Code Samples.
' * 
' * Copyright (C) Microsoft Corporation.  All rights reserved.
' * 
' * This source code is intended only as a supplement to Microsoft
' * Development Tools and/or on-line documentation.  See these other
' * materials for detailed information regarding Microsoft code samples.
' *
' * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' * PARTICULAR PURPOSE.
' *
' ****************************************************************************

Imports System
Imports System.Windows
Imports System.Diagnostics
Imports System.Windows.Automation
Imports System.Threading
Imports System.IO
Imports System.Text
Imports System.ComponentModel
Imports System.Windows.Threading
Imports System.Windows.Controls

Namespace SelectionPatternClient

    ''' <summary>
    ''' Interaction logic for Client.xaml
    ''' </summary>

    Partial Public Class Client
        Inherits Window
        Private targetApp As AutomationElement

        Private targetHandler As TargetHandler

        Private filePath As String = System.Windows.Forms.Application.StartupPath + "\SelectionPatternTarget.exe"

        Private clientListBox() As ListBox
        Private clientGroupBox() As GroupBox
        Private clientTextBlockMultiple() As TextBlock
        Private clientTextBlockRequired() As TextBlock
        Private clientTextBlockSelected() As TextBlock

        Private selectionControlCounter As Integer
        Private feedbackText As StringBuilder

        '''--------------------------------------------------------------------
        ''' <summary>
        ''' The various permutations of client controls.
        ''' </summary>
        ''' <remarks>
        ''' The state of the client application impacts which client controls 
        ''' are available.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Public Enum ControlState
            ''' <summary>
            ''' Initial state of the client.
            ''' </summary>
            Initial
            ''' <summary>
            ''' Target is available.
            ''' </summary>
            TargetStarted
            ''' <summary>
            ''' The UI Automation worker has started.
            ''' </summary>
            UIAStarted
            ''' <summary>
            ''' The UI Automation worker has stopped or target has been closed.
            ''' </summary>
            UIAStopped
        End Enum 'ControlState

        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Handle window loaded event.
        ''' </summary>
        ''' <param name="sender">The object that raised the event.</param>
        ''' <param name="e">Event arguments</param>
        '''--------------------------------------------------------------------
        Private Sub Client_OnLoad(ByVal sender As Object, ByVal e As RoutedEventArgs)
            SetClientControlState(ControlState.Initial)
        End Sub 'Client_OnLoad


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Handles the click event for the Start App button.
        ''' </summary>
        ''' <param name="sender">The object that raised the event.</param>
        ''' <param name="e">Event arguments.</param>
        '''--------------------------------------------------------------------
        Private Sub buttonStartTarget_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            targetApp = StartTargetApp(filePath)

            If targetApp Is Nothing Then
                Feedback("Unable to start target application.")
                SetClientControlState(ControlState.UIAStopped)
                Return
            End If
            Feedback("Target started.")

            targetHandler = New TargetHandler(Me, targetApp)
            targetHandler.StartWork()

            SetClientControlState(ControlState.TargetStarted)
        End Sub 'buttonStartTarget_Click


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Echo the target selection controls in the client.
        ''' </summary>
        '''--------------------------------------------------------------------
        Friend Sub EchoTargetControls()
            textControlsCounter.Text = targetHandler.TargetControls.Count.ToString()
            If targetHandler.TargetControls.Count <= 0 Then
                Return
            End If

            clientGroupBox = New GroupBox(targetHandler.TargetControls.Count) {}
            clientListBox = New ListBox(targetHandler.TargetControls.Count) {}
            clientTextBlockMultiple = New TextBlock(targetHandler.TargetControls.Count) {}
            clientTextBlockRequired = New TextBlock(targetHandler.TargetControls.Count) {}
            clientTextBlockSelected = New TextBlock(targetHandler.TargetControls.Count) {}

            Dim controlCounter As Integer
            For controlCounter = 0 To targetHandler.TargetControls.Count - 1
                ' You can set the cache request tree scope to cache the 
                ' children of the selection control at the same time.
                ' However, this presents a problem with the combo box. 
                ' Since the combo box is an aggregate control
                ' the children include more than just the selection items.
                Dim selectionItemControls As AutomationElementCollection = targetHandler.GetSelectionItemsFromTarget(targetHandler.TargetControls(controlCounter))

                ' Initialize the control mappings.
                clientGroupBox(controlCounter) = New GroupBox()
                clientGroupBox(controlCounter).Padding = New Thickness(2)
                clientGroupBox(controlCounter).Margin = New Thickness(10)
                clientGroupBox(controlCounter).Header = targetHandler.TargetControls(controlCounter).Current.AutomationId.ToString()
                stackControls.Children.Add(clientGroupBox(controlCounter))

                clientTextBlockMultiple(controlCounter) = New TextBlock()
                clientTextBlockRequired(controlCounter) = New TextBlock()
                clientTextBlockSelected(controlCounter) = New TextBlock()
                SetControlPropertiesText(controlCounter)

                clientListBox(controlCounter) = New ListBox()
                EchoTargetControlProperties(targetHandler.TargetControls(controlCounter), controlCounter)

                Dim clientStackPanel As New StackPanel()
                clientStackPanel.Children.Add(clientTextBlockMultiple(controlCounter))
                clientStackPanel.Children.Add(clientTextBlockRequired(controlCounter))
                clientStackPanel.Children.Add(clientTextBlockSelected(controlCounter))
                clientStackPanel.Children.Add(clientListBox(controlCounter))
                clientGroupBox(controlCounter).Content = clientStackPanel

                targetHandler.SetTargetSelectionEventHandlers()

                Dim selectionItem As Integer
                For selectionItem = 0 To selectionItemControls.Count - 1
                    clientListBox(controlCounter).Items.Add(selectionItemControls(selectionItem).Current.Name)
                Next selectionItem
            Next controlCounter

        End Sub 'EchoTargetControls


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Display the relevant target control properties.
        ''' </summary>
        ''' <param name="controlCounter">
        ''' Current selection control in target.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Sub SetControlPropertiesText(ByVal controlCounter As Integer)
            ' Check if we need to call BeginInvoke.
            If Me.Dispatcher.CheckAccess() = False Then
                ' Pass the same function to BeginInvoke,
                ' but the call would come on the correct
                ' thread and InvokeRequired will be false.
                Me.Dispatcher.BeginInvoke(DispatcherPriority.Send, New SetControlPropertiesTextDelegate(AddressOf SetControlPropertiesText), controlCounter)
                Return
            End If

            Dim propertyText As String

            propertyText = "Can select multiple: " + targetHandler.TargetControls(controlCounter).GetCurrentPropertyValue(SelectionPattern.CanSelectMultipleProperty).ToString()

            clientTextBlockMultiple(controlCounter).Text = propertyText
            clientTextBlockMultiple(controlCounter).Foreground = System.Windows.Media.Brushes.Black

            If (CBool(targetHandler.TargetControls(controlCounter).GetCurrentPropertyValue(SelectionPattern.CanSelectMultipleProperty))) Then
                clientTextBlockMultiple(controlCounter).Background = System.Windows.Media.Brushes.White
            Else
                clientTextBlockMultiple(controlCounter).Background = System.Windows.Media.Brushes.LightYellow
            End If

            propertyText = "Is selection required: " + targetHandler.TargetControls(controlCounter).GetCurrentPropertyValue(SelectionPattern.IsSelectionRequiredProperty).ToString()
            clientTextBlockRequired(controlCounter).Text = propertyText
            If (CBool(targetHandler.TargetControls(controlCounter).GetCurrentPropertyValue(SelectionPattern.IsSelectionRequiredProperty))) Then
                clientTextBlockRequired(controlCounter).Foreground = System.Windows.Media.Brushes.Blue
            Else
                clientTextBlockRequired(controlCounter).Foreground = System.Windows.Media.Brushes.Black
            End If

            propertyText = "Items selected: " + targetHandler.GetTargetCurrentSelection(targetHandler.TargetControls(controlCounter)).Length.ToString()
            clientTextBlockSelected(controlCounter).Text = propertyText
            clientTextBlockSelected(controlCounter).Foreground = System.Windows.Media.Brushes.Black

        End Sub 'SetControlPropertiesText


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Thread-safe delegate.
        ''' </summary>
        ''' <param name="controlCounter">
        ''' Index of current selection control.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Delegate Sub SetControlPropertiesTextDelegate(ByVal controlCounter As Integer)


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Echo the target control properties in the client.
        ''' </summary>
        ''' <param name="targetSelectionControl">
        ''' Current selection control in target.
        ''' </param>
        ''' <param name="controlCounter">
        ''' Index of current selection control.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Sub EchoTargetControlProperties(ByVal targetSelectionControl As AutomationElement, ByVal controlCounter As Integer)
            ' Check if we need to call BeginInvoke.
            If Me.Dispatcher.CheckAccess() = False Then
                ' Pass the same function to BeginInvoke,
                ' but the call would come on the correct
                ' thread and InvokeRequired will be false.
                Me.Dispatcher.BeginInvoke(DispatcherPriority.Send, New EchoTargetControlPropertiesDelegate(AddressOf EchoTargetControlProperties), targetSelectionControl, controlCounter)
                Return
            End If

            clientListBox(controlCounter).Name = targetSelectionControl.Current.AutomationId.ToString()

            If (CBool(targetSelectionControl.GetCurrentPropertyValue(SelectionPattern.CanSelectMultipleProperty))) Then
                clientListBox(controlCounter).SelectionMode = SelectionMode.Multiple
            Else
                clientListBox(controlCounter).SelectionMode = SelectionMode.Single
            End If

            If (clientListBox(controlCounter).SelectionMode = SelectionMode.Single) Then
                clientListBox(controlCounter).Background = System.Windows.Media.Brushes.LightYellow
            Else
                clientListBox(controlCounter).Background = System.Windows.Media.Brushes.White
            End If

            If (CBool(targetSelectionControl.GetCurrentPropertyValue(SelectionPattern.IsSelectionRequiredProperty))) Then
                clientListBox(controlCounter).Foreground = System.Windows.Media.Brushes.Blue
            Else
                clientListBox(controlCounter).Foreground = System.Windows.Media.Brushes.Black
            End If

        End Sub 'EchoTargetControlProperties


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Thread-safe delegate.
        ''' </summary>
        ''' <param name="targetSelectionControl">
        ''' Current selection control in target.
        ''' </param>
        ''' <param name="controlCounter">
        ''' Index of current selection control.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Delegate Sub EchoTargetControlPropertiesDelegate(ByVal targetSelectionControl As AutomationElement, ByVal controlCounter As Integer)


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Mirrors target selections in client list boxes.
        ''' </summary>
        ''' <param name="targetSelectionControl">
        ''' Current target selection control.
        ''' </param>
        ''' <param name="controlCounter">
        ''' Index of current selection control.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Sub EchoTargetControlSelections(ByVal targetSelectionControl As AutomationElement, ByVal controlCounter As Integer)
            ' Check if we need to call BeginInvoke.
            If Me.Dispatcher.CheckAccess() = False Then
                ' Pass the same function to BeginInvoke,
                ' but the call would come on the correct
                ' thread and InvokeRequired will be false.
                Me.Dispatcher.BeginInvoke(DispatcherPriority.Send, New EchoTargetControlSelectionsDelegate(AddressOf EchoTargetControlSelections), targetSelectionControl, controlCounter)
                Return
            End If
            Dim targetSelectionItems As AutomationElementCollection = targetHandler.GetSelectionItemsFromTarget(targetSelectionControl)
            Dim itemCounter As Integer
            For itemCounter = 0 To targetSelectionItems.Count - 1
                Dim selectionItemPattern As SelectionItemPattern = DirectCast(targetSelectionItems(itemCounter).GetCurrentPattern(selectionItemPattern.Pattern), SelectionItemPattern)
                Dim listboxItem As ListBoxItem = GetClientItemFromIndex(clientListBox(controlCounter), itemCounter)
                listboxItem.IsSelected = selectionItemPattern.Current.IsSelected
            Next itemCounter

        End Sub 'EchoTargetControlSelections


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Thread-safe delegate.
        ''' </summary>
        ''' <param name="targetSelectionControl">
        ''' Current selection control in target.
        ''' </param>
        ''' <param name="controlCounter">
        ''' Index of current selection control.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Delegate Sub EchoTargetControlSelectionsDelegate(ByVal targetSelectionControl As AutomationElement, ByVal controlCounter As Integer)


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Gets an item from a WPF list box based on index.
        ''' </summary>
        ''' <param name="listBox">
        ''' Current client list box.
        ''' </param>
        ''' <param name="index">
        ''' Index of current list box item.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Function GetClientItemFromIndex(ByVal listBox As ListBox, ByVal index As Integer) As ListBoxItem
            Return DirectCast(listBox.ItemContainerGenerator.ContainerFromIndex(index), ListBoxItem)

        End Function 'GetClientItemFromIndex


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Handles the request to change the direction of user input from 
        ''' client to target and vice versa.
        ''' </summary>
        ''' <param name="sender">The object that raised the event.</param>
        ''' <param name="e">Event arguments.</param>
        '''--------------------------------------------------------------------
        Private Sub ChangeEchoDirection(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Feedback("")

            Dim echoDirection As String = CType(sender, RadioButton).Content.ToString()

            ResetSelectionHandlers(clientListBox, echoDirection)

        End Sub 'ChangeEchoDirection


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Manages the selection handlers for client and target.
        ''' </summary>
        ''' <param name="clientListBox">
        ''' The array of ListBox controls in the client.
        ''' </param>
        ''' <param name="echoDirection">
        ''' The direction to echo selection events (that is, client or target).
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Sub ResetSelectionHandlers(ByVal clientListBox() As ListBox, ByVal echoDirection As String)
            Dim controlCounter As Integer
            For controlCounter = 0 To targetHandler.TargetControls.Count - 1

                targetHandler.RemoveTargetSelectionEventHandlers()
                RemoveHandler clientListBox(controlCounter).SelectionChanged, AddressOf ClientSelectionHandler

                If echoDirection = "Client" Then
                    targetHandler.SetTargetSelectionEventHandlers()
                Else
                    AddHandler clientListBox(controlCounter).SelectionChanged, AddressOf ClientSelectionHandler
                End If
            Next controlCounter

        End Sub 'ResetSelectionHandlers


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Handles user input in the client.
        ''' </summary>
        ''' <param name="sender">The object that raised the event.</param>
        ''' <param name="e">Event arguments.</param>
        '''--------------------------------------------------------------------
        Friend Sub ClientSelectionHandler(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
            feedbackText = New StringBuilder()
            Dim clientListBox As ListBox = DirectCast(sender, ListBox)
            Dim targetControl As AutomationElement
            For Each targetControl In targetHandler.TargetControls
                If clientListBox.Name = targetControl.Current.AutomationId.ToString() Then
                    Dim selectionControlPattern As SelectionPattern = DirectCast(targetControl.GetCurrentPattern(SelectionPattern.Pattern), SelectionPattern)
                    Dim targetSelectionItems As AutomationElementCollection = targetHandler.GetSelectionItemsFromTarget(targetControl)

                    If e.AddedItems.Count > 0 Then
                        Dim itemCounter As Integer
                        For itemCounter = 0 To clientListBox.Items.Count - 1
                            Dim listboxItem As ListBoxItem = GetClientItemFromIndex(clientListBox, itemCounter)
                            Dim selectionItemPattern As SelectionItemPattern = DirectCast(targetSelectionItems(itemCounter).GetCurrentPattern(selectionItemPattern.Pattern), SelectionItemPattern)

                            If Not selectionItemPattern.Current.IsSelected AndAlso listboxItem.IsSelected Then
                                If selectionControlPattern.Current.GetSelection().Length < 1 Then
                                    Try
                                        selectionItemPattern.Select()
                                        feedbackText.Append(targetSelectionItems(itemCounter).Current.Name).Append(" of the ").Append(targetControl.Current.AutomationId).Append(" control was selected.")
                                        Feedback(feedbackText.ToString())
                                    Catch exc As InvalidOperationException
                                        Feedback(exc.Message)
                                    End Try
                                Else
                                    Try
                                        If selectionControlPattern.Current.CanSelectMultiple Then
                                            selectionItemPattern.AddToSelection()
                                            feedbackText.Append(targetSelectionItems(itemCounter).Current.Name).Append(" of the ").Append(targetControl.Current.AutomationId).Append(" control was added to the selection.")
                                            Feedback(feedbackText.ToString())
                                        Else
                                            selectionItemPattern.Select()
                                            feedbackText.Append(targetSelectionItems(itemCounter).Current.Name).Append(" of the ").Append(targetControl.Current.AutomationId).Append(" control was selected.")
                                            Feedback(feedbackText.ToString())
                                        End If
                                    Catch exc As InvalidOperationException
                                        Feedback(exc.Message)
                                    End Try
                                End If
                                Exit For
                            End If
                        Next itemCounter
                    ElseIf e.RemovedItems.Count > 0 Then
                        Dim itemCounter As Integer
                        For itemCounter = 0 To clientListBox.Items.Count - 1
                            Dim listboxItem As ListBoxItem = GetClientItemFromIndex(clientListBox, itemCounter)
                            Dim selectionItemPattern As SelectionItemPattern = DirectCast(targetSelectionItems(itemCounter).GetCurrentPattern(selectionItemPattern.Pattern), SelectionItemPattern)

                            If selectionItemPattern.Current.IsSelected AndAlso Not listboxItem.IsSelected Then
                                Try
                                    selectionItemPattern.RemoveFromSelection()
                                    feedbackText.Append(targetSelectionItems(itemCounter).Current.Name).Append(" of the ").Append(targetControl.Current.AutomationId).Append(" control was removed from the selection.")
                                    Feedback(feedbackText.ToString())
                                    Exit For
                                Catch exc As InvalidOperationException
                                    Feedback(exc.Message)
                                End Try
                            End If
                        Next itemCounter
                    End If
                End If
                Dim controlCounter As Integer
                For controlCounter = 0 To targetHandler.TargetControls.Count - 1
                    SetControlPropertiesText(controlCounter)
                    EchoTargetControlProperties(targetHandler.TargetControls(controlCounter), controlCounter)
                Next controlCounter
            Next targetControl

        End Sub 'ClientSelectionHandler


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Starts the target application.
        ''' </summary>
        ''' <returns>The target automation element.</returns>
        '''--------------------------------------------------------------------
        Private Function StartTargetApp(ByVal target As String) As AutomationElement
            If Not File.Exists(target) Then
                feedbackText = New StringBuilder()
                feedbackText.Append(target)
                feedbackText.Append(" not found.")
                Feedback(feedbackText.ToString())
                Return Nothing
            End If
            Try
                ' Start target application.
                Dim startInfo As New ProcessStartInfo(target)
                startInfo.WindowStyle = ProcessWindowStyle.Normal
                startInfo.UseShellExecute = True

                Dim p As Process = Process.Start(startInfo)

                ' Give the target application some time to startup.
                ' For Win32 applications, WaitForInputIdle can be used instead.
                ' Another alternative is to listen for WindowOpened events.
                ' Otherwise, an ArgumentException results when you try to
                ' retrieve an automation element from the window handle.
                Thread.Sleep(5000)

                ' Return the automation element
                Dim windowHandle As IntPtr = p.MainWindowHandle
                Return AutomationElement.FromHandle(windowHandle)
            Catch exc As ArgumentException
                ' To do: error handling
                Return Nothing
            Catch exc As Win32Exception
                ' To do: error handling
                Return Nothing
            End Try

        End Function 'StartTargetApp


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Outputs information to the client window.
        ''' </summary>
        ''' <param name="message">The string to display.</param>
        '''--------------------------------------------------------------------
        Friend Sub Feedback(ByVal message As String)
            ' Check if we need to call BeginInvoke.
            If Me.Dispatcher.CheckAccess() = False Then
                ' Pass the same function to BeginInvoke,
                ' but the call would come on the correct
                ' thread and InvokeRequired will be false.
                Me.Dispatcher.BeginInvoke(DispatcherPriority.Send, New FeedbackDelegate(AddressOf Feedback), message)
                Return
            End If
            textFeedback.Text = message + Environment.NewLine

        End Sub 'Feedback


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Thread-safe delegate.
        ''' </summary>
        ''' <param name="message">The string to display.</param>
        '''--------------------------------------------------------------------
        Friend Delegate Sub FeedbackDelegate(ByVal message As String)


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Modifies the state of the controls on the form.
        ''' </summary>
        ''' <param name="controlState">
        ''' The current state of the target application.
        ''' </param>
        ''' <remarks>
        ''' Check thread safety for client updates based on target events.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Friend Sub SetClientControlState(ByVal controlState As ControlState)
            ' Check if we need to call BeginInvoke.
            If Me.Dispatcher.CheckAccess() = False Then
                ' Pass the same function to BeginInvoke,
                ' but the call would come on the correct
                ' thread and InvokeRequired will be false.
                Me.Dispatcher.BeginInvoke(DispatcherPriority.Send, New SetClientControlStateDelegate(AddressOf SetClientControlState), controlState)
                Return
            End If
            Select Case controlState
                Case controlState.Initial
                    buttonStartTarget.IsEnabled = True
                    textControlsCounter.Text = selectionControlCounter.ToString()
                    Echo.Visibility = Windows.Visibility.Hidden
                Case controlState.TargetStarted
                    buttonStartTarget.IsEnabled = False
                    Echo.Visibility = Windows.Visibility.Visible
                Case controlState.UIAStarted
                    buttonStartTarget.IsEnabled = False
                    textControlsCounter.Text = selectionControlCounter.ToString()
                    Echo.Visibility = Windows.Visibility.Visible
                Case controlState.UIAStopped
                    buttonStartTarget.IsEnabled = True
                    buttonStartTarget.Content = "Target has been closed - Restart"
                    echoClient.IsChecked = True
                    selectionControlCounter = 0
                    stackControls.Children.Clear()
                    textControlsCounter.Text = selectionControlCounter.ToString()
                    Echo.Visibility = Windows.Visibility.Hidden
            End Select

        End Sub 'SetClientControlState


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Thread-safe delegate.
        ''' </summary>
        ''' <param name="controlState">
        ''' The current state of the target application.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Delegate Sub SetClientControlStateDelegate(ByVal controlState As ControlState)
    End Class 'Client
End Namespace

 '******************************************************************************
' *
' * File: TargetHandler.cs
' *
' * Description: A Class that implements UI Automation functionality
' *              on a separate thread.
' * 
' * For a full description of the sample, see Client.xaml.cs.
' *
' *     
' * This file is part of the Microsoft WinfFX SDK Code Samples.
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
' *****************************************************************************
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows.Automation
Imports System.Windows.Controls
Imports System.Windows
Imports System.Windows.Threading


Namespace SelectionPatternClient

    '''--------------------------------------------------------------------
    ''' <summary>
    ''' UI Automation worker class.
    ''' </summary>
    '''--------------------------------------------------------------------

    Class TargetHandler
        Private clientApp As Client
        Private targetApp As AutomationElement
        Private rootElement As AutomationElement
        Private targetControlCollection As AutomationElementCollection
        Private feedbackText As StringBuilder
        Private targetCloseListener As AutomationEventHandler


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Constructor.
        ''' </summary>
        ''' <param name="client">The client application.</param>
        ''' <param name="target">The target application.</param>
        ''' <remarks>
        ''' Initializes components.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Friend Sub New(ByVal client As Client, ByVal target As AutomationElement)
            ' Initialize member variables.
            clientApp = client
            targetApp = target

        End Sub 'New

        '''--------------------------------------------------------------------
        ''' <summary>
        ''' The collection of Selection controls in the target.
        ''' </summary>
        '''--------------------------------------------------------------------
        Friend ReadOnly Property TargetControls() As AutomationElementCollection
            Get
                Return targetControlCollection
            End Get
        End Property


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Start the UI Automation worker.
        ''' </summary>
        '''--------------------------------------------------------------------
        Friend Sub StartWork()
            ' Get UI Automation root element.
            rootElement = AutomationElement.RootElement

            ' Position the target relative to the client.
            Dim clientLocationTop As Double = clientApp.Top
            Dim clientLocationRight As Double = clientApp.Width + clientApp.Left + 100
            Dim transformPattern As TransformPattern = DirectCast(targetApp.GetCurrentPattern(transformPattern.Pattern), TransformPattern)
            If Not (transformPattern Is Nothing) Then
                transformPattern.Move(clientLocationRight, clientLocationTop)
            End If

            RegisterTargetClosedListener()

            ' Get the target controls.
            CompileTargetControls()

        End Sub 'StartWork


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Register the target closed event listener.
        ''' </summary>
        '''--------------------------------------------------------------------
        Private Sub RegisterTargetClosedListener()
            targetCloseListener = New AutomationEventHandler(AddressOf OnTargetClosed)
            Automation.AddAutomationEventHandler(WindowPattern.WindowClosedEvent, targetApp, TreeScope.Element, targetCloseListener)

        End Sub 'RegisterTargetClosedListener


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' The target closed event handler.
        ''' </summary>
        ''' <param name="src">Object that raised the event.</param>
        ''' <param name="e">Event arguments.</param>
        ''' <remarks>
        ''' Changes the state of client controls and removes event listeners.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Private Sub OnTargetClosed(ByVal src As Object, ByVal e As AutomationEventArgs)
            Feedback("Target has been closed. Please wait.")
            ' Automation.RemoveAllEventHandlers is not used here since we don't
            ' want to lose the window closed event listener for the target.
            RemoveTargetSelectionEventHandlers()

            Automation.RemoveAutomationEventHandler(WindowPattern.WindowClosedEvent, targetApp, targetCloseListener)
            clientApp.SetClientControlState(SelectionPatternClient.Client.ControlState.UIAStopped)

            Feedback(Nothing)

        End Sub 'OnTargetClosed


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Finds the selection and selection item controls of interest 
        ''' and initializes necessary event listeners.
        ''' </summary>
        ''' <remarks>
        ''' Handles the special case of two Selection controls being returned
        ''' for a ComboBox control.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Private Sub CompileTargetControls()
            Dim condition As New PropertyCondition(AutomationElement.IsSelectionPatternAvailableProperty, True)
            ' A ComboBox is an aggregate control containing a ListBox 
            ' as a child. 
            ' Both the ComboBox and its child ListBox support the 
            ' SelectionPattern control pattern but all related 
            ' functionality is delegated to the child.
            ' For the purposes of this sample we can filter the child 
            ' as we do not need to display the redundant information.
            Dim andCondition As New AndCondition(condition, New NotCondition(New PropertyCondition(AutomationElement.ClassNameProperty, "ComboLBox")))
            targetControlCollection = targetApp.FindAll(TreeScope.Children Or TreeScope.Descendants, andCondition)
            clientApp.EchoTargetControls()

        End Sub 'CompileTargetControls


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Gets the currently selected SelectionItem objects from target.
        ''' </summary>
        ''' <param name="selectionContainer">
        ''' The target Selection container.
        ''' </param>
        '''--------------------------------------------------------------------
        Friend Function GetTargetCurrentSelection(ByVal selectionContainer As AutomationElement) As AutomationElement()
            Try
                Dim selectionControlPattern As SelectionPattern = DirectCast(selectionContainer.GetCurrentPattern(SelectionPattern.Pattern), SelectionPattern)
                Return selectionControlPattern.Current.GetSelection()
            Catch exc As InvalidOperationException
                Feedback(exc.Message)
                Return Nothing
            End Try

        End Function 'GetTargetCurrentSelection


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Subscribe to the selection events.
        ''' </summary>
        ''' <remarks>
        ''' The events are raised by the SelectionItem elements, 
        ''' not the Selection container.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Friend Sub SetTargetSelectionEventHandlers()
            Dim control As AutomationElement
            For Each control In targetControlCollection
                Dim selectionHandler As New AutomationEventHandler(AddressOf TargetSelectionHandler)
                Automation.AddAutomationEventHandler(SelectionItemPattern.ElementSelectedEvent, control, TreeScope.Element Or TreeScope.Descendants, selectionHandler)
                Automation.AddAutomationEventHandler(SelectionItemPattern.ElementAddedToSelectionEvent, control, TreeScope.Element Or TreeScope.Descendants, selectionHandler)
                Automation.AddAutomationEventHandler(SelectionItemPattern.ElementRemovedFromSelectionEvent, control, TreeScope.Element Or TreeScope.Descendants, selectionHandler)
            Next control

        End Sub 'SetTargetSelectionEventHandlers


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Unsubscribe from the selection events.
        ''' </summary>
        ''' <remarks>
        ''' The events are raised by the SelectionItem elements, 
        ''' not the Selection container.
        ''' </remarks>
        '''--------------------------------------------------------------------
        Friend Sub RemoveTargetSelectionEventHandlers()
            Dim control As AutomationElement
            For Each control In targetControlCollection
                Dim selectionHandler As New AutomationEventHandler(AddressOf TargetSelectionHandler)
                Automation.RemoveAutomationEventHandler(SelectionItemPattern.ElementSelectedEvent, control, selectionHandler)
                Automation.RemoveAutomationEventHandler(SelectionItemPattern.ElementAddedToSelectionEvent, control, selectionHandler)
                Automation.RemoveAutomationEventHandler(SelectionItemPattern.ElementRemovedFromSelectionEvent, control, selectionHandler)
            Next control

        End Sub 'RemoveTargetSelectionEventHandlers



        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Handles user input in the target.
        ''' </summary>
        ''' <param name="src">Object that raised the event.</param>
        ''' <param name="e">Event arguments.</param>
        '''--------------------------------------------------------------------
        Private Sub TargetSelectionHandler(ByVal src As Object, ByVal e As AutomationEventArgs)
            feedbackText = New StringBuilder()

            ' Get the name of the item, which is equivalent to its text.
            Dim sourceItem As AutomationElement = DirectCast(src, AutomationElement)

            Dim selectionItemPattern As SelectionItemPattern = DirectCast(sourceItem.GetCurrentPattern(selectionItemPattern.Pattern), SelectionItemPattern)

            Dim sourceContainer As AutomationElement

            ' Special case handling for composite controls.
            Dim treeWalker As New TreeWalker(New PropertyCondition(AutomationElement.IsSelectionPatternAvailableProperty, True))
            If (treeWalker.GetParent(selectionItemPattern.Current.SelectionContainer) Is Nothing) Then
                sourceContainer = selectionItemPattern.Current.SelectionContainer
            Else
                sourceContainer = treeWalker.GetParent(selectionItemPattern.Current.SelectionContainer)
            End If

            Select Case e.EventId.ProgrammaticName
                Case "SelectionItemPatternIdentifiers.ElementSelectedEvent"
                    feedbackText.Append(sourceItem.Current.Name).Append(" of the ").Append(sourceContainer.Current.AutomationId).Append(" control was selected.")
                Case "SelectionItemPatternIdentifiers.ElementAddedToSelectionEvent"
                    feedbackText.Append(sourceItem.Current.Name).Append(" of the ").Append(sourceContainer.Current.AutomationId).Append(" control was added to the selection.")
                Case "SelectionItemPatternIdentifiers.ElementRemovedFromSelectionEvent"
                    feedbackText.Append(sourceItem.Current.Name).Append(" of the ").Append(sourceContainer.Current.AutomationId).Append(" control was removed from the selection.")
            End Select
            Feedback(feedbackText.ToString())

            Dim controlCounter As Integer
            For controlCounter = 0 To targetControlCollection.Count - 1
                clientApp.SetControlPropertiesText(controlCounter)
                clientApp.EchoTargetControlSelections(targetControlCollection(controlCounter), controlCounter)
                clientApp.EchoTargetControlProperties(targetControlCollection(controlCounter), controlCounter)
            Next controlCounter

        End Sub 'TargetSelectionHandler


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Gets the selection items from each target control.
        ''' </summary>
        ''' <param name="selectionContainer">
        ''' The target Selection container.
        ''' </param>
        ''' <returns>
        ''' Automation elements that satisfy the specified conditions.
        ''' </returns>
        '''--------------------------------------------------------------------
        Friend Function GetSelectionItemsFromTarget(ByVal selectionContainer As AutomationElement) As AutomationElementCollection
            Dim condition As New PropertyCondition(AutomationElement.IsSelectionItemPatternAvailableProperty, True)
            Return selectionContainer.FindAll(TreeScope.Children Or TreeScope.Descendants, condition)

        End Function 'GetSelectionItemsFromTarget


        '''--------------------------------------------------------------------
        ''' <summary>
        ''' Prints a line of text to the textbox.
        ''' </summary>
        ''' <param name="outputStr">The string to print.</param>
        '''--------------------------------------------------------------------
        Private Sub Feedback(ByVal outputStr As String)
            clientApp.Feedback(outputStr)

        End Sub 'Feedback
    End Class 'TargetHandler
End Namespace

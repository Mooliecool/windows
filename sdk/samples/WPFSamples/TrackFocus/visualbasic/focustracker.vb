 '******************************************************************************
'  
'   File: FocusTracker.vb
'  
'   Description: This is a simple console application that might be used as a 
'   starting-point for an application that uses UI Automation to track events on 
'   the desktop, such as a screen reader.
'   
'   The program announces when the input focus changes. If the focus moves to a 
'   different application window, the name (usually the caption) of the window is 
'   announced. If the focus moves within an application window, the type and name
'   of the control being read are announced.
'   
'   To know when the focus switches from one application to another, a TreeWalker 
'   is used to find the parent window, and that window is compared with the last 
'   window that had focus.
'   
'   
'    This file is part of the Microsoft Windows SDK Code Samples.
'   
'    Copyright (C) Microsoft Corporation.  All rights reserved.
'   
'   This source code is intended only as a supplement to Microsoft
'   Development Tools and/or on-line documentation.  See these other
'   materials for detailed information regarding Microsoft code samples.
'   
'   THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'   PARTICULAR PURPOSE.
'   
'  ****************************************************************************


Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows.Automation
Imports System.Collections



Class Tracker
    Private lastTopLevelWindow As AutomationElement
    
    
    ''' <summary>
    ''' Constructor.
    ''' </summary>
    Public Sub New() 
        Startup()
    
    End Sub 'New
    
    
    ''' <summary>
    ''' Entry point.
    ''' </summary>
    ''' <param name="args">Command-line arguments; not used.</param>
    Public Shared Sub Main(ByVal args() As String) 
        Console.Title = "UI Automation Focus-tracking Sample"
        Console.WriteLine("Please wait while UI Automation initializes...")
        Dim reader As New Tracker()
        Console.WriteLine("Tracking focus. Press Enter to quit.")
        Console.ReadLine()
        Automation.RemoveAllEventHandlers()
    
    End Sub 'Main
    
    
    ''' <summary>
    ''' Initialization.
    ''' </summary>
    Sub Startup()
        Automation.AddAutomationFocusChangedEventHandler(AddressOf OnFocusChanged)
    End Sub 'Startup



    ''' <summary>
    ''' Retrieves the top-level window that contains the specified 
    ''' UI Automation element.
    ''' </summary>
    ''' <param name="element">The contained element.</param>
    ''' <returns>The  top-level window element.</returns>
    Private Function GetTopLevelWindow(ByVal element As AutomationElement) As AutomationElement
        Dim walker As TreeWalker = TreeWalker.ControlViewWalker
        Dim elementParent As AutomationElement
        Dim node As AutomationElement = element

        Try ' In case the element disappears suddenly, as menu items are 
            ' likely to do.
            If node = AutomationElement.RootElement Then
                Return node
            End If
            ' Walk up the tree to the child of the root.
            While True
                elementParent = walker.GetParent(node)
                If elementParent Is Nothing Then
                    Return Nothing
                End If
                If elementParent = AutomationElement.RootElement Then
                    Exit While
                End If
                node = elementParent
            End While
        Catch
            node = Nothing
        End Try
        Return node

    End Function 'GetTopLevelWindow



    ''' <summary>
    ''' Handles focus-changed events. If the element that received focus is 
    ''' in a different top-level window, announce that. If not, just announce
    ''' which element received focus.
    ''' </summary>
    ''' <param name="src">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Private Sub OnFocusChanged(ByVal src As Object, ByVal e As AutomationFocusChangedEventArgs) 
        Try
            Dim elementFocused As AutomationElement = CType(src, AutomationElement)
            Dim topLevelWindow As AutomationElement = GetTopLevelWindow(elementFocused)
            
            If topLevelWindow = Nothing Then
                Return
            End If

            ' If top-level window has changed, announce it.
            If topLevelWindow <> lastTopLevelWindow Then
                lastTopLevelWindow = topLevelWindow
                Console.WriteLine("Focus moved to top-level window:")
                Console.WriteLine("  " + topLevelWindow.Current.Name)
                Console.WriteLine()
            Else
                ' Announce focused element.
                Console.WriteLine("Focused element: ")
                Console.WriteLine("  Type: " + elementFocused.Current.LocalizedControlType)
                Console.WriteLine("  Name: " + elementFocused.Current.Name)
                Console.WriteLine()
            End If
        Catch
            Return
        End Try
    
    End Sub 'OnFocusChanged
End Class 'Tracker 

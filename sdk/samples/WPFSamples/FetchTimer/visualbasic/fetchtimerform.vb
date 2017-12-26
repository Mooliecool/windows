 '******************************************************************************
' * File: FetchTimerForm.vb
' *
' * Description: 
' * 
' * The sample demonstrates how to cache (prefetch) and retrieve properties, and 
' * is also a simple tool that enables you to see performance data when retrieving
' *  current and cached properties.
' * 
' * To use the application, run it, set options, and leave it in the foreground. 
' * Then move the cursor over any element on the screen and press Alt+G. The 
' * application displays information about the element, along with performance data. 
' * 
' * Note that if you select Descendants in the scope, it may take a long time to 
' * cache properties for complex elements such as the toolbar docking area in 
' * Microsoft Visual Studio.
' * 
' * It is recommended that you try testing each element several times. Initial runs
' * may take longer, and you may obtain anomalous results as the Windows scheduler 
' * interrupts the caching process.
' * 
' * Try caching for both Win32 controls and WPF controls, and note that the 
' * advantage is generally much greater for WPF controls.
' *
' *     
' *  This file is part of the Microsoft Windows SDK Code Samples.
' * 
' *  Copyright (C) Microsoft Corporation.  All rights reserved.
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
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Windows.Automation
Imports System.Diagnostics
Imports System.Threading
Imports System.Security.Permissions



Partial Class FetchTimerForm
    Inherits Form

    Private targetPoint As System.Windows.Point
    Private cacheScope As TreeScope
    Private elementMode As AutomationElementMode
    Private workerThread As Thread
    Private fetcher As UIAutomationFetcher

    Public Delegate Sub OutputDelegate(ByVal results As String)
    Public outputMethodInstance As OutputDelegate


    ''' <summary>
    ''' Constructor.
    ''' </summary>
    Public Sub New()

        InitializeComponent()
        outputMethodInstance = New OutputDelegate(AddressOf OutputResults)

    End Sub 'New



    ''' <summary>
    ''' Enables/disables Children check box when Descendants changed, because
    ''' Descendants includes Children.
    ''' </summary>
    ''' <param name="sender">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Private Sub cbDescendants_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) _
        Handles cbDescendants.CheckedChanged

        Dim box As CheckBox = CType(sender, CheckBox)
        If box.Checked Then
            cbChildren.Enabled = False
        Else
            cbChildren.Enabled = True
        End If

    End Sub 'cbDescendants_CheckedChanged


    ''' <summary>
    ''' Prints information to the text box.
    ''' </summary>
    ''' <param name="output">String to print.</param>
    Public Sub OutputResults(ByVal output As String)

        tbOutput.AppendText(output)

    End Sub 'OutputResults


    ''' <summary>
    ''' Clears the output box.
    ''' </summary>
    ''' <param name="sender">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Private Sub btnClear_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnClear.Click

        tbOutput.Clear()

    End Sub 'btnClear_Click


    ''' <summary>
    ''' Responds to Get Properties button click; saves options and starts 
    ''' the UI Automation worker thread.
    ''' </summary>
    ''' <param name="sender">Object that raised the event.</param>
    ''' <param name="e">Event arguments.</param>
    Private Sub btnProps_Click(ByVal sender As Object, ByVal e As EventArgs) _
            Handles btnProps.Click

        ' Convert Drawing.Point to Windows.Point.
        Dim drawingPoint As System.Drawing.Point = Windows.Forms.Cursor.Position
        targetPoint = New System.Windows.Point(drawingPoint.X, drawingPoint.Y)

        ' Save caching settings in member variables so UI isn't accessed 
        ' directly by the other thread.
        If rbFull.Checked Then
            elementMode = AutomationElementMode.Full
        Else
            elementMode = AutomationElementMode.None
        End If

        ' For simplicity, always include Element in scope.
        cacheScope = TreeScope.Element
        If cbDescendants.Checked Then
            cacheScope = cacheScope Or TreeScope.Descendants
            ' Note: if descendants are specified, children 
            ' are automatically included.
        ElseIf cbChildren.Checked Then
            cacheScope = cacheScope Or TreeScope.Children
        End If

        fetcher = New UIAutomationFetcher(Me, targetPoint, cacheScope, elementMode)

        ' Start another thread to do the UI Automation work.
        Dim threadDelegate As New ThreadStart(AddressOf StartWorkerThread)
        workerThread = New Thread(threadDelegate)
        workerThread.Priority = ThreadPriority.Highest
        workerThread.Start()
        OutputResults(("Wait..." + Environment.NewLine))

    End Sub 'btnProps_Click


    ''' <summary>
    ''' Delegated method for ThreadStart.
    ''' </summary>
    ''' <remarks>
    ''' UI Automation must be called on a separate thread if the client application
    ''' itself might be a target.
    ''' </remarks>
    Sub StartWorkerThread()

        fetcher.DoWork()

    End Sub 'StartWorkerThread

End Class 'FetchTimerForm 


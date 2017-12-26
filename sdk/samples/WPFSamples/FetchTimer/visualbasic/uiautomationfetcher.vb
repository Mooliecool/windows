 '******************************************************************************
' *
' * File: UIAutoFetcher.vb
' *
' * Description: A Class that implements UI Automation functionality on a separate thread.
' * 
' * For a full description of the sample, see FetchTimerForm.vb.
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
' ******************************************************************************
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Windows.Forms
Imports System.Windows.Automation
Imports System.Diagnostics
Imports System.Threading



Class UIAutomationFetcher
    ' Member variables
    ' Application form, for output.
    Private appForm As FetchTimerForm
    
    ' Number of properties fetched for each test.
    Private Const numberOfFetches As Integer = 5
    
    Private treeScope As TreeScope
    Private mode As AutomationElementMode
    Private currentPropCount As Integer
    Private cachedPropCount As Integer
    Private elementCount As Integer
    Private targetPoint As System.Windows.Point
    
    
    
    ''' <summary>
    ''' Constructor.
    ''' </summary>
    ''' <param name="form">The application form.</param>
    ''' <param name="targetPoint">The screen coordinates of the cursor.</param>
    ''' <param name="scope">The TreeScope for caching.</param>
    ''' <param name="mode">The mode for caching.</param>
    Public Sub New(ByVal form As FetchTimerForm, ByVal targetPt As System.Windows.Point, _
        ByVal scope As TreeScope, ByVal cacheMode As AutomationElementMode)

        appForm = form
        treeScope = scope
        targetPoint = targetPt
        mode = cacheMode

    End Sub 'New


    Public Sub DoWork()
        Dim timeToGetUncached As Long = 0
        Dim timeToGetCached As Long = 0

        ' Create a System.Diagnostics.Stopwatch.
        Dim stopWatchTimer As New Stopwatch()

        ' TEST 1: Get the target element without caching, and retrieve
        '  current properties.

        stopWatchTimer.Start()
        Dim targetNoCache As AutomationElement = Nothing
        Try
            targetNoCache = AutomationElement.FromPoint(targetPoint)
        Catch
            OutputLine("Could not retrieve element.")
            Return
        End Try

        ' Get current properties.
        currentPropCount = 0
        GetCurrentProperties(targetNoCache, 0)
        stopWatchTimer.Stop()
        timeToGetUncached = stopWatchTimer.Elapsed.Ticks


        ' TEST 2: Get the target element with caching, and retrieve
        '   cached properties.

        ' Create CacheRequest.
        Dim fetchRequest As New CacheRequest()

        ' Add properties to fetch, and set options.
        With fetchRequest
            .Add(AutomationElement.NameProperty)
            .Add(AutomationElement.AutomationIdProperty)
            .Add(AutomationElement.ControlTypeProperty)
            .Add(AutomationElement.FrameworkIdProperty)
            .Add(AutomationElement.IsContentElementProperty)
            .AutomationElementMode = mode
            .TreeScope = treeScope
            .TreeFilter = Automation.RawViewCondition
        End With
        Dim targetCached As AutomationElement

        ' Activate the CacheRequest and fetch the target.
        Using fetchRequest.Activate
            stopWatchTimer.Reset()
            stopWatchTimer.Start()
            Try
                targetCached = AutomationElement.FromPoint(targetPoint)
            Catch Exception1 As System.InvalidOperationException
                OutputLine("InvalidOperationException. Could not retrieve element.")
                Return
            Catch Exception2 As System.Windows.Automation.ElementNotAvailableException
                OutputLine("ElementNotAvailableException. Could not retrieve element.")
                Return
            End Try
        End Using

        ' The CacheRequest is now inactive.

        ' Get cached properties.
        GetCachedProperties(targetCached, True)
        stopWatchTimer.Stop()
        timeToGetCached = stopWatchTimer.Elapsed.Ticks

        ' TEST 3: Get updated cache.

        stopWatchTimer.Reset()
        stopWatchTimer.Start()
        Dim updatedTargetCached As AutomationElement = Nothing
        Dim cacheUpdated As Boolean = False
        If mode = AutomationElementMode.Full Then
            updatedTargetCached = targetCached.GetUpdatedCache(fetchRequest)
            GetCachedProperties(updatedTargetCached, False)
            ' Fetches were counted again, so divide count by 2.
            cachedPropCount = cachedPropCount \ 2
            cacheUpdated = True
            stopWatchTimer.Stop()
        End If
        Dim updateTicks As Long = stopWatchTimer.Elapsed.Ticks

        ' END OF TESTS. 

        ' Display results
        Dim nameProperty As String = targetNoCache.Current.Name
        Dim framework As String = targetNoCache.Current.FrameworkId
        OutputLine(("Name: " + nameProperty))
        OutputLine(("Framework: " + framework))
        OutputLine((elementCount.ToString() + " cached element(s)."))

        OutputLine((timeToGetUncached.ToString("N0") + " Ticks to retrieve element(s) and get " + currentPropCount.ToString() + " current properties."))
        OutputLine((timeToGetCached.ToString("N0") + " Ticks to retrieve element(s) and get " + cachedPropCount.ToString() + " cached properties."))

        ' Show ratio between current and cached performance.
        Dim ratio As Single = System.Convert.ToSingle(timeToGetUncached) / System.Convert.ToSingle(timeToGetCached)
        If ratio > 2 Then
            OutputLine(("Current:Cached = " + ratio.ToString("N0") + ":1"))
        Else
            ' Print with decimal.
            OutputLine(("Current:Cached = " + ratio.ToString("N1") + ":1"))
        End If
        If cacheUpdated Then
            OutputLine((updateTicks.ToString("N0") + " Ticks to update cache and get properties."))
        Else
            OutputLine("Cannot update cache in None mode.")
        End If
        OutputLine("")

    End Sub 'DoWork


    ''' <summary>
    ''' Walks the tree and gets properties from all elements found. Recursive.
    ''' </summary>
    ''' <param name="element">Node to walk.</param>
    ''' <param name="depth">Depth of this iteration (distance from initial node).
    ''' </param>
    ''' <remarks>
    ''' Nothing is done with the objects retrieved.
    ''' </remarks>
    Private Sub GetCurrentProperties(ByVal element As AutomationElement, _
        ByVal depth As Integer)

        If treeScope = treeScope.Element AndAlso depth > 0 Then
            Return
        End If
        If (treeScope And treeScope.Descendants) = 0 AndAlso depth > 1 Then
            Return
        End If
        Dim name As String = element.Current.Name
        Dim id As String = element.Current.AutomationId
        Dim controlType As ControlType = element.Current.ControlType
        Dim framework As String = element.Current.FrameworkId
        currentPropCount = +currentPropCount + numberOfFetches

        Dim walker As TreeWalker = TreeWalker.ContentViewWalker
        Dim elementChild As AutomationElement = walker.GetFirstChild(element)
        While Not (elementChild Is Nothing)
            GetCurrentProperties(elementChild, depth + 1)
            elementChild = walker.GetNextSibling(elementChild)
        End While

    End Sub 'GetCurrentProperties

    
    ''' <summary>
    ''' Gets a set of cached properties. Recursive.
    ''' </summary>
    ''' <param name="element">The target element.</param>
    ''' <remarks>
    ''' Nothing is done with the objects retrieved.  
    ''' </remarks>
    Private Sub GetCachedProperties(ByVal element As AutomationElement, _
        ByVal updateCount As Boolean)

        If updateCount Then
            elementCount = elementCount + 1
        End If
        Dim name As String = element.Cached.Name
        Dim s As String = element.Cached.AutomationId
        Dim controlType As ControlType = element.Cached.ControlType
        Dim frame As String = element.Cached.FrameworkId
        cachedPropCount = cachedPropCount + numberOfFetches

        Dim child As AutomationElement
        ' Expect an error, as there might be no CachedChildren, in which 
        ' case an exception is raised when the property is accessed.
        On Error Resume Next
        For Each child In element.CachedChildren
            GetCachedProperties(child, updateCount)
        Next child

    End Sub 'GetCachedProperties


    ''' <summary>
    ''' Prints a line of text to the textbox.
    ''' </summary>
    ''' <param name="outputStr">The string to print.</param>
    ''' <remarks>
    ''' Must use Invoke so that UI is not being called directly from this thread.
    ''' </remarks>
    Private Sub OutputLine(ByVal outputStr As String) 

        appForm.Invoke(appForm.outputMethodInstance, _
            New [Object]() {outputStr + Environment.NewLine})
    
    End Sub 'OutputLine

End Class ' UIAutomationFetcher

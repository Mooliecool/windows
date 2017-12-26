'************************** Module Header ******************************'
' Module Name:  CpuUsageMonitorBase.vb
' Project:      VBCpuUsage
' Copyright (c) Microsoft Corporation.
' 
' This is the base class of ProcessCpuUsageMonitor and TotalCpuUsageMonitor. It
' supplies basic fields/events/functions/interfaces of the monitors, such as Timer, 
' PerformanceCounter and IDisposable interface.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Threading

Public MustInherit Class CpuUsageMonitorBase
    Implements IDisposable
    Private _locker As New Object()

    ' Specify whether this instance is disposed.
    Private _disposed As Boolean = False

    ' The timer is used to get the performance counter value every few seconds.
    Private _timer As Timer

    ' The CPU usage performance counter will be initialized in ProcessCpuUsageMonitor
    ' and TotalCpuUsageMonitor.
    Protected _cpuPerformanceCounter As PerformanceCounter = Nothing

    ' The max length of the CpuUsageValueArray.
    Private _valueArrayCapacity As Integer

    ' The list is used to store the values.
    Private _cpuUsageValueArray As List(Of Double)

    ''' <summary>
    ''' Occurred if there is a new added value.
    ''' </summary>
    Public Event CpuUsageValueArrayChanged As EventHandler(Of CpuUsageValueArrayChangedEventArg)

    Public Event ErrorOccurred As EventHandler(Of ErrorEventArgs)

    ''' <summary>
    ''' Initialize the timer and performance counter.
    ''' </summary>
    ''' <param name="timerPeriod">
    ''' If this value is no more than 0, then the timer will not be enabled.
    ''' </param>
    ''' <param name="valueArrayCapacity">
    ''' The max length of the CpuUsageValueArray.
    ''' </param>
    ''' <param name="categoryName">
    ''' The name of the performance counter category (performance object) with which 
    ''' this performance counter is associated. 
    ''' </param>
    ''' <param name="counterName">
    ''' The name of the performance counter. 
    ''' </param>
    ''' <param name="instanceName">
    ''' The name of the performance counter category instance, or an empty string (""),
    ''' if the category contains a single instance.
    ''' </param>
    Public Sub New(ByVal timerPeriod As Integer, ByVal valueArrayCapacity As Integer, _
                   ByVal categoryName As String, ByVal counterName As String, _
                   ByVal instanceName As String)

        ' Initialize the PerformanceCounter.
        Me._cpuPerformanceCounter = _
            New PerformanceCounter(categoryName, counterName, instanceName)

        Me._valueArrayCapacity = valueArrayCapacity
        _cpuUsageValueArray = New List(Of Double)()

        If timerPeriod > 0 Then

            ' Delay the timer to invoke callback.
            Me._timer = New Timer( _
                New TimerCallback(AddressOf Timer_Callback), Nothing, timerPeriod, timerPeriod)

        End If

    End Sub

    ''' <summary>
    ''' The method to be executed in the timer callback.
    ''' </summary>
    Private Sub Timer_Callback(ByVal obj As Object)
        SyncLock _locker

            ' Clear the list.
            If Me._cpuUsageValueArray.Count > Me._valueArrayCapacity Then
                Me._cpuUsageValueArray.Clear()
            End If

            Try
                Dim value As Double = GetCpuUsage()

                If value < 0 Then
                    value = 0
                End If
                If value > 100 Then
                    value = 100
                End If

                Me._cpuUsageValueArray.Add(value)

                Dim values(_cpuUsageValueArray.Count - 1) As Double
                _cpuUsageValueArray.CopyTo(values, 0)

                ' Raise the event.
                Me.OnCpuUsageValueArrayChanged( _
                    New CpuUsageValueArrayChangedEventArg() With {.Values = values})
            Catch ex As Exception
                Me.OnErrorOccurred(New ErrorEventArgs With {.Error = ex})
            End Try
        End SyncLock
    End Sub

    ''' <summary>
    ''' Get current CPU usage.
    ''' </summary>
    Protected Overridable Function GetCpuUsage() As Double
        If Not IsInstanceExist() Then
            Throw New ApplicationException( _
                String.Format("The instance {0} is not available. ", _
                              Me._cpuPerformanceCounter.InstanceName))
        End If


        Dim value As Double = _cpuPerformanceCounter.NextValue()
        Return value
    End Function

    ''' <summary>
    ''' Child class may override this method to determine whether the instance exists.
    ''' </summary>
    Protected Overridable Function IsInstanceExist() As Boolean
        Return True
    End Function

    ''' <summary>
    ''' Raise the CpuUsageValueArrayChanged event.
    ''' </summary>
    Protected Sub OnCpuUsageValueArrayChanged(ByVal e As CpuUsageValueArrayChangedEventArg)
        RaiseEvent CpuUsageValueArrayChanged(Me, e)
    End Sub

    ''' <summary>
    ''' Raise the ErrorOccurred event.
    ''' </summary>
    Protected Overridable Sub OnErrorOccurred(ByVal e As ErrorEventArgs)
        RaiseEvent ErrorOccurred(Me, e)
    End Sub

    ' Release the resources.
    Public Sub Dispose() Implements IDisposable.Dispose
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub

    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        ' Protect from being called multiple times.
        If _disposed Then
            Return
        End If

        If disposing Then
            If _timer IsNot Nothing Then
                _timer.Dispose()
            End If

            If _cpuPerformanceCounter IsNot Nothing Then
                _cpuPerformanceCounter.Dispose()
            End If
            _disposed = True
        End If
    End Sub
End Class

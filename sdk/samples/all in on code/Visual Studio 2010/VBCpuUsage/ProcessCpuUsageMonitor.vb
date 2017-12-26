'************************** Module Header ******************************'
' Module Name:  ProcessCpuUsageMonitor.vb
' Project:      VBCpuUsage
' Copyright (c) Microsoft Corporation.
' 
' This class inherits CpuUsageMonitorBase and it is used to monitor the CPU usage of 
' a specified process. It also supplies a method to get available processes.
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


Public Class ProcessCpuUsageMonitor
    Inherits CpuUsageMonitorBase

    Private Const _categoryName As String = "Process"
    Private Const _counterName As String = "% Processor Time"

    Private Shared _category As PerformanceCounterCategory
    Public Shared ReadOnly Property Category() As PerformanceCounterCategory
        Get
            If _category Is Nothing Then
                _category = New PerformanceCounterCategory(_categoryName)
            End If
            Return _category
        End Get
    End Property

    Public Sub New(ByVal processName As String, ByVal timerPeriod As Integer,
                   ByVal valueArrayCapacity As Integer)
        MyBase.New(timerPeriod, valueArrayCapacity, _categoryName, _counterName, processName)
    End Sub

    Protected Overrides Function IsInstanceExist() As Boolean
        Return Category.InstanceExists(Me._cpuPerformanceCounter.InstanceName)
    End Function

    Public Shared Function GetAvailableProcesses() As String()
        Return Category.GetInstanceNames().OrderBy(Function(name) name).ToArray()
    End Function
End Class

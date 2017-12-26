'************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:      VBCpuUsage
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to handle the UI
' event and display the CPU usage charts.
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


Partial Public Class MainForm
    Inherits Form

    ' The monitor of the total CPU usage.
    Private _totalCpuUsageMonitor As TotalCpuUsageMonitor

    ' The CPU usage monitor of a specified process.
    Private _processCpuUsageMonitor As ProcessCpuUsageMonitor

    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    '''  Add the available processes to the combobox.
    ''' </summary>
    Private Sub cmbProcess_DropDown(ByVal sender As Object, ByVal e As EventArgs) _
        Handles cmbProcess.DropDown

        cmbProcess.DataSource = ProcessCpuUsageMonitor.GetAvailableProcesses()
        cmbProcess.SelectedIndex = 0
    End Sub

    ''' <summary>
    ''' Handle the btnStart Click event,
    ''' </summary>
    Private Sub btnStart_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnStart.Click

        If (Not chkTotalUsage.Checked) AndAlso (Not chkProcessCpuUsage.Checked) Then
            Return
        End If

        Try
            StartMonitor()
        Catch ex As Exception
            StopMonitor()
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    ''' <summary>
    ''' Handle the btnStop Click event.
    ''' If this button is clicked, dispose totalCpuUsageMonitor and processCpuUsageMonitor.
    ''' </summary>
    Private Sub btnStop_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnStop.Click

        StopMonitor()
    End Sub

    Private Sub StartMonitor()
        ' Initialize the totalCpuUsageMonitor and register the CpuUsageValueArrayChanged
        ' event.
        If chkTotalUsage.Checked Then
            _totalCpuUsageMonitor = New TotalCpuUsageMonitor(1000, 100)
            AddHandler _totalCpuUsageMonitor.CpuUsageValueArrayChanged,
                AddressOf totalCpuUsageMonitor_CpuUsageValueArrayChanged
            AddHandler _totalCpuUsageMonitor.ErrorOccurred,
                AddressOf totalCpuUsageMonitor_ErrorOccurred

        End If

        ' Initialize the processCpuUsageMonitor and register the CpuUsageValueArrayChanged
        ' event.
        If chkProcessCpuUsage.Checked AndAlso _
            (Not String.IsNullOrEmpty(TryCast(cmbProcess.SelectedItem, String))) Then

            _processCpuUsageMonitor =
                New ProcessCpuUsageMonitor(TryCast(cmbProcess.SelectedItem, String), 1000, 100)
            AddHandler _processCpuUsageMonitor.CpuUsageValueArrayChanged,
                AddressOf processCpuUsageMonitor_CpuUsageValueArrayChanged

            AddHandler _processCpuUsageMonitor.ErrorOccurred,
                AddressOf processCpuUsageMonitor_ErrorOccurred
        End If

        ' Update the UI.
        btnStart.Enabled = False
        btnStop.Enabled = True
    End Sub

    Private Sub StopMonitor()
        If _totalCpuUsageMonitor IsNot Nothing Then
            _totalCpuUsageMonitor.Dispose()
            _totalCpuUsageMonitor = Nothing
        End If

        If _processCpuUsageMonitor IsNot Nothing Then
            _processCpuUsageMonitor.Dispose()
            _processCpuUsageMonitor = Nothing
        End If

        ' Update the UI.
        btnStart.Enabled = True
        btnStop.Enabled = False
    End Sub

    ''' <summary>
    ''' Invoke the processCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
    ''' the CpuUsageValueArrayChanged event of processCpuUsageMonitor.
    ''' </summary>
    Private Sub processCpuUsageMonitor_CpuUsageValueArrayChanged(ByVal sender As Object,
                                                                 ByVal e As CpuUsageValueArrayChangedEventArg)
        Me.Invoke(New EventHandler(Of CpuUsageValueArrayChangedEventArg)(
                  AddressOf processCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e)
    End Sub

    Private Sub processCpuUsageMonitor_CpuUsageValueArrayChangedHandler(ByVal sender As Object,
                                                                        ByVal e As CpuUsageValueArrayChangedEventArg)
        Dim processCpuUsageSeries = chartProcessCupUsage.Series("ProcessCpuUsageSeries")
        Dim values = e.Values

        ' Display the process CPU usage in the chart.
        processCpuUsageSeries.Points.DataBindY(e.Values)

    End Sub

    ''' <summary>
    ''' Invoke the processCpuUsageMonitor_ErrorOccurredHandler to handle
    ''' the ErrorOccurred event of processCpuUsageMonitor.
    ''' </summary>
    Private Sub processCpuUsageMonitor_ErrorOccurred(ByVal sender As Object,
                                                     ByVal e As ErrorEventArgs)
        Me.Invoke(New EventHandler(Of ErrorEventArgs)(
                  AddressOf processCpuUsageMonitor_ErrorOccurredHandler), sender, e)
    End Sub

    Private Sub processCpuUsageMonitor_ErrorOccurredHandler(ByVal sender As Object,
                                                            ByVal e As ErrorEventArgs)
        If _processCpuUsageMonitor IsNot Nothing Then
            _processCpuUsageMonitor.Dispose()
            _processCpuUsageMonitor = Nothing

            Dim processCpuUsageSeries = chartProcessCupUsage.Series("ProcessCpuUsageSeries")
            processCpuUsageSeries.Points.Clear()
        End If
        MessageBox.Show(e.Error.Message)
    End Sub

    ''' <summary>
    ''' Invoke the totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
    ''' the CpuUsageValueArrayChanged event of processCpuUsageMonitor.
    ''' </summary>
    Private Sub totalCpuUsageMonitor_CpuUsageValueArrayChanged(ByVal sender As Object,
                                                               ByVal e As CpuUsageValueArrayChangedEventArg)
        Me.Invoke(New EventHandler(Of CpuUsageValueArrayChangedEventArg)(
                  AddressOf totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e)
    End Sub
    Private Sub totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler(ByVal sender As Object,
                                                                      ByVal e As CpuUsageValueArrayChangedEventArg)
        Dim totalCpuUsageSeries = chartTotalCpuUsage.Series("TotalCpuUsageSeries")
        Dim values = e.Values

        ' Display the total CPU usage in the chart.
        totalCpuUsageSeries.Points.DataBindY(e.Values)

    End Sub

    ''' <summary>
    ''' Invoke the totalCpuUsageMonitor_ErrorOccurredHandler to handle
    ''' the ErrorOccurred event of totalCpuUsageMonitor.
    ''' </summary>
    Private Sub totalCpuUsageMonitor_ErrorOccurred(ByVal sender As Object,
                                                   ByVal e As ErrorEventArgs)
        Me.Invoke(New EventHandler(Of ErrorEventArgs)(
                  AddressOf totalCpuUsageMonitor_ErrorOccurredHandler), sender, e)
    End Sub

    Private Sub totalCpuUsageMonitor_ErrorOccurredHandler(ByVal sender As Object,
                                                          ByVal e As ErrorEventArgs)
        If _totalCpuUsageMonitor IsNot Nothing Then
            _totalCpuUsageMonitor.Dispose()
            _totalCpuUsageMonitor = Nothing

            Dim totalCpuUsageSeries = chartTotalCpuUsage.Series("TotalCpuUsageSeries")
            totalCpuUsageSeries.Points.Clear()
        End If
        MessageBox.Show(e.Error.Message)
    End Sub
End Class

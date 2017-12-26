'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFThreading
' Copyright (c) Microsoft Corporation.

' The VBWPFThreading sample project illustrates two WPF threading models. The
' first one divides a long-running process into many snippets of workitems.  
' Then the dispather of WPF will pick up the workitems one by one from the 
' queue by their priority. The background workitem does not affect the UI 
' operation, so it just looks like the background workitem is processed by 
' another thread. But actually, all of them are executed in the same thread. 
' This trick is very useful if you want single threaded GUI application, and 
' also want to keep the GUI responsive when doing expensive operations in  
' the UI thread. 
'
' The second model is similar to the traditional WinForm threading model. The 
' background work item is executed in another thread and it calls the 
' Dispatcher.BeginInvoke method to update the UI.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Using directives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Data
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Imaging
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Windows.Threading
Imports System.Threading
#End Region


''' <summary> 
''' Interaction logic for MainWindow.xaml 
''' </summary> 
Partial Public Class MainWindow
    Inherits Window

#Region "Long-Running Calculation in UI Thread"

    Public Delegate Sub NextPrimeDelegate()
    Private num As Long = 3
    Private continueCalculating As Boolean = False
    Private fNotAPrime As Boolean = False

    Private Sub btnPrimeNumber_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        If continueCalculating Then
            continueCalculating = False
            btnPrimeNumber.Content = "Resume"
        Else
            continueCalculating = True
            btnPrimeNumber.Content = "Stop"
            btnPrimeNumber.Dispatcher.BeginInvoke(DispatcherPriority.Normal, New NextPrimeDelegate(AddressOf CheckNextNumber))
        End If
    End Sub

    Public Sub CheckNextNumber()
        ' Reset flag. 
        fNotAPrime = False

        For i As Long = 3 To Math.Sqrt(num)
            If num Mod i = 0 Then
                ' Set not-a-prime flag to true. 
                fNotAPrime = True
                Exit For
            End If
        Next

        ' If a prime number. 
        If Not fNotAPrime Then
            tbPrime.Text = num.ToString()
        End If

        num += 2
        If continueCalculating Then
            btnPrimeNumber.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.SystemIdle, New NextPrimeDelegate(AddressOf Me.CheckNextNumber))
        End If
    End Sub

#End Region


#Region "Blocking Operation in Worker Thread"

    Private Delegate Sub NoArgDelegate()
    Private Delegate Sub OneArgDelegate(ByVal arg As Int32())

    Private Sub btnRetrieveData_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.btnRetrieveData.IsEnabled = False
        Me.btnRetrieveData.Content = "Contacting Server"

        Dim fetcher As New NoArgDelegate(AddressOf Me.RetrieveDataFromServer)
        fetcher.BeginInvoke(Nothing, Nothing)
    End Sub

    ''' <summary> 
    ''' Retrieve data in a worker thread. 
    ''' </summary> 
    Private Sub RetrieveDataFromServer()
        ' Simulate the delay from network access. 
        Thread.Sleep(5000)

        ' Generate random data to be displayed. 
        Dim rand As New Random()
        Dim data As Int32() = {rand.[Next](1000), rand.[Next](1000), rand.[Next](1000), rand.[Next](1000)}

        ' Schedule the update function in the UI thread. 
        Me.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Normal, New OneArgDelegate(AddressOf UpdateUserInterface), data)
    End Sub

    ''' <summary> 
    ''' Update the UI about the new data. The function runs in the UI thread. 
    ''' </summary> 
    ''' <param name="data"></param> 
    Private Sub UpdateUserInterface(ByVal data As Int32())
        Me.btnRetrieveData.IsEnabled = True
        Me.btnRetrieveData.Content = "Retrieve Data from Server"
        Me.tbData1.Text = data(0).ToString()
        Me.tbData2.Text = data(1).ToString()
        Me.tbData3.Text = data(2).ToString()
        Me.tbData4.Text = data(3).ToString()
    End Sub

#End Region

End Class
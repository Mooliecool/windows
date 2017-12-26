'*************************** Module Header ******************************\
' Module Name:  RegistryWatcher.vb
' Project:	    VBMonitorRegistryChange
' Copyright (c) Microsoft Corporation.
' 
' This class derived from ManagementEventWatcher. It is used to 
' 1. Supply the supported hives.
' 2. Construct a WqlEventQuery from Hive and KeyPath.
' 3. Wrap the EventArrivedEventArgs to RegistryKeyChangeEventArg.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.Collections.ObjectModel
Imports System.Management
Imports Microsoft.Win32

Friend Class RegistryWatcher
    Inherits ManagementEventWatcher
    Implements IDisposable

    Private Shared _supportedHives As ReadOnlyCollection(Of RegistryKey) = Nothing

    ''' <summary>
    ''' Changes to the HKEY_CLASSES_ROOT and HKEY_CURRENT_USER hives are not supported
    ''' by RegistryEvent or classes derived from it, such as RegistryKeyChangeEvent. 
    ''' </summary>
    Public Shared ReadOnly Property SupportedHives() As ReadOnlyCollection(Of RegistryKey)
        Get
            If _supportedHives Is Nothing Then
                Dim hives() As RegistryKey = {Registry.LocalMachine, Registry.Users, Registry.CurrentConfig}
                _supportedHives = Array.AsReadOnly(Of RegistryKey)(hives)
            End If
            Return _supportedHives
        End Get
    End Property


    Private _hive As RegistryKey
    Public Property Hive() As RegistryKey
        Get
            Return _hive
        End Get
        Private Set(ByVal value As RegistryKey)
            _hive = value
        End Set
    End Property

    Private _keyPath As String
    Public Property KeyPath() As String
        Get
            Return _keyPath
        End Get
        Private Set(ByVal value As String)
            _keyPath = value
        End Set
    End Property

    Private _keyToMonitor As RegistryKey
    Public Property KeyToMonitor() As RegistryKey
        Get
            Return _keyToMonitor
        End Get
        Private Set(ByVal value As RegistryKey)
            _keyToMonitor = value
        End Set
    End Property

    Public Event RegistryKeyChangeEvent As EventHandler(Of RegistryKeyChangeEventArgs)

    ''' <exception cref="System.Security.SecurityException">
    ''' Thrown when current user does not have the permission to access the key 
    ''' to monitor.
    ''' </exception> 
    ''' <exception cref="System.ArgumentException">
    ''' Thrown when the key to monitor does not exist.
    ''' </exception> 
    Public Sub New(ByVal hive As RegistryKey, ByVal keyPath As String)
        Me.Hive = hive
        Me.KeyPath = keyPath

        ' If you set the platform of this project to x86 and run it on a 64bit 
        ' machine, you will get the Registry Key under 
        ' HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node when the key path is
        ' HKEY_LOCAL_MACHINE\SOFTWARE
        Me.KeyToMonitor = hive.OpenSubKey(keyPath)

        If KeyToMonitor IsNot Nothing Then
            ' Construct the query string.
            Dim queryString As String = String.Format("SELECT * FROM RegistryKeyChangeEvent " & ControlChars.CrLf & "                   WHERE Hive = '{0}' AND KeyPath = '{1}' ", Me.Hive.Name, Me.KeyPath)

            Dim query As New WqlEventQuery()
            query.QueryString = queryString
            query.EventClassName = "RegistryKeyChangeEvent"
            query.WithinInterval = New TimeSpan(0, 0, 0, 1)
            Me.Query = query

            AddHandler EventArrived, AddressOf RegistryWatcher_EventArrived
        Else
            Dim message As String = String.Format("The registry key {0}\{1} does not exist", hive.Name, keyPath)
            Throw New ArgumentException(message)

        End If
    End Sub

    Private Sub RegistryWatcher_EventArrived(ByVal sender As Object, ByVal e As EventArrivedEventArgs)

        ' Get RegistryKeyChangeEventArgs from EventArrivedEventArgs.NewEvent.Properties.
        Dim args As New RegistryKeyChangeEventArgs(e.NewEvent)

        ' Raise the event handler. 
        RaiseEvent RegistryKeyChangeEvent(sender, args)

    End Sub

    ''' <summary>
    ''' Dispose the RegistryKey.
    ''' </summary>
    Public Shadows Sub Dispose()
        MyBase.Dispose()
        If Me.KeyToMonitor IsNot Nothing Then
            Me.KeyToMonitor.Close()
        End If
    End Sub

End Class

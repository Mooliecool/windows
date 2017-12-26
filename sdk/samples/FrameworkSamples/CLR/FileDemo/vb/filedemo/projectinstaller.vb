'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------

Imports System.ComponentModel
Imports System.Configuration.Install

<RunInstaller(True)> Public Class ProjectInstaller
    Inherits System.Configuration.Install.Installer

#Region " Component Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Component Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Installer overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub
    Friend WithEvents eventLogMainInstaller As System.Diagnostics.EventLogInstaller

    'Required by the Component Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Component Designer
    'It can be modified using the Component Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> Private Sub InitializeComponent()
        Me.eventLogMainInstaller = New System.Diagnostics.EventLogInstaller
        '
        'eventLogMainInstaller
        '
        Me.eventLogMainInstaller.CategoryCount = 3
        Me.eventLogMainInstaller.CategoryResourceFile = "C:\Demos\FileDemo\bin\ExplorerResources.dll"
        Me.eventLogMainInstaller.Log = "ManagedExplorer"
        Me.eventLogMainInstaller.MessageResourceFile = "C:\Demos\FileDemo\bin\ExplorerResources.dll"
        Me.eventLogMainInstaller.ParameterResourceFile = Nothing
        Me.eventLogMainInstaller.Source = "ManagedExplorer"
        '
        'ProjectInstaller
        '
        Me.Installers.AddRange(New System.Configuration.Install.Installer() {Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller, Me.eventLogMainInstaller})

    End Sub

#End Region

End Class

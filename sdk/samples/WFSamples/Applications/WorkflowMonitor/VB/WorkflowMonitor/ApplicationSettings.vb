'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
'---------------------------------------------------------------------

Imports System.IO
Imports System.Xml.Serialization

' This class is used to store user settings such as server and database names
Public Class ApplicationSettings
    ' Set to true if the settings have changed since last saved
    Private applicationSettingsChangedValue As Boolean
    Private pollingIntervalValue As Integer
    Private serverNameValue As String
    Private databaseNameValue As String
    Private autoSelectLatestValue As Boolean

    Friend Sub New()
        applicationSettingsChangedValue = False
    End Sub

    ' Save app info to the config file
    Friend Sub SaveSettings(ByVal path As String)
        If applicationSettingsChangedValue Then
            Dim writer As StreamWriter = Nothing
            Dim serializer As XmlSerializer = Nothing
            Try
                ' Create an XmlSerializer for the 
                ' ApplicationSettings type.
                serializer = New XmlSerializer(GetType(ApplicationSettings))
                writer = New StreamWriter(path, False)
                ' Serialize this instance of the ApplicationSettings 
                ' class to the config file.
                serializer.Serialize(writer, Me)
            Catch
            Finally
                ' If the FileStream is open, close it.
                If writer IsNot Nothing Then
                    writer.Close()
                End If
            End Try
        End If
    End Sub

    ' Load app info from the config file
    Friend Function LoadAppSettings(ByVal path As String) As Boolean
        Dim serializer As XmlSerializer = Nothing
        Dim fileStream As FileStream = Nothing
        Dim fileExists As Boolean = False

        Try
            ' Create an XmlSerializer for the ApplicationSettings type.
            serializer = New XmlSerializer(GetType(ApplicationSettings))
            Dim info As FileInfo = New FileInfo(path)
            ' If the config file exists, open it.
            If info.Exists Then
                fileStream = info.OpenRead()
                ' Create a new instance of the ApplicationSettings by
                ' deserializing the config file.
                Dim applicationSettings As ApplicationSettings = CType(serializer.Deserialize(fileStream), ApplicationSettings)
                ' Assign the property values to this instance of 
                ' the ApplicationSettings class.
                Me.databaseNameValue = applicationSettings.databaseNameValue
                Me.serverNameValue = applicationSettings.serverNameValue
                Me.pollingIntervalValue = applicationSettings.pollingIntervalValue
                Me.autoSelectLatestValue = applicationSettings.autoSelectLatestValue

                fileExists = True
            End If
        Catch
        Finally
            ' If the FileStream is open, close it.
            If fileStream IsNot Nothing Then
                fileStream.Close()
            End If
        End Try

        Return fileExists
    End Function

    Public Property PollingInterval() As Integer
        Get
            Return pollingIntervalValue
        End Get
        Set(ByVal Value As Integer)
            If Value <> pollingIntervalValue Then
                pollingIntervalValue = Value
                applicationSettingsChangedValue = True
            End If
        End Set
    End Property

    Public ReadOnly Property ApplicationSettingsChanged() As Boolean
        Get
            Return applicationSettingsChangedValue
        End Get
    End Property

    Public Property ServerName() As String
        Get
            Return serverNameValue
        End Get
        Set(ByVal Value As String)
            If Value IsNot serverNameValue Then
                serverNameValue = Value
                applicationSettingsChangedValue = True
            End If
        End Set
    End Property

    Public Property DatabaseName() As String
        Get
            Return databaseNameValue
        End Get
        Set(ByVal Value As String)
            If Value IsNot databaseNameValue Then
                databaseNameValue = Value
                applicationSettingsChangedValue = True
            End If
        End Set
    End Property

    Public Property AutoSelectLatest() As Boolean
        Get
            Return autoSelectLatestValue
        End Get
        Set(ByVal Value As Boolean)
            If Value <> autoSelectLatestValue Then
                autoSelectLatestValue = Value
                applicationSettingsChangedValue = True
            End If
        End Set
    End Property
End Class

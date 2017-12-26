'---------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'   Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System.Configuration
Imports System.Data.SqlClient
Imports System.IO
Imports System.Workflow.Runtime.Tracking

Public Class TrackingProfileStore
    Public ReadOnly Property ConnectionString() As String
        Get
            ' Try to read the connection string from the config file
            Dim reader As New AppSettingsReader()
            Try
                Return CType(reader.GetValue("ConnectionString", GetType(String)), String)
            Catch
            End Try
            Return Nothing
        End Get
    End Property


    ' Validate that a connection can be established
    Public Function ValidateConnection(ByVal errorMessage As String) As Boolean
        errorMessage = String.Empty

        Using connection As New SqlConnection(ConnectionString)
            Try
                connection.Open()
            Catch ex As Exception
                errorMessage = ex.Message
                Return False
            End Try
        End Using
        Return True
    End Function


    ' Loads a workflow and profile from the SQL Tracking Database, using the LoadFromStore winform.

    Public Sub LoadWorkflowAndProfile(ByRef workflowType As Type, ByRef profile As TrackingProfile)
        Dim loadFromStoreForm As New LoadFromStore()
        loadFromStoreForm.WorkflowProfiles = GetWorkflowAndProfiles()
        Dim result As DialogResult = loadFromStoreForm.ShowDialog()
        If result = Windows.Forms.DialogResult.OK Then
            workflowType = loadFromStoreForm.SelectedWorkflow
            If workflowType IsNot Nothing Then
                profile = GetWorkflowProfile(workflowType, loadFromStoreForm.SelectedProfileVersion, True)
            Else
                profile = Nothing

            End If
        Else
            workflowType = Nothing
            profile = Nothing
            Return
        End If
    End Sub


    ' Retrieves a table containing all the workflow types and associated profile versions, from the SQL Tracking Database
    Function GetWorkflowAndProfiles() As DataTable
        Dim query As String = _
            "select TypeId,TypeFullName,AssemblyFullName,Version from Type, TrackingProfile where Type.TypeId = TrackingProfile.WorkflowTypeId"

        Using connection As New SqlConnection(ConnectionString)
            Dim adapter As New SqlDataAdapter(query, connection)
            Dim table As New DataTable()
            adapter.Fill(table)
            Return table
        End Using
    End Function


    ' Saves a profile for the specified workflow type
    Public Sub SaveProfile(ByVal workflowType As Type, ByVal profile As TrackingProfile)
        Dim currentProfile As TrackingProfile = GetWorkflowProfile(workflowType, False)

        If currentProfile IsNot Nothing AndAlso currentProfile.Version >= profile.Version Then

            ' If there already exists a profile (with a later version)
            ' for this workflow, prompt the user to rev the version.    
            Dim updateVersionForm As New UpdateProfileVersion()
            updateVersionForm.CurrentTrackingProfile = currentProfile
            updateVersionForm.NewTrackingProfile = profile
            updateVersionForm.WorkflowType = workflowType
            Dim result As DialogResult = updateVersionForm.ShowDialog()
            If Not result = Windows.Forms.DialogResult.OK Then Return
        End If

        Try
            Using connection As New SqlConnection(ConnectionString)
                Using command As New SqlCommand()
                    command.CommandText = "[dbo].[UpdateTrackingProfile]"
                    command.CommandType = CommandType.StoredProcedure
                    command.Connection = connection

                    Dim typeFullName As New SqlParameter("@TypeFullName", workflowType.FullName)
                    Dim assemblyFullName As New SqlParameter("@AssemblyFullName", workflowType.Assembly.FullName)

                    command.Parameters.Add(typeFullName)
                    command.Parameters.Add(assemblyFullName)

                    Dim version As New SqlParameter("@Version", profile.Version.ToString())
                    Dim serializedProfile As New SqlParameter("@TrackingProfileXml", New TrackingProfileManager(profile).SerializeProfile())

                    command.Parameters.Add(version)
                    command.Parameters.Add(serializedProfile)

                    connection.Open()
                    command.ExecuteNonQuery()
                    MessageBox.Show("Save successful!", "Success")
                End Using
            End Using
        Catch ex As SqlException
            MessageBox.Show(String.Format("Error saving profile: {0}", ex.Message), "Error")
        End Try
    End Sub

    ' Retrieves a profile from the SQL Tracking Database
        
    Function GetWorkflowProfile(ByVal workflowType As Type, ByVal createDefault As Boolean) As TrackingProfile
        Return GetWorkflowProfile(workflowType, Nothing, createDefault)
    End Function


    ' Retrieves a profile from the SQL Tracking Database

    Function GetWorkflowProfile(ByVal workflowType As Type, ByVal version As Version, ByVal createDefault As Boolean) As TrackingProfile
        Dim reader As SqlDataReader = Nothing
        Dim profile As TrackingProfile = Nothing
        Try
            Using connection As New SqlConnection(ConnectionString)
                Using command As New SqlCommand()
                    command.CommandType = CommandType.StoredProcedure
                    command.CommandText = "[dbo].[GetTrackingProfile]"
                    command.Connection = connection

                    Dim typeFullName As New SqlParameter("@TypeFullName", workflowType.FullName)
                    Dim assemblyFullName As New SqlParameter("@AssemblyFullName", workflowType.Assembly.FullName)
                    Dim createDefaultParameter As New SqlParameter("@CreateDefault", IIf(createDefault, 1, 0))

                    command.Parameters.Add(typeFullName)
                    command.Parameters.Add(assemblyFullName)
                    command.Parameters.Add(createDefaultParameter)

                    ' Filter by version as well, if specified
                    If version IsNot Nothing Then
                        Dim versionParameter As New SqlParameter("@Version", version.ToString())
                        command.Parameters.Add(versionParameter)
                    End If

                    connection.Open()
                    reader = command.ExecuteReader(CommandBehavior.CloseConnection)
                    If (reader.Read()) Then
                        If (reader.IsDBNull(0)) Then
                            Return Nothing
                        End If

                        Dim tmp As String = reader.GetString(0)
                        Dim serializer As New TrackingProfileSerializer()
                        Dim pReader As StringReader = Nothing
                        Try
                            pReader = New StringReader(tmp)
                            profile = serializer.Deserialize(pReader)
                        Finally
                            If pReader IsNot Nothing Then
                                pReader.Close()
                            End If
                        End Try
                    End If
                End Using
            End Using
        Catch ex As SqlException
            MessageBox.Show(String.Format("Error retrieving profile: {0}", ex.Message), "Error")
        End Try
        Return profile
    End Function
End Class

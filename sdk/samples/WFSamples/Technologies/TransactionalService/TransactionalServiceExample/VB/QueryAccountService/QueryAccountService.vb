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

Imports System
Imports System.Collections.Generic
Imports System.Data
Imports System.Data.SqlClient

' This class is an implementation of the IQueryAccount Service
' This service will be invoked by the workflow to query the amount
Public Class QueryAccountService
    Implements IQueryAccountService

    Private connectionStringValue As String

    Public Sub New(ByVal connectionString As String)
        Me.connectionStringValue = connectionString
    End Sub

    Public Function QueryAccount(ByVal accountNumber As Integer) As Int32() Implements IQueryAccountService.QueryAccount
        Dim accountBalances() As Int32 = New Int32() {0, 0}

        Dim cmd As SqlCommand = New SqlCommand()
        Dim reader As SqlDataReader = Nothing

        cmd.CommandType = CommandType.StoredProcedure
        cmd.CommandText = "dbo.GetAccountBalances"
        cmd.Connection = New SqlConnection(connectionStringValue)

        Dim param As SqlParameter = New SqlParameter()
        param.ParameterName = "@AccountNumber"
        param.SqlDbType = SqlDbType.Int
        param.SqlValue = accountNumber
        cmd.Parameters.Add(param)

        Try
            cmd.Connection.Open()
            reader = cmd.ExecuteReader(CommandBehavior.CloseConnection)

            If Not (reader.HasRows) Then
                Throw New Exception("There are no rows in the database matching the account number")
            End If

            While reader.Read()
                ' Checking Amount
                accountBalances(0) = reader.GetInt32(0)

                ' Savings Amount
                accountBalances(1) = reader.GetInt32(1)
            End While
        Finally
            reader.Close()
            If (cmd IsNot Nothing) AndAlso (cmd.Connection IsNot Nothing) AndAlso (ConnectionState.Closed <> cmd.Connection.State) Then
                cmd.Connection.Close()
            End If
        End Try

        Return accountBalances
    End Function
End Class

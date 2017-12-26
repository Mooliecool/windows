'***************************** Module Header ******************************\
'* Module Name:    DBProcess.vb
'* Project:        VBASPNETExcelLikeGridView
'* Copyright (c) Microsoft Corporation
'*
'* This module is managing the Connection,adapter as well as datatable
'* instances.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'\****************************************************************************


Imports System.Data.SqlClient
Imports System.Configuration
Imports System.Data
Imports System.IO
Imports System.Runtime.Serialization.Formatters.Binary

''' <summary>
''' This is a class that manages the whole db's connection, create a 
''' memory-based datatable, maintaince the state and do a batch save 
''' things together in different functions.
''' </summary>
Public NotInheritable Class DBProcess
    Private Shared conn As SqlConnection = Nothing
    Private Shared adapter As SqlDataAdapter = Nothing
    Private dt As DataTable = Nothing

    ''' <summary>
    ''' This static constructor will read out the whole connect string
    ''' from defined web.config. The connection and adapter are both
    ''' pointing to the same db, so only create once.
    ''' </summary>
    Shared Sub New()
        Dim constr As String = ConfigurationManager.ConnectionStrings("MyConn").ConnectionString
        conn = New SqlConnection(constr)
        Dim command As String = "select * from tb_personInfo"
        adapter = New SqlDataAdapter(command, conn)
        Dim builder As New SqlCommandBuilder(adapter)
        builder.GetDeleteCommand(True)
        builder.GetInsertCommand(True)
        builder.GetUpdateCommand(True)
    End Sub

    ''' <summary>
    ''' This function will create a datatable to reload all the data
    ''' from the db.
    ''' </summary>
    Public Function GetDataTable() As DataTable
        dt = New DataTable()
        adapter.Fill(dt)
        dt.Columns(0).AutoIncrement = True
        dt.Columns(0).AutoIncrementStep = 1
        dt.Columns(0).AutoIncrementSeed = dt.Rows.Count
        Return dt
    End Function

    ''' <summary>
    ''' Update the DataTable and delete the serialized file.
    ''' </summary>
    Public Sub BatchSave(ByVal dt As DataTable)
        adapter.Update(dt)
    End Sub
End Class

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
Imports System.IO
Imports System.ComponentModel.Design.Serialization
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design

Friend NotInheritable Class WorkflowLoader
    Inherits WorkflowDesignerLoader

    Friend Sub New()
    End Sub

    Protected Overrides Sub Initialize()
        MyBase.Initialize()

        Dim host As IDesignerLoaderHost = LoaderHost
        If Not host Is Nothing Then
            Dim typeProvider As TypeProvider = New TypeProvider(host)
            typeProvider.AddAssemblyReference(GetType(String).Assembly.Location)
        End If
    End Sub

    Public Overrides Sub Dispose()
        Try
            Dim host As IDesignerLoaderHost = LoaderHost
            If Not host Is Nothing Then
                host.RemoveService(GetType(ITypeProvider), True)
            End If
        Finally
            MyBase.Dispose()
        End Try
    End Sub

    Public Overrides ReadOnly Property FileName() As String
        Get
            Return String.Empty
        End Get
    End Property

    Public Overrides Function GetFileReader(ByVal filePath As String) As System.IO.TextReader
        Return New StreamReader(New FileStream(filePath, FileMode.OpenOrCreate))
    End Function

    Public Overrides Function GetFileWriter(ByVal filePath As String) As System.IO.TextWriter
        Return New StreamWriter(New FileStream(filePath, FileMode.OpenOrCreate))
    End Function
End Class

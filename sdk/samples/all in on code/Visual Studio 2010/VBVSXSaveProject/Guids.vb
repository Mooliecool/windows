Imports System

Class GuidList
    Private Sub New()
    End Sub

    Public Const guidVBVSXSaveProjectPkgString As String = "7c3477b5-5abb-4275-b1be-c25d848afc6b"
    Public Const guidVBVSXSaveProjectCmdSetString As String = "98f58ea4-d1bf-41c0-826a-109723d3bbe9"

    ' Create a Guid  for the new command string.
    Public Const guidVBVSXSaveProjectContextCmdSetString As String = "4FBD7DA8-EBC5-409D-BDB6-687139AFD158"

    Public Shared ReadOnly guidVBVSXSaveProjectCmdSet As New Guid(guidVBVSXSaveProjectCmdSetString)

    ' Add the Guid of the new command
    Public Shared ReadOnly guidVBVSXSaveProjectContextCmdSet As New Guid(guidVBVSXSaveProjectContextCmdSetString)

End Class
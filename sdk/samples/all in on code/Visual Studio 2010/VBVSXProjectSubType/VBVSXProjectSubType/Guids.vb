Imports System

Class GuidList
    Private Sub New()
    End Sub

    Public Const guidVBVSXProjectSubTypePkgString As String = "9bf379d8-3210-418b-9464-4daaa12d1206"
    Public Const guidVBVSXProjectSubTypeCmdSetString As String = "13c075f4-c001-4c5e-ba5d-4c5033e2df4f"

    Public Shared ReadOnly guidVBVSXProjectSubTypeCmdSet As New Guid(guidVBVSXProjectSubTypeCmdSetString)
End Class
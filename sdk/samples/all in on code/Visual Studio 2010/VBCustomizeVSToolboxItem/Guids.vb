Imports System

Class GuidList
    Private Sub New()
    End Sub

    Public Const guidVBCustomizeVSToolboxItemPkgString As String = "6612086d-0b0a-425a-b54b-3bef6b6f6cef"
    Public Const guidVBCustomizeVSToolboxItemCmdSetString As String = "30b794c7-cb4a-4378-b27b-6fb92fcde57d"

    Public Shared ReadOnly guidVBCustomizeVSToolboxItemCmdSet As New Guid(guidVBCustomizeVSToolboxItemCmdSetString)
End Class
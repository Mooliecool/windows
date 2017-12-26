Imports System.Activities
Imports System.Activities.Statements
Imports System.Diagnostics
Imports System.Linq

Module MainModule

    Dim s As Sequence

    Sub Main()
        WorkflowInvoker.Invoke(New Workflow1())
    End Sub

End Module

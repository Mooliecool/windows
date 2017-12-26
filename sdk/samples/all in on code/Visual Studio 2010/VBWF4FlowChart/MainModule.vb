Imports System.Activities
Imports System.Activities.Statements

Module MainModule

    Dim s As Sequence

    Sub Main()
        WorkflowInvoker.Invoke(New GuessNumberGameInFlowChart())
    End Sub

End Module

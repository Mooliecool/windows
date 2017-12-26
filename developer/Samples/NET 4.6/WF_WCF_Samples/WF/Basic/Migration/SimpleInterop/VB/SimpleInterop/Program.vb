'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.Activities
Imports System.Activities.Statements

Namespace Microsoft.Samples.WF.Migration
    Friend Class Program
        Shared Sub Main()
            Dim v As Variable(Of String) = New Variable(Of String)()
            Dim s As New Sequence

            s.Variables.Add(v)
            s.Activities.Add(New Assign(Of String)() With {.To = v, .Value = "hello, world"})

            Dim iop As Interop = New Interop With {.ActivityType = GetType(WriteLine)}

            ' Bind the Text property of the WriteLine to the Variable v
            iop.ActivityProperties.Add("Text", New InArgument(Of String)(v))

            ' Provide a value for the Name meta-property of the WriteLine
            iop.ActivityMetaProperties.Add("Name", "WriteLine")

            s.Activities.Add(iop)

            WorkflowInvoker.Invoke(s)

            Console.ReadLine()
        End Sub
    End Class
End Namespace
